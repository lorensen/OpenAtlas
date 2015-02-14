#include "OpenAtlasUtilities.h"
#include "itkMacro.h"
#include <itksys/SystemTools.hxx>
#include <itksys/RegularExpression.hxx>

int ReadLabelFile(std::string filename, std::vector<std::string> &labels)
{
  std::ifstream file;
  file.open(filename.c_str());
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
        labels.resize((label + 1) * 2);
        }
      labels[label] = name;
      }
    }
  file.close();
  return 0;
}

int ReadColorFile(std::string filename, std::vector<std::vector<float> > &colors)
{
  std::ifstream file;
  file.open(filename.c_str());
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
    int r, g, b, alpha;
    if (line.find("#") == std::string::npos)
      {
      isstream >> label >> name;
      if (label >= colors.size())
        {
        colors.resize((label + 1) * 2);
        }
      isstream >> r >> g >> b >> alpha;
      colors[label].resize(4);
      colors[label][0] = r / 255.0;
      colors[label][1] = g / 255.0;
      colors[label][2] = b / 255.0;
      colors[label][3] = alpha / 255.0;
      }
    }
  file.close();
  return 0;
}

int ReadAdjacenyFile(std::string filename, std::vector<std::set<unsigned int> > &adjacencies)
{
  std::ifstream file;
  file.open(filename.c_str());
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
    if (line.find("#") == std::string::npos)
      {
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
    }
  file.close();
  return 0;
}

namespace OpenAtlas
{
void Configuration::ParseFile(std::string filename)
{
  // open the file
  std::ifstream file;
  file.open(filename.c_str());
  if (file.fail())
    {
    itkGenericExceptionMacro(
      "The file " << filename <<" cannot be opened for reading!"
      << std::endl
      << "Reason: "
      << itksys::SystemTools::GetLastSystemError() );
    }

  // read a line
  std::string line;
  while (!file.eof())
    {
    std::getline(file, line, '\n');
    if (line.find("#") == std::string::npos)
      {
      itksys::RegularExpression re("AtlasName[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        this->m_AtlasName = re.match(1);
        continue;
        }
      re.compile("VolumeFileName[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        this->m_VolumeFileName = re.match(1);
        continue;
        }
      re.compile("LabelFileName[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        this->m_LabelFileName = re.match(1);
        continue;
        }
      re.compile("AdjacenciesFileName[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        this->m_AdjacenciesFileName = re.match(1);
        continue;
        }
      re.compile("ColorTableFileName[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_ColorTableFileName = re.match(1);
        continue;
        }
      re.compile("ModelsDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_ModelsDirectory = re.match(1);
        continue;
        }
      re.compile("VTKDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_VTKDirectory = re.match(1);
        continue;
        }
      re.compile("STLDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_STLDirectory = re.match(1);
        continue;
        }
      re.compile("ScreenshotDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_ScreenshotDirectory = re.match(1);
        continue;
        }
      re.compile("ChangesDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_ChangesDirectory = re.match(1);
        continue;
        }
      re.compile("StatisticsDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_StatisticsDirectory = re.match(1);
        continue;
        }
      re.compile("MRMLDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_MRMLDirectory = re.match(1);
        continue;
        }
      re.compile("GitDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_GitDirectory = re.match(1);
        continue;
        }
      re.compile("GitWikiDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_GitWikiDirectory = re.match(1);
        continue;
        }
      re.compile("FiducialsDirectory[^:]*:[ ]*([^$]*)");
      if (re.find(line))
        {
        m_FiducialsDirectory = re.match(1);
        continue;
        }
      }
    }
  file.close();
}
}
