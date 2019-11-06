#! /bin/bash

set -o errexit
set -o pipefail
set -o nounset

cmake --version
cmake CMakeLists.txt
cmake --build .
