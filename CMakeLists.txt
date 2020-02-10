cmake_minimum_required(VERSION 3.8)
project(finalbigboss)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp TextFilter.h Libraries.h BloomFilter.h Spider.h Spider.cpp BloomFilter.cpp TextFilter.cpp splitString.cpp Saver.h Saver.cpp pages/news.ifeng.com.h)
add_executable(finalbigboss ${SOURCE_FILES})