#ifndef RAY_TRACER_H_
#define RAY_TRACER_H_

#include <vector>

#include <SFML/Graphics/Image.hpp>

#include "geometry.h"
#include "entity.h"

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
                   unsigned int width, unsigned int height);

 private:
  struct LightSource {
    Point position;
    Real intensity;
  };

  Entity * Trace(Ray ray, Point *intersection) const;

  Real GetIlluminance(Point p, Vector normal);

  sf::Color GetColor(Entity *entity, Point p);

  sf::Color MixColors(sf::Color a, sf::Color b, double alpha);

  std::vector<Entity *> scene_;
  std::vector<LightSource> light_sources_;
  Real base_illuminance_;
};

#endif // RAY_TRACER_H_
