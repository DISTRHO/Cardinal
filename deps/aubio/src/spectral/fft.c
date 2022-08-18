/*
  Copyright (C) 2003-2009 Paul Brossier <piem@aubio.org>

  This file is part of aubio.

  aubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "aubio_priv.h"
#include "fvec.h"
#include "cvec.h"
#include "mathutils.h"
#include "spectral/fft.h"

/* note that <complex.h> is not included here but only in aubio_priv.h, so that
 * c++ projects can still use their own complex definition. */
#include <fftw3.h>
#include <pthread.h>

/** fft data type with complex.h and fftw3f */
#define FFTW_TYPE fftwf_complex

/** fft data type */
typedef FFTW_TYPE fft_data_t;

#define fftw_malloc            fftwf_malloc
#define fftw_free              fftwf_free
#define fftw_execute           fftwf_execute
#define fftw_plan_dft_r2c_1d   fftwf_plan_dft_r2c_1d
#define fftw_plan_dft_c2r_1d   fftwf_plan_dft_c2r_1d
#define fftw_plan_r2r_1d       fftwf_plan_r2r_1d
#define fftw_plan              fftwf_plan
#define fftw_destroy_plan      fftwf_destroy_plan

#if HAVE_AUBIO_DOUBLE
#error "Using aubio in double precision with fftw3 in single precision"
#endif /* HAVE_AUBIO_DOUBLE */

#define real_t float

#ifndef SKIP_FFTW_MUTEX
// a global mutex for FFTW thread safety
pthread_mutex_t aubio_fftw_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

struct _aubio_fft_t {
  uint_t winsize;
  uint_t fft_size;

  real_t *in, *out;
  fftw_plan pfw, pbw;
  fft_data_t * specdata; /* complex spectral data */

  fvec_t * compspec;
};

aubio_fft_t * new_aubio_fft (uint_t winsize) {
  aubio_fft_t * s = AUBIO_NEW(aubio_fft_t);
  if ((sint_t)winsize < 2) {
    AUBIO_ERR("fft: got winsize %d, but can not be < 2\n", winsize);
    goto beach;
  }

  uint_t i;
  s->winsize  = winsize;
  /* allocate memory */
  s->in       = AUBIO_ARRAY(real_t,winsize);
  s->out      = AUBIO_ARRAY(real_t,winsize);
  s->compspec = new_fvec(winsize);
  /* create plans */
#ifndef SKIP_FFTW_MUTEX
  pthread_mutex_lock(&aubio_fftw_mutex);
#endif
  s->fft_size = winsize/2 + 1;
  s->specdata = (fft_data_t*)fftw_malloc(sizeof(fft_data_t)*s->fft_size);
  s->pfw = fftw_plan_dft_r2c_1d(winsize, s->in,  s->specdata, FFTW_ESTIMATE);
  s->pbw = fftw_plan_dft_c2r_1d(winsize, s->specdata, s->out, FFTW_ESTIMATE);
#ifndef SKIP_FFTW_MUTEX
  pthread_mutex_unlock(&aubio_fftw_mutex);
#endif
  for (i = 0; i < s->winsize; i++) {
    s->in[i] = 0.;
    s->out[i] = 0.;
  }
  for (i = 0; i < s->fft_size; i++) {
    s->specdata[i] = 0.;
  }

  return s;

beach:
  AUBIO_FREE(s);
  return NULL;
}

void del_aubio_fft(aubio_fft_t * s) {
  /* destroy data */
#ifndef SKIP_FFTW_MUTEX
  pthread_mutex_lock(&aubio_fftw_mutex);
#endif
  fftw_destroy_plan(s->pfw);
  fftw_destroy_plan(s->pbw);
  fftw_free(s->specdata);
#ifndef SKIP_FFTW_MUTEX
  pthread_mutex_unlock(&aubio_fftw_mutex);
#endif

  del_fvec(s->compspec);
  AUBIO_FREE(s->in);
  AUBIO_FREE(s->out);
  AUBIO_FREE(s);
}

void aubio_fft_do(aubio_fft_t * s, const fvec_t * input, cvec_t * spectrum) {
  aubio_fft_do_complex(s, input, s->compspec);
  aubio_fft_get_spectrum(s->compspec, spectrum);
}

void aubio_fft_rdo(aubio_fft_t * s, const cvec_t * spectrum, fvec_t * output) {
  aubio_fft_get_realimag(spectrum, s->compspec);
  aubio_fft_rdo_complex(s, s->compspec, output);
}

