# cmake版本
cmake_minimum_required(VERSION 3.10.2)
#替换，相当于宏定义
set(PROJECT_NAME opencv)
#项目名称，非必选
project(${PROJECT_NAME})

set(OpenCV_DIR D:\\Environment\\OpenCV-android-sdk\\sdk\\native\\jni)
find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
        include_directories(${OpenCV_INCLUDE_DIRS})

        message(STATUS "OpenCV library status:")
        message(STATUS "    version: ${OpenCV_VERSION}")
        message(STATUS "    libraries: ${OpenCV_LIBS}")
        message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
else(OpenCV_FOUND)
        message(FATAL_ERROR "OpenCV library not found")
endif(OpenCV_FOUND)


add_library(
#        库的名称
        ${PROJECT_NAME}
        # 将库设置为.so动态连接库
        SHARED
        # 添加动态库里包含的所有cpp文件，包括引用的lib里的cpp
        OpenCVHelper.cpp
)

find_library(
#        库位置对应的名称，给其他地方使用
        log-lib
#        库生成的中间文件的位置
        log)

target_link_libraries(
        ${PROJECT_NAME}
        jnigraphics
        ${OpenCV_LIBRARIES}
        ${log-lib})
