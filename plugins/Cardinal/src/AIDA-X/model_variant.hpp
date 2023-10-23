#include <variant>
#include <RTNeural/RTNeural.h>

#define MAX_INPUT_SIZE 3
struct NullModel { static constexpr int input_size = 0; static constexpr int output_size = 0; };
using ModelType_GRU_8_1 = RTNeural::ModelT<float, 1, 1, RTNeural::GRULayerT<float, 1, 8>, RTNeural::DenseT<float, 8, 1>>;
using ModelType_GRU_8_2 = RTNeural::ModelT<float, 2, 1, RTNeural::GRULayerT<float, 2, 8>, RTNeural::DenseT<float, 8, 1>>;
using ModelType_GRU_8_3 = RTNeural::ModelT<float, 3, 1, RTNeural::GRULayerT<float, 3, 8>, RTNeural::DenseT<float, 8, 1>>;
using ModelType_GRU_12_1 = RTNeural::ModelT<float, 1, 1, RTNeural::GRULayerT<float, 1, 12>, RTNeural::DenseT<float, 12, 1>>;
using ModelType_GRU_12_2 = RTNeural::ModelT<float, 2, 1, RTNeural::GRULayerT<float, 2, 12>, RTNeural::DenseT<float, 12, 1>>;
using ModelType_GRU_12_3 = RTNeural::ModelT<float, 3, 1, RTNeural::GRULayerT<float, 3, 12>, RTNeural::DenseT<float, 12, 1>>;
using ModelType_GRU_16_1 = RTNeural::ModelT<float, 1, 1, RTNeural::GRULayerT<float, 1, 16>, RTNeural::DenseT<float, 16, 1>>;
using ModelType_GRU_16_2 = RTNeural::ModelT<float, 2, 1, RTNeural::GRULayerT<float, 2, 16>, RTNeural::DenseT<float, 16, 1>>;
using ModelType_GRU_16_3 = RTNeural::ModelT<float, 3, 1, RTNeural::GRULayerT<float, 3, 16>, RTNeural::DenseT<float, 16, 1>>;
using ModelType_GRU_20_1 = RTNeural::ModelT<float, 1, 1, RTNeural::GRULayerT<float, 1, 20>, RTNeural::DenseT<float, 20, 1>>;
using ModelType_GRU_20_2 = RTNeural::ModelT<float, 2, 1, RTNeural::GRULayerT<float, 2, 20>, RTNeural::DenseT<float, 20, 1>>;
using ModelType_GRU_20_3 = RTNeural::ModelT<float, 3, 1, RTNeural::GRULayerT<float, 3, 20>, RTNeural::DenseT<float, 20, 1>>;
using ModelType_GRU_32_1 = RTNeural::ModelT<float, 1, 1, RTNeural::GRULayerT<float, 1, 32>, RTNeural::DenseT<float, 32, 1>>;
using ModelType_GRU_32_2 = RTNeural::ModelT<float, 2, 1, RTNeural::GRULayerT<float, 2, 32>, RTNeural::DenseT<float, 32, 1>>;
using ModelType_GRU_32_3 = RTNeural::ModelT<float, 3, 1, RTNeural::GRULayerT<float, 3, 32>, RTNeural::DenseT<float, 32, 1>>;
using ModelType_GRU_40_1 = RTNeural::ModelT<float, 1, 1, RTNeural::GRULayerT<float, 1, 40>, RTNeural::DenseT<float, 40, 1>>;
using ModelType_GRU_40_2 = RTNeural::ModelT<float, 2, 1, RTNeural::GRULayerT<float, 2, 40>, RTNeural::DenseT<float, 40, 1>>;
using ModelType_GRU_40_3 = RTNeural::ModelT<float, 3, 1, RTNeural::GRULayerT<float, 3, 40>, RTNeural::DenseT<float, 40, 1>>;
using ModelType_GRU_64_1 = RTNeural::ModelT<float, 1, 1, RTNeural::GRULayerT<float, 1, 64>, RTNeural::DenseT<float, 64, 1>>;
using ModelType_GRU_64_2 = RTNeural::ModelT<float, 2, 1, RTNeural::GRULayerT<float, 2, 64>, RTNeural::DenseT<float, 64, 1>>;
using ModelType_GRU_64_3 = RTNeural::ModelT<float, 3, 1, RTNeural::GRULayerT<float, 3, 64>, RTNeural::DenseT<float, 64, 1>>;
using ModelType_LSTM_8_1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 8>, RTNeural::DenseT<float, 8, 1>>;
using ModelType_LSTM_8_2 = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 8>, RTNeural::DenseT<float, 8, 1>>;
using ModelType_LSTM_8_3 = RTNeural::ModelT<float, 3, 1, RTNeural::LSTMLayerT<float, 3, 8>, RTNeural::DenseT<float, 8, 1>>;
using ModelType_LSTM_12_1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 12>, RTNeural::DenseT<float, 12, 1>>;
using ModelType_LSTM_12_2 = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 12>, RTNeural::DenseT<float, 12, 1>>;
using ModelType_LSTM_12_3 = RTNeural::ModelT<float, 3, 1, RTNeural::LSTMLayerT<float, 3, 12>, RTNeural::DenseT<float, 12, 1>>;
using ModelType_LSTM_16_1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 16>, RTNeural::DenseT<float, 16, 1>>;
using ModelType_LSTM_16_2 = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 16>, RTNeural::DenseT<float, 16, 1>>;
using ModelType_LSTM_16_3 = RTNeural::ModelT<float, 3, 1, RTNeural::LSTMLayerT<float, 3, 16>, RTNeural::DenseT<float, 16, 1>>;
using ModelType_LSTM_20_1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 20>, RTNeural::DenseT<float, 20, 1>>;
using ModelType_LSTM_20_2 = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 20>, RTNeural::DenseT<float, 20, 1>>;
using ModelType_LSTM_20_3 = RTNeural::ModelT<float, 3, 1, RTNeural::LSTMLayerT<float, 3, 20>, RTNeural::DenseT<float, 20, 1>>;
using ModelType_LSTM_32_1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 32>, RTNeural::DenseT<float, 32, 1>>;
using ModelType_LSTM_32_2 = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 32>, RTNeural::DenseT<float, 32, 1>>;
using ModelType_LSTM_32_3 = RTNeural::ModelT<float, 3, 1, RTNeural::LSTMLayerT<float, 3, 32>, RTNeural::DenseT<float, 32, 1>>;
using ModelType_LSTM_40_1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 40>, RTNeural::DenseT<float, 40, 1>>;
using ModelType_LSTM_40_2 = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 40>, RTNeural::DenseT<float, 40, 1>>;
using ModelType_LSTM_40_3 = RTNeural::ModelT<float, 3, 1, RTNeural::LSTMLayerT<float, 3, 40>, RTNeural::DenseT<float, 40, 1>>;
using ModelType_LSTM_64_1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 64>, RTNeural::DenseT<float, 64, 1>>;
using ModelType_LSTM_64_2 = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 64>, RTNeural::DenseT<float, 64, 1>>;
using ModelType_LSTM_64_3 = RTNeural::ModelT<float, 3, 1, RTNeural::LSTMLayerT<float, 3, 64>, RTNeural::DenseT<float, 64, 1>>;
using ModelVariantType = std::variant<NullModel,ModelType_GRU_8_1,ModelType_GRU_8_2,ModelType_GRU_8_3,ModelType_GRU_12_1,ModelType_GRU_12_2,ModelType_GRU_12_3,ModelType_GRU_16_1,ModelType_GRU_16_2,ModelType_GRU_16_3,ModelType_GRU_20_1,ModelType_GRU_20_2,ModelType_GRU_20_3,ModelType_GRU_32_1,ModelType_GRU_32_2,ModelType_GRU_32_3,ModelType_GRU_40_1,ModelType_GRU_40_2,ModelType_GRU_40_3,ModelType_GRU_64_1,ModelType_GRU_64_2,ModelType_GRU_64_3,ModelType_LSTM_8_1,ModelType_LSTM_8_2,ModelType_LSTM_8_3,ModelType_LSTM_12_1,ModelType_LSTM_12_2,ModelType_LSTM_12_3,ModelType_LSTM_16_1,ModelType_LSTM_16_2,ModelType_LSTM_16_3,ModelType_LSTM_20_1,ModelType_LSTM_20_2,ModelType_LSTM_20_3,ModelType_LSTM_32_1,ModelType_LSTM_32_2,ModelType_LSTM_32_3,ModelType_LSTM_40_1,ModelType_LSTM_40_2,ModelType_LSTM_40_3,ModelType_LSTM_64_1,ModelType_LSTM_64_2,ModelType_LSTM_64_3>;

