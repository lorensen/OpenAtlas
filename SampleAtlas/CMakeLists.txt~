cmake_minimum_required(VERSION 2.8)
 
project(SPLBrainAtlas)

configure_file(${SPLBrainAtlas_SOURCE_DIR}/SPLBrainAtlas.config.in  ${SPLBrainAtlas_BINARY_DIR}/SPLBrainAtlas.config @ONLY)

set(ATLAS_NAME SPLBrainAtlas)
set(ATLAS_VOLUME ${SPLBrainAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}Volume.nrrd)
set(ATLAS_VOLUME_NAME ${ATLAS_NAME}Volume)
set(ATLAS_COLOR_TABLE ${SPLBrainAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}Colors.ctbl)
set(ATLAS_COLOR_TABLE_NAME ${ATLAS_NAME}Colors)
set(ATLAS_LABELS ${SPLBrainAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}Labels.nrrd)
set(ATLAS_LABELS_NAME ${ATLAS_NAME}Labels)
set(MRML_FILENAME ${SPLBrainAtlas_SOURCE_DIR}/Atlas/${ATLAS_NAME}.mrml.in)

configure_file(${SPLBrainAtlas_SOURCE_DIR}/Atlas/SPLBrainAtlas.mrml.in  ${SPLBrainAtlas_BINARY_DIR}/SPLBrainAtlas.mrml @ONLY)
