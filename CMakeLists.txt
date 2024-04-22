cmake_minimum_required(VERSION 3.27)
 
 
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CUDA_STANDARD 20)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CUDA_STANDARD 17)



set(CMAKE_CUDA_ARCHITECTURES 86)
set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
project(MCGS LANGUAGES CXX CUDA)

#project(MCGS LANGUAGES CXX CUDA)


# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)


add_library(src ${SRC})
message(STATUS ${SRC})
target_include_directories(src PUBLIC ${PROJECT_SOURCE_DIR}/include)


# include_directories(${PROJECT_SOURCE_DIR}/origin_cuda)
# file(GLOB_RECURSE ORIGIN_CUDA ${PROJECT_SOURCE_DIR}/origin_cuda/*.cpp ${PROJECT_SOURCE_DIR}/origin_cuda/*.cu)
# add_library(origin_cuda ${ORIGIN_CUDA})

 



# file(GLOB_RECURSE TESTSRC ${PROJECT_SOURCE_DIR}/src/ply_loader.cpp)
# add_library(testsrc ${TESTSRC})
# set(Torch_DIR "/home/mocheng/software/libtorch-cxx11-abi-shared-with-deps-2.0.1+cu118/libtorch/share/cmake/Torch")
# find_package(Torch REQUIRED)
# target_include_directories(testsrc PUBLIC ${TORCH_INCLUDE_DIRS})
# add_executable(ORIGIN_TEST main.cpp)
# target_link_libraries(ORIGIN_TEST src origin_cuda torch_cuda  )
# target_include_directories(ORIGIN_TEST PUBLIC ${PROJECT_SOURCE_DIR}/include)
# target_include_directories(ORIGIN_TEST PUBLIC ${TORCH_INCLUDE_DIRS})


# link MCRT


file(GLOB_RECURSE MCRT_SRC ${PROJECT_SOURCE_DIR}/MCRT/src/*.cpp )
add_library(mcrt_src ${MCRT_SRC})
# std::foreach using tbb
find_package(TBB REQUIRED COMPONENTS tbb)
set (MCRT_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/MCRT ${PROJECT_SOURCE_DIR}/MCRT/example/base ${PROJECT_SOURCE_DIR}/MCRT/include ${PROJECT_SOURCE_DIR}/MCRT/include/Tool)
target_include_directories(mcrt_src PUBLIC ${MCRT_INCLUDE_DIRS})

include(MCRT/cmake/FindVulkan.cmake)
include(MCRT/cmake/FindGLFW.cmake)
include(MCRT/cmake/FindIMGUI.cmake)

 include_directories(${PROJECT_SOURCE_DIR}/include)
 link_libraries(src)
# IF (WIN32)
#     target_link_libraries(imgui glfw Vulkan)
# ELSEIF (UNIX)
#     # in linux link Vulkan::Vulkan
#     target_link_libraries(imgui glfw Vulkan::Vulkan)
# ENDIF ()

   target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)
   target_include_directories(imgui PUBLIC ${MCRT_INCLUDE_DIRS})

   find_package(TBB REQUIRED COMPONENTS tbb)
   target_link_libraries(mcrt_src PUBLIC imgui tbb)


   add_subdirectory(${PROJECT_SOURCE_DIR}/App)






