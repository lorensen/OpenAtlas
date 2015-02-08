OpenAtlas
=========

The OpenAtlas Project maintains Open Data anatomy atlases and tools to modify, maintain and test the atlases.

## Building the tools
The OpenAtlas Project uses cmake to configure the tools. We recommend using the cmake Superbuild to build vtk, itk and the openatlas tools.

1. Clone this repository.

   git clone https://github.com/lorensen/OpenAtlas.git  

2. Configure and build with cmake

   mkdir OpenAtlas-Superbuild  
   cd OpenAtlas-Superbuild  
   cmake ../OpenAltas/Superbuild  
   make  

3. The tools will be located in:

   OpenAtlas-Superbuild/OpenAtlas-build  
   
## Configuring an Atlas
The Atlas configuration is defined with an atlas config file. Keywords are separated by colons.  
   *ColorTableFileName*: The file containing the color table.    
   *AdjacenciesFileName*: The file containing adjacencies. This file is generated.  
   *VolumeFileName*: The file containing the volume data associated with the atlas.  
   *LabelFileName*: The file containing the atlas labels.  
   *ModelsDirectory*: The directory holding the generated models.  
   *VTKDirectory*: The directory cojntaing the generated cuberille files.  
   *STLDirectory*: The directory containing the generated STL files.  
   *ScreenshotDirectory*: The directory containing the generated screenshots.  
   *ChangesDirectory*: The directory containing the generated diff screenshots.  
   *StatisticsDirectory*: The directory cojntaining the generated statistics.  
   *MRMLDirectory*: The directory containing the configured MRML file for the atlas.  
   *GitDirectory*: The directory containing the git clone of the atlas.  
   *FiducialsDirectory*: The directory containing the generated fiducial work lists.  
   
## Sample Atlas
To start, you may want to clone the SPLBrainAtlas. It has all of the required files and can serve as a guide to setup new atlases.

1. clone the atlas

   git clone https://github.com/lorensen/SPLBrainAtlas.git  

2. configure the atlas

   mkdir SPLBrainAtlas-build  
   cd SPLBrainAtlas-build  
   cmake ../SPLBrainAtlas  
3. The SPLBrainAtlas-build directory will contain a file SPLBrainAtlas.config
