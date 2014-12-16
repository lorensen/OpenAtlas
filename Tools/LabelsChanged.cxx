//
// LabelChanged
//   Usage: Report labels that have changed
//          where
//          OldVolume is a nrrd file containing a 3D volume of
//            discrete labels.
//          NewVolume is a nrrd file containing a 3D volume of
//            discrete labels.
//
#include <itkImageFileReader.h>
#include <itkImageRegionConstIterator.h>
#include <set>

int main (int argc, char *argv[])
{
  if (argc < 3)
    {
    std::cout << "Usage: " << argv[0] << " OldVolume NewVolume" << std::endl;
    return EXIT_FAILURE;
    }
 
  // Create all of the classes we will need
  typedef itk::Image<unsigned short, 3>            ImageType;
  typedef itk::ImageFileReader<ImageType>          ReaderType;
  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;

  ReaderType::Pointer oldReader = ReaderType::New();
  ReaderType::Pointer newReader = ReaderType::New();

  oldReader->SetFileName(argv[1]);
  try
    {
    oldReader->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e;
    return EXIT_FAILURE;
    }

  newReader->SetFileName(argv[2]);
  try
    {
    newReader->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected: "  << e;
    return EXIT_FAILURE;
    }

  ConstIteratorType oldIt(oldReader->GetOutput(),
                          oldReader->GetOutput()->GetLargestPossibleRegion());
  ConstIteratorType newIt(newReader->GetOutput(),
                          newReader->GetOutput()->GetLargestPossibleRegion());
  std::set<unsigned short> changed;
  while (!oldIt.IsAtEnd())
    {
    if (oldIt.Get() != newIt.Get())
      {
      changed.insert(oldIt.Get());
      changed.insert(newIt.Get());
      }
    ++oldIt;
    ++newIt;
    }

  // report labels that have changed
  std::set<unsigned short>::iterator it;
  for (it=changed.begin(); it!=changed.end(); ++it)
    {
    if (*it != 0)
      {
      std::cout << ' ' << *it << std::endl;;
      }
    }

 return EXIT_SUCCESS;
}
