#!/bin/sh

# The purpose of this script is to automatically install all the required
# dependencies for building the project. A few different operating systems are
# supported. If your particular OS is not supported, you need to manually
# install the dependencies.

install_unknown() {
  echo "You need to install:"
  echo " - CMake (2.8.12+, https://cmake.org)"
  echo " - A compiler and system libraries capable of C++11 (e.g. GCC 4.8+)"
  echo " - Window system (e.g. X11) and OpenGL development libraries"
}

install_apt() {
  echo "[Linux / apt-get]"
  sudo apt-get install cmake ninja-build build-essential xorg-dev libgl1-mesa-dev
}

install_yum() {
  echo "[Linux / yum]"
  sudo yum groupinstall "Development Tools"
  sudo yum install cmake libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel mesa-libGL-devel
}

install_freebsd_pkg() {
  echo "[FreeBSD / pkg]"
  sudo pkg install cmake ninja glproto dri2proto libXi libXrandr
}

install_mac() {
  echo "[Mac OS X]"
  # Attempt to install dependencies using Homebrew.
  if ! command -v brew >/dev/null 2>&1; then
    echo "Installing Homebrew."
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  fi
  if command -v brew >/dev/null 2>&1; then
    if ! command -v cmake >/dev/null 2>&1; then
      echo "Installing CMake."
      brew install cmake
    fi

    if ! command -v ninja >/dev/null 2>&1; then
      echo "Installing Ninja."
      brew install ninja
    fi
  fi

  if ! command -v xcodebuild >/dev/null 2>&1; then
    echo "You need to install Xcode."
  fi
}

if [ "$(uname -s)" = "Linux" ]; then
  # Linux
  if command -v apt-get >/dev/null 2>&1; then
    install_apt
  elif command -v yum >/dev/null 2>&1; then
    install_yum
  else
    install_unknown
  fi
elif [ "$(uname -s)" = "FreeBSD" ]; then
  # FreeBSD
  if command -v pkg >/dev/null 2>&1; then
    install_freebsd_pkg
  else
    install_unknown
  fi
elif [ "$(uname -s)" = "Darwin" ]; then
  # Mac OS X
  install_mac
else
  install_unknown
fi

