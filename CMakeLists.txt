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
find_package(Threads)
find_package(glog REQUIRED)

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

target_link_libraries(rmcv ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS} glog::glog libgxiapi.so libMNN.so)
target_link_libraries(calibrate ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS} glog::glog libgxiapi.so libMNN.so)


