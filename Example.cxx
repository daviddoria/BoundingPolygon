// Custom
#include "BoundingPolygon.h"
#include "Helpers.h"

// VTK
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

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
 
  std::vector<unsigned int> path = GetRoughPath(reader->GetOutput()->GetPoints());

  vtkSmartPointer<vtkPolyData> contour =
    vtkSmartPointer<vtkPolyData>::New();

  CreateContourFromPointsAndPath(reader->GetOutput()->GetPoints(), path, contour);

  // Write the file
  vtkSmartPointer<vtkXMLPolyDataWriter> writer =  
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(outputFileName.c_str());
  writer->SetInputData(contour);
  writer->Write();

   
  return EXIT_SUCCESS;
}
