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
CPP_SOURCE_FILES=$(git ls-files 'src/viewer/*.cc' 'src/ui/*.cc')
H_SOURCE_FILES=$(git ls-files 'src/viewer/*.h' 'src/ui/*.h')
ALL_SOURCE_FILES="${CPP_SOURCE_FILES} ${H_SOURCE_FILES}"

# Run cpplint on all source files.
echo "Running cpplint..."
tools/cpplint.py --root=src ${ALL_SOURCE_FILES}

# Check if we have any supported Clang static analysis tools.
FOUND_CLANG_TOOLS=no
if command -v clang-tidy >/dev/null 2>&1; then
  FOUND_CLANG_TOOLS=yes
elif command -v clang-check >/dev/null 2>&1; then
  FOUND_CLANG_TOOLS=yes
fi

# Run Clang static analysis, if we have it.
if [ "${FOUND_CLANG_TOOLS}" = "yes" ]; then
  # Generate a compile command database for our project.
  echo ""
  echo "Building compile command database..."
  TMP_BUILD=/tmp/build-$$
  mkdir ${TMP_BUILD}
  cd ${TMP_BUILD}
  cmake -G"Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${PROJECT_DIR}/src
  cd ${PROJECT_DIR}

  # Run clang-tidy on all our CPP files.
  if command -v clang-tidy >/dev/null 2>&1; then
    echo ""
    echo "Running clang-tidy..."
    clang-tidy -p ${TMP_BUILD} ${CPP_SOURCE_FILES}
  fi

  # Run clang-check on all our CPP files.
  if command -v clang-check >/dev/null 2>&1; then
    echo ""
    echo "Running clang-check..."
    clang-check -analyze -p ${TMP_BUILD} ${CPP_SOURCE_FILES}
  fi

  # Delete the temporary directory.
  rm -rf ${TMP_BUILD}

  # TODO(m): Exit with an error code if any of the tests failed.
fi
