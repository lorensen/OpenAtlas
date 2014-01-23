cmake_minimum_required(VERSION 2.8)
 
PROJECT(OpenAtlas)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
 
add_library(OpenAtlasUtilities OpenAtlasUtilities.cxx)

include_directories(${OpenAtlas_SOURCE_DIR})

add_executable(GenerateModelsFromLabels MACOSX_BUNDLE GenerateModelsFromLabels)
target_link_libraries(GenerateModelsFromLabels
          OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(GenerateCubesFromLabels MACOSX_BUNDLE GenerateCubesFromLabels)
target_link_libraries(GenerateCubesFromLabels
        OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(GenerateAdjacencies MACOSX_BUNDLE GenerateAdjacencies)
target_link_libraries(GenerateAdjacencies ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(DisplayAdjacentModels MACOSX_BUNDLE DisplayAdjacentModels)
target_link_libraries(DisplayAdjacentModels
        OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})
