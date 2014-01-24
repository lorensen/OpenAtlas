/*=========================================================================

This file reads a 3D label volume and associates with each label a list 
of labels adjacent to that label.

To find the adjacent labels in 3D, it uses 8 iterators.  These
iterators are arranged as follows, where 1 is a location where there
is an iterator and 0 is where there is not.

For a given 3*3*3 neighborhood:
Slice 0:
0 1 0 
1 1 1 
0 1 0 

Slice 1:
0 1 0 
1 1 0 
0 0 0 

Slice 2:
0 0 0 
0 0 0 
0 0 0 

The value of the iterator with index (1,1,1) is compared to the other 7
iterators.  If it is different from the value of a given iterator,
the anatomy labels of those two voxels must be different.
Therefore, for both of those anatomies, we insert the corresponding 
adjacent label into its corresponding list.

Inputs: Label image
Outputs: Adjacency file

=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkExtractImageFilter.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkConstNeighborhoodIterator.h"

#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <map>

int main( int argc, char *argv[] )
{
  if (argc < 2)
    {
    std::cout << "Usage: inputImageName adjacencyFile" << std::endl;
    return EXIT_FAILURE;
    }  

  char * fileName = argv[1];
  char * adjacencyFile = argv[2];

  // Test the output file
  std::ofstream fout ( adjacencyFile );
  if (!fout)
    {
    std::cout << "Could not open Adjacency Mapping File" << std::endl;
    return EXIT_FAILURE;
    }

  typedef   unsigned short              PixelType;
  typedef itk::Image< PixelType,  2 >   Image2DType;
  typedef itk::Image< PixelType,  3 >   Image3DType;

  typedef itk::ImageFileReader< Image3DType >              ReaderType;
  typedef itk::ExtractImageFilter<Image3DType,Image2DType> ExtractType;
  typedef itk::ConstNeighborhoodIterator< Image2DType >    NeighborhoodIteratorType;
  
  typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<
    Image2DType > FaceCalculatorType;
  
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName);
  reader->Update();

  Image3DType::RegionType inputRegion =
    reader->GetOutput()->GetLargestPossibleRegion();
  Image3DType::SizeType inputSize = inputRegion.GetSize();

  int numberOfSlices = inputSize[2];
  std::cout << "Processing " << numberOfSlices << " slices" << std::endl;

  ExtractType::Pointer extractSlice = ExtractType::New();
  extractSlice->SetInput(reader->GetOutput());

  Image2DType::Pointer prevImage;
  Image2DType::Pointer currImage;

  FaceCalculatorType faceCalculator;
  FaceCalculatorType::FaceListType faceList;
  FaceCalculatorType::FaceListType::iterator fit;
  
  typedef std::set< PixelType > PixelTypeSetType;
  typedef std::map< PixelType, PixelTypeSetType > PixelTypeToPixelTypeSetTypeMapType;
  PixelTypeToPixelTypeSetTypeMapType adjacencyIntArrayList;
  Image3DType::SizeType extractSize = inputSize;
  Image3DType::IndexType extractIndex;
  Image3DType::RegionType extractRegion = inputRegion;
  try 
    { 
    // Slice 0 
    extractSize[2] = 0;
    extractRegion.SetSize(extractSize);

    extractIndex[0] = extractIndex[1] = 0;
    extractIndex[2] = 0;

    extractRegion.SetIndex(extractIndex);
    extractSlice->SetExtractionRegion(extractRegion);
    extractSlice->SetDirectionCollapseToIdentity();
    extractSlice->Update();

    prevImage = extractSlice->GetOutput();
    prevImage->Update();
    prevImage->DisconnectPipeline();
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return EXIT_FAILURE;
    } 

  // The radius of the neighborhood should be 1
  NeighborhoodIteratorType::RadiusType radius;
  radius.Fill(1);

  faceList = faceCalculator( prevImage, 
                             prevImage->GetRequestedRegion(), 
                             radius );
  
  NeighborhoodIteratorType itPrev;
  NeighborhoodIteratorType itCurr;
  
  // Loop through each label image, starting with the second one
  for (int slice = 1; slice < numberOfSlices; ++slice)
    {
    std::cout << "Slice: " << slice << std::endl;
    
    // Read the next image
    try 
      { 
      extractIndex[2] = slice;
      extractRegion.SetIndex(extractIndex);
      extractSlice->SetExtractionRegion(extractRegion);
      extractSlice->Update();
      currImage = extractSlice->GetOutput();
      currImage->Update();
      currImage->DisconnectPipeline();
      } 
    catch( itk::ExceptionObject & err ) 
      { 
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      return EXIT_FAILURE;
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
        std::vector<unsigned int> pIndices;
//        pIndices.push_back(1);
//        pIndices.push_back(3);
        pIndices.push_back(4);
//        pIndices.push_back(5);
//        pIndices.push_back(7);
        for (unsigned int i = 0; i < pIndices.size(); ++i)
          {
          NeighborhoodIteratorType::PixelType neighPixel =
            itPrev.GetPixel(pIndices[i]);
          if (centerPixelCurr != neighPixel &&
              neighPixel != 0 &&
              centerPixelCurr != 0)
            {
            adjacencyIntArrayList[centerPixelCurr].insert(neighPixel);
            adjacencyIntArrayList[neighPixel].insert(centerPixelCurr);
            }
          }
        
        // Loop through each active element of each neighborhood for the 
        // current image.  For every neighbor pixel that is different
        // from the center pixel, add the neighbor label to the list
        // of adjacent labels for the center pixel and vice versa.
        std::vector<unsigned int> cIndices;
        cIndices.push_back(1);
        cIndices.push_back(3);
        cIndices.push_back(4);
        for (unsigned int i = 0; i < cIndices.size(); ++i)
          {
          NeighborhoodIteratorType::PixelType neighPixel =
            itCurr.GetPixel(cIndices[i]);
          if (centerPixelCurr != neighPixel &&
              neighPixel != 0 &&
              centerPixelCurr != 0)
            {
             adjacencyIntArrayList[centerPixelCurr].insert(neighPixel);
             adjacencyIntArrayList[neighPixel].insert(centerPixelCurr);
            }
          }
        }
      }
    prevImage = currImage;
    }
  
  // Write out the adjaceny mapping values
  for (unsigned int i = 1; i < 5000; ++i)
    {
    if (adjacencyIntArrayList[i].size() == 0)
      {
      continue;
      }
    std::cout << "Size for " << i << ": " << adjacencyIntArrayList[i].size() << std::endl;
    fout << i << " " << adjacencyIntArrayList[i].size();
    for (PixelTypeSetType::iterator sit = adjacencyIntArrayList[ i ].begin();
         sit != adjacencyIntArrayList[ i ].end(); sit++)
      {
      fout  << " " << *sit;
      }

    fout << std::endl;
    }
  fout.close();
  
  return EXIT_SUCCESS;
  
}
