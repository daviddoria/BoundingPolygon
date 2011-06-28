cmake_minimum_required(VERSION 2.6)

PROJECT(BoundingPolygon)

#### Libraries ####
#VTK
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

#ITK
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

#Boost
set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.38 COMPONENTS program_options required)

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${LINK_DIRECTORIES} ${Boost_LIBRARY_DIRS})

#### Executables ####
ADD_EXECUTABLE(BoundingPolygonExample Example.cxx BoundingPolygon.cxx Helpers.cxx)
TARGET_LINK_LIBRARIES(BoundingPolygonExample vtkHybrid vtkInfovis ${ITK_LIBRARIES})

ADD_EXECUTABLE(FindBoundaryPoints FindBoundaryPoints.cxx Helpers.cxx)
TARGET_LINK_LIBRARIES(FindBoundaryPoints vtkHybrid vtkInfovis ${ITK_LIBRARIES})

ADD_EXECUTABLE(CreateContourFromPointsAndPath CreateContourFromPointsAndPath.cxx BoundingPolygon.cxx Helpers.cxx)
TARGET_LINK_LIBRARIES(CreateContourFromPointsAndPath vtkHybrid vtkInfovis ${ITK_LIBRARIES})
