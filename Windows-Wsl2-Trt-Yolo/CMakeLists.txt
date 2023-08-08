cmake_minimum_required(VERSION 3.15)

project(TRTSegServer LANGUAGES CUDA CXX)

set(CMAKE_CXX_STANDARD 11)

#cmake在编译过程中打印编印信息
set(CMAKE_VERBOSE_MAKEFILEON ON) 
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

find_package(OpenCV)
enable_language(CUDA)
set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
set(CUDA_HOME /usr/local/cuda)
set(TRT_HOME /home/ylr/tensorrt/TensorRT-8.5.1.7)

# 设置动态链接库路径
set(CUDA_LIB_DIR ${CUDA_HOME}/lib64)
set(TRT_LIB_DIR ${TRT_HOME}/lib)
set(LD_CUDA_LIBS cuda cudart)
set(LD_TRT_LIBS nvinfer nvinfer_plugin nvonnxparser nvparsers)

# 设置输出bin文件路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations")
# cuda 和 cudnn 头文件
include_directories(${CUDA_HOME}/include)
include_directories(${CUDA_HOME}/targets/x86_64-linux/include)

# TensorRT 头文件
include_directories(${TRT_HOME}/include)
include_directories(${TRT_HOME}/sample)

# OpenCV 头文件
include_directories(${OpenCV_INCLUDE_DIRS})

# src 链接库
add_subdirectory(${CMAKE_SOURCE_DIR}/src/TrtLib/common)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/TrtLib/builder)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/TrtLib/infer)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/app_yolo)

set(EXTRA_LIBS ${EXTRA_LIBS} yolo TrtInfer TrtBuilder common)

link_directories(${CUDA_LIB_DIR} ${TRT_LIB_DIR}) 

# add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cpp)
# 链接动态链接库
# target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS})

add_executable(test_yolov8_detect ${CMAKE_SOURCE_DIR}/test/test_yolo_detect.cpp)
# 链接动态链接库
target_link_libraries(test_yolov8_detect  ${LD_TRT_LIBS} ${LD_CUDA_LIBS} ${OpenCV_LIBS} ${EXTRA_LIBS})

