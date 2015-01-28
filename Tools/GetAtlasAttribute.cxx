#include <cstdlib>
#include <iostream>

#include "OpenAtlasUtilities.h"

int main (int argc, char *argv[])
{
  OpenAtlas::Configuration config(argv[1]);
  std::string name = argv[2];
  if (name == "AtlasName")
    {
    std::cout << config.AtlasName() << std::endl;
    }
  else if (name == "ColorTableFileName")
    {
    std::cout << config.ColorTableFileName() << std::endl;
    }
  else if (name == "AdjacenciesFileName")
    {
    std::cout << config.AdjacenciesFileName() << std::endl;
    }
  else if (name == "VolumeFileName")
    {
    std::cout << config.VolumeFileName() << std::endl;
    }
  else if (name == "LabelFileName")
    {
    std::cout << config.LabelFileName() << std::endl;
    }
  else if (name == "ModelsDirectory")
    {
    std::cout << config.ModelsDirectory() << std::endl;
    }
  else if (name == "VTKDirectory")
    {
    std::cout << config.VTKDirectory() << std::endl;
    }
  else if (name == "STLDirectory")
    {
    std::cout << config.STLDirectory() << std::endl;
    }
  else if (name == "ScreenshotDirectory")
    {
    std::cout << config.ScreenshotDirectory() << std::endl;
    }
  else if (name == "ChangesDirectory")
    {
    std::cout << config.ChangesDirectory() << std::endl;
    }
  else if (name == "StatisticsDirectory")
    {
    std::cout << config.StatisticsDirectory() << std::endl;
    }
  else if (name == "MRMLDirectory")
    {
    std::cout << config.MRMLDirectory() << std::endl;
    }
  else if (name == "GitDirectory")
    {
    std::cout << config.GitDirectory() << std::endl;
    }
  else if (name == "FiducialsDirectory")
    {
    std::cout << config.FiducialsDirectory() << std::endl;
    }
  return EXIT_SUCCESS;
}
