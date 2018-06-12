cmake_minimum_required(VERSION 3.9)
project(DIANA)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(
        datagen
        datagen/datagen.cpp
)

add_executable(
        simple_diana
        simple_diana.cpp
        Dendrogram.cpp
        Dendrogram.h
        DianaClustering.cpp
        DianaClustering.h
        kmeans/kmeans.h
        kmeans/kmeans_clustering.h
        kmeans/kmeans_clustering.c
)

add_executable(
        radiation_diana
        radiation_diana.cpp
        Dendrogram.cpp
        Dendrogram.h
        DianaClustering.cpp
        DianaClustering.h
        kmeans/kmeans.h
        kmeans/kmeans_clustering.h
        kmeans/kmeans_clustering.c
)
