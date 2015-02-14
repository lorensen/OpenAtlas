#include <vector>
#include <set>
#include <map>
#include <string>
#include <fstream>

int ReadLabelFile(std::string, std::vector<std::string> &);
int ReadColorFile(std::string, std::vector<std::vector<float> >&);
int ReadAdjacenyFile(std::string, std::vector<std::set<unsigned int> > &);
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
  const std::string &ChangesDirectory() {return m_ChangesDirectory;}
  const std::string &StatisticsDirectory() {return m_StatisticsDirectory;}
  const std::string &MRMLDirectory() {return m_MRMLDirectory;}
  const std::string &GitDirectory() {return m_GitDirectory;}
  const std::string &GitWikiDirectory() {return m_GitWikiDirectory;}
  const std::string &FiducialsDirectory() {return m_FiducialsDirectory;}

protected:
  Configuration() {}
  void ParseFile(std::string);

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
  std::string m_ChangesDirectory;
  std::string m_StatisticsDirectory;
  std::string m_MRMLDirectory;
  std::string m_GitDirectory;
  std::string m_GitWikiDirectory;
  std::string m_FiducialsDirectory;
};
}

