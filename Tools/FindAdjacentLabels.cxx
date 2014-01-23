/*=========================================================================

This file reads a 3D label volume and associates with each label a list 
of labels adjacent to that label.

To find the adjacent labels in 3D, it uses 14 iterators.  These
iterators are arranged as follows, where 1 is a location where there
is an iterator and 0 is where there is not.

For a given 3*3*3 neighborhood:
Slice 0:
1 1 1 
1 1 1 
1 1 1 

Slice 1:
1 1 1 
1 1 0 
0 0 0 

Slice 2:
0 0 0 
0 0 0 
0 0 0 

The value of the iterator with index (1,1,1) is compared to the other 13
iterators.  If it is different from the value of a given iterator,
the anatomy labels of those two voxels must be different.
Therefore, for both of those anatomies, we insert the corresponding 
adjacent label into its corresponding list.

Inputs: Label image prefix
Outputs: Adjacency file

=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkArchetypeSeriesFileNames.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkConstNeighborhoodIterator.h"

#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <map>

#include "FindStructureAttributes.h"

int main( int argc, char *argv[] )
{
  if (argc < 3)
    {
    std::cout << "Usage: inputImageName masterAnatomyFile adjacencyFile" << std::endl;
    return 0;
    }  

  char * fileName = argv[1];
  char * masterAnatomyFile = argv[2];
  char * adjacencyFile = argv[3];

  // Test the master anatomy file
  std::ifstream anatomyFile(masterAnatomyFile);
  if (!anatomyFile) 
    {
    std::cout << "In FindAnatomyLabelFromName.cxx: Could not open master anatomy file: " << anatomyFile << std::endl;
    return false;
    }

  // Test the output file
  std::ofstream fout ( adjacencyFile );
  if (!fout)
    {
    std::cout << "Could not open Adjacency Mapping File" << std::endl;
    return 0;
    }

  typedef   unsigned short   InputPixelType;
  typedef itk::Image< InputPixelType,  2 >   InputImageType;
  typedef itk::ImageFileReader< InputImageType >  ReaderType;
  typedef itk::ArchetypeSeriesFileNames ArchetypeNameGeneratorType;
  typedef itk::ConstNeighborhoodIterator< InputImageType > NeighborhoodIteratorType;
  
  typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<
    InputImageType > FaceCalculatorType;
  
  ReaderType::Pointer reader = ReaderType::New();
  InputImageType::Pointer prevImage;
  InputImageType::Pointer currImage;

  FaceCalculatorType faceCalculator;
  FaceCalculatorType::FaceListType faceList;
  FaceCalculatorType::FaceListType::iterator fit;
  
  // The following populates the mapper, which will contain a mapping 
  // from the anatomy name to its label.
  typedef std::map< std::string, InputPixelType > StringToInputPixelTypeMapType;
  StringToInputPixelTypeMapType nameToLabelMapper;
  StringToInputPixelTypeMapType::iterator nameToLabelMapperIt;

  FindStructureAttributes< InputPixelType > structures(masterAnatomyFile);
  if( structures.GetParsingFailed() )
    {
    std::cout << "Parsing Failed" << std::endl;    
    return 0;
    }
  std::vector< std::string > allStructureNames = structures.GetAllStructureNames();
  std::vector< std::vector < InputPixelType > > allStructureAttributes = structures.GetAllStructureAttributes();

  std::vector< std::string >::iterator nameIt = allStructureNames.begin();;
  std::vector< std::vector< InputPixelType > >::iterator allAttributesIt = allStructureAttributes.begin();
  int structureLabel;
  while( nameIt != allStructureNames.end() )
    {
    structureLabel = (*allAttributesIt)[0];
    nameToLabelMapper[ *nameIt ] = structureLabel;
    nameIt++;
    allAttributesIt++;
    }
  
  typedef std::set < InputPixelType > InputPixelTypeSetType;
  typedef std::map< InputPixelType, InputPixelTypeSetType > InputPixelTypeToInputPixelTypeSetTypeMapType;
  InputPixelTypeToInputPixelTypeSetTypeMapType adjacencyIntArrayList;

  ArchetypeNameGeneratorType::Pointer inputImageFileGenerator = ArchetypeNameGeneratorType::New();      
  inputImageFileGenerator->SetArchetype( fileName );

  std::vector <std::string > inputImageNames = inputImageFileGenerator->GetFileNames();
  inputImageNames = inputImageFileGenerator->GetFileNames();

  std::cout << "File name start: " << inputImageNames[0] << std::endl;
  std::cout << "File name end: " << inputImageNames[inputImageNames.size()-1] << std::endl;
  
  reader->SetFileName(inputImageNames[0].c_str());

  try 
    { 
    prevImage = reader->GetOutput();
    prevImage->Update();
    prevImage->DisconnectPipeline();
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return -1;
    } 

  // The radius of the neighborhood should be 1
  NeighborhoodIteratorType::RadiusType radius;
  radius.Fill(1);

  faceList = faceCalculator( prevImage, 
                             prevImage->GetRequestedRegion(), 
                             radius );
  
  NeighborhoodIteratorType itPrev;
  NeighborhoodIteratorType itCurr;
  
  int slice;
  std::vector <std::string> ::iterator iinit = inputImageNames.begin();
  ++iinit;

  // Loop through each label image, starting with the second one
  while (iinit != inputImageNames.end())
    {
    slice = iinit - inputImageNames.begin();
    std::cout << "Slice: " << slice << std::endl;
    
    // Read the next image
    try 
      { 
      reader->SetFileName ( (*iinit).c_str() );
      currImage = reader->GetOutput();
      currImage->Update();
      currImage->DisconnectPipeline();
      } 
    catch( itk::ExceptionObject & err ) 
      { 
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      return -1;
      } 

    // Loop through each face
    for ( fit=faceList.begin();
          fit != faceList.end(); ++fit)
      {
      itPrev = NeighborhoodIteratorType( radius, prevImage , *fit );
      itCurr = NeighborhoodIteratorType( radius, currImage , *fit );

      // Loop through each neighborhood
      for (itCurr.GoToBegin(), itPrev.GoToBegin(); !itCurr.IsAtEnd();
           ++itCurr, ++itPrev)
        {
        NeighborhoodIteratorType::PixelType centerPixelCurr = itCurr.GetCenterPixel();

        // Loop through each active element of each neighborhood for the 
        // previous image.  For every neighbor pixel that is different
        // from the center pixel, add the neighbor label to the list
        // of adjacent labels for the center pixel and vice versa.
        for (unsigned int indexValue = 0; indexValue < 9;
             indexValue++)
          {
          NeighborhoodIteratorType::PixelType neighPixel = itPrev.GetPixel(indexValue);
          if (centerPixelCurr != neighPixel)
            {
            adjacencyIntArrayList[centerPixelCurr].insert(neighPixel);
            adjacencyIntArrayList[neighPixel].insert(centerPixelCurr);
            }
          }
        
        // Loop through each active element of each neighborhood for the 
        // current image.  For every neighbor pixel that is different
        // from the center pixel, add the neighbor label to the list
        // of adjacent labels for the center pixel and vice versa.
        for (unsigned int indexValue = 0; indexValue < 4;
             indexValue++)
          {
          NeighborhoodIteratorType::PixelType neighPixel = itCurr.GetPixel(indexValue);
          if (centerPixelCurr != neighPixel)
            {
             adjacencyIntArrayList[centerPixelCurr].insert(neighPixel);
             adjacencyIntArrayList[neighPixel].insert(centerPixelCurr);
            }
          }
        }
      }
    prevImage = currImage;
    ++iinit;
    }


  // Write out the adjaceny mapping values
  fout << "Anatomy,Adjacent labels" << std::endl;
  for( nameToLabelMapperIt=nameToLabelMapper.begin(); nameToLabelMapperIt!=nameToLabelMapper.end(); nameToLabelMapperIt++ )
    {
    fout << nameToLabelMapperIt->first;

    // For each structure, write out the adjacent labels
    for (InputPixelTypeSetType::iterator sit = adjacencyIntArrayList[ nameToLabelMapperIt->second ].begin();
         sit != adjacencyIntArrayList[ nameToLabelMapperIt->second ].end(); sit++)
      {
      fout  << "," << *sit;
      }

    fout << std::endl;
    }
  fout.close();
     
  return 0;
  
}
