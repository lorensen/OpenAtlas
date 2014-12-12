#include <OpenAtlasUtilities.h>

#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkBalloonRepresentation.h>
#include <vtkBalloonWidget.h>
#include <vtkCamera.h>
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

#include <sstream>

int main (int argc, char *argv[])
{
  if (argc < 4)
    {
    std::cout << "Usage: " << argv[0]
              << " label anatomyFile adjacencyFile [snapshot dir]"
              << std::endl;
    return EXIT_FAILURE;
    }
  unsigned int label = atoi(argv[1]);
  char *anatomyFile = argv[2];
  char *adjacencyFile = argv[3];
  std::vector<std::string> labels;
  std::vector<std::set<unsigned int> > neighbors;
  std::vector<std::vector<float> > colors;

  ReadLabelFile(anatomyFile, labels);
  ReadAdjacenyFile(adjacencyFile, neighbors);
  ReadColorFile(anatomyFile, colors);

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

  std::cout << labels[label] << "(" << label << ")" << " is connected to" << std::endl;
  neighbors[label].insert(label);
  for (std::set<unsigned int>::iterator sit = neighbors[ label ].begin();
       sit != neighbors[ label ].end(); ++sit)
    {
    if (labels[*sit] != labels[label])
      {
      std::cout << "\t" << labels[*sit] << "(" << *sit << ")" <<std::endl;
      }
    if ((*sit == 2 || *sit == 41))
      {
      continue;
      }
    vtkSmartPointer<vtkSTLReader> reader = 
      vtkSmartPointer<vtkSTLReader>::New();
    std::ostringstream fileName;
    fileName << "STL/" << labels[*sit] << ":" << *sit << ".stl" << std::ends;
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
    if (*sit == label)
      {
      actor->GetProperty()->EdgeVisibilityOn();
      actor->GetProperty()->SetEdgeColor(colors[*sit][0] * .4,
                                         colors[*sit][1] * .4,
                                         colors[*sit][2] * .4);
      iconMapper->SetInputConnection(reader->GetOutputPort());
      iconActor->GetProperty()->SetColor(colors[*sit][0],
                                         colors[*sit][1],
                                         colors[*sit][2]);
      }
    leftRenderer->AddActor(actor);
    std::stringstream anatomyLabel;
    anatomyLabel << labels[*sit] << "(" << *sit << ")";
    balloonWidget->AddBalloon(actor, anatomyLabel.str().c_str(), NULL);
    }

  for (std::set<unsigned int>::iterator sit = neighbors[ label ].begin();
       sit != neighbors[ label ].end(); ++sit)
    {
    if (*sit == 2 || *sit == 41)
      {
      continue;
      }
    vtkSmartPointer<vtkPolyDataReader> reader = 
      vtkSmartPointer<vtkPolyDataReader>::New();
    std::ostringstream fileName;
    fileName << "VTK/" << labels[*sit] << ":" << *sit << ".vtk" << std::ends;
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
    actor->GetProperty()->SetEdgeColor(colors[*sit][0] * .4,
                                       colors[*sit][1] * .4,
                                       colors[*sit][2] * .4);
    actor->GetProperty()->EdgeVisibilityOn();
    rightRenderer->AddActor(actor);
    }

  vtkSmartPointer<vtkTextProperty> textProperty =
    vtkSmartPointer<vtkTextProperty>::New();
  textProperty->SetFontSize(16);
  textProperty->SetFontFamilyToCourier();
  textProperty->SetJustificationToCentered();

  vtkSmartPointer<vtkTextMapper> textMapper =
    vtkSmartPointer<vtkTextMapper>::New();
  textMapper->SetTextProperty(textProperty);
  textMapper->SetInput(labels[label].c_str());

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
  
  leftRenderer->SetActiveCamera(camera);
  rightRenderer->SetActiveCamera(camera);

  leftRenderer->ResetCamera();
  leftRenderer->ResetCameraClippingRange();
  camera->Azimuth(30);
  camera->Elevation(30);

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
  if (argc < 5)
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
  
    std::ostringstream snapshotFileName;
    snapshotFileName << argv[4] << "/" << labels[label] << ".png" << std::ends;
    vtkSmartPointer<vtkPNGWriter> writer = 
      vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(snapshotFileName.str().c_str());
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
    }    
  return EXIT_SUCCESS;
}
