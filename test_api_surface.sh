#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage $0 [PATH_TO_libgravix2.so]"
    exit 1
fi

if [ ! -f api_surface.txt ]; then
    echo "Error: Could not find api_surface.txt"
    exit 2
fi

sort -o api_surface.txt{,}

API=`nm -D -f posix $1 | awk '{ print $2 "," $1}' | grep "^T," | cut -d, -f2 | sort`
if echo "$API" | cmp --silent - api_surface.txt; then
    echo SUCCESS
    exit 0
fi

echo "$API" | diff - api_surface.txt
echo FAILURE
exit 3
