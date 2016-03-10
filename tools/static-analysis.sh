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

# Run clang-tidy static analysis, if we have it.
if command -v clang-tidy >/dev/null 2>&1; then
  echo ""
  echo "Running clang-tidy..."

  # Generate a compile command database for our project.
  TMP_BUILD=/tmp/build-$$
  mkdir ${TMP_BUILD}
  cd ${TMP_BUILD}
  cmake -G"Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${PROJECT_DIR}/src
  cd ${PROJECT_DIR}

  # Run clang-tidy on all our CPP files.
  clang-tidy -p ${TMP_BUILD} ${CPP_SOURCE_FILES}

  # Delete the temporary directory.
  rm -rf ${TMP_BUILD}
fi
