#! /bin/bash

set -o errexit
set -o pipefail
set -o nounset

# Runs the Clang Formatter in parallel on the code base.
# Return codes:
#  - 1 there are files to be formatted
#  - 0 everything looks fine

# Get CPU count
OS=$(uname)
NPROC=1
if [[ $OS = "Linux" ]] ; then
    NPROC=$(nproc)
elif [[ ${OS} = "Darwin" ]] ; then
    NPROC=$(sysctl -n hw.physicalcpu)
fi

# Discover clang-format
if type clang-format-8 2> /dev/null ; then
    CLANG_FORMAT=clang-format-8
elif type clang-format 2> /dev/null ; then
    # Clang format found, but need to check version
    CLANG_FORMAT=clang-format
    V=$(clang-format --version)
    if [[ $V != *8* ]] ; then
        echo "clang-format is not 8 (returned ${V})"
        exit 1
    fi
else
    echo "No appropriate clang-format found (expected clang-format-8, or clang-format)"
    exit 1
fi

find . -type f -name '*.hpp' -o -name '*.cpp' \
    | xargs -I{} -P ${NPROC} ${CLANG_FORMAT} -i -style=file {}

git diff > clang_format.patch

if [ -s clang_format.patch ]
then
    echo "Clang format found possible changes:"
    cat clang_format.patch
    exit 1
fi

exit 0

