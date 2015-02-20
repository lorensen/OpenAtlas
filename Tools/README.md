# The OpenAtlas Tool Set
The OpenAtlas tool set consists of bash shell scripts, C++ programs
and a library of utility routines and classes. The shell scripts
typically run one or more of the compiled programs. Most of the
scripts require at leas one command line argument, the Atlas
Configuration file. See blah for a description of the config file.

First we describe the shell scripts and what they accomplish. Then we
describe each of the programs with some detail about their inner
workings. In the following, items in __bold__ refer to atlas
attributes defined in the atlas configuration file.

## OpenAtlas Shell Scripts
* GenerateAdjacencies.sh _AtlasConfigFile_  
  Generates a file (__AdjacenciesFileName__) that lists each label and
  the labels that are adjacent to that label. The file contains one
  line per label:  
    _label_ _#ofAdjecentLabels_ _adjacentLabel1_ _adjacentLabel2_ ... _adjacentLabelN_
* GenerateStatisticsFromLabels.sh _AtlasConfigFile_  
  For each label in the __ColorTableFile__, creates a file in
  __StatisticsDirectory__ with the name _AnatomyName-Label.txt_
* GenerateModelsFromLabels.sh _AtlasConfigFile_  
  For each label in the range _Start_,_End_, creates a STL
  (STereoLithography) file in __STLDirectory__ with the name
  _AnatomyName-Label.stl_.  
* GenerateCubesFromLabels.sh _AtlasConfigFile_  
  For each label in the range _Start_,_End_, creates a VTK polydata
  file in __VTKDirectory__ with the name _AnatomyName-Label.vtk_.
* DisplayAllAdjacentModels.sh _AtlasConfigFile_  
  Creates screenshots of a label and its adjacent label. Two views are
  generated side-by-side: one view of the STL models and on view of
  the Cube models. The screenshots are stored in
  __ScreenshotDirectory__ with files named _AnatomyName.png_.
* GenerateMrmlScene.sh _AtlasConfigFile_ _[VTK/STL]_  
  Creates a Slicer MRML scene that contains _ModelStorage_,
  _ModelDisplay_ and _Model_ mrml nodes. The scenes are stored in
  __MRMLDirectory__ with the name __AtlasName__VTK.mrml or
  __AtlasName__STL.mrml.
* ReportAllDisconnectedRegions _AtlasConfigFile_  
  Reports to standard output atlas parts that have disconnnected
  regions in the cube geometry files stored in __VTKDirectory__. For
  each atlas part, creates files in __FiducialsDirectory__ named
  _AnatomyName-Label.mrml_ and _AnatomyName-Label.csv_.
* GenerateMRB.sh _AtlasConfigFile_  
  Creates a Slicer mrb (medical reality bundle file) that can be used
  locally to look at the volume, label volume and fiducial lists.
* GeneratePortableMRB.sh _AtlasConfigFile_  
  Creates a Slicer mrb (medical reality bundle file) that can be
  shared and used on other machines to look at the volume, label
  volume and fiducial lists. In contrast to GenerateMRB.sh, this mrb
  will contain the mrml scenes and all of the required data (volumes,
  labels, color tables, cube files and fiducials).
* GenerateDiffsToHead.sh
  Creates difference images for labels that have changed since the
  last git commit in __GitDirectory__. Compares the current
  __LabelFileName__ and the last committed (HEAD)
  __LableFileName__. The difference images are stored in
  __ChangesDirectory__ with names _AnatomyName-Label_diff.png_.
* GenerateDiffsBetweenLastTwo.sh
  Creates difference images for the last two git revisions of
  __LabelFileName__ in __GitDirectory__.  The difference images are
  stored in __ChangesDirectory__ with names
  _AnatomyName-Label_diff.png_. Also creates a file
  _Changes-as-of-DATE.md that can be saved in __GitWikiDirectory__.

## OpenAtlas Programs
* GenerateAdjacencies _AtlasConfigFile_  
* GenerateStatisticsFromLabels _AtlasConfigFile_  
* GenerateModelsFromLabels _AtlasConfigFile_ _Start_ _End_  
* GenerateCubesFromLabels _AtlasConfigFile_ _Start_ _End_  
* GetAtlasAttribute _AtlasConfigFile_ _AtlasAttribute_  
* GenerateMrmlScene _AtlasConfigFile_ [_VTK or STL_]
* LabelDiff _label_ OldLabelVolume NewLabelVolume ColorsFile ChangesDirectory  
* LabelsChanged OldLabelVolume NewLabelVolume  
  Reports on standard output any labels that have changed between the two provided label volumes.
* ReportDisconnectedRegions _VTKCubeFile_ _FiducialsDirectory_  

## OpenAtlasUtilities Library  
This library contains a variety of utility routines that the C++ program use.  
* ReadLabelFile(std::string, std::vector<std::string> &)  
* ReadColorFile(std::string, std::vector<std::vector<float> >&)  
* ReadAdjacenyFile(std::string, std::vector<std::set<unsigned int> > &)  
* OpenAtlas::Configiuration  
