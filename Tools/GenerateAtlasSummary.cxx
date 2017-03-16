/*
  Generate an Atlas-Summary.md file for the wiki
*/

#include <OpenAtlasUtilities.h>
#include <itkMacro.h>
#include <itksys/SystemTools.hxx>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main (int argc, char *argv[])
{
  if (argc < 2)
    {
    std::cout << "Usage: " << argv[0] << " AtlasConfigFile" << std::endl;
    return EXIT_FAILURE;
    }

  // Read the config file
  OpenAtlas::Configuration config(argv[1]);
  
  // Make sure required attributes are define
  bool allPresent = true;
  std::string atlasName = config.AtlasName();
  if (atlasName == "")
    {
    std::cout << itksys::SystemTools::GetFilenameName(argv[0]) << ": AtlasName attribute is missing from the atlas config file " << argv[1] << std::endl;
    allPresent = false;
    }
  std::string colorTableFileName = config.ColorTableFileName();
  if (colorTableFileName == "")
    {
    std::cout << itksys::SystemTools::GetFilenameName(argv[0]) << ": ColorTableFileName attribute is missing from the atlas config file " << argv[1] << std::endl;
    allPresent = false;
    }
  std::string gitURL = config.GitURL();
  if (gitURL == "")
    {
    std::cout << itksys::SystemTools::GetFilenameName(argv[0]) << ": GitURL attribute is missing from the atlas config file " << argv[1] << std::endl;
    allPresent = false;
    }
  std::string gitWikiDirectory = config.GitWikiDirectory();
  if (gitWikiDirectory == "")
    {
    std::cout << itksys::SystemTools::GetFilenameName(argv[0]) << ": GitWikiDirectory attribute is missing from the atlas config file " << argv[1] << std::endl;
    allPresent = false;
    }
  std::string modelsDirectory = config.ModelsDirectory();
  if (modelsDirectory == "")
    {
    std::cout << itksys::SystemTools::GetFilenameName(argv[0]) << ": ModelsDirectory attribute is missing from the atlas config file " << argv[1] << std::endl;
    allPresent = false;
    }
  std::string screenShotDirectory = config.ScreenshotDirectory();
  if (screenShotDirectory == "")
    {
    std::cout << itksys::SystemTools::GetFilenameName(argv[0]) << ": ScreenShotDirectory attribute is missing from the atlas config file " << argv[1] << std::endl;
    allPresent = false;
    }
  if (!allPresent)
    {
    std::cout << itksys::SystemTools::GetFilenameName(argv[0]) << ": Atlas-Summary.md will not be created." << std::endl;
    return EXIT_FAILURE;
    }

  // Read the label file
  std::vector<std::string> labels;
  try
    {
    ReadLabelFile(config.ColorTableFileName(), labels);
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e;
    return EXIT_FAILURE;
    }

  // Open the atlas summary file
  std::stringstream atlasSummaryFileName;  
  atlasSummaryFileName << gitWikiDirectory << "/" << "Atlas-Summary.md";
  std::ofstream afout(atlasSummaryFileName.str().c_str());
  if (afout.fail())
    {
    std::cout << argv[0] << ": Cannot open " << atlasSummaryFileName.str() << std::endl;
    return EXIT_FAILURE;
    }

  // write intro
  afout << "# Atlas Summary for " << atlasName << std::endl;
  afout << "| Label | Name | STL file |" << std::endl;
  afout << "|-------:|------|------|" << std::endl;
  // write one line per label
  for (size_t i = 0; i < labels.size(); ++i)
    {
    if (labels[i] == "")
      {
      continue;
      }
    // Only create a line if the Screenshot and STL files exist
    std::stringstream screenShotFileName;
    screenShotFileName << screenShotDirectory << "/" << labels[i] << ".png";
    if (!itksys::SystemTools::FileExists(screenShotFileName.str()))
      {
      continue;
      }
    std::stringstream stlFileName;
    stlFileName << modelsDirectory << "/STL/" << labels[i] << "-" << i << ".stl";
    if (!itksys::SystemTools::FileExists(stlFileName.str()))
      {
      continue;
      }
    char shortLabel[54];
    std::size_t length = labels[i].copy(shortLabel,50,0);
    if (length < labels[i].size())
      {
      shortLabel[length++]='.';
      shortLabel[length++]='.';
      shortLabel[length++]='.';
      }
    shortLabel[length]='\0';
    afout << "|" << i << "|"
          << "[" << shortLabel << "]"
          << "(" << gitURL << "/raw/master/Models/Screenshot/" << labels[i] << ".png)"
          << "|[STL](" << gitURL << "/blob/master/Models/STL/" << labels[i] << "-" << i << ".stl) |"
          << std::endl;
    }
  afout.close();

  return EXIT_SUCCESS;
}
