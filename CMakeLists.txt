cmake_minimum_required(VERSION 3.11)

project(RemoteLamps LANGUAGES C CXX)

add_library(Utility Commands.h Commands.cpp)

add_executable(Lamp Lamp.cpp)
target_link_libraries(Lamp Utility)

add_executable(LampServer LampServer.cpp)
target_link_libraries(LampServer Utility pthread)

