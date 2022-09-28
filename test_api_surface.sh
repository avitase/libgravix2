#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage $0 [PATH_TO_libgravix2.so]"
    exit 1
fi

if [ ! -f api_surface.txt ]; then
    echo "Error: Could not find api_surface.txt"
    exit 2
fi

nm -D -f posix $1 | awk '{ print $2 "," $1}' | grep "^T," | cut -d, -f2 | cmp - api_surface.txt || exit 3

echo "Success"
