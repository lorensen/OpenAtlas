/*
  Generate a Mrml Scene for the atlas
*/
#include <itksys/Directory.hxx>
#include <itksys/SystemTools.hxx>
#include <itksys/RegularExpression.hxx>
#include <OpenAtlasUtilities.h>

#include <iostream>
#include <fstream>

int main (int argc, char *argv[])
{
  if (argc < 4)
    {
    std::cout << "Usage: " << argv[0] << "ModelDirectory ColorFile MrmlScene" << std::endl;
    return EXIT_FAILURE;
    }

  std::vector<std::vector<float> > colors;
  // Read the color file
  ReadColorFile(argv[2], colors);

  // Open mrml file and write header
  std::ofstream mout(argv[3]);
  if (!mout)
    {
    std::cerr << argv[0] << ": Could not open Mrml file " << argv[3] << " for output" << std::endl;
    perror(argv[0]);
    return EXIT_FAILURE;
    }
  mout << "<MRML  version=\"Slicer4.4.0\" userTags=\"\">" << std::endl;
 
  // Get the names of each model in the directory
  itksys::Directory dir;
  std::cout << "Loading directory: " << argv[1] << std::endl;

  dir.Load(argv[1]);
  std::cout << "Number of files is: " <<   dir.GetNumberOfFiles() << std::endl;

  for (unsigned long i = 0; i < dir.GetNumberOfFiles(); ++i)
    {
    const char *fullFileName = dir.GetFile(i);
    std::string fullPathName = std::string(argv[1]) + "/" + fullFileName;
    if (itksys::SystemTools::FileIsDirectory(fullPathName))
      {
      std::cout << "Skipping directory " << fullPathName << std::endl;
      continue;
      }
    // Extract label number from filename
    std::string fileName = itksys::SystemTools::GetFilenameWithoutExtension(fullFileName);
    std::string fileExt = itksys::SystemTools::GetFilenameLastExtension(fullFileName);

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
    b = colors[label][1];
    g = colors[label][2];

    // Create ModelStorage, ModelDisplay, and Model nodes
    mout << " <ModelStorage" << std::endl;
    mout << "  id=\"vtkMRMLModelStorageNode_" << fileName << "(" << extString << ")"
         << "\"  name=\"ModelStorage\""
         << " fileName=\""
         << argv[1] << "/" << fullFileName
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
