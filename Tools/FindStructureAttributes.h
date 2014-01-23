/*=========================================================================


Inputs:
Outputs:

=========================================================================*/

#ifndef _FindStructureAttributes_h
#define _FindStructureAttributes_h

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

template< class AttributeType >
class FindStructureAttributes
{
public:
  // Find the attributes associated with one structure and pass them back.
  FindStructureAttributes(std::string inputFile, std::string structureName);
  // Find the attributes associated with all structures and pass them
  // all back.
  FindStructureAttributes(std::string inputFile);

  ~FindStructureAttributes() {};

  bool TestFile() const;
  bool ParseFile();
  bool ParseLine(std::string line);

  std::vector < std::string > GetAllStructureNames() const { return m_AllStructureNames; }
  std::vector< AttributeType > GetStructureAttributes() const { return m_StructureAttributes; }
  std::vector< std::vector< AttributeType > > GetAllStructureAttributes() const { return m_AllStructureAttributes; }
  
  bool GetParsingFailed() const { return m_ParsingFailed; }

protected:


private:
  FindStructureAttributes(); // Purposely not implemented.

  std::string m_InputFile;
  std::string m_StructureName;
  std::vector < std::string > m_AllStructureNames;
  std::vector< AttributeType > m_StructureAttributes;
  std::vector< std::vector< AttributeType > > m_AllStructureAttributes;

  bool m_ParsingFailed;

};

#include "FindStructureAttributes.hxx"

#endif
