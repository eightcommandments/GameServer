#最低cmake版本
cmake_minimum_required (VERSION 2.8)

#项目名
project (Kaiye)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#添加头文件搜索路径
include_directories(/usr/local)
include_directories(../src)

#源文件头
include_directories(../src)

#用于将当前目录下的所有源文件的名字保存在变量 DIR_SRCS 中
aux_source_directory(../src DIR_SRC)

add_executable(Main ${DIR_SRC})
set(EXECUTABLE_OUTPUT_PATH ../bin)