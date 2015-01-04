#include "OpenAtlasUtilities.h"

#include <cstdlib>
#include <iostream>

#include <set>
#include <vector>
#include <string>

int main (int argc, char *argv[])
{
  if (argc < 3)
    {
    std::cout << "Usage: " << argv[0] << " AdjacencyFile ColorTableFile" << std::endl;
    return EXIT_FAILURE;
    }

  char *adjacencyFile = argv[1];
  std::vector<std::set<unsigned int> > neighbors;
  ReadAdjacenyFile(adjacencyFile, neighbors);

  char *anatomyFile = argv[2];
  std::vector<std::string> labels;
  ReadLabelFile(anatomyFile, labels);

  // For each label, if it contains "right"(or "left") see if it is
  // adjacent to "left"(or "right")
  for (size_t l = 0; l < labels.size(); ++l)
    {
    std::string name = labels[l];
    // first right
    if (name.find("right") != std::string::npos)
      {
      for (std::set<unsigned int>::iterator sit = neighbors[l].begin();
           sit != neighbors[l].end();
           ++sit)
        {
        std::string neighbor = labels[*sit];
        if (neighbor.find("left") != std::string::npos)
          {
          std::cout << "WARNING: " << name << " is adjacent to " << neighbor << std::endl;
          }
        }
      }
    // then left
    if (name.find("left") != std::string::npos)
      {
      for (std::set<unsigned int>::iterator sit = neighbors[l].begin();
           sit != neighbors[l].end();
           ++sit)
        {
        std::string neighbor = labels[*sit];
        if (neighbor.find("right") != std::string::npos)
          {
          std::cout << "WARNING: " << name << " is adjacent to " << neighbor << std::endl;
          }
        }
      }
    }
  return EXIT_SUCCESS;
}
