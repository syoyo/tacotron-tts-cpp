#include <cstdio>
#include <cstdlib>
#include <fstream>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#include "cxxopts.hpp"
#include "json.hpp"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "tf_synthesizer.h"

template<typename T>
bool GetNumberArray(const nlohmann::json &j, const std::string &name,
                    std::vector<T> *value) {
  if (j.find(name) == j.end()) {
    std::cerr << "Property not found : " << name << std::endl;
    return false;
  }

  if (!j.at(name).is_array()) {
    std::cerr << "Property is not an array type : " << name << std::endl;
    return false;
  }

  std::vector<T> v;
  for (auto &element : j.at(name)) {
    if (!element.is_number()) {
      std::cerr << "An array element is not a number" << std::endl;
      return false;
    }

    v.push_back(static_cast<T>(element.get<double>()));
  }

  (*value) = v;
  return true;
}


// Load sequence from JSON array
bool LoadSequence(const std::string &sequence_filename, std::vector<int32_t> *sequence)
{
  std::ifstream is(sequence_filename);
  if (!is) {
    std::cerr << "Failed to open/read file : " << sequence_filename << std::endl;
    return false;
  }

  nlohmann::json j;
  is >> j;

  return GetNumberArray(j, "sequence", sequence);
  
}


int main(int argc, char **argv) {
  cxxopts::Options options("tts", "Tacotron text to speec in C++");
  options.add_options()("i,input", "Input sequence file(JSON)",
                        cxxopts::value<std::string>())(
      "g,graph", "Input freezed graph file", cxxopts::value<std::string>())
      ("h,hparams", "Hyper parameters(JSON)", cxxopts::value<std::string>());

  auto result = options.parse(argc, argv);

  if (!result.count("input")) {
    std::cerr << "Please specify input sequence file with -i or --input option."
              << std::endl;
    return -1;
  }

  if (!result.count("graph")) {
    std::cerr << "Please specify freezed graph with -g or --graph option."
              << std::endl;
    return -1;
  }

  if (result.count("hparams")) {
  }

  std::string input_filename = result["input"].as<std::string>();
  std::string graph_filename = result["graph"].as<std::string>();

  std::vector<int32_t> sequence;
  if (!LoadSequence(input_filename, &sequence)) {
    std::cerr << "Failed to load sequence data : " << input_filename << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "sequence = [";
  for (size_t i = 0; i < sequence.size(); i++) {
    std::cout << sequence[i];
    if (i != (sequence.size() - 1)) {
      std::cout << ", ";
    }
  }
  std::cout << "]" << std::endl;

  // Synthesize(generate wav from sequence)
  tts::TensorflowSynthesizer tf_synthesizer;
  tf_synthesizer.init(argc, argv);
  if (!tf_synthesizer.load(graph_filename, "inputs",
                    "model/griffinlim/Squeeze")) {
    std::cerr << "Failed to load/setup Tensorflow model from a frozen graph : " << graph_filename << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Synthesize..." << std::endl;

  std::vector<int32_t> input_lengths;
  input_lengths.push_back(int(sequence.size()));

  if (!tf_synthesizer.synthesize(sequence, input_lengths)) {
    std::cerr << "Failed to synthesize for a given sequence." << std::endl;
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}
