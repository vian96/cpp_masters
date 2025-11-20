#!/bin/bash
set -e

echo "--- CONFIGURING ---"
cmake -S . -B build

echo "--- BUILDING ---"
cmake --build build

echo "--- TESTING ---"
ctest --test-dir build
