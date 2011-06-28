#ifndef BOUNDING_POLYGON_H
#define BOUNDING_POLYGON_H

// STL
#include <vector>

// VTK
#include <vtkPolyData.h>

std::vector<unsigned int> GetRoughPath(vtkPoints* points);

void CreateContourFromPointsAndPath(vtkPoints* points, const std::vector<unsigned int>& path, vtkPolyData* contour);

#endif
