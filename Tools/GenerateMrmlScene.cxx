/*
  Generate a Mrml Scene for the atlas
*/
#include <itksys/Directory.hxx>
#include <itksys/SystemTools.hxx>
#include <itksys/RegularExpression.hxx>
#include "OpenAtlasUtilities.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
int main (int argc, char *argv[])
{
  if (argc < 3)
    {
    std::cout << "Usage: " << argv[0] << "AtlasConfigFile Model(VTK or STL)" << std::endl;
    return EXIT_FAILURE;
    }

  OpenAtlas::Configuration config(argv[1]);

  std::vector<std::vector<float> > colors;
  // Read the color file
  ReadColorFile(config.ColorTableFileName(), colors);

  // Open mrml file and write header
  itksys::SystemTools::MakeDirectory(config.MRMLDirectory());

  std::string mrmlFile;
  mrmlFile = config.MRMLDirectory() + "/" + config.AtlasName() + argv[2] + ".mrml";
  std::ofstream mout(mrmlFile.c_str());
  if (!mout)
    {
    std::cerr << argv[0] << ": Could not open Mrml file " << mrmlFile << " for output" << std::endl;
    perror(argv[0]);
    return EXIT_FAILURE;
    }
  mout << "<MRML  version=\"Slicer4.4.0\" userTags=\"\">" << std::endl;
 
  // Get the names of each model in the directory
  itksys::Directory dir;
  std::string modelDir;
  modelDir = config.ModelsDirectory() + "/" + argv[2];
  std::cout << "Loading directory: " << modelDir << std::endl;

  dir.Load(modelDir.c_str());
  std::cout << "Number of files is: " <<   dir.GetNumberOfFiles() << std::endl;

  std::vector<std::string> sortedFileNames;
  for (unsigned long i = 0; i < dir.GetNumberOfFiles(); ++i)
    {
    const char *fullFileName = dir.GetFile(i);
    std::string fullPathName = modelDir + "/" + fullFileName;
    if (itksys::SystemTools::FileIsDirectory(fullPathName))
      {
      continue;
      }
    std::string s = fullFileName;
    sortedFileNames.push_back(s);
    }

  // Sort the model names
  std::sort(sortedFileNames.begin(), sortedFileNames.end());

  // Create ModelStorage, ModelDisplay, and Model nodes
  for (unsigned long i = 0; i < sortedFileNames.size(); ++i)
    {
     const char* fullFileName = sortedFileNames[i].c_str();
     std::cout << fullFileName << std::endl;
    // Extract label number from filename
     std::string fileName = itksys::SystemTools::GetFilenameWithoutExtension(sortedFileNames[i].c_str());
     std::string fileExt = itksys::SystemTools::GetFilenameLastExtension(sortedFileNames[i].c_str());

    // Get the extension (without a dot)
    itksys::RegularExpression extRe(".(.*)");
    extRe.find(fileExt);
    std::string extString = extRe.match(1);

    itksys::RegularExpression labelRe("[^-]*-(.*)");
    labelRe.find(fileName);
    std::string labelString = labelRe.match(1);
    int label = atoi(labelString.c_str());

    // Get color
    float r, g, b;
    r = colors[label][0];
    g = colors[label][1];
    b = colors[label][2];

    // Output the mrml nodes
    mout << " <ModelStorage" << std::endl;
    mout << "  id=\"vtkMRMLModelStorageNode_" << fileName << "(" << extString << ")"
         << "\"  name=\"ModelStorage\""
         << " fileName=\""
         << config.ModelsDirectory() << "/" << argv[2] << "/" << fullFileName
         << "\">"
         << "</ModelStorage>" << std::endl;

    mout << " <ModelDisplay" << std::endl;
    mout << "  id=\"vtkMRMLModelDisplayNode_" << fileName  << "(" << extString << ")"
         << "\" name=\"ModelDisplay\""
         << " color=\"" << r << " " << g << " " << b << "\">"
         <<"</ModelDisplay>" << std::endl;

    mout << " <Model" << std::endl;
    mout << "  id=\"vtkMRMLModelNode_" << fileName << "(" << extString << ")"
         << "\" name=\"" << fileName << "(" << extString << ")" << "\""
         << " displayNodeRef=\"vtkMRMLModelDisplayNode_" << fileName << "(" << extString << ")" << "\""
         << " storageNodeRef=\"vtkMRMLModelStorageNode_" << fileName << "(" << extString << ")" << "\">"
         << "</Model>" << std::endl;
    }
  // Write trailer
  mout << "</MRML>" << std::endl;
  mout.close();
  return EXIT_SUCCESS;
}
