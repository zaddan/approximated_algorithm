set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11 -g) #to add the -std=c++11 flag 
project(app_project)
file(GLOB all_src1 "src/CSrc/*.cpp") 
file(GLOB all_src2 "src/Operators/*.cpp")
file(GLOB all_src3  "src/BookKeeping/*.cpp") #includes all the files in the folder src/
add_executable(simple ${all_src1} ${all_src2} ${all_src3}) #which files to build
#install(TARGETS tool_exe DESTINATION bin) #where to place the binary
include_directories(inc/BookKeeping inc/OperatorHeaders inc/CSrc_inc)  #adding the include folder
