# Text-to-speech in C++ using Tensorflow for C.

Experiment.

## Performance

Currently TensorFlow C++ code path only uses single CPU core, and time for synthesis is roughly 10x slower on 2018's CPU than synthesized audio length(e.g. 60 secs for 6 secs audio).

## License

MIT license.

### Third party licenses

- json.hpp : MIT license
- cxxopts.hpp : MIT license
- dr_wav : Public domain
