//
// GenerateModelsFromLabels
//   Usage: GenerateModelsFromLabels InputVolume Startlabel Endlabel
//          where
//          ConfigFile is an atlas configuration file that defines
//            locations of various atlas file
//          StartLabel is the first label to be processed
//          EndLabel is the last label to be processed
//          NOTE: There can be gaps in the labeling. If a label does
//          not exist in the volume, it will be skipped.
//      
//
#include <itkImageFileReader.h>
#include <itkOrientImageFilter.h>
#include <itkChangeInformationImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itksys/SystemTools.hxx>

#include <vtkImageAccumulate.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkMaskFields.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkThreshold.h>
#include <vtkGeometryFilter.h>
#include <vtkSTLWriter.h>
#include <vtkSmartPointer.h>
 
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
 
#include <OpenAtlasUtilities.h>

#include <string>
#include <vector>
#include <fstream>

int main (int argc, char *argv[])
{
  if (argc < 4)
    {
    std::cout << "Usage: " << argv[0] << " ConfigFile StartLabel EndLabel" << std::endl;
    return EXIT_FAILURE;
    }
 
  OpenAtlas::Configuration config(argv[1]);
  itksys::SystemTools::MakeDirectory(config.STLDirectory());

  std::vector<std::string> labels;

    // Read the label file
  try
    {
    ReadLabelFile(config.ColorTableFileName(), labels);
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e;
    return EXIT_FAILURE;
    }

  // Create all of the classes we will need

  typedef itk::Image<unsigned short, 3>            ImageType;

  typedef itk::ImageFileReader<ImageType>          ReaderType;
  ReaderType::Pointer reader = ReaderType::New();

  typedef itk::OrientImageFilter<ImageType,ImageType> OrienterType;
  OrienterType::Pointer orienter = OrienterType::New();

  typedef itk::ChangeInformationImageFilter<ImageType> ChangeType;
  ChangeType::Pointer change = ChangeType::New();

  typedef itk::ImageToVTKImageFilter<ImageType>    ConnectorType;
  ConnectorType::Pointer connector = ConnectorType::New();

  vtkSmartPointer<vtkImageAccumulate> histogram =
    vtkSmartPointer<vtkImageAccumulate>::New();
  vtkSmartPointer<vtkDiscreteMarchingCubes> discreteCubes =
    vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
  vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother =
    vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
  vtkSmartPointer<vtkThreshold> selector =
    vtkSmartPointer<vtkThreshold>::New();
  vtkSmartPointer<vtkMaskFields> scalarsOff =
    vtkSmartPointer<vtkMaskFields>::New();
  vtkSmartPointer<vtkGeometryFilter> geometry =
    vtkSmartPointer<vtkGeometryFilter>::New();
  vtkSmartPointer<vtkSTLWriter> writer =
    vtkSmartPointer<vtkSTLWriter>::New();
 
  // Define all of the variables
  unsigned int startLabel = atoi(argv[2]);
  unsigned int endLabel = atoi(argv[3]);
  std::string filePrefix = "Label";
  unsigned int smoothingIterations = 15;
  double passBand = 0.001;
  double featureAngle = 120.0;
 
  // Generate models from labels
  // 1) Read the meta file
  // 2) Generate a histogram of the labels
  // 3) Generate models from the labeled volume
  // 4) Smooth the models
  // 5) Output each model into a separate file
 
  reader->SetFileName(config.LabelFileName().c_str());

  // Convert to RAS (note: orientation filter uses a different
  // notation
  orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI); // RAS
  orienter->SetInput(reader->GetOutput());
  orienter->UseImageDirectionOn();
  orienter->Update();

  ImageType::PointType origin;
  origin[0] = -orienter->GetOutput()->GetOrigin()[0];
  origin[1] = -orienter->GetOutput()->GetOrigin()[1];
  origin[2] = orienter->GetOutput()->GetOrigin()[2];
  change->SetInput(orienter->GetOutput());
  change->SetOutputOrigin(origin);
  change->ChangeOriginOn();

  connector->SetInput(change->GetOutput());
  connector->Update();

  histogram->SetInputData(connector->GetOutput());
  histogram->SetComponentExtent(0, endLabel, 0, 0, 0, 0);
  histogram->SetComponentOrigin(0, 0, 0);
  histogram->SetComponentSpacing(1, 1, 1);
  histogram->Update();
 
  discreteCubes->SetInputData(connector->GetOutput());
  discreteCubes->GenerateValues(
    endLabel - startLabel + 1, startLabel, endLabel);
 
  smoother->SetInputConnection(discreteCubes->GetOutputPort());
  smoother->SetNumberOfIterations(smoothingIterations);
  smoother->BoundarySmoothingOff();
  smoother->FeatureEdgeSmoothingOff();
  smoother->SetFeatureAngle(featureAngle);
  smoother->SetPassBand(passBand);
  smoother->NonManifoldSmoothingOn();
  smoother->NormalizeCoordinatesOn();
  smoother->Update();
 
  selector->SetInputConnection(smoother->GetOutputPort());
  selector->SetInputArrayToProcess(0, 0, 0,
                                   vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                   vtkDataSetAttributes::SCALARS);
 
  // Strip the scalars from the output
  scalarsOff->SetInputConnection(selector->GetOutputPort());
  scalarsOff->CopyAttributeOff(vtkMaskFields::POINT_DATA,
                               vtkDataSetAttributes::SCALARS);
  scalarsOff->CopyAttributeOff(vtkMaskFields::CELL_DATA,
                               vtkDataSetAttributes::SCALARS);
 
  geometry->SetInputConnection(scalarsOff->GetOutputPort());
 
  vtkSmartPointer<vtkPolyDataConnectivityFilter> confilter =
    vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
  confilter->SetInputConnection(geometry->GetOutputPort());
  confilter->SetExtractionModeToLargestRegion();

  writer->SetInputConnection(confilter->GetOutputPort());
  writer->SetInputConnection(geometry->GetOutputPort());
 
  for (unsigned int i = startLabel; i <= endLabel; i++)
    {
    // see if the label exists, if not skip it
    double frequency =
      histogram->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);
    if (frequency == 0.0)
      {
      continue;
      }
 
    // select the cells for a given label
    selector->ThresholdBetween(i, i);
 
   if (labels[i] == "")
     {
     std::cout << "WARNING: label " << i
               << " is in the label volume but not in the color table and will not be generated" << std::endl;
     continue;
     }

    // output the polydata
    std::stringstream ss;
    ss << config.STLDirectory() << "/" << labels[i] << "-" << i << ".stl";
    std::cout << itksys::SystemTools::GetFilenameName(argv[0])
              << " writing "
              << ss.str() << std::endl;
    writer->SetFileTypeToBinary();
    writer->SetFileName(ss.str().c_str());
    writer->Write();
    confilter->Update();
    if (confilter->GetNumberOfExtractedRegions() > 1)
      {
      std::cout << "WARNING: "
                 << ss.str()
                 << " has "
                 << confilter->GetNumberOfExtractedRegions()
                 << " disconnected labels" << std::endl;
      }
    }
  return EXIT_SUCCESS;
}
