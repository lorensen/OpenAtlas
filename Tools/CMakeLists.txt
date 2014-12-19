cmake_minimum_required(VERSION 2.8)
 
PROJECT(OpenAtlas)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
 
set(VTK_MODULES_REQUESTED "")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
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

add_executable(GenerateStatisticsFromLabels MACOSX_BUNDLE GenerateStatisticsFromLabels)
target_link_libraries(GenerateStatisticsFromLabels
        OpenAtlasUtilities ${ITK_LIBRARIES})

add_executable(DisplayAdjacentModels MACOSX_BUNDLE DisplayAdjacentModels)
target_link_libraries(DisplayAdjacentModels
        OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(LabelDiff MACOSX_BUNDLE LabelDiff)
target_link_libraries(LabelDiff
        OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

Add_executable(LabelsChanged MACOSX_BUNDLE LabelsChanged)
target_link_libraries(LabelsChanged
        OpenAtlasUtilities ${ITK_LIBRARIES})

Add_executable(ReportDisconnectedRegions MACOSX_BUNDLE ReportDisconnectedRegions)
target_link_libraries(ReportDisconnectedRegions
        ${VTK_LIBRARIES})

enable_testing()
include(CTest)

configure_file(${OpenAtlas_SOURCE_DIR}/GenerateAll.sh.in ${OpenAtlas_BINARY_DIR}/GenerateAll.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/DisplayAllAdjacentModels.sh.in ${OpenAtlas_BINARY_DIR}/DisplayAllAdjacentModels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/GenerateCubesFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateCubesFromLabels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/GenerateModelsFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateModelsFromLabels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/DisplayAdjacentModels.sh.in ${OpenAtlas_BINARY_DIR}/DisplayAdjacentModels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/GenerateAdjacencies.sh.in ${OpenAtlas_BINARY_DIR}/GenerateAdjacencies.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/GenerateStatisticsFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateStatisticsFromLabels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/GenerateDiffs.sh.in ${OpenAtlas_BINARY_DIR}/GenerateDiffs.sh)
