#message($ENV{HADOOP_HOME})
if (NOT DEFINED ENV{HADOOP_HOME})
    # 没有定义HADOOP_HOME环境变量
    message(FATAL_ERROR "not define environment variable::HADOOP_HOME")
endif ()

include_directories($ENV{HADOOP_HOME}/include)
link_directories($ENV{HADOOP_HOME}/lib/native)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FILE(GLOB_RECURSE SOURCE_FILES main.cc "./*.cc")

add_executable(HLZO ${SOURCE_FILES})

target_link_libraries(HLZO libglog.a)
target_link_libraries(HLZO libhdfs.a)
target_link_libraries(HLZO liblzo2.a)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)