#include "Helpers.h"

// ITK
#include "itkImageRegionConstIterator.h"

// VTK
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkPolyLine.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

// Boost
#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace Helpers
{

void PixelListToPolyData(std::vector<itk::Index<2> > pixelList, vtkSmartPointer<vtkPolyData> polydata)
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  for(unsigned int i = 0; i < pixelList.size(); i++)
    {
    points->InsertNextPoint(pixelList[i][0], pixelList[i][1], 0);
    //std::cout << pixelList[i][0] << " " << pixelList[i][1] << std::endl;
    }
  polydata->SetPoints(points);
  //std::cout << "There are " << polydata->GetNumberOfPoints() << " points." << std::endl;
}



unsigned int FindKeyByValue(std::map <unsigned int, unsigned int> myMap, unsigned int value)
{
  std::map<unsigned int, unsigned int>::const_iterator iterator;
  for (iterator = myMap.begin(); iterator != myMap.end(); ++iterator)
    {
    if (iterator->second == value)
      {
      return iterator->first;
      break;
      }
    }
    
  std::cout << "Key was not found for value: " << value << std::endl;
  exit(-1);
  
  // Prevent compiler warning about no return value. This should never be reached.
  return 0;
}
  
unsigned int CountFalse(std::vector<bool> boolVector)
{
  unsigned int numberFalse = 0;
  for(unsigned int i = 0; i < boolVector.size(); ++i)
    {
    if(!boolVector[i])
      {
      numberFalse++;
      }
    }
  return numberFalse;
}


float GetDistanceBetweenPoints(vtkPolyData* polydata, vtkIdType a, vtkIdType b)
{
  double currentPoint[3];
  double nextPoint[3];
  
  polydata->GetPoint(a, currentPoint);
  polydata->GetPoint(b, nextPoint);
  return sqrt(vtkMath::Distance2BetweenPoints(currentPoint, nextPoint));  
}

void OutputVector(std::vector<unsigned int> &v)
{
  for(unsigned int i = 0; i < v.size(); ++i)
    {
    std::cout << v[i] << " ";
    }
  std::cout << std::endl;
}

void WritePoints(vtkPolyData* polydata, std::string filename)
{
  // Write the points ina polydata to a vtp file
  
  vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  glyphFilter->SetInputData(polydata);
  glyphFilter->Update();
  
  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(filename.c_str());
  writer->SetInputData(glyphFilter->GetOutput());
  writer->Write();
}

void WritePathAsPolyLine(std::vector<unsigned int> order, vtkPolyData* graphPolyData, std::string filename)
{
  // Save the order as a PolyLine in a PolyData
  vtkSmartPointer<vtkPolyLine> polyLine = 
    vtkSmartPointer<vtkPolyLine>::New();
  polyLine->GetPointIds()->SetNumberOfIds(order.size());
  for(unsigned int i = 0; i < order.size(); ++i)
    {
    polyLine->GetPointIds()->SetId(i,order[i]);
    //std::cout << "Line point " << i << " is id " << order[i] << std::endl;
    }
    
  // Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> cells = 
      vtkSmartPointer<vtkCellArray>::New();
  cells->InsertNextCell(polyLine);
  
  // Create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> polyData = 
    vtkSmartPointer<vtkPolyData>::New();
  
  // Add the points to the dataset
  polyData->SetPoints(graphPolyData->GetPoints());
  
  // Add the lines to the dataset
  polyData->SetLines(cells);
  
  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(filename.c_str());
  writer->SetInputData(polyData);
  writer->Write();
   
}

void WritePathAsLines(std::vector<unsigned int> order, vtkPolyData* polyData, std::string filename)
{
  // Save the order as a PolyLine in a PolyData
  vtkSmartPointer<vtkCellArray> lines =
    vtkSmartPointer<vtkCellArray>::New();

  for(unsigned int i = 0; i < order.size(); ++i)
    {

    vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();

    line->GetPointIds()->SetId(0,order[i]);
    if(i != order.size()-1)
    {
      line->GetPointIds()->SetId(1,order[i+1]);
    }
    else // connect the last point to the first point
    {
      line->GetPointIds()->SetId(1,order[0]);
    }

    lines->InsertNextCell(line);
    }

  vtkSmartPointer<vtkPolyData> outputPolyData = 
    vtkSmartPointer<vtkPolyData>::New();

  // Add the points to the dataset
  outputPolyData->SetPoints(polyData->GetPoints());

  // Add the lines to the dataset
  outputPolyData->SetLines(lines);

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(filename.c_str());
  writer->SetInputData(outputPolyData);
  writer->Write();

}

} // end namespace Helpers

