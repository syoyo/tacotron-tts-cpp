#!/bin/bash

# source code directory of tensorflow
TF_DIR=/tensorflow

# external source code directory of tensorflow(e.g. Eigen)
TF_EXTERNAL_DIR=/tensorflow/bazel-tensorflow

# bazel build directory of tensorflow where `libtensorflow.so` exists.
# Please specify absolute path, otherwise cmake cannot find lib**.a
TF_BUILD_DIR=/tensorflow/bazel-bin/tensorflow

rm -rf build

cmake -DTENSORFLOW_DIR=${TF_DIR} \
      -DTENSORFLOW_EXTERNAL_DIR=${TF_EXTERNAL_DIR} \
      -DTENSORFLOW_BUILD_DIR=${TF_BUILD_DIR} \
      -DTENSORFLOW_LIB=tensorflow \
      -DSANITIZE_ADDRESS=On \
      -Bbuild \
      -H.
