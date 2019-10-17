#!/bin/bash

# Edit path to Android NDK
# default installation directory of NDK through Android Studio
ANDROID_NDK_ROOT=/android/ndk

# source code directory of tensorflow
TF_DIR=/tensorflow

# external source code directory of tensorflow(e.g. Eigen)
TF_EXTERNAL_DIR=/tensorflow/bazel-tensorflow

# bazel build directory of tensorflow where `libtensorflow.so` exists.
# Please specify absolute path, otherwise cmake cannot find lib**.a
TF_BUILD_DIR=/tensorflow/bazel-bin/tensorflow/core

rm -rf build-android

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_NATIVE_API_LEVEL=23 \
      -DANDROID_ARM_MODE=arm \
      -DANDROID_ARM_NEON=TRUE \
      -DANDROID_STL=c++_shared \
      -DTENSORFLOW_DIR=${TF_DIR} \
      -DTENSORFLOW_EXTERNAL_DIR=${TF_EXTERNAL_DIR} \
      -DTENSORFLOW_BUILD_DIR=${TF_BUILD_DIR} \
      -DTENSORFLOW_LIB=android_tensorflow_lib \
      -DSANITIZE_ADDRESS=On \
      -Bbuild-android -H.
