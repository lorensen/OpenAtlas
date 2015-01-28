//
// GenerateStatisticsFromLabels
//   Usage: GenerateStatisticsFromLabels InputVolume Startlabel Endlabel
//          where
//          InputVolume is a nrrd file containing a 3D volume of
//            discrete labels.
//          StartLabel is the first label to be processed
//          EndLabel is the last label to be processed
//          NOTE: There can be gaps in the labeling. If a label does
//          not exist in the volume, it will be skipped.
//      
//
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageFileReader.h"
#include "itkResampleImageFilter.h"

#include <itksys/SystemTools.hxx>

#include <vtkImageData.h>
#include <OpenAtlasUtilities.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

int main (int argc, char *argv[])
{
  if (argc < 2)
    {
    std::cout << "Usage: " << argv[0] << " ConfigFile" << std::endl;
    return EXIT_FAILURE;
    }
 

  OpenAtlas::Configuration config(argv[1]);

  std::vector<std::string> labels;

  // Read the label file
  try
    {
    ReadLabelFile(config.ColorTableFileName(), labels);
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected reading label file "  << e;
    return EXIT_FAILURE;
    }

  // Create all of the classes we will need

  typedef itk::Image<unsigned short, 3>          ImageType;
  typedef itk::Image<unsigned int, 3>            LabelImageType;
  typedef double                                 CoordRepType;

  typedef itk::ImageFileReader<ImageType>        ImageReaderType;
  ImageReaderType::Pointer imageReader = ImageReaderType::New();

  typedef itk::ImageFileReader<LabelImageType>   LabelImageReaderType;
  LabelImageReaderType::Pointer labelReader = LabelImageReaderType::New();

  typedef itk::ResampleImageFilter< ImageType, ImageType > ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  typedef itk::IdentityTransform<CoordRepType,3> TransformType;
  TransformType::Pointer identityTransform = TransformType::New();

  typedef itk::LinearInterpolateImageFunction<ImageType,CoordRepType>
    InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  typedef itk::LabelStatisticsImageFilter< ImageType, LabelImageType >
    LabelStatisticsImageFilterType;
  LabelStatisticsImageFilterType::Pointer labelStatistics =
    LabelStatisticsImageFilterType::New();

  // Read the grayscale volume
  imageReader->SetFileName(config.VolumeFileName());
  try
    {
    imageReader->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected reading image file "  << e;
    return EXIT_FAILURE;
    }

  // Read the label volume
  labelReader->SetFileName(config.LabelFileName().c_str());
  try
    {
    labelReader->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected reading label image file "  << e;
    return EXIT_FAILURE;
    }

  // Resample grayscale into label coordinate space
  resample->SetInput(imageReader->GetOutput());
  resample->SetReferenceImage( labelReader->GetOutput() );
  resample->UseReferenceImageOn();
  resample->SetTransform( identityTransform );
  resample->SetInterpolator( interpolator );

  // Compute the statistics
  labelStatistics->SetLabelInput( labelReader->GetOutput());
  labelStatistics->SetInput(resample->GetOutput());
  labelStatistics->UseHistogramsOn(); // needed to compute median
  try
    {
    labelStatistics->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected computing statistics "  << e;
    return EXIT_FAILURE;
    }

  // Output the statistics
  std::string filePrefix = "Label";
  std::cout << "Number of labels: "
            << labelStatistics->GetNumberOfLabels() << std::endl;
  
  typedef LabelStatisticsImageFilterType::ValidLabelValuesContainerType
    ValidLabelValuesType;
  typedef LabelStatisticsImageFilterType::LabelPixelType
    LabelPixelType;
  typedef LabelStatisticsImageFilterType::BoundingBoxType
    BoundingBoxType;

  for(ValidLabelValuesType::const_iterator vIt =
        labelStatistics->GetValidLabelValues().begin();
      vIt != labelStatistics->GetValidLabelValues().end();
      ++vIt)
    {
    if ( labelStatistics->HasLabel(*vIt) )
      {
      LabelPixelType labelValue = *vIt;
      std::stringstream ss;
      if (labels[labelValue] == "")
        {
        continue;
        }
      ss << config.StatisticsDirectory() << "/" << labels[labelValue] << "-" << labelValue << ".txt";
      std::ofstream fout(ss.str().c_str());
      if (!fout)
        {
        std::cout << "Could not open Statistics file " << ss.str() << std::endl;
        return EXIT_FAILURE;
        }
    
      fout << "Name:  " << labels[labelValue] << std::endl;
      fout << "Label: " << *vIt << std::endl;
      fout << "    count: "
                << labelStatistics->GetCount( labelValue )
                << std::endl;
      fout << "    min: "
           << labelStatistics->GetMinimum( labelValue )
                << std::endl;
      fout << "    max: "
                << labelStatistics->GetMaximum( labelValue )
                << std::endl;
      fout << "    median: "
                << labelStatistics->GetMedian( labelValue )
                << std::endl;
      fout << "    mean: "
                << labelStatistics->GetMean( labelValue )
                << std::endl;
      fout << "    sigma: "
                << labelStatistics->GetSigma( labelValue )
                << std::endl;
      fout << "    variance: "
                << labelStatistics->GetVariance( labelValue )
                << std::endl;
      fout << "    sum: "
                << labelStatistics->GetSum( labelValue )
                << std::endl;
      fout << "    region: "
           << "index("
           << labelStatistics->GetRegion( labelValue ).GetIndex()[0] << ", "
           << labelStatistics->GetRegion( labelValue ).GetIndex()[1] << ", "
           << labelStatistics->GetRegion( labelValue ).GetIndex()[2] << ")"
           << " size("
           << labelStatistics->GetRegion( labelValue ).GetSize()[0] << ", "
           << labelStatistics->GetRegion( labelValue ).GetSize()[1] << ", "
           << labelStatistics->GetRegion( labelValue ).GetSize()[2] << ")"
           << std::endl;
      BoundingBoxType box = labelStatistics->GetBoundingBox(labelValue);
      fout << "    bounding box: "
           << "(" << box[0] << ", " << box[1] << ") "
           << "(" << box[2] << ", " << box[3] << ") "
           << "(" << box[4] << ", " << box[5] << ") "
           << std::endl;
      fout.close();
      }
    }
  return EXIT_SUCCESS;
}
