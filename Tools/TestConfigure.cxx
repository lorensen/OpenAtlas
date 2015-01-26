#include <fstream>
#include <iostream>
#include <cstdlib>

#include "OpenAtlasUtilities.h"

int main (int argc, char *argv[])
{
  OpenAtlas::Configuration config(argv[1]);
  std::cout << "AtlasName: " << config.AtlasName() << std::endl;
  std::cout << "ColorTableFileName: " << config.ColorTableFileName() << std::endl;
  std::cout << "AdjacenciesFileName: " << config.AdjacenciesFileName() << std::endl;
  std::cout << "VolumeFileName: " << config.VolumeFileName() << std::endl;
  std::cout << "LabelFileName: " << config.LabelFileName() << std::endl;
  std::cout << "ModelsDirectory: " << config.ModelsDirectory() << std::endl;
  std::cout << "VTKDirectory: " << config.VTKDirectory() << std::endl;
  std::cout << "STLDirectory: " << config.STLDirectory() << std::endl;
  std::cout << "ScreenshotDirectory: " << config.ScreenshotDirectory() << std::endl;
  std::cout << "StatisticsDirectory: " << config.StatisticsDirectory() << std::endl;
  std::cout << "MRMLDirectory: " << config.MRMLDirectory() << std::endl;

  return EXIT_SUCCESS;
}
