#!/bin/bash

sudo apt install clang-format cppcheck

cd cpputest
autoreconf . -i
./configure
make tdd