inline bool is_model_type_ModelType_GRU_8_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 8;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_8_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 8;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_8_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 8;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_12_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 12;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_12_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 12;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_12_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 12;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_16_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 16;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_16_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 16;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_16_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 16;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_20_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 20;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_20_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 20;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_20_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 20;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_32_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 32;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_32_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 32;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_32_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 32;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_40_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 40;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_40_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 40;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_40_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 40;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_64_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 64;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_64_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 64;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_GRU_64_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "gru";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 64;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_8_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 8;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_8_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 8;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_8_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 8;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_12_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 12;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_12_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 12;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_12_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 12;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_16_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 16;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_16_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 16;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_16_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 16;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_20_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 20;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_20_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 20;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_20_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 20;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_32_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 32;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_32_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 32;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_32_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 32;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_40_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 40;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_40_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 40;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_40_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 40;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_64_1 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 64;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 1;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_64_2 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 64;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 2;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool is_model_type_ModelType_LSTM_64_3 (const nlohmann::json& model_json) {
    const auto json_layers = model_json.at ("layers");
    const auto rnn_layer_type = json_layers.at (0).at ("type").get<std::string>();
    const auto is_layer_type_correct = rnn_layer_type == "lstm";
    const auto hidden_size = json_layers.at (0).at ("shape").back().get<int>();
    const auto is_hidden_size_correct = hidden_size == 64;
    const auto input_size = model_json.at ("in_shape").back().get<int>();
    const auto is_input_size_correct = input_size == 3;
    return is_layer_type_correct && is_hidden_size_correct && is_input_size_correct;
}

