#! /bin/bash

set -o errexit
set -o pipefail
set -o nounset

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/build/lib
./build/bin/robot $@