void aubio_fft_do_complex(aubio_fft_t * s, const fvec_t * input, fvec_t * compspec) {
  uint_t i;
#ifndef HAVE_MEMCPY_HACKS
  for (i=0; i < s->winsize; i++) {
    s->in[i] = input->data[i];
  }
#else
  memcpy(s->in, input->data, s->winsize * sizeof(smpl_t));
#endif /* HAVE_MEMCPY_HACKS */

  fftw_execute(s->pfw);
  compspec->data[0] = REAL(s->specdata[0]);
  for (i = 1; i < s->fft_size -1 ; i++) {
    compspec->data[i] = REAL(s->specdata[i]);
    compspec->data[compspec->length - i] = IMAG(s->specdata[i]);
  }
  compspec->data[s->fft_size-1] = REAL(s->specdata[s->fft_size-1]);
}

void aubio_fft_rdo_complex(aubio_fft_t * s, const fvec_t * compspec, fvec_t * output) {
  uint_t i;
  const smpl_t renorm = 1./(smpl_t)s->winsize;
  s->specdata[0] = compspec->data[0];
  for (i=1; i < s->fft_size - 1; i++) {
    s->specdata[i] = compspec->data[i] +
      I * compspec->data[compspec->length - i];
  }
  s->specdata[s->fft_size - 1] = compspec->data[s->fft_size - 1];
  fftw_execute(s->pbw);
  for (i = 0; i < output->length; i++) {
    output->data[i] = s->out[i]*renorm;
  }
}

void aubio_fft_get_spectrum(const fvec_t * compspec, cvec_t * spectrum) {
  aubio_fft_get_phas(compspec, spectrum);
  aubio_fft_get_norm(compspec, spectrum);
}

void aubio_fft_get_realimag(const cvec_t * spectrum, fvec_t * compspec) {
  aubio_fft_get_imag(spectrum, compspec);
  aubio_fft_get_real(spectrum, compspec);
}

void aubio_fft_get_phas(const fvec_t * compspec, cvec_t * spectrum) {
  uint_t i;
  if (compspec->data[0] < 0) {
    spectrum->phas[0] = PI;
  } else {
    spectrum->phas[0] = 0.;
  }
  for (i=1; i < spectrum->length - 1; i++) {
    spectrum->phas[i] = ATAN2(compspec->data[compspec->length-i],
        compspec->data[i]);
  }
  // for even length only, make sure last element is 0 or PI
  if (2 * (compspec->length / 2) == compspec->length) {
    if (compspec->data[compspec->length/2] < 0) {
      spectrum->phas[spectrum->length - 1] = PI;
    } else {
      spectrum->phas[spectrum->length - 1] = 0.;
    }
  } else {
    i = spectrum->length - 1;
    spectrum->phas[i] = ATAN2(compspec->data[compspec->length-i],
        compspec->data[i]);
  }
}

void aubio_fft_get_norm(const fvec_t * compspec, cvec_t * spectrum) {
  uint_t i = 0;
  spectrum->norm[0] = ABS(compspec->data[0]);
  for (i=1; i < spectrum->length - 1; i++) {
    spectrum->norm[i] = SQRT(SQR(compspec->data[i])
        + SQR(compspec->data[compspec->length - i]) );
  }
  // for even length, make sure last element is > 0
  if (2 * (compspec->length / 2) == compspec->length) {
    spectrum->norm[spectrum->length-1] =
      ABS(compspec->data[compspec->length/2]);
  } else {
    i = spectrum->length - 1;
    spectrum->norm[i] = SQRT(SQR(compspec->data[i])
        + SQR(compspec->data[compspec->length - i]) );
  }
}

void aubio_fft_get_imag(const cvec_t * spectrum, fvec_t * compspec) {
  uint_t i;
  for (i = 1; i < ( compspec->length + 1 ) / 2 /*- 1 + 1*/; i++) {
    compspec->data[compspec->length - i] =
      spectrum->norm[i]*SIN(spectrum->phas[i]);
  }
}

void aubio_fft_get_real(const cvec_t * spectrum, fvec_t * compspec) {
  uint_t i;
  for (i = 0; i < compspec->length / 2 + 1; i++) {
    compspec->data[i] =
      spectrum->norm[i]*COS(spectrum->phas[i]);
  }
}
