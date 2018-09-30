#!/bin/bash

# source code directory of tensorflow
TF_DIR=`pwd`/../tensorflow-cmake

# external source code directory of tensorflow(e.g. Eigen)
TF_EXTERNAL_DIR=`pwd`/../tensorflow-cmake/bazel-tensorflow-cmake

# bazel build directory of tensorflow where `libtensorflow.so` exists.
# Please specify absolute path, otherwise cmake cannot find lib**.a
TF_BUILD_DIR=`pwd`/../tensorflow-cmake/bazel-bin/tensorflow

rm -rf build

cmake -DTENSORFLOW_DIR=${TF_DIR} \
      -DTENSORFLOW_EXTERNAL_DIR=${TF_EXTERNAL_DIR} \
      -DTENSORFLOW_BUILD_DIR=${TF_BUILD_DIR} \
      -DSANITIZE_ADDRESS=On \
      -Bbuild \
      -H.
