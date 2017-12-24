#include <OpenAtlasUtilities.h>

#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkBalloonRepresentation.h>
#include <vtkBalloonWidget.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtksys/SystemTools.hxx>

#include <sstream>

void Room (vtkSmartPointer<vtkRenderer> &renderer, double bounds[6], bool fromRight);

int main (int argc, char *argv[])
{
  if (argc < 3)
    {
    std::cout << "Usage: " << argv[0]
              << " AtlasConfigFile label [snapshot]"
              << std::endl;
    return EXIT_FAILURE;
    }
  unsigned int label = atoi(argv[2]);

  std::vector<std::string> labels;
  std::vector<std::set<unsigned int> > neighbors;
  std::vector<std::vector<float> > colors;

  OpenAtlas::Configuration config(argv[1]);
  if (argc > 3)
    {
    vtksys::SystemTools::MakeDirectory(config.ScreenshotDirectory());
    }

  ReadLabelFile(config.ColorTableFileName(), labels);
  ReadAdjacenyFile(config.AdjacenciesFileName(), neighbors);
  ReadColorFile(config.ColorTableFileName(), colors);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkSmartPointer<vtkRenderer> leftRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(.6, .5, .4);

  vtkSmartPointer<vtkRenderer> rightRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(.4, .5, .6);

  // An interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();

  // Create the widget
  vtkSmartPointer<vtkBalloonRepresentation> balloonRep =
    vtkSmartPointer<vtkBalloonRepresentation>::New();
  balloonRep->SetBalloonLayoutToImageRight();
 
  vtkSmartPointer<vtkBalloonWidget> balloonWidget =
    vtkSmartPointer<vtkBalloonWidget>::New();
  balloonWidget->SetInteractor(renderWindowInteractor);
  balloonWidget->SetRepresentation(balloonRep);

  vtkSmartPointer<vtkPolyDataMapper> iconMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();

  vtkSmartPointer<vtkActor> iconActor =
    vtkSmartPointer<vtkActor>::New();
  iconActor->SetMapper(iconMapper);

  double bounds[6];
  bounds[0] = VTK_DOUBLE_MAX;
  bounds[1] = VTK_DOUBLE_MIN;
  bounds[2] = VTK_DOUBLE_MAX;
  bounds[3] = VTK_DOUBLE_MIN;
  bounds[4] = VTK_DOUBLE_MAX;
  bounds[5] = VTK_DOUBLE_MIN;

  std::cout << labels[label] << "(" << label << ")" << " is connected to" << std::endl;
  neighbors[label].insert(label);
  for (std::set<unsigned int>::iterator sit = neighbors[ label ].begin();
       sit != neighbors[ label ].end(); ++sit)
    {
    // Skip labels that are not listed in color table file
    if (labels[*sit] == "")
      {
      continue;
      }
    if (labels[*sit] != labels[label])
      {
      std::cout << "\t" << labels[*sit] << "(" << *sit << ")" <<std::endl;
     // Skip displaying adjacent models that have some transparency
     if (colors[*sit][3] != 1.0)
       {
       continue;
       }
      }
    vtkSmartPointer<vtkSTLReader> reader = 
      vtkSmartPointer<vtkSTLReader>::New();
    std::ostringstream fileName;
    fileName << config.STLDirectory() << "/" << labels[*sit] << "-" << *sit << ".stl" << std::ends;
    reader->SetFileName(fileName.str().c_str());
    reader->Update();
    double actorBounds[6];
    reader->GetOutput()->GetBounds(actorBounds);
    for (int b = 0; b < 6; b += 2)
      {
      if (actorBounds[b] < bounds[b])
        {
        bounds[b] = actorBounds[b];
        }
      if (actorBounds[b + 1] > bounds[b + 1])
        {
        bounds[b + 1] = actorBounds[b + 1];
        }
      }
    vtkSmartPointer<vtkPolyDataMapper> mapper = 
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = 
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(colors[*sit][3]);
    actor->GetProperty()->SetColor(colors[*sit][0],
                                   colors[*sit][1],
                                   colors[*sit][2]);
    if (*sit == label)
      {
      actor->GetProperty()->EdgeVisibilityOn();
      actor->GetProperty()->SetEdgeColor(1,
                                         1,
                                         1);
      if (colors[*sit][3] == 0.0)
        {
        actor->GetProperty()->SetOpacity(.4);
        actor->GetProperty()->EdgeVisibilityOff();
        }
      else if (colors[*sit][3] != 1.0)
        {
        actor->GetProperty()->EdgeVisibilityOff();
        }
      iconMapper->SetInputConnection(reader->GetOutputPort());
      iconActor->GetProperty()->SetColor(colors[*sit][0],
                                         colors[*sit][1],
                                         colors[*sit][2]);
      iconActor->GetProperty()->SetOpacity(1.0);
      }
    leftRenderer->AddActor(actor);
    std::stringstream anatomyLabel;
    anatomyLabel << labels[*sit] << "(" << *sit << ")";
    balloonWidget->AddBalloon(actor, anatomyLabel.str().c_str(), NULL);
    }
  
  // Adjust bounds
  for (int b = 0; b < 6; b += 2)
    {
    bounds[b] = bounds[b] - (bounds[b + 1] - bounds[b]) * .1;
    bounds[b + 1] = bounds[b + 1] + (bounds[b + 1] - bounds[b]) * .1;
    }

  for (std::set<unsigned int>::iterator sit = neighbors[ label ].begin();
       sit != neighbors[ label ].end(); ++sit)
    {
    if (labels[*sit] == "")
      {
      continue;
      }
    if (labels[*sit] != labels[label] && colors[*sit][3] != 1.0)
      {
      continue;
      }
    vtkSmartPointer<vtkPolyDataReader> reader = 
      vtkSmartPointer<vtkPolyDataReader>::New();
    std::ostringstream fileName;
    fileName << config.VTKDirectory() << "/" << labels[*sit] << "-" << *sit << ".vtk" << std::ends;
    reader->SetFileName(fileName.str().c_str());
    vtkSmartPointer<vtkPolyDataMapper> mapper = 
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = 
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors[*sit][0],
                                   colors[*sit][1],
                                   colors[*sit][2]);
    actor->GetProperty()->SetEdgeColor(colors[*sit][0] * 1.1,
                                       colors[*sit][1] * 1.1,
                                       colors[*sit][2] * 1.1);
    actor->GetProperty()->EdgeVisibilityOn();
    rightRenderer->AddActor(actor);
    }

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

  // Set up the widget
  vtkSmartPointer<vtkOrientationMarkerWidget> orientationWidget =
    vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  orientationWidget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
  orientationWidget->SetOrientationMarker( iconActor );
  orientationWidget->SetInteractor( renderWindowInteractor );
  orientationWidget->SetViewport( 0.0, 0.0, 0.15, 0.15 );

  // A renderer and render window
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
  camera->Elevation(30);
  
  if (labels[label].find("right") != std::string::npos ||
      labels[label].find("Right") != std::string::npos)
    {
    camera->Azimuth(-30);
    Room(rightRenderer, bounds, false);
    Room(leftRenderer, bounds, false);
    }
  else
    {
    camera->Azimuth(30);
    Room(rightRenderer, bounds, true);
    Room(leftRenderer, bounds, true);
    }

  leftRenderer->SetActiveCamera(camera);
  rightRenderer->SetActiveCamera(camera);

  leftRenderer->ResetCamera();
  leftRenderer->ResetCameraClippingRange();
  rightRenderer->ResetCameraClippingRange();


  // Render
  renderWindow->Render();
  balloonWidget->EnabledOn();
  orientationWidget->EnabledOn();
  orientationWidget->InteractiveOff();

  vtkSmartPointer<vtkActor2D> textActor =
    vtkSmartPointer<vtkActor2D>::New();
  textActor->SetMapper(textMapper);
  textActor->SetPosition(320, 16);

  leftRenderer->AddActor(textActor);
  renderWindow->Render();

  // Begin mouse interaction
  if (argc < 4)
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
    windowToImageFilter->ReadFrontBufferOff();
    windowToImageFilter->Modified();
    windowToImageFilter->Update();
  
    std::ostringstream snapshotFileName;
    snapshotFileName << config.ScreenshotDirectory() << "/" << labels[label] << ".png" << std::ends;
    vtkSmartPointer<vtkPNGWriter> writer = 
      vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(snapshotFileName.str().c_str());
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
    }    
  return EXIT_SUCCESS;
}
void Room (vtkSmartPointer<vtkRenderer> &renderer, double bounds[6], bool fromRight)
{
#define THICKNESS 0.0
#define OPACITY .2
  vtkSmartPointer<vtkCubeSource> basePlane =
    vtkSmartPointer<vtkCubeSource>::New();
  basePlane->SetCenter((bounds[1] + bounds[0]) / 2.0,
                       bounds[2] + THICKNESS / 2.0,
                       (bounds[5] + bounds[4]) / 2.0);
  basePlane->SetXLength(bounds[1] - bounds[0]);
  basePlane->SetYLength(THICKNESS);
  basePlane->SetZLength(bounds[5] - bounds[4]);
  vtkSmartPointer<vtkPolyDataMapper> baseMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  baseMapper->SetInputConnection(basePlane->GetOutputPort());
  vtkSmartPointer<vtkActor> base =
    vtkSmartPointer<vtkActor>::New();
  base->SetMapper(baseMapper);
  base->GetProperty()->SetOpacity(OPACITY);
  base->GetProperty()->EdgeVisibilityOn();
  renderer->AddActor(base);

  vtkSmartPointer<vtkCubeSource> backPlane =
    vtkSmartPointer<vtkCubeSource>::New();
  backPlane->SetCenter((bounds[1] + bounds[0]) / 2.0,
                       (bounds[3] + bounds[2]) / 2.0,
                       bounds[4] + THICKNESS / 2.0);
  backPlane->SetXLength(bounds[1] - bounds[0]);
  backPlane->SetYLength(bounds[3] - bounds[2]);
  backPlane->SetZLength(THICKNESS);
  vtkSmartPointer<vtkPolyDataMapper> backMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  backMapper->SetInputConnection(backPlane->GetOutputPort());
  vtkSmartPointer<vtkActor> back =
    vtkSmartPointer<vtkActor>::New();
  back->SetMapper(backMapper);
  back->GetProperty()->SetOpacity(OPACITY);
  back->GetProperty()->EdgeVisibilityOn();
  renderer->AddActor(back);

  if (fromRight)
    {
    vtkSmartPointer<vtkCubeSource> leftPlane =
      vtkSmartPointer<vtkCubeSource>::New();
    leftPlane->SetCenter(bounds[1] - THICKNESS / 2.0,
                         (bounds[3] + bounds[2]) / 2.0,
                         (bounds[5] + bounds[4]) / 2.0);
    leftPlane->SetXLength(THICKNESS);
    leftPlane->SetYLength(bounds[3] - bounds[2]);
    leftPlane->SetZLength(bounds[5] - bounds[4]);
    vtkSmartPointer<vtkPolyDataMapper> leftMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    leftMapper->SetInputConnection(leftPlane->GetOutputPort());
    vtkSmartPointer<vtkActor> left =
      vtkSmartPointer<vtkActor>::New();
    left->SetMapper(leftMapper);
    left->GetProperty()->SetOpacity(OPACITY);
    left->GetProperty()->EdgeVisibilityOn();
    renderer->AddActor(left);
    }
  else
    {
    vtkSmartPointer<vtkCubeSource> rightPlane =
      vtkSmartPointer<vtkCubeSource>::New();
    rightPlane->SetCenter(bounds[0] + THICKNESS / 2.0,
                          (bounds[3] + bounds[2]) / 2.0,
                          (bounds[5] + bounds[4]) / 2.0);
    rightPlane->SetXLength(THICKNESS);
    rightPlane->SetYLength(bounds[3] - bounds[2]);
    rightPlane->SetZLength(bounds[5] - bounds[4]);
    vtkSmartPointer<vtkPolyDataMapper> rightMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    rightMapper->SetInputConnection(rightPlane->GetOutputPort());
    vtkSmartPointer<vtkActor> right =
      vtkSmartPointer<vtkActor>::New();
    right->SetMapper(rightMapper);
    right->GetProperty()->SetOpacity(OPACITY);
    right->GetProperty()->EdgeVisibilityOn();
    renderer->AddActor(right);
    }
}
