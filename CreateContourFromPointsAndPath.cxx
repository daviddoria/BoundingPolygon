// Custom
#include "BoundingPolygon.h"
#include "Helpers.h"

// VTK
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

std::vector<std::string> split(const std::string &s);

int main(int argc, char *argv[])
{
  // Verify arguments
  if(argc < 3)
    {
    std::cerr << "Required arguments: inputFileName.vtp outputFileName.vtp" << std::endl;
    return EXIT_FAILURE;
    }
  
  // Parse arguments
  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];
  
  // Output arguments
  std::cout << "Input: " << inputFileName << std::endl;
  std::cout << "Output: " << outputFileName << std::endl;
  
  // Read the file
  vtkSmartPointer<vtkXMLPolyDataReader> reader =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(inputFileName.c_str());
  reader->Update();
 
  std::string myfile = "0 1 2 3 5 7 9 11 13 15 16 18 20 21 23 25 26 28 30 32 35 37 40 39 38 36 34 33 31 29 27 24 22 4 6 8 10 12 14 17 19 0";
  std::vector<std::string> myfileNumbers = split(myfile);
  std::cout << myfileNumbers.size() << std::endl;
  
  std::vector<unsigned int> path(myfileNumbers.size());
  
  for(unsigned int i = 0; i < myfileNumbers.size(); ++i)
    {
    std::stringstream ss;
    ss << myfileNumbers[i];
    ss >> path[i];
    std::cout << path[i] << " ";
    }
  std::cout << std::endl;
    
  vtkSmartPointer<vtkPolyData> contour =
    vtkSmartPointer<vtkPolyData>::New();

  CreateContourFromPointsAndPath(reader->GetOutput()->GetPoints(), path, contour);

  // Write the file
  vtkSmartPointer<vtkXMLPolyDataWriter> writer =  
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(outputFileName.c_str());
  writer->SetInputConnection(contour->GetProducerPort());
  writer->Write();

  return EXIT_SUCCESS;
}

std::vector<std::string> split(const std::string &s)
{
  std::istringstream iss(s);
  std::vector<std::string> words;
  std::copy(std::istream_iterator<std::string>(iss),
             std::istream_iterator<std::string>(),
             std::back_inserter<std::vector<std::string> >(words));
  return words;
}
