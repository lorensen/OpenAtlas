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
#include <vtksys/SystemTools.hxx>

#include <vtkSmartPointer.h>
#include <vtkImageAccumulate.h>
#include <vtkImageWrapPad.h>
#include <vtkMaskFields.h>
#include <vtkThreshold.h>
#include <vtkGeometryFilter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
 
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextMapper.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkActor2D.h>

#include <vtkCellData.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
 
#include <OpenAtlasUtilities.h>

#include <sstream>
#include <string>
#include <vector>
#include <map>

int main (int argc, char *argv[])
{
  if (argc < 6)
    {
    std::cout << "Usage: " << argv[0] << " Label OldVolume NewVolume LabelLUT SnapshotDir" << std::endl;
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

  vtkSmartPointer<vtkThreshold> selector =
    vtkSmartPointer<vtkThreshold>::New();
  vtkSmartPointer<vtkMaskFields> scalarsOff =
    vtkSmartPointer<vtkMaskFields>::New();
  vtkSmartPointer<vtkGeometryFilter> geometry =
    vtkSmartPointer<vtkGeometryFilter>::New();
 
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
  diffImage->SetOrigin(newReader->GetOutput()->GetOrigin());
  diffImage->SetSpacing(newReader->GetOutput()->GetSpacing());
  diffImage->SetDirection(newReader->GetOutput()->GetDirection());

  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
  typedef itk::ImageRegionIterator< ImageType>       IteratorType;
  ConstIteratorType oldIt(oldReader->GetOutput(),
                          oldReader->GetOutput()->GetLargestPossibleRegion());
  ConstIteratorType newIt(newReader->GetOutput(),
                          newReader->GetOutput()->GetLargestPossibleRegion());
  IteratorType diffIt(diffImage,
                      diffImage->GetLargestPossibleRegion());

  std::map<unsigned int, unsigned int> addedLabels;
  std::map<unsigned int, unsigned int>::iterator aIt;

  std::map<unsigned int, unsigned int> addedFromBackgroundLabels;
  std::map<unsigned int, unsigned int>::iterator abIt;

  std::map<unsigned int, unsigned int> removedLabels;
  std::map<unsigned int, unsigned int>::iterator rIt;

  while (!oldIt.IsAtEnd())
    {
    // no change
    if (oldIt.Get() == label && newIt.Get() == label)
      {
      diffIt.Set(label);
      }
    // added from background
    else if (oldIt.Get() != label && newIt.Get() == label && oldIt.Get() == 0)
      {
      diffIt.Set(label + 2);
      abIt = addedFromBackgroundLabels.find(oldIt.Get());
      if (abIt == addedFromBackgroundLabels.end())
        {
        addedFromBackgroundLabels[oldIt.Get()] = 1;
        }
      else
        {
        addedFromBackgroundLabels[oldIt.Get()] = addedFromBackgroundLabels[oldIt.Get()] + 1;
        }
      }
    // added from another label
    else if (oldIt.Get() != label && newIt.Get() == label)
      {
      diffIt.Set(label + 1);
      aIt = addedLabels.find(oldIt.Get());
      if (aIt == addedLabels.end())
        {
        addedLabels[oldIt.Get()] = 1;
        }
      else
        {
        addedLabels[oldIt.Get()] = addedLabels[oldIt.Get()] + 1;
        }
      }
    // removed
    else if (oldIt.Get() == label && newIt.Get() != label)
      {
      diffIt.Set(20000);
      rIt = removedLabels.find(newIt.Get());
      if (rIt == removedLabels.end())
        {
        removedLabels[newIt.Get()] = 1;
        }
      else
        {
        removedLabels[newIt.Get()] = removedLabels[newIt.Get()] + 1;        
        }
      }
    ++oldIt;
    ++newIt;
    ++diffIt;
    }

  // Output is in markdown format
  std::cout << "* " << labels[label] << "  " << std::endl;
  for(std::map<unsigned int, unsigned int>::iterator a = addedLabels.begin(); a != addedLabels.end(); ++a)
    {
    std::cout << "     added " << a->second << " voxel(s) from " << (a->first ? labels[a->first] : "background") << "  " << std::endl;
    }
  for(std::map<unsigned int, unsigned int>::iterator ab = addedFromBackgroundLabels.begin(); ab != addedFromBackgroundLabels.end(); ++ab)
    {
    std::cout << "     added " << ab->second << " voxel(s) from background" << "  " << std::endl;
    }
  for(std::map<unsigned int, unsigned int>::iterator r = removedLabels.begin(); r != removedLabels.end(); ++r)
    {
    std::cout << "     changed " << r->second << " voxel(s) to " << (r->first ? labels[r->first] : "background") << "  " << std::endl;
    }

  // Now, convert the point data to cell data
  orienter->SetDesiredCoordinateOrientation(
    itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI);
  orienter->SetInput(diffImage);
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
  selector->ThresholdBetween(label, label + 2);
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

  vtkSmartPointer<vtkPolyData> addedFromBackground =
    vtkSmartPointer<vtkPolyData>::New();
  selector->ThresholdBetween(label + 2, label + 2);
  geometry->Update();
  addedFromBackground->DeepCopy(geometry->GetOutput());

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
  // Color original label color
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
  // Color original label color
  newActor->GetProperty()->SetColor(colors[label][0],
                                 colors[label][1],
                                 colors[label][2]);

  vtkSmartPointer<vtkPolyDataMapper> removedMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  removedMapper->SetInputData(removed);
  vtkSmartPointer<vtkActor> removedActor = 
    vtkSmartPointer<vtkActor>::New();
  removedActor->SetMapper(removedMapper);
  // Color red
  removedActor->GetProperty()->SetColor(1.0,
                                        0.0,
                                        0.0);

  vtkSmartPointer<vtkPolyDataMapper> addedMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  addedMapper->SetInputData(added);
  vtkSmartPointer<vtkActor> addedActor = 
    vtkSmartPointer<vtkActor>::New();
  addedActor->SetMapper(addedMapper);
  // Color green
  addedActor->GetProperty()->SetColor(0.0,
                                      1.0,
                                      0.0);

  vtkSmartPointer<vtkPolyDataMapper> addedFromBackgroundMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  addedFromBackgroundMapper->SetInputData(addedFromBackground);
  vtkSmartPointer<vtkActor> addedFromBackgroundActor = 
    vtkSmartPointer<vtkActor>::New();
  addedFromBackgroundActor->SetMapper(addedFromBackgroundMapper);
  // Color black
  addedFromBackgroundActor->GetProperty()->SetColor(0.0,
                                                    0.0,
                                                    0.0);

  leftRenderer->AddActor(originalActor);
  leftRenderer->AddActor(removedActor);
  leftRenderer->AddActor(addedActor);
  leftRenderer->AddActor(addedFromBackgroundActor);
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

  if (argc < 6)
    {
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
    }
  else
    {
    // Screenshot  
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = 
      vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->Update();
  
    vtksys::SystemTools::MakeDirectory(argv[5]);
    std::ostringstream snapshotFileName;
    snapshotFileName << argv[5] << "/" << labels[label] << "_diff.png" << std::ends;

    vtkSmartPointer<vtkPNGWriter> writer = 
      vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(snapshotFileName.str().c_str());
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
    }
 return EXIT_SUCCESS;
}
