set(ITK_IO_MODULES_USED "ITKIONRRD;ITKIOPNG;ITKIONIFTI")
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
  vtkRenderingOpenGL2
)
include(${VTK_USE_FILE})
 
message(STATUS "OpenAtlas: ITK VERSION: ${ITK_VERSION}")
message(STATUS "OpenAtlas: VTK VERSION: ${VTK_VERSION}")

add_library(OpenAtlasUtilities OpenAtlasUtilities.cxx)

include_directories(${OpenAtlas_SOURCE_DIR}/Tools)

add_executable(TestConfigure MACOSX_BUNDLE TestConfigure)
target_link_libraries(TestConfigure
          OpenAtlasUtilities ${ITK_LIBRARIES})

add_executable(GetAtlasAttribute MACOSX_BUNDLE GetAtlasAttribute)
target_link_libraries(GetAtlasAttribute
          OpenAtlasUtilities ${ITK_LIBRARIES})

add_executable(GenerateModelsFromLabels MACOSX_BUNDLE GenerateModelsFromLabels)
target_link_libraries(GenerateModelsFromLabels
          OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(GenerateCubesFromLabels MACOSX_BUNDLE GenerateCubesFromLabels)
target_link_libraries(GenerateCubesFromLabels
        OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(GenerateAdjacencies MACOSX_BUNDLE GenerateAdjacencies)
target_link_libraries(GenerateAdjacencies OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(GenerateStatisticsFromLabels MACOSX_BUNDLE GenerateStatisticsFromLabels)
target_link_libraries(GenerateStatisticsFromLabels
        OpenAtlasUtilities ${ITK_LIBRARIES})

add_executable(DisplayAdjacentModels MACOSX_BUNDLE DisplayAdjacentModels)
target_link_libraries(DisplayAdjacentModels
        OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(LabelDiff MACOSX_BUNDLE LabelDiff)
target_link_libraries(LabelDiff
        OpenAtlasUtilities ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(LabelsChanged MACOSX_BUNDLE LabelsChanged)
target_link_libraries(LabelsChanged
        OpenAtlasUtilities ${ITK_LIBRARIES})

add_executable(ReportDisconnectedRegions MACOSX_BUNDLE ReportDisconnectedRegions)
target_link_libraries(ReportDisconnectedRegions
        ${VTK_LIBRARIES})

add_executable(GenerateMrmlScene MACOSX_BUNDLE GenerateMrmlScene)
target_link_libraries(GenerateMrmlScene
        OpenAtlasUtilities
        ${ITK_LIBRARIES})

add_executable(GenerateAtlasSummary MACOSX_BUNDLE GenerateAtlasSummary)
target_link_libraries(GenerateAtlasSummary
        OpenAtlasUtilities
        ${ITK_LIBRARIES})

add_executable(ReportLeftRightAdjacencies MACOSX_BUNDLE ReportLeftRightAdjacencies)
target_link_libraries(ReportLeftRightAdjacencies
        OpenAtlasUtilities
        ${ITK_LIBRARIES})

configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateAll.sh.in ${OpenAtlas_BINARY_DIR}/GenerateAll.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/DisplayAllAdjacentModels.sh.in ${OpenAtlas_BINARY_DIR}/DisplayAllAdjacentModels.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateCubesFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateCubesFromLabels.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateModelsFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateModelsFromLabels.sh  @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/DisplayAdjacentModels.sh.in ${OpenAtlas_BINARY_DIR}/DisplayAdjacentModels.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateAdjacencies.sh.in ${OpenAtlas_BINARY_DIR}/GenerateAdjacencies.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateStatisticsFromLabels.sh.in ${OpenAtlas_BINARY_DIR}/GenerateStatisticsFromLabels.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateDiffs.sh.in ${OpenAtlas_BINARY_DIR}/GenerateDiffs.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateDiffsToHead.sh.in ${OpenAtlas_BINARY_DIR}/GenerateDiffsToHead.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateDiffsBetweenLastTwo.sh.in ${OpenAtlas_BINARY_DIR}/GenerateDiffsBetweenLastTwo.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/ReportAllDisconnectedRegions.sh.in ${OpenAtlas_BINARY_DIR}/ReportAllDisconnectedRegions.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateMrmlScene.sh.in ${OpenAtlas_BINARY_DIR}/GenerateMrmlScene.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GenerateMRB.sh.in ${OpenAtlas_BINARY_DIR}/GenerateMRB.sh @ONLY)
configure_file(${OpenAtlas_SOURCE_DIR}/Tools/GeneratePortableMRB.sh.in ${OpenAtlas_BINARY_DIR}/GeneratePortableMRB.sh @ONLY)


# For each atlas:
# 1) generate a disconnected regions test
# 2) generate a Slicer mrml scene containing the volumes and labels

file(GLOB ATLASES ${OpenAtlas_SOURCE_DIR}/Atlases/*Atlas)
file(GLOB_RECURSE MRML_SCENES ${OpenAtlas_SOURCE_DIR}/Atlases/*.mrml.in)

foreach(ATLAS_DIR ${ATLASES})
  string(REPLACE ${OpenAtlas_SOURCE_DIR}/Atlases/ "" ATLAS_NAME ${ATLAS_DIR})
  message(STATUS "OpenAtlas - Configuring ${ATLAS_NAME}")
  add_test(NAME ReportAllDisconnectedRegions-${ATLAS_NAME}
           COMMAND ${OpenAtlas_BINARY_DIR}/ReportAllDisconnectedRegions.sh ${ATLAS_NAME})
  set(ATLAS_VOLUME ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}Volume.nrrd)
  set(ATLAS_VOLUME_NAME ${ATLAS_NAME}Volume)
  set(ATLAS_COLOR_TABLE ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}Colors.ctbl)
  set(ATLAS_COLOR_TABLE_NAME ${ATLAS_NAME}Colors)
  set(ATLAS_LABELS ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}Labels.nrrd)
  set(ATLAS_LABELS_NAME ${ATLAS_NAME}Labels)
  set(MRML_FILENAME ${OpenAtlas_SOURCE_DIR}/Atlases/${ATLAS_NAME}/Atlas/${ATLAS_NAME}.mrml.in)
  get_filename_component(SCENE_FILE_IN ${MRML_FILENAME} NAME)
  string(REPLACE ".in" "" SCENE_FILE ${SCENE_FILE_IN})
  if (EXISTS ${MRML_FILENAME})
    configure_file(${MRML_FILENAME} ${OpenAtlas_BINARY_DIR}/${SCENE_FILE} @ONLY)
  endif()
endforeach()
