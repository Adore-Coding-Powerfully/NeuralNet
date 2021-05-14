cmake_minimum_required(VERSION 3.19)
project(NeuralNet)

set(CMAKE_CXX_STANDARD 17)

add_executable(NeuralNet main.cpp NNet/NNet.h NNet/NNet.cpp utils.h)
