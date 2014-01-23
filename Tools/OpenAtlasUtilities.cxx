#include "OpenAtlasUtilities.h"
#include "itkMacro.h"
#include <itksys/SystemTools.hxx>

int ReadLabelFile(const char * filename, std::vector<std::string> &labels)
{
  std::ifstream file;
  file.open(filename);
  if (file.fail())
    {
    itkGenericExceptionMacro(
      "The file " << filename <<" cannot be opened for reading!"
      << std::endl
      << "Reason: "
      << itksys::SystemTools::GetLastSystemError() );
    }
  std::string line;
  while (!file.eof())
    {
    std::getline(file, line, '\n');
    std::istringstream isstream(line);
    std::string::size_type label = 0;
    std::string name;
    if (line.find("#") == std::string::npos)
      {
      isstream >> label >> name;
      if (label >= labels.size())
        {
        labels.resize((labels.size() + 1) * 2);
        }
      labels[label] = name;
      }
    }
  file.close();
  return 0;
}

int ReadColorFile(const char * filename, std::vector<std::vector<float> > &colors)
{
  std::ifstream file;
  file.open(filename);
  if (file.fail())
    {
    itkGenericExceptionMacro(
      "The file " << filename <<" cannot be opened for reading!"
      << std::endl
      << "Reason: "
      << itksys::SystemTools::GetLastSystemError() );
    }
  std::string line;
  while (!file.eof())
    {
    std::getline(file, line, '\n');
    std::istringstream isstream(line);
    std::string::size_type label = 0;
    std::string name;
    int r, g, b;
    if (line.find("#") == std::string::npos)
      {
      isstream >> label >> name;
      if (label >= colors.size())
        {
        colors.resize((label + 1) * 2);
        }
      isstream >> r >> g >> b;
      colors[label].resize(3);
      colors[label][0] = r / 255.0;
      colors[label][1] = g / 255.0;
      colors[label][2] = b / 255.0;
      }
    }
  file.close();
  return 0;
}

int ReadAdjacenyFile(const char * filename, std::vector<std::set<unsigned int> > &adjacencies)
{
  std::ifstream file;
  file.open(filename);
  if (file.fail())
    {
    itkGenericExceptionMacro(
      "The file " << filename <<" cannot be opened for reading!"
      << std::endl
      << "Reason: "
      << itksys::SystemTools::GetLastSystemError() );
    }

  std::string line;
  unsigned int label;
  unsigned int count;
  unsigned int neighbor;
  while (!file.eof())
    {
    std::getline(file, line, '\n');
    std::istringstream isstream(line);
    isstream >> label;
    isstream >> count;
    if (label >= adjacencies.size())
      {
      adjacencies.resize((label + 1) * 2);
      }
    for (unsigned int i = 0; i < count; ++i)
      {
      isstream >> neighbor;
      adjacencies[label].insert(neighbor);
      }
    }
  return 0;
}
