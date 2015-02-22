cmake_minimum_required(VERSION 2.8)

project(SampleAtlas)

configure_file(${SampleAtlas_SOURCE_DIR}/SampleAtlas.config.in  ${SampleAtlas_BINARY_DIR}/SampleAtlas.config @ONLY)

# These are used to customize the SampleAtlas.mrml file
set(ATLAS_NAME SampleAtlas)
set(ATLAS_VOLUME ${SampleAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}Volume.nrrd)
set(ATLAS_VOLUME_NAME ${ATLAS_NAME}Volume)
set(ATLAS_COLOR_TABLE ${SampleAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}Colors.ctbl)
set(ATLAS_COLOR_TABLE_NAME ${ATLAS_NAME}Colors)
set(ATLAS_LABELS ${SampleAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}Labels.nrrd)
set(ATLAS_LABELS_NAME ${ATLAS_NAME}Labels)
set(MRML_FILENAME ${SampleAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}.mrml.in)

configure_file(${SampleAtlas_SOURCE_DIR}/Atlas/SampleAtlas.mrml.in  ${SampleAtlas_BINARY_DIR}/SampleAtlas.mrml @ONLY)
