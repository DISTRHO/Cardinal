#include "../Bidoo/src/dep/waves.hpp"
#include "../Bidoo/src/dep/AudioFile/AudioFile.h"
// #define DR_WAV_IMPLEMENTATION
#include "../cf/src/dr_wav.h"
#include <dsp/resampler.hpp>

#ifndef DRWAV_ASSERT
#include <assert.h>
#define DRWAV_ASSERT(expression)           assert(expression)
#endif
#ifndef DRWAV_MALLOC
#define DRWAV_MALLOC(sz)                   malloc((sz))
#endif
#ifndef DRWAV_FREE
#define DRWAV_FREE(p)                      free((p))
#endif

#define drwav_assert                       DRWAV_ASSERT

#if defined(SIZE_MAX)
    #define DRWAV_SIZE_MAX  SIZE_MAX
#else
    #if defined(_WIN64) || defined(_LP64) || defined(__LP64__)
        #define DRWAV_SIZE_MAX  ((drwav_uint64)0xFFFFFFFFFFFFFFFF)
    #else
        #define DRWAV_SIZE_MAX  0xFFFFFFFF
    #endif
#endif

extern "C" {
float* drwav_open_file_and_read_f32(const char* filename, unsigned int* channels, unsigned int* sampleRate, drwav_uint64* totalSampleCount);
}

static float* drwav__read_and_close_f32(drwav* pWav, unsigned int* channels, unsigned int* sampleRate, drwav_uint64* totalSampleCount)
{
    drwav_uint64 sampleDataSize;
    float* pSampleData;
    drwav_uint64 samplesRead;

    drwav_assert(pWav != NULL);

    sampleDataSize = pWav->totalSampleCount * sizeof(float);
    if (sampleDataSize > DRWAV_SIZE_MAX) {
        drwav_uninit(pWav);
        return NULL;    /* File's too big. */
    }

    pSampleData = (float*)DRWAV_MALLOC((size_t)sampleDataSize);    /* <-- Safe cast due to the check above. */
    if (pSampleData == NULL) {
        drwav_uninit(pWav);
        return NULL;    /* Failed to allocate memory. */
    }

    samplesRead = drwav_read_f32(pWav, (size_t)pWav->totalSampleCount, pSampleData);
    if (samplesRead != pWav->totalSampleCount) {
        DRWAV_FREE(pSampleData);
        drwav_uninit(pWav);
        return NULL;    /* There was an error reading the samples. */
    }

    drwav_uninit(pWav);

    if (sampleRate) {
        *sampleRate = pWav->sampleRate;
    }
    if (channels) {
        *channels = pWav->channels;
    }
    if (totalSampleCount) {
        *totalSampleCount = pWav->totalSampleCount;
    }

    return pSampleData;
}

float* drwav_open_file_and_read_f32(const char* filename, unsigned int* channels, unsigned int* sampleRate, drwav_uint64* totalSampleCount)
{
    drwav wav;

    if (sampleRate) {
        *sampleRate = 0;
    }
    if (channels) {
        *channels = 0;
    }
    if (totalSampleCount) {
        *totalSampleCount = 0;
    }

    if (!drwav_init_file(&wav, filename)) {
        return NULL;
    }

    return drwav__read_and_close_f32(&wav, channels, sampleRate, totalSampleCount);
}

namespace waves {

  std::vector<rack::dsp::Frame<1>> getMonoWav(const std::string path, const float currentSampleRate, std::string &waveFileName, std::string &waveExtension, int &sampleChannels, int &sampleRate, int &sampleCount) {
    waveFileName = rack::system::getFilename(path);
    waveExtension = rack::system::getExtension(waveFileName);
    std::vector<rack::dsp::Frame<1>> result;
    if (waveExtension == ".wav") {
      unsigned int c;
      unsigned int sr;
      drwav_uint64 sc;
      float* pSampleData;
      pSampleData = drwav_open_file_and_read_f32(path.c_str(), &c, &sr, &sc);
      if (pSampleData != NULL)  {
        sampleChannels = c;
        sampleRate = sr;
        for (long long unsigned int i=0; i < sc; i = i + c) {
          rack::dsp::Frame<1> frame;
          if (sampleChannels == 2) {
            frame.samples[0] = (pSampleData[i] + pSampleData[i+1])/2.0f;
          }
          else {
            frame.samples[0] = pSampleData[i];
          }
          result.push_back(frame);
        }
        sampleCount = sc/c;
        drwav_free(pSampleData);
      }
    }
    else if (waveExtension == ".aiff") {
      AudioFile<float> audioFile;
      if (audioFile.load (path.c_str()))  {
        sampleChannels = audioFile.getNumChannels();
        sampleRate = audioFile.getSampleRate();
        sampleCount = audioFile.getNumSamplesPerChannel();
        for (int i=0; i < sampleCount; i++) {
          rack::dsp::Frame<1> frame;
          if (sampleChannels == 2) {
            frame.samples[0] = (audioFile.samples[0][i] + audioFile.samples[1][i])/2.0f;
          }
          else {
            frame.samples[0] = audioFile.samples[0][i];
          }
          result.push_back(frame);
        }
      }
    }

    if ((sampleRate != currentSampleRate) && (sampleCount>0)) {
      rack::dsp::SampleRateConverter<1> conv;
      conv.setRates(currentSampleRate, sampleRate);
      int outCount = sampleCount;
      std::vector<rack::dsp::Frame<1>> subResult;
      for (int i=0;i<sampleCount;i++) {
        rack::dsp::Frame<1> frame;
        frame.samples[0]=0.0f;
        subResult.push_back(frame);
      }
      conv.process(&result[0], &sampleCount, &subResult[0], &outCount);
      sampleCount = outCount;
      return subResult;
    }

    return result;
  }

