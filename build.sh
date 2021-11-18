#!/bin/sh

mkdir -p build && cd build && rm -rf * && conan install -s build_type=Debug .. --build=missing && conan build .. && cd ..


