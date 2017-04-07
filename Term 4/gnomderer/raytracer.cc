#include "raytracer.h"

#include <limits>
#include <cmath>

#include <SFML/Graphics/Image.hpp>

#include "geometry.h"

RayTracer::RayTracer(std::vector<Entity *> scene, Real base_illuminance)
   : scene_(scene), base_illuminance_(base_illuminance), kd_tree_(scene) {}

void RayTracer::AddLightSource(Point position, Real intensity) {
  light_sources_.push_back(LightSource{position, intensity});
}

sf::Image RayTracer::Render(Point observer, Screen screen,
                            unsigned int width, unsigned int height) {
  auto box = kd_tree_.root_->box;
  sf::Image image;
  image.create(width, height, sf::Color::Black);

  for (unsigned int x = 0; x < width; ++x) {
    for (unsigned int y = 0; y < height; ++y) {
      Point pixel = screen.position + screen.x  * ((1 + 2 * x) / (2. * width))
                                    + screen.y * ((1 + 2 * y) / (2. * height));
      Ray ray(pixel, pixel - observer);
      image.setPixel(x, y, GetColor(ray, 4));
    }
  }

  return image;
}

Entity * RayTracer::Trace(Ray ray, Point *intersection) const {
  Real t_in, t_out;
  KDTree::KDNode &root = *kd_tree_.root_;
  if (!root.box.Intersection(ray, &t_in, &t_out))
    return nullptr;

  return TraceNode(root, t_in, t_out, ray, intersection);
};

Entity * RayTracer::TraceNode(KDTree::KDNode &node, Real t_in, Real t_out,
                              Ray ray, Point *intersection) const {
  if (t_out <= 0)
    return nullptr;
  if (!node.left)
    return BruteTrace(ray, node.entities_, intersection);

  Real t_mid = node.split.Intersection(ray);

  if (t_mid > t_in) {
    auto target = TraceNode(*node.left, t_in, t_mid, ray, intersection);
    if (target)
      return target;
  }

  if (t_mid < t_out) {
    auto target = TraceNode(*node.right, t_mid, t_out, ray, intersection);
    if (target)
      return target;
  }

  return nullptr;
}

Entity * RayTracer::BruteTrace(Ray ray, const std::vector<Entity *> &entities, Point *intersection) const {
  Entity *target = nullptr;
  Real distance2(std::numeric_limits<double>::infinity());

  for (Entity *entity : entities) {
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

    Real angle_cos = Dot(normal, diff).abs() / Length(diff);
    if (angle_cos > 0)
      illuminance += source.intensity * angle_cos;
  }

  return illuminance;
}

sf::Color RayTracer::GetColor(Ray ray, unsigned int depth) {
  Point intersection;
  Entity *entity = Trace(ray, &intersection);
  if (!entity)
    return sf::Color::Black;
  auto normal = entity->Normal(intersection);

  if (depth == 0) {
    double illuminance = GetIlluminance(intersection, normal).value();
    return illuminance * entity->GetColor(intersection);
  }

  double alpha = entity->alpha;
  Color mirrored_color;
  if (alpha > 0) {
    Ray mirrored_ray = ray.Mirror(intersection, normal);
    mirrored_color = GetColor(mirrored_ray, depth - 1);
  }

  Color base = Color::Black;
  if (entity->refraction) {
    Real cos_phi = Dot(ray.direction(), normal);
    double k; // outside / inside
    if (cos_phi < 0) {
      cos_phi = -cos_phi;
      k = entity->refraction;
    } else {
      k = 1 / entity->refraction;
      normal = -normal;
    }

    Real cos2_psi = 1 - k * k * (1 - cos_phi * cos_phi);
    if (cos2_psi > 0) {
      auto refracted = k * ray.direction() + (cos_phi * k - cos2_psi.Sqrt()) * normal;
      Ray refracted_ray(intersection, refracted);
      base = GetColor(refracted_ray, depth - 1);
    } else {
      alpha = 1;
    }
  } else {
    double illuminance = GetIlluminance(intersection, normal).value();
    base = illuminance * entity->GetColor(intersection);
  }

  return (1 - alpha) * base + alpha * mirrored_color;
}
