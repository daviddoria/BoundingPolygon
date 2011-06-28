/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// This algorithm is based on "Using Aerial Lidar Data to Segment And Model Buildings" by Oliver Wang

// VTK
#include <vtkActor.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkGraphToPolyData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkKdTreePointLocator.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkProperty.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

// ITK
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageRegionConstIterator.h>

// Custom
#include "Helpers.h"
#include "Types.h"

// Boost
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/adjacency_list.hpp>

std::vector<unsigned int> GetRoughPath(vtkPoints* allPoints)
{
  // This function starts at point index 0, and then greedily finds the closest point which has not yet been
  // found. This should hopefully make a reasonable outline of the points.
  // OLD: The loop is closed - that is the first and last elements of the output vector are the same vertex
  // NEW: The loop is open - that is, if you connect all vertices i to i+1, you will not reach the beginning (you would need to additionally
  //      connect the last point to the first point.
  
  std::cout << "The input has " << allPoints->GetNumberOfPoints() << " points." << std::endl;
  
  // Track if each point has been found yet or not
  std::vector<bool> used(allPoints->GetNumberOfPoints(), false);
    
  // Start at point id 0
  unsigned int currentPointId = 0;
  
  // Track the order in which the points were found
  std::vector<unsigned int> pointOrder;
  
  // Loop until all points have been used
  while(Helpers::CountFalse(used) > 1)
    {
    // Mark the current point as used
    //std::cout << "marking " << currentPointId << " as used." << std::endl;
    used[currentPointId] = true;
  
    //std::cout << "There are " << Helpers::CountFalse(used) << " points remaining." << std::endl;
  
    // Save the current point in the point order tracking
    pointOrder.push_back(currentPointId);
  
    // Create a vtkPoints of the remaining points (one of which will be the closest to the current point)
    vtkSmartPointer<vtkPoints> remainingPoints = 
      vtkSmartPointer<vtkPoints>::New();
    
    // Save the mapping from the unused points id to the original point ids
    std::map <unsigned int, unsigned int> idMap;

    // Keep track of which unused point we are on
    unsigned int unusedPointId = 0; 
    for(unsigned int i = 0; i < allPoints->GetNumberOfPoints(); ++i)
      {
      if(used[i])
	{
	continue;
	}
	
      // Store the id mapping in the map
      idMap.insert(std::pair<unsigned int, unsigned int>(unusedPointId, i));
      //std::cout << "adding map entry from " << unusedPointId << " to " << i << std::endl;
      unusedPointId++;
    
      // Add the point to the remainingPoints
      double p[3];
      allPoints->GetPoint(i, p);
      remainingPoints->InsertNextPoint(p);
      }
    
    //std::cout << "remainingPoints has " << remainingPoints->GetNumberOfPoints() << " points." << std::endl;
    
    vtkSmartPointer<vtkPolyData> remainingPolyData = 
      vtkSmartPointer<vtkPolyData>::New();
    remainingPolyData->SetPoints(remainingPoints);
      
    // Create the tree
    vtkSmartPointer<vtkKdTreePointLocator> kDTree = 
      vtkSmartPointer<vtkKdTreePointLocator>::New();
    kDTree->SetDataSet(remainingPolyData);
    kDTree->BuildLocator();
    
    double currentPoint[3];
    allPoints->GetPoint(currentPointId, currentPoint);
    //std::cout << "CurrentPoint is " << currentPoint[0] << " " << currentPoint[1] << " " << currentPoint[2] << std::endl;
    
    // Find the closest points to TestPoint
    unsigned int closestRemainingPointId = kDTree->FindClosestPoint(currentPoint);
    //std::cout << "closestRemainingPointId is " << closestRemainingPointId << std::endl;
    
    // Set the current point to the closest point
    //std::cout << "Changing currentPointId from " << currentPointId << " to " << idMap[closestRemainingPointId] << std::endl;
    currentPointId = idMap[closestRemainingPointId];
    }
    
  // Add the last point to the list, as there is no decision to be made about which one is closest!
  pointOrder.push_back(currentPointId);
  
  // Add the first point to the list again to close the loop
  //pointOrder.push_back(pointOrder[0]);

  return pointOrder;
}

void CreateContourFromPointsAndPath(vtkPoints* points, const std::vector<unsigned int>& path, vtkPolyData* contour)
{
  // Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> lines =
    vtkSmartPointer<vtkCellArray>::New();
 
  for(unsigned int i = 0; i < path.size() - 1; ++i)
    {
    vtkSmartPointer<vtkLine> line =
      vtkSmartPointer<vtkLine>::New();
    line->GetPointIds()->SetId(0,path[i]);
    line->GetPointIds()->SetId(1,path[i+1]);
    lines->InsertNextCell(line);
    }

  // Close the loop
  vtkSmartPointer<vtkLine> line =
      vtkSmartPointer<vtkLine>::New();
  line->GetPointIds()->SetId(0,path[path.size()-1]);
  line->GetPointIds()->SetId(1,0);
  lines->InsertNextCell(line);

  contour->SetPoints(points);

  // Add the lines to the dataset
  contour->SetLines(lines);

  /*
  // Add the contour AND the vertices - this doesn't work for some reason - the edges disappear after the vertex filter
  vtkSmartPointer<vtkPolyData> polydata =
    vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  // Add the lines to the dataset
  polydata->SetLines(lines);

  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexGlyphFilter->AddInputConnection(polydata->GetProducerPort());
  vertexGlyphFilter->Update();
  
  contour->ShallowCopy(vertexGlyphFilter->GetOutput());
  */
}
