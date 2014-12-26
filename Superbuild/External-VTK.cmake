#-----------------------------------------------------------------------------
# Get and build VTK
#
#-----------------------------------------------------------------------------
# December 26, 2014
set( VTK_TAG "5ec3a52453e0cee64e83053e796d586fa9019cb4" )

ExternalProject_Add(VTK
  GIT_REPOSITORY "${git_protocol}://vtk.org/VTK.git"
  GIT_TAG "${VTK_TAG}"
  SOURCE_DIR VTK
  BINARY_DIR VTK-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=TRUE
    -DVTK_DEBUG_LEAKS:BOOL=FALSE
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DVTK_BUILD_ALL_MODULES_FOR_TESTS:BOOL=OFF
    -DVTK_Group_Rendering:BOOL=OFF
    -DVTK_Group_StandAlone:BOOL=OFF
    -DModule_vtkCommonCore:BOOL=ON
    -DModule_vtkFiltersGeneral:BOOL=ON
    -DModule_vtkFiltersGeneric:BOOL=ON
    -DModule_vtkIOGeometry:BOOL=ON
    -DModule_vtkIOImage:BOOL=ON
    -DModule_vtkIOLegacy:BOOL=ON
    -DModule_vtkImagingStatistics:BOOL=ON
    -DModule_vtkInteractionWidgets:BOOL=ON
    -DModule_vtkRenderingOpenGL:BOOL=ON

  INSTALL_COMMAND ""
)

set(VTK_DIR ${CMAKE_BINARY_DIR}/VTK-build)
