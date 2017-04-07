#ifndef RAY_TRACER_H_
#define RAY_TRACER_H_

#include <vector>

#include <SFML/Graphics/Image.hpp>

#include "color.h"
#include "geometry.h"
#include "entity.h"
#include "kdtree.h"

class RayTracer {
 public:
  struct Screen {
    Point position;
    Vector x;
    Vector y;
  };

  RayTracer(std::vector<Entity *> scene, Real base_illuminance = 0);

  void AddLightSource(Point position, Real intensity);

  sf::Image Render(Point observer, Screen screen,
                   unsigned int width, unsigned int height,
                   unsigned int antialiasing);

  sf::Image Render(Point observer, Screen screen,
                   unsigned int width, unsigned int height);

 private:
  struct LightSource {
    Point position;
    Real intensity;
  };

  Entity * Trace(Ray ray, Point *intersection) const;
  Entity * TraceNode(KDTree::KDNode &node, Real t_in, Real t_out,
                     Ray ray, Point *intersection) const;
  Entity * BruteTrace(Ray ray, const std::vector<Entity *> &entities,
                      Point *intersection) const;

  Real GetIlluminance(Point p, Vector normal);

  Color GetColor(Ray ray, unsigned int depth);

  std::vector<Entity *> scene_;
  std::vector<LightSource> light_sources_;
  Real base_illuminance_;

  KDTree kd_tree_;
};

#endif // RAY_TRACER_H_
