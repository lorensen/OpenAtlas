//
// GenerateModelsFromLabels
//   Usage: GenerateModelsFromLabels AtlasConfigFile StartLabel EndLabel
//          where
//          AtlasConfigFile is an atlas configuration file that defines
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

#include <vtkPolyDataConnectivityFilter.h>
#include <vtkImageAccumulate.h>
#include <vtkImageWrapPad.h>
#include <vtkMaskFields.h>
#include <vtkThreshold.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
 
#include <vtkFieldData.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkMatrix4x4.h>
#include <vtkCellData.h>
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
  itksys::SystemTools::MakeDirectory(config.VTKDirectory());

  std::vector<std::string> labels;
  std::vector<std::vector<float> > colors;

  // Read the label and color file
  try
    {
    ReadLabelFile(config.ColorTableFileName(), labels);
    ReadColorFile(config.ColorTableFileName(), colors);
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
  vtkSmartPointer<vtkThreshold> selector =
    vtkSmartPointer<vtkThreshold>::New();
  vtkSmartPointer<vtkMaskFields> scalarsOff =
    vtkSmartPointer<vtkMaskFields>::New();
  vtkSmartPointer<vtkGeometryFilter> geometry =
    vtkSmartPointer<vtkGeometryFilter>::New();
  vtkSmartPointer<vtkPolyDataWriter> writer =
    vtkSmartPointer<vtkPolyDataWriter>::New();
 
  // Define all of the variables
  unsigned int startLabel = atoi(argv[2]);
  unsigned int endLabel = atoi(argv[3]);
 
  // Generate models from labels
  // 1) Read the volume label file
  // 2) Generate a histogram of the labels
  // 3) Generate models from the labeled volume
  // 4) Output each model into a separate file
 
  reader->SetFileName(config.LabelFileName());

  // Convert to RAS (note: orientation filter uses a different notation)
  orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI); // RAS
  orienter->SetInput(reader->GetOutput());
  orienter->UseImageDirectionOn();
  orienter->Update();

  // Change origin to RAS
  ImageType::PointType origin;
  origin[0] = -orienter->GetOutput()->GetOrigin()[0] - orienter->GetOutput()->GetSpacing()[0] * .5;
  origin[1] = -orienter->GetOutput()->GetOrigin()[1] - orienter->GetOutput()->GetSpacing()[1] * .5;
  origin[2] = orienter->GetOutput()->GetOrigin()[2] + orienter->GetOutput()->GetSpacing()[2] * .5;
  change->SetInput(orienter->GetOutput());
  change->SetOutputOrigin(origin);
  change->ChangeOriginOn();

  connector->SetInput(change->GetOutput());
  connector->Update();

  int *extent = connector->GetOutput()->GetExtent();
  vtkSmartPointer<vtkImageWrapPad> pad =
    vtkSmartPointer<vtkImageWrapPad>::New();
  pad->SetInputData(connector->GetOutput());
  pad->SetOutputWholeExtent(extent[0], extent[1] + 1,
                            extent[2], extent[3] + 1,
                            extent[4], extent[5] + 1);
  pad->Update();

  // Copy the scalar point data of the volume into the scalar cell data
  pad->GetOutput()->GetCellData()->SetScalars(
    connector->GetOutput()->GetPointData()->GetScalars());

  histogram->SetInputData(pad->GetOutput());
  histogram->SetComponentExtent(0, endLabel, 0, 0, 0, 0);
  histogram->SetComponentOrigin(0, 0, 0);
  histogram->SetComponentSpacing(1, 1, 1);
  histogram->Update();
 
  selector->SetInputConnection(pad->GetOutputPort());
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
 
   geometry->Update();
   if (labels[i] == "")
     {
     std::cout << "WARNING: label " << i
               << " is in the label volume but not in the color table and will not be generated" << std::endl;
     continue;
     }
    // output the polydata
    std::stringstream ss;
    ss << config.VTKDirectory() << "/" << labels[i] << "-" << i << ".vtk";
    std::cout << itksys::SystemTools::GetFilenameName(argv[0])
              << " writing "
              << ss.str() << std::endl;
 
    
    // Store some info from the volume in the field data
    vtkSmartPointer<vtkDoubleArray> spacingArray = 
      vtkSmartPointer<vtkDoubleArray>::New();
    double spacingValue[3];
    spacingValue[0] = change->GetOutput()->GetSpacing()[0];
    spacingValue[1] = change->GetOutput()->GetSpacing()[1];
    spacingValue[2] = change->GetOutput()->GetSpacing()[2];
    spacingArray->SetNumberOfComponents(3);
    spacingArray->SetName("Spacing");
    spacingArray->InsertNextTuple(spacingValue);

    vtkSmartPointer<vtkDoubleArray> originArray = 
      vtkSmartPointer<vtkDoubleArray>::New();
    double originValue[3];
    originValue[0] = change->GetOutput()->GetOrigin()[0];
    originValue[1] = change->GetOutput()->GetOrigin()[1];
    originValue[2] = change->GetOutput()->GetOrigin()[2];
    originArray->SetNumberOfComponents(3);
    originArray->SetName("Origin");
    originArray->InsertNextTuple(originValue);

    vtkSmartPointer<vtkFloatArray> colorArray = 
      vtkSmartPointer<vtkFloatArray>::New();
    double colorValue[3];
    colorValue[0] = colors[i][0];
    colorValue[1] = colors[i][1];
    colorValue[2] = colors[i][2];
    colorArray->SetNumberOfComponents(3);
    colorArray->SetName("Color");
    colorArray->InsertNextTuple(colorValue);

    geometry->GetOutput()->GetFieldData()->AddArray(spacingArray);
    geometry->GetOutput()->GetFieldData()->AddArray(originArray);
    geometry->GetOutput()->GetFieldData()->AddArray(colorArray);

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