inline bool custom_model_creator (const nlohmann::json& model_json, ModelVariantType& model) {
    if (is_model_type_ModelType_GRU_8_1 (model_json)) {
        model.emplace<ModelType_GRU_8_1>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_8_2 (model_json)) {
        model.emplace<ModelType_GRU_8_2>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_8_3 (model_json)) {
        model.emplace<ModelType_GRU_8_3>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_12_1 (model_json)) {
        model.emplace<ModelType_GRU_12_1>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_12_2 (model_json)) {
        model.emplace<ModelType_GRU_12_2>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_12_3 (model_json)) {
        model.emplace<ModelType_GRU_12_3>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_16_1 (model_json)) {
        model.emplace<ModelType_GRU_16_1>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_16_2 (model_json)) {
        model.emplace<ModelType_GRU_16_2>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_16_3 (model_json)) {
        model.emplace<ModelType_GRU_16_3>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_20_1 (model_json)) {
        model.emplace<ModelType_GRU_20_1>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_20_2 (model_json)) {
        model.emplace<ModelType_GRU_20_2>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_20_3 (model_json)) {
        model.emplace<ModelType_GRU_20_3>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_32_1 (model_json)) {
        model.emplace<ModelType_GRU_32_1>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_32_2 (model_json)) {
        model.emplace<ModelType_GRU_32_2>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_32_3 (model_json)) {
        model.emplace<ModelType_GRU_32_3>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_40_1 (model_json)) {
        model.emplace<ModelType_GRU_40_1>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_40_2 (model_json)) {
        model.emplace<ModelType_GRU_40_2>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_40_3 (model_json)) {
        model.emplace<ModelType_GRU_40_3>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_64_1 (model_json)) {
        model.emplace<ModelType_GRU_64_1>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_64_2 (model_json)) {
        model.emplace<ModelType_GRU_64_2>();
        return true;
    }
    else if (is_model_type_ModelType_GRU_64_3 (model_json)) {
        model.emplace<ModelType_GRU_64_3>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_8_1 (model_json)) {
        model.emplace<ModelType_LSTM_8_1>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_8_2 (model_json)) {
        model.emplace<ModelType_LSTM_8_2>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_8_3 (model_json)) {
        model.emplace<ModelType_LSTM_8_3>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_12_1 (model_json)) {
        model.emplace<ModelType_LSTM_12_1>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_12_2 (model_json)) {
        model.emplace<ModelType_LSTM_12_2>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_12_3 (model_json)) {
        model.emplace<ModelType_LSTM_12_3>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_16_1 (model_json)) {
        model.emplace<ModelType_LSTM_16_1>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_16_2 (model_json)) {
        model.emplace<ModelType_LSTM_16_2>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_16_3 (model_json)) {
        model.emplace<ModelType_LSTM_16_3>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_20_1 (model_json)) {
        model.emplace<ModelType_LSTM_20_1>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_20_2 (model_json)) {
        model.emplace<ModelType_LSTM_20_2>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_20_3 (model_json)) {
        model.emplace<ModelType_LSTM_20_3>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_32_1 (model_json)) {
        model.emplace<ModelType_LSTM_32_1>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_32_2 (model_json)) {
        model.emplace<ModelType_LSTM_32_2>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_32_3 (model_json)) {
        model.emplace<ModelType_LSTM_32_3>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_40_1 (model_json)) {
        model.emplace<ModelType_LSTM_40_1>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_40_2 (model_json)) {
        model.emplace<ModelType_LSTM_40_2>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_40_3 (model_json)) {
        model.emplace<ModelType_LSTM_40_3>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_64_1 (model_json)) {
        model.emplace<ModelType_LSTM_64_1>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_64_2 (model_json)) {
        model.emplace<ModelType_LSTM_64_2>();
        return true;
    }
    else if (is_model_type_ModelType_LSTM_64_3 (model_json)) {
        model.emplace<ModelType_LSTM_64_3>();
        return true;
    }
    model.emplace<NullModel>();
    return false;
}
