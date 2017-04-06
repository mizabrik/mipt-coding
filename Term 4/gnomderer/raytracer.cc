#include "raytracer.h"

#include <limits>
#include <cmath>

#include <SFML/Graphics/Image.hpp>

#include "geometry.h"

RayTracer::RayTracer(std::vector<Entity *> scene, Real base_illuminance)
   : scene_(scene), base_illuminance_(base_illuminance) {}

void RayTracer::AddLightSource(Point position, Real intensity) {
  light_sources_.push_back(LightSource{position, intensity});
}

sf::Image RayTracer::Render(Point observer, Screen screen,
                            unsigned int width, unsigned int height) {
  sf::Image image;
  image.create(width, height, sf::Color::Black);

  for (unsigned int x = 0; x < width; ++x) {
    for (unsigned int y = 0; y < height; ++y) {
      Point pixel = screen.position + (screen.x / (2. * width)) * (1 + 2 * x)
                                    + (screen.y / (2. * height)) * (1 + 2 * y);
      Ray ray(pixel, pixel - observer);
      Point intersection;
      Entity *entity = Trace(ray, &intersection);
      if (!entity)
        continue;

      auto color = GetColor(entity, intersection);
      if (entity->alpha > 0) {
        auto n = entity->Normal(intersection);
        Ray mirrored_ray(intersection,
                         ray.direction() - 2 * Dot(ray.direction(), n) * n);
        Point mirrored;
        Entity *mirrored_entity = Trace(mirrored_ray, &mirrored);
        sf::Color mirrored_color = sf::Color::Black;
        if (mirrored_entity)
            mirrored_color = GetColor(mirrored_entity, mirrored);
        if (x == 450 && y == 300) {
          std::cout << (int)mirrored_color.r << std::endl;
          std::cout << (int)mirrored_color.g << std::endl;
          std::cout << (int)mirrored_color.b << std::endl;
        }
        color = MixColors(color, mirrored_color, 1 - entity->alpha);
      }
      image.setPixel(x, y, color);
    }
  }

  return image;
}

Entity * RayTracer::Trace(Ray ray, Point *intersection) const {
  Entity *target = nullptr;
  Real distance2(std::numeric_limits<double>::infinity());

  for (Entity *entity : scene_) {
    Point new_intersection;
    if(entity->Intersection(ray, &new_intersection)) {
      Real new_distance2 = Length2(ray.origin() - new_intersection);
      if (distance2 > new_distance2) {
        target = entity;
        *intersection = new_intersection;
        distance2 = new_distance2;
      }
    }
  }

  return target;
}

Real RayTracer::GetIlluminance(Point p, Vector normal) {
  Real illuminance = base_illuminance_;

  for (auto &source : light_sources_) {
    Vector diff = p - source.position;
    Point q;
    Trace(Ray(source.position, diff), &q);
    if (p != q)
      continue;

    illuminance += source.intensity * Dot(normal, diff).abs() / Length(diff);
  }

  return illuminance;
}

sf::Color RayTracer::GetColor(Entity *entity, Point p) {
  double illuminance = GetIlluminance(p, entity->Normal(p)).value();

  sf::Color base = entity->GetColor(p);
  sf::Color color;
  color.r = std::min(255, (int) std::round(base.r * illuminance));
  color.g = std::min(255, (int) std::round(base.g * illuminance));
  color.b = std::min(255, (int) std::round(base.b * illuminance));

  return color;
}

sf::Color RayTracer::MixColors(sf::Color a, sf::Color b, double alpha) {
  sf::Color mix;

  mix.r = (int) std::round(alpha * a.r + (1 - alpha) * b.r);
  mix.g = (int) std::round(alpha * a.g + (1 - alpha) * b.g);
  mix.b = (int) std::round(alpha * a.b + (1 - alpha) * b.b);

  return mix;
}
