#!/bin/bash

# Build the compiler
#
# You MUST replace the following commands with the commands for building your compiler

make clean -C ./src
if [[ $# == 1 ]] && [[ $1 == "clang" ]]
then
    make -C ./src TARGET=clang
else
    make -C ./src
fi
