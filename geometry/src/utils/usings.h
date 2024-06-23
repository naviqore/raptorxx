//
// Created by MichaelBrunner on 20/06/2024.
//

#ifndef USINGS_H
#define USINGS_H

#include <K2DTree.h>
#include <CoordinateComponent.h>
#include <Coordinate.h>

namespace geometry::kd_tree {
  using spatialCoordinate = Coordinate<double>;
  using coordinateComponent = CoordinateComponent<spatialCoordinate>;
  using spatialK2dTree = K2DTree<coordinateComponent>;
}


#endif //USINGS_H
