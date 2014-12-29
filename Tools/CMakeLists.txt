set(ITK_IO_MODULES_USED "ITKIONRRD;ITKIOPNG")
find_package(ITK COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKImageGrid
  ITKImageStatistics
  ITKVtkGlue
  ${ITK_IO_MODULES_USED}
)
include(${ITK_USE_FILE})
 
find_package(VTK COMPONENTS
  vtkFiltersGeneral
  vtkFiltersGeometry
  vtkIOGeometry
  vtkIOImage
  vtkIOLegacy
  vtkImagingStatistics
  vtkInteractionWidgets
  vtkRenderingOpenGL
)
include(${VTK_USE_FILE})
 
message(STATUS "OpenAtlas: ITK VERSION: ${ITK_VERSION}")
message(STATUS "OpenAtlas: VTK VERSION: ${VTK_VERSION}")

add_library(OpenAtlasUtilities OpenAtlasUtilities.cxx)

include_directories(${OpenAtlas_SOURCE_DIR}/Tools)

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

Add_executable(GenerateMrmlScene MACOSX_BUNDLE GenerateMrmlScene)

configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateAll.sh.in ${OpenAtlas_BINARY_DIR}/GenerateAll.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/DisplayAllAdjacentModels.sh.in ${OpenAtlas_BINARY_DIR}/DisplayAllAdjacentModels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateCubesFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateCubesFromLabels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateModelsFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateModelsFromLabels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/DisplayAdjacentModels.sh.in ${OpenAtlas_BINARY_DIR}/DisplayAdjacentModels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateAdjacencies.sh.in ${OpenAtlas_BINARY_DIR}/GenerateAdjacencies.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateStatisticsFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateStatisticsFromLabels.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateDiffs.sh.in ${OpenAtlas_BINARY_DIR}/GenerateDiffs.sh)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/ReportAllDisconnectedRegions.sh.in ${OpenAtlas_BINARY_DIR}/ReportAllDisconnectedRegions.sh)

add_test(NAME ReportAllDisconnectedRegions
         COMMAND ${OpenAtlas_BINARY_DIR}/ReportAllDisconnectedRegions.sh SPLBrainAtlas)

file(GLOB_RECURSE MRML_SCENES ${OpenAtlas_SOURCE_DIR}/Atlases/*.mrml.in)
message(STATUS "MRML_SCENES ${MRML_SCENES}")
file(GLOB ATLASES ${OpenAtlas_SOURCE_DIR}/Atlases/*Atlas)
message(STATUS "ATLASES ${ATLASES}")
foreach(ATLAS_DIR ${ATLASES})
  string(REPLACE ${OpenAtlas_SOURCE_DIR}/Atlases/ "" ATLAS_NAME ${ATLASES})
  message(STATUS "OpenAtlas - Configuring ${ATLAS_NAME}")
  set(ATLAS_VOLUME ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}Volume.nrrd)
  set(ATLAS_VOLUME_NAME ${ATLAS_NAME}Volume.nrrd)
  set(ATLAS_COLOR_TABLE ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}Colors.ctbl)
  set(ATLAS_COLOR_TABLE_NAME ${ATLAS_NAME}Colors)
  set(ATLAS_LABELS ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}Labels.nrrd)
  set(ATLAS_LABELS_NAME ${ATLAS_NAME}Labels)
  set(MRML_FILENAME ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}.mrml.in)
  get_filename_component(SCENE_FILE_IN ${MRML_FILENAME} NAME)
  string(REPLACE ".in" "" SCENE_FILE ${SCENE_FILE_IN})
  configure_file(${MRML_FILENAME} ${OpenAtlas_BINARY_DIR}/${SCENE_FILE})
endforeach()
