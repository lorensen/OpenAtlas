//
// LabelDiff
//   Usage: Report differences for a label
//          where
//          Label is the label to be diff'ed
//          OldVolume is a nrrd file containing a 3D volume of
//            discrete labels.
//          NewVolume is a nrrd file containing a 3D volume of
//            discrete labels.
//          LabelLUT is the label lookup table file
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
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkThreshold.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
 
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <vtkCellData.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
 
#include <OpenAtlasUtilities.h>

#include <sstream>
#include <string>
#include <vector>
#include <fstream>

int main (int argc, char *argv[])
{
  if (argc < 4)
    {
    std::cout << "Usage: " << argv[0] << " Label OldVolume NewVolume LabelLUT" << std::endl;
    return EXIT_FAILURE;
    }
 
  unsigned int label = atoi(argv[1]);

  // Read the label file
  std::vector<std::string> labels;
  std::vector<std::vector<float> > colors;
  try
    {
    ReadLabelFile(argv[4], labels);
    ReadColorFile(argv[4], colors);
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e;
    return EXIT_FAILURE;
    }

  // Create all of the classes we will need
  typedef itk::Image<unsigned short, 3>            ImageType;

  typedef itk::ImageFileReader<ImageType>          ReaderType;
  ReaderType::Pointer oldReader = ReaderType::New();
  ReaderType::Pointer newReader = ReaderType::New();

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
 
  oldReader->SetFileName(argv[2]);
  try
    {
    oldReader->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e;
    return EXIT_FAILURE;
    }

  newReader->SetFileName(argv[3]);
  try
    {
    newReader->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e;
    return EXIT_FAILURE;
    }

  // Create a new label volume
  ImageType::Pointer diffImage = ImageType::New();
  diffImage->SetRegions(newReader->GetOutput()->GetLargestPossibleRegion());
  diffImage->Allocate();
  diffImage->FillBuffer(0);

  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
  typedef itk::ImageRegionIterator< ImageType>       IteratorType;
  ConstIteratorType oldIt(oldReader->GetOutput(),
                          oldReader->GetOutput()->GetLargestPossibleRegion());
  ConstIteratorType newIt(newReader->GetOutput(),
                          newReader->GetOutput()->GetLargestPossibleRegion());
  IteratorType diffIt(diffImage,
                      diffImage->GetLargestPossibleRegion());

  while (!oldIt.IsAtEnd())
    {
    // no change
    if (oldIt.Get() == label && newIt.Get() == label)
      {
      diffIt.Set(label);
      }
    // added
    else if (oldIt.Get() != label && newIt.Get() == label)
      {
      diffIt.Set(label + 1);
      }
    // removed
    else if (oldIt.Get() == label && newIt.Get() != label)
      {
      diffIt.Set(20000);
      }
    ++oldIt;
    ++newIt;
    ++diffIt;
    }

  // Now, convert the point data to cell data
  orienter->SetDesiredCoordinateOrientation(
    itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALS);
  orienter->SetInput(diffImage);

  ImageType::PointType origin;
  origin.Fill(-127.5);
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

  selector->SetInputConnection(pad->GetOutputPort());
  selector->SetInputArrayToProcess(0, 0, 0,
                                   vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                   vtkDataSetAttributes::SCALARS);
   // Shift the geometry by 1/2
  vtkSmartPointer<vtkTransform> transform =
    vtkSmartPointer<vtkTransform>::New();
  transform->Translate (-.5, -.5, -.5);

  vtkSmartPointer<vtkTransformFilter> transformModel =
    vtkSmartPointer<vtkTransformFilter>::New();
  transformModel->SetTransform(transform);
  transformModel->SetInputConnection(selector->GetOutputPort());

  // Strip the scalars from the output
  scalarsOff->SetInputConnection(transformModel->GetOutputPort());
  scalarsOff->CopyAttributeOff(vtkMaskFields::POINT_DATA,
                               vtkDataSetAttributes::SCALARS);
  scalarsOff->CopyAttributeOff(vtkMaskFields::CELL_DATA,
                               vtkDataSetAttributes::SCALARS);
 
  geometry->SetInputConnection(scalarsOff->GetOutputPort());

  // actors
  vtkSmartPointer<vtkTextProperty> textProperty =
    vtkSmartPointer<vtkTextProperty>::New();
  textProperty->SetFontSize(16);
  textProperty->SetFontFamilyToCourier();
  textProperty->SetJustificationToCentered();

  std::ostringstream partName;
  partName << labels[label] << "(" << label << ")" << std::ends;
  vtkSmartPointer<vtkTextMapper> textMapper =
    vtkSmartPointer<vtkTextMapper>::New();
  textMapper->SetTextProperty(textProperty);
  textMapper->SetInput(partName.str().c_str());

  vtkSmartPointer<vtkActor2D> textActor =
    vtkSmartPointer<vtkActor2D>::New();
  textActor->SetMapper(textMapper);
  textActor->SetPosition(320, 16);

  // Generate the polydata for the original, added and removed voxels
  vtkSmartPointer<vtkPolyData> original =
    vtkSmartPointer<vtkPolyData>::New();
  selector->ThresholdBetween(label, label);
  geometry->Update();
  original->DeepCopy(geometry->GetOutput());

  vtkSmartPointer<vtkPolyData> newPD =
    vtkSmartPointer<vtkPolyData>::New();
  selector->ThresholdBetween(label, label + 1);
  geometry->Update();
  newPD->DeepCopy(geometry->GetOutput());

  vtkSmartPointer<vtkPolyData> removed =
    vtkSmartPointer<vtkPolyData>::New();
  selector->ThresholdBetween(20000, 20000);
  geometry->Update();
  removed->DeepCopy(geometry->GetOutput());

  vtkSmartPointer<vtkPolyData> added =
    vtkSmartPointer<vtkPolyData>::New();
  selector->ThresholdBetween(label + 1, label + 1);
  geometry->Update();
  added->DeepCopy(geometry->GetOutput());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  vtkSmartPointer<vtkRenderer> leftRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(.6, .5, .4);

  vtkSmartPointer<vtkRenderer> rightRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(.4, .5, .6);

  leftRenderer->AddActor(textActor);

  // An interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();

  vtkSmartPointer<vtkPolyDataMapper> originalMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  originalMapper->SetInputData(original);
  vtkSmartPointer<vtkActor> originalActor = 
    vtkSmartPointer<vtkActor>::New();
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(colors[label][0],
                                 colors[label][1],
                                 colors[label][2]);
  originalActor->GetProperty()->SetRepresentationToWireframe();

  vtkSmartPointer<vtkPolyDataMapper> newMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  newMapper->SetInputData(newPD);
  vtkSmartPointer<vtkActor> newActor = 
    vtkSmartPointer<vtkActor>::New();
  newActor->SetMapper(newMapper);
  newActor->GetProperty()->SetColor(colors[label][0],
                                 colors[label][1],
                                 colors[label][2]);

  vtkSmartPointer<vtkPolyDataMapper> removedMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  removedMapper->SetInputData(removed);
  vtkSmartPointer<vtkActor> removedActor = 
    vtkSmartPointer<vtkActor>::New();
  removedActor->SetMapper(removedMapper);
  removedActor->GetProperty()->SetColor(1.0,
                                        0.0,
                                        0.0);

  vtkSmartPointer<vtkPolyDataMapper> addedMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  addedMapper->SetInputData(added);
  vtkSmartPointer<vtkActor> addedActor = 
    vtkSmartPointer<vtkActor>::New();
  addedActor->SetMapper(addedMapper);
  addedActor->GetProperty()->SetColor(0.0,
                                      1.0,
                                      0.0);

  leftRenderer->AddActor(originalActor);
  leftRenderer->AddActor(removedActor);
  leftRenderer->AddActor(addedActor);
  rightRenderer->AddActor(newActor);

  vtkSmartPointer<vtkRenderWindow> renderWindow = 
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(1280, 640);
  renderWindow->AddRenderer(leftRenderer);
  renderWindow->AddRenderer(rightRenderer);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  vtkSmartPointer<vtkCamera> camera =
    vtkSmartPointer<vtkCamera>::New();
  camera->SetViewUp(0, 0, 1);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetPosition(0, 1, 0);

  leftRenderer->SetActiveCamera(camera);
  rightRenderer->SetActiveCamera(camera);

  leftRenderer->ResetCamera();
  leftRenderer->ResetCameraClippingRange();
  camera->Azimuth(30);
  camera->Elevation(30);

  renderWindow->Render();

  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

 return EXIT_SUCCESS;
}
