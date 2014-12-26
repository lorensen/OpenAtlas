#---------------------------------------------------------------------------
# Get and build itk

set( ITK_TAG "v4.7.0" )

ExternalProject_Add( ITK
  GIT_REPOSITORY "${git_protocol}://itk.org/ITK.git"
  GIT_TAG "${ITK_TAG}"
  SOURCE_DIR ITK
  BINARY_DIR ITK-build
  CMAKE_GENERATOR ${gen}
  DEPENDS VTK
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DITK_BUILD_DEFAULT_MODULES:BOOL=OFF
    -DITKGroup_Core:BOOL=OFF
    -DModule_ITKCommon:BOOL=ON
    -DModule_ITKIOImageBase:BOOL=ON
    -DModule_ITKImageGrid:BOOL=ON
    -DModule_ITKImageStatistics:BOOL=ON
    -DModule_ITKVtkGlue:BOOL=ON
    -DModule_ITKIONRRD:BOOL=ON
    -DModule_ITKIOPNG:BOOL=ON
    -DVTK_DIR:PATH=${VTK_DIR}
  INSTALL_COMMAND ""
)

set(ITK_DIR ${CMAKE_BINARY_DIR}/ITK-build)
