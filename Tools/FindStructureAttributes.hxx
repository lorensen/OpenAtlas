#ifndef _FindStructureAttributes_hxx
#define _FindStructureAttributes_hxx

#include "FindStructureAttributes.h"
#include <fstream>
#include <iostream>

template< class AttributeType >
FindStructureAttributes< AttributeType >::FindStructureAttributes(std::string inputFile, std::string structureName)
{
  m_InputFile = inputFile;
  m_StructureName = structureName;
  m_AllStructureNames.clear();
  m_StructureAttributes.clear();
  m_AllStructureAttributes.clear();
  m_ParsingFailed = true;

  if ( ParseFile() )
    {
    m_ParsingFailed = false;
    }
  
}

template< class AttributeType >
FindStructureAttributes< AttributeType >::FindStructureAttributes(std::string inputFile)
{
  m_InputFile = inputFile;
  m_StructureName = "all";
  m_AllStructureNames.clear();
  m_StructureAttributes.clear();
  m_AllStructureAttributes.clear();
  m_ParsingFailed = true;

  if ( ParseFile() )
    {
    m_ParsingFailed = false;
    }
  
}


// template< class AttributeType >
// FindStructureAttributes< AttributeType >::~FindStructureAttributes()
// {
//   m_Fin.close();
// }

template< class AttributeType >
bool FindStructureAttributes< AttributeType >::TestFile() const
{
  // Test the input file
  std::ifstream fin;
  fin.open(m_InputFile.c_str());

  if (!fin)
    {
    std::cout << "\tCould not open input file: " << m_InputFile << std::endl;
    return false;
    }

  return true;
}


template< class AttributeType >
bool FindStructureAttributes< AttributeType >::ParseFile()
{

  std::cout << "In FindStructureAttributes.hxx: Before m_StructureName: " << m_StructureName << std::endl;

  // If the structure name has underscores, change them to spaces.
  int nextUnderscore = -1;
  int prevUnderscore;
  for (float i = 0; i < m_StructureName.size(); i++)
    {
    prevUnderscore = nextUnderscore;
    nextUnderscore = m_StructureName.find('_',prevUnderscore+1);
    if (nextUnderscore == -1)
      break;
    else
      m_StructureName.replace(nextUnderscore , 1 ," ");
    }


  std::cout << "In FindStructureAttributes.hxx: After m_StructureName: " << m_StructureName << std::endl;

  if ( !TestFile() )
    {
    return false;
    }

  std::ifstream fin;
  fin.open(m_InputFile.c_str());

  // Remove the header
  char entireLine[255];
  fin.getline(entireLine,255);

  // Loop through the file looking for the anatomy name.  If it is
  // found, assign the structure attributes to a vector and break.
  std::string line;

  while (! fin.eof() )
    {
    fin.getline(entireLine,255);
    line=std::string(entireLine); 

    if (! line.empty() )
      {
      // If the structure is found and we are not finding all of the structures, break out of the loop.
      if ( ParseLine( line ) && m_StructureName != "all" )
        {
        return true;
        }
      }
    }

  fin.close();

  if ( m_StructureName == "all")
    { return true; }
  else
    { return false; }
}


template< class AttributeType >
bool FindStructureAttributes< AttributeType >::ParseLine(std::string line)
{
  m_StructureAttributes.clear();
  int endLine = line.length();
  int firstComma = line.find(',',0);
  std::string fileAnatomyName = line.substr(0, firstComma);

  if (fileAnatomyName == m_StructureName || m_StructureName == "all")
    {
    // Find all of the commas in the line
    std::vector< int > commaIndices;
    std::string::iterator it = line.begin();
    while (it != line.end() )
      {
      if (*it == ',')
        {
        commaIndices.push_back( it-line.begin() );
        }
      it++;
      }

    // Push back what is in between the commas
    unsigned int i;
    for (i = 1; i < commaIndices.size(); i++)
      {
      m_StructureAttributes.push_back( atoi(line.substr(commaIndices[i-1]+1, commaIndices[i]-(commaIndices[i-1]+1)).c_str()) );
      }
    m_StructureAttributes.push_back( atoi(line.substr(commaIndices[i-1]+1, endLine-commaIndices[i-1]+1).c_str()) );

    if (m_StructureName == "all")
      {
      m_AllStructureNames.push_back( fileAnatomyName );
      m_AllStructureAttributes.push_back( m_StructureAttributes );
      }
    return true;
    }
  else
    {
    return false;
    }

  return false;
}

#endif
