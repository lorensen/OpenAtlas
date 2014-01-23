#include <vector>
#include <set>
#include <string>
#include <fstream>

int ReadLabelFile(const char *, std::vector<std::string> &);
int ReadColorFile(const char *, std::vector<std::vector<float> >&);
int ReadAdjacenyFile(const char *, std::vector<std::set<unsigned int> > &);
