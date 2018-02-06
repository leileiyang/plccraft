#!/bin/bash

# This a gcc build script.

g++ $1.cpp -g -I../../ -o $1 -lzmq -lczmq -lpthread -lboost_serialization
