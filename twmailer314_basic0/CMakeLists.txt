cmake_minimum_required(VERSION 3.24)
project(twmailer314_basic)

set(CMAKE_CXX_STANDARD 23)


include_directories(/usr/include/readline)


add_executable(twmailer314_basic client314/main.cpp client314/ClientLogic.cpp)
#add_executable(twmailer314_basic client314/main.c server314/main.c server314/ServerLogic.h server314/ServerLogic.c)


target_link_libraries(twmailer314_basic readline)
