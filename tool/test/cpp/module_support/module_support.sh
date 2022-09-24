#!/usr/bin/env bash

water::tests::module_support::build() {
  g++-11 \
    module_support.cpp \
    -std=c++2a \
    -fPIC \
    -fmodules-ts \
    -o module_support.test
}

water::tests::module_support::run() {
  ./module_support.test
}

water::tests::module_support() {
  if water::tests::module_support::build; then
    if water::tests::module_support::run; then
      echo """[passing]
  - built and ran"""
      return 0
    else
      echo """[failing]
  - could not run"""
      return 1
    fi
  else
    echo """[failing]
  - could not build"""
    return 2
  fi
}

water::tests::module_support
exit $?
