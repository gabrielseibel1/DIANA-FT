cmake_minimum_required(VERSION 3.9)
project(DIANA-FT)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

add_executable(datagen

        datagen/datagen.cpp
)

add_executable(simple_diana

        diana/simple_diana.cpp
        diana/Dendrogram.cpp
        diana/Dendrogram.h
        diana/DianaClustering.cpp
        diana/DianaClustering.h
        diana/DataReader.cpp
        diana/DataReader.h
        kmeans/kmeans.h
        kmeans/kmeans_clustering.h
        kmeans/kmeans_clustering.c
)

add_executable(dwc_diana

        dwc/dwc_diana.cpp
        dwc/DianaDuplicator.cpp
        dwc/DianaDuplicator.h
        dwc/DendrogramComparator.cpp
        dwc/DendrogramComparator.h
        diana/DataReader.cpp
        diana/DataReader.h
        diana/Dendrogram.cpp
        diana/Dendrogram.h
        diana/DianaClustering.cpp
        diana/DianaClustering.h
        kmeans/kmeans.h
        kmeans/kmeans_clustering.h
        kmeans/kmeans_clustering.c
)

add_executable(sh_diana

        selective/sh_diana.cpp
        selective/DendrogramHardened.cpp
        selective/DendrogramHardened.h
        selective/DianaClusteringHardened.cpp
        selective/DianaClusteringHardened.h
        selective/DataReaderHardened.cpp
        selective/DataReaderHardened.h
        kmeans/kmeans.h
        kmeans/kmeans_clustering.h
        kmeans/kmeans_clustering.c
        selective/HardeningUtils.cpp
        selective/HardeningUtils.h
)

add_executable(radiation_diana

        radiation/radiation_diana.cpp
        diana/Dendrogram.cpp
        diana/Dendrogram.h
        diana/DianaClustering.cpp
        diana/DianaClustering.h
        kmeans/kmeans.h
        kmeans/kmeans_clustering.h
        kmeans/kmeans_clustering.c
        radiation/log_helper.h
        radiation/log_helper.c
)
