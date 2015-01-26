#include <vector>
#include <set>
#include <map>
#include <string>
#include <fstream>

int ReadLabelFile(const char *, std::vector<std::string> &);
int ReadColorFile(const char *, std::vector<std::vector<float> >&);
int ReadAdjacenyFile(const char *, std::vector<std::set<unsigned int> > &);
namespace OpenAtlas
{
class Configuration
{
public:
  Configuration(std::string file)
  {
    this->ParseFile(file.c_str());
  }
  const std::string &AtlasName() {return m_AtlasName;}
  const std::string &ColorTableFileName() {return m_ColorTableFileName;}
  const std::string &AdjacenciesFileName() {return m_AdjacenciesFileName;}
  const std::string &VolumeFileName() {return m_VolumeFileName;}
  const std::string &LabelFileName() {return m_LabelFileName;}
  const std::string &ModelsDirectory() {return m_ModelsDirectory;}
  const std::string &VTKDirectory() {return m_VTKDirectory;}
  const std::string &STLDirectory() {return m_STLDirectory;}
  const std::string &ScreenshotDirectory() {return m_ScreenshotDirectory;}
  const std::string &StatisticsDirectory() {return m_StatisticsDirectory;}
  const std::string &MRMLDirectory() {return m_MRMLDirectory;}

protected:
  Configuration() {}
  void ParseFile(const char *);

private:
  std::string m_AtlasName;
  std::string m_ColorTableFileName;
  std::string m_AdjacenciesFileName;
  std::string m_VolumeFileName;
  std::string m_LabelFileName;
  std::string m_ModelsDirectory;
  std::string m_VTKDirectory;
  std::string m_STLDirectory;
  std::string m_ScreenshotDirectory;
  std::string m_StatisticsDirectory;
  std::string m_MRMLDirectory;
};
}

