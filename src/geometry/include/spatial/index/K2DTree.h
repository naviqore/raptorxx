//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef K2DTREE_H
#define K2DTREE_H

#include <vector>
#include <algorithm>
#include <memory>
#include <concepts>
#include <functional>
#include "../CoordinateComponent.h"
#include "../../utils/utils.h"

#include <stdexcept>

namespace geometry::kd_tree {
  // https://medium.com/smucs/a-look-into-k-dimensional-trees-290ec69dffe9#:~:text=What%20is%20a%20K%2DDimensional,in%20the%20multi%20dimensional%20space.
  template<CoordinateComponentConcept T>
  class K2DTree
  {
    struct Node
    {
      T point;
      std::shared_ptr<Node> left{nullptr};
      std::shared_ptr<Node> right{nullptr};

      explicit Node(const T& point)
        : point(point) {}
    };

    std::shared_ptr<Node> root;
    const int kDimensions = 2; // 2D tree
    std::function<int(int, int)> axisFunc = [](const int depth, const int kDimension) { return depth % kDimension; };

    std::shared_ptr<Node> buildTree(std::vector<T>& points, const int depth = 0) {
      if (points.empty())
      {
        // base case
        return nullptr;
      }
      int axis = axisFunc(depth, kDimensions);

      std::ranges::sort(points, [axis](CoordinateComponentConcept auto& a, CoordinateComponentConcept auto& b) {
        switch (axis)
        {
          case 0: // x-axis
            return a.getFirstCoordinate() < b.getFirstCoordinate();
          case 1: //  y-axis
            return a.getSecondCoordinate() < b.getSecondCoordinate();
          default:
            return false;
        }
      });

      const auto median = points.size() / 2;
      auto node = std::make_shared<Node>(points[median]);

      std::vector<T> left_points(points.begin(), points.begin() + median);
      std::vector<T> right_points(points.begin() + median + 1, points.end());

      node->left = buildTree(left_points, depth + 1);
      node->right = buildTree(right_points, depth + 1);

      return node;
    }

  public:
    explicit K2DTree(std::vector<T> points) {
      root = buildTree(points);
    }

    K2DTree(std::initializer_list<T> points) {
      std::vector<T> pointsVector(points);
      root = buildTree(pointsVector);
    }

    void nearest(const std::shared_ptr<Node>& node, const T& point, const int depth, std::shared_ptr<Node>& best, double& bestDist) {
      if (nullptr == node)
      {
        return;
      }

      if (const double distance = utils::haversineDistance(node->point, point);
          distance < bestDist)
      {
        bestDist = distance;
        best = node;
      }

      int axis = axisFunc(depth, kDimensions);
      const double difference = utils::getCoordinateComponent<T>(point, axis) - utils::getCoordinateComponent<T>(node->point, axis);

      std::shared_ptr<Node> near, far;
      if (difference <= 0)
      {
        near = node->left;
        far = node->right;
      }
      else
      {
        near = node->right;
        far = node->left;
      }

      nearest(near, point, depth + 1, best, bestDist);

      //
      if (difference * difference < bestDist)
      {
        nearest(far, point, depth + 1, best, bestDist);
      }
    }

    T nearest(const T& point) {
      std::shared_ptr<Node> best = nullptr;
      double bestDist = std::numeric_limits<double>::infinity(); // start with INFINITY
      nearest(root, point, 0, best, bestDist);
      if (nullptr == best)
      {
        throw std::runtime_error("No nearest point found");
      }
      return best->point;
    }

    void rangeSearch(const std::shared_ptr<Node>& node, const T& point, const double range, const int depth, std::vector<T>& results) {
      if (nullptr == node)
      {
        return;
      }

      if (const double distance = utils::haversineDistance(node->point, point);
          distance <= range)
      {
        results.push_back(node->point);
      }

      int axis = axisFunc(depth, kDimensions);
      const double difference = utils::getCoordinateComponent<T>(point, axis) - utils::getCoordinateComponent<T>(node->point, axis);

      if (difference <= range)
      {
        rangeSearch(node->left, point, range, depth + 1, results);
      }
      if (difference >= -range)
      {
        rangeSearch(node->right, point, range, depth + 1, results);
      }
    }

    std::vector<T> rangeSearch(const T& point, const double range) {
      std::vector<T> results;
      rangeSearch(root, point, range, 0, results);
      return results;
    }
  };

} // namespace geometry

#endif //K2DTREE_H
