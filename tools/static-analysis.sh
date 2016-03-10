#!/bin/sh

# Run static analysis tests on the sources of this project.

# Move to the root directory of the project.
if [ "x${BASH_SOURCE}" != "x" ]; then
  PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/.."
else
  cd "$(dirname $0)/.."
  PROJECT_DIR="$(pwd)"
fi
cd ${PROJECT_DIR}

# Get a list of all the C++ source code files in this project.
CPP_SOURCE_FILES=$(git ls-files 'src/viewer/*.cc')
H_SOURCE_FILES=$(git ls-files 'src/viewer/*.h')
ALL_SOURCE_FILES="${CPP_SOURCE_FILES} ${H_SOURCE_FILES}"


# Run cpplint on all source files.
echo "Running cpplint..."
tools/cpplint.py --root=src ${ALL_SOURCE_FILES}

