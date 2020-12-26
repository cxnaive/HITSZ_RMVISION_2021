CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(HITSZ-RM-CV)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")


#Include
include_directories(${PROJECT_SOURCE_DIR}/energy/include)
include_directories(${PROJECT_SOURCE_DIR}/armor/include)
include_directories(${PROJECT_SOURCE_DIR}/net/include)
include_directories(${PROJECT_SOURCE_DIR}/others/include)
include_directories(${PROJECT_SOURCE_DIR}/config/include)
include_directories(${PROJECT_SOURCE_DIR}/com/include)
find_package(OpenCV 4 REQUIRED)
# find_package(glog REQUIRED)
option(WITH_TINY_TENSORRT "use tensorrt for dnn" ON)
#Src
file(GLOB_RECURSE sourcefiles 
"others/**/*.h" "others/**/*.cpp"
"energy/**/*.h" "energy/**/*.cpp" 
"net/**/*.cpp" "net/**/*.h" 
"config/**/*.cpp" "config/**/*.h" 
"com/**/*.h"  "com/**/*.cpp" "com/**/*.cc"
"armor/**/*.h" "armor/**/*.cpp"
)

file(GLOB_RECURSE camfiles "others/**/*.h" "others/**/*.cpp" "config/src/rmconfig.cpp" "config/include/rmconfig.h")

add_executable(rmcv main.cpp ${sourcefiles})
add_executable(calibrate calibrate_get_imgs.cpp ${camfiles})
link_directories(${OpenCV_LIBRARY_DIRS})

target_link_libraries(rmcv ${OpenCV_LIBS} -lglog -lgxiapi -lpthread)
target_link_libraries(calibrate  ${OpenCV_LIBS} -lglog -lgxiapi  -lpthread)

if(WITH_TINY_TENSORRT)
add_definitions("-DWITH_TINY_TENSORRT")
target_link_libraries(rmcv libtinytrt.so nvinfer nvinfer_plugin nvparsers nvonnxparser)
target_link_libraries(calibrate libtinytrt.so nvinfer nvinfer_plugin nvparsers nvonnxparser)
else()
target_link_libraries(rmcv libMNN.so)
target_link_libraries(rmcv libMNN.so)
endif()