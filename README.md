OpenAtlas
=========

The Open Atlas Project maintains Open Data anatomy atlases and tools to modify, maintain and test the atlases.

## Background
In December 2005, [DARPA](http://www.darpa.mil/default.aspx) sponsored a Virtual Face workshop. The purpose of the workshop was to discuss approaches to create a DARPA program that would address facial trauma in soldiers returning from Iraq and Afghanistan. Rick Satava, DARPA Program Officer, brought together clinicians, bio-medical researchers and computer scientists to brainstorm on the requirements of such a project.

At that time, GE Research was involved in another DARPA Program called [Virtual Soldier](http://www.virtualsoldier.us/). GE's role was to support anatomy modelling and visualization for the project. Brigham and Women's [Surgical Planning Laboratory](http://www.spl.harvard.edu/) was a GE subcontractor. GE and BWH used the Visible Human segmented Thorax data as a base for modeling the chest of military combatants. A number of software tools were created to process this data.

After the Virtual Face Meeting, Bill Lorensen and Tim Kelliher, both from GE Research, brainstormed about creating a face atlas for the project. The meeting took place at the Rock Bottom Brewery in Arlington, VA. No facial atlas existed, but they came up with the notion of using the world community to create and maintain such an atlas. They would use an "Extreme Testing" approach that [GE Research pioneered for VTK and ITK](http://marchingcubes.org/images/9/90/VTKExtremeTesting.pdf). The approach would adapt the nightly build/test dashboard techniques that GE had developed.

_Unfortunately, the Virtual Face Program was never funded by DARPA._

In January 2007, Terry Yoo (the father of [ITK](http://itk.org)) at the [National Library of Medicine](http://www.nlm.nih.gov/), held a workshop on future directions for NLM software initiatives. Lorensen presented a talk called [Community Driven Data Annotation/Curation](http://marchingcubes.org/images/f/f7/CommunityDataAnnotation.pdf)  This talk synthesized many of the Kelliher/Lorensen notions.

_Unfortunately, the program was never funded by NLM._

In 2007, Lorensen retired from GE Research and became an unpaid intern in BillsBasement at _Noware_. Fighting the winter blues, he revived, modified and extended many of the [original Virtual Soldier atlas processing tools](https://gitorious.org/virtualsoldier). He applied these tools to the Brigham and Womens [SPL Brain Atlas](http://www.spl.harvard.edu/publications/item/view/1265). _Noware_ graciously provided funding and the Open Atlas project was born. Additional funding provied by the _Social Security Adminstration_ and the _GE Pension Fund_.

__At last, the Open Atlas Program is funded by Noware!__

Some additional progress was made at the 2015 [NA-MIC](na-mic.org) [Project Week](http://www.na-mic.org/Wiki/index.php/2015_Winter_Project_Week:OpenAtlas).

## Building the tools
The OpenAtlas Project uses [cmake](http://cmake.org) to configure the tools. We recommend using the cmake Superbuild to build [vtk](vtk.org), [itk](itk.org) and the openatlas tools

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
   *VolumeFileName*: The file containing the volume data associated with the atlas.  
   *LabelFileName*: The file containing the atlas labels.  
   *ColorTableFileName*: The file containing the color table.    
   *AdjacenciesFileName*: The file containing adjacencies. This file is generated.  
   *ModelsDirectory*: The directory holding the generated models.  
   *VTKDirectory*: The directory contaning the generated cuberille files.  
   *STLDirectory*: The directory containing the generated STL files.  
   *ScreenshotDirectory*: The directory containing the generated screenshots.  
   *ChangesDirectory*: The directory containing the generated diff screenshots.  
   *StatisticsDirectory*: The directory cojntaining the generated statistics.  
   *MRMLDirectory*: The directory containing the configured MRML file for the atlas.  
   *GitDirectory*: The directory containing the git clone of the atlas.  
   *FiducialsDirectory*: The directory containing the generated fiducial work lists.  
   
## Sample Atlas
To start, you may want to clone the [SPL Brain Atlas](http://www.spl.harvard.edu/publications/item/view/1265). It has all of the required files and can serve as a guide to setup new atlases.

1. clone the atlas

   git clone https://github.com/lorensen/SPLBrainAtlas.git  

2. configure the atlas
   
   mkdir SPLBrainAtlas-build  
   cd SPLBrainAtlas-build  
   cmake ../SPLBrainAtlas  
3. The SPLBrainAtlas-build directory will contain a file SPLBrainAtlas.config
4. Now, go to your OpenAtlas-Superbuild/OpenAtlas-build directory

   ./GenerateAll.sh "your SPLBrainAtlas-build"/SPLBrainAtlas.config
5. If you have [Slicer](http://slicer.org/), you can see the atlas volume, atlas labels, cube models and fiducial work lists by running:

   Slicer SPLBrainAtlas/SPLBrainAtlas.mrb

## Other atlases
The following atlases can also be configured for Open Atlas tools.

1. [SPL Abdominal Atlas](https://github.com/lorensen/SPLAbdominalAtlas)
2. [SPL Head and Neck Atlas](https://github.com/lorensen/SPLHeadNeckAtlas)
3. [SPL Knee Atlas](https://github.com/lorensen/SPLKneeAtlas)
4. [ICBM Brain Template](https://github.com/lorensen/ICBMBrainAtlas)
5. [FreeSurfer Sample Atlas](https://github.com/lorensen/FreeSurferSampleAtlas)
