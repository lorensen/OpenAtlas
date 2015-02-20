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
* GenerateModelsFromLabels.sh _AtlasConfigFile_  
* GenerateCubesFromLabels.sh _AtlasConfigFile_  
* DisplayAllAdjacentModels.sh _AtlasConfigFile_  
* GenerateMrmlScene.sh _AtlasConfigFile_ _[VTK/STL]_  
* ReportAllDisconnectedRegions _AtlasConfigFile_  
* GenerateMRB.sh _AtlasConfigFile_  

## OpenAtlas Programs
* GenerateAdjacencies _AtlasConfigFile_  
* GenerateStatisticsFromLabels _AtlasConfigFile_  
  For each label in the __ColorTableFile__, creates a file in
  __StatisticsDirectory__ with the name _AnatomyName-Label.txt_
* GenerateModelsFromLabels _AtlasConfigFile_ _Start_ _End_  
  For each label in the range _Start_,_End_, creates a STL
  (STereoLithography) file in __STLDirectory__ with the name
  _AnatomyName-Label.stl_.  
* GenerateCubesFromLabels _AtlasConfigFile_ _Start_ _End_  
  For each label in the range _Start_,_End_, creates a VTK polydata
  file in __VTKDirectory__ with the name _AnatomyName-Label.vtk_.
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