  std::vector<rack::dsp::Frame<2>> getStereoWav(const std::string path, const float currentSampleRate, std::string &waveFileName, std::string &waveExtension, int &sampleChannels, int &sampleRate, int &sampleCount) {
    waveFileName = rack::system::getFilename(path);
    waveExtension = rack::system::getExtension(waveFileName);
    std::vector<rack::dsp::Frame<2>> result;
    if (waveExtension == ".wav") {
      unsigned int c;
      unsigned int sr;
      drwav_uint64 sc;
      float* pSampleData;
      pSampleData = drwav_open_file_and_read_f32(path.c_str(), &c, &sr, &sc);
      if (pSampleData != NULL)  {
        sampleChannels = c;
        sampleRate = sr;
        for (long long unsigned int i=0; i < sc; i = i + c) {
          rack::dsp::Frame<2> frame;
          frame.samples[0] = pSampleData[i];
  				if (sampleChannels == 2)
  					frame.samples[1] = (float)pSampleData[i+1];
  				else
  					frame.samples[1] = (float)pSampleData[i];
          result.push_back(frame);
        }
        sampleCount = sc/c;
        drwav_free(pSampleData);
      }
    }
    else if (waveExtension == ".aiff") {
      AudioFile<float> audioFile;
      if (audioFile.load (path.c_str()))  {
        sampleChannels = audioFile.getNumChannels();
        sampleRate = audioFile.getSampleRate();
        sampleCount = audioFile.getNumSamplesPerChannel();
        for (int i=0; i < sampleCount; i++) {
          rack::dsp::Frame<2> frame;
          frame.samples[0] = audioFile.samples[0][i];
  				if (sampleChannels == 2)
  					frame.samples[1] = audioFile.samples[1][i];
  				else
  					frame.samples[1] = audioFile.samples[0][i];
          result.push_back(frame);
        }
      }
    }

    if ((sampleRate != currentSampleRate) && (sampleCount>0)) {
      rack::dsp::SampleRateConverter<2> conv;
      conv.setRates(sampleRate, currentSampleRate);
      conv.setQuality(SPEEX_RESAMPLER_QUALITY_DESKTOP);
      int outCount = 16*sampleCount;
      std::vector<rack::dsp::Frame<2>> subResult;
      for (int i=0;i<outCount;i++) {
        rack::dsp::Frame<2> frame;
        frame.samples[0]=0.0f;
        frame.samples[1]=0.0f;
        subResult.push_back(frame);
      }
      conv.process(&result[0], &sampleCount, &subResult[0], &outCount);
      sampleCount = outCount;
      return subResult;
    }

    return result;
  }

  void saveWave(std::vector<rack::dsp::Frame<2>> &sample, int sampleRate, std::string path) {
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = 2;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32;

    int *pSamples = (int*)calloc(2*sample.size(),sizeof(int));
    memset(pSamples, 0, 2*sample.size()*sizeof(int));
    for (unsigned int i = 0; i < sample.size(); i++) {
    	*(pSamples+2*i)= floor(sample[i].samples[0]*2147483647);
    	*(pSamples+2*i+1)= floor(sample[i].samples[1]*2147483647);
    }

    drwav* pWav = drwav_open_file_write(path.c_str(), &format);
    drwav_write(pWav, 2*sample.size(), pSamples);
    drwav_close(pWav);
    free(pSamples);
  }

}
