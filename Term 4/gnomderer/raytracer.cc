#include "raytracer.h"

#include <functional>
#include <limits>
#include <cmath>
#include <iostream>
#include <thread>
#include <atomic>

#include <SFML/Graphics/Image.hpp>

#include "geometry.h"

RayTracer::RayTracer(std::vector<Entity *> scene,
                     std::vector<LightSource> light_sources,
                     Real base_illuminance)
  : scene_(scene),
    light_sources_(light_sources),
    base_illuminance_(base_illuminance),
    kd_tree_(scene) {}

sf::Image RayTracer::Render(Point observer, Screen screen,
                            unsigned int width, unsigned int height,
                            unsigned int antialiasing) {
  auto raw_image = Render(observer, screen,
                          antialiasing * width, antialiasing * height);

  sf::Image image;
  image.create(width, height, Color::Black);

  std::vector<Color> colors(antialiasing * antialiasing);
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      for (int i = 0; i < antialiasing; ++i) {
        for (int j = 0; j < antialiasing; ++j) {
          colors[i * antialiasing + j] = raw_image.getPixel(
              antialiasing * x + i, antialiasing * y + j);
        }
      }

      image.setPixel(x, y, Average(begin(colors), end(colors)));
    }
  }

  return image;
}


sf::Image RayTracer::Render(Point observer, Screen screen,
                            unsigned int width, unsigned int height) {
  std::vector<Color> image_data(width * height);
  std::atomic<int> pixel_id(0);

  auto worker = [&] {
    int my_pixel = 0;
    while (true) {
      my_pixel = pixel_id.fetch_add(1);
      if (my_pixel >= width * height)
        return;
      unsigned int x = my_pixel % width;
      unsigned int y = my_pixel / width;

      Point pixel = screen.position + screen.x  * ((1 + 2 * x) / (2. * width))
                                    + screen.y * ((1 + 2 * y) / (2. * height));
      Ray ray(pixel, pixel - observer);
      image_data[my_pixel] = GetColor(ray, 4);
    }
  };

  std::vector<std::thread> workers;
  int concurrency = std::thread::hardware_concurrency(); 
  for (int i = 1; i < concurrency; ++i) {
    workers.emplace_back(worker);
  }
  worker();

  for (auto &t : workers)
    t.join();

  sf::Image image;
  image.create(width, height, (sf::Uint8 *) image_data.data());
  return image;
}

Entity * RayTracer::Trace(Ray ray, Point *intersection) const {
  Real t_in, t_out;
  KDTree::KDNode &root = *kd_tree_.root_;
  if (!root.box.Intersection(ray, &t_in, &t_out))
    return nullptr;
  if (t_out <= 0)
    return nullptr;

  return TraceNode(root, t_in, t_out, ray, intersection);
};

Entity * RayTracer::TraceNode(KDTree::KDNode &node, Real t_in, Real t_out,
                              Ray ray, Point *intersection) const {
  if (t_out <= 0)
    return nullptr;
  if (!node.left)
    return BruteTrace(ray, node.entities_, intersection);

  SimplePlane split = node.split;
  Real t_mid = split.Intersection(ray);

  KDTree::KDNode *near = ray.direction().Axis(split.axis) >= 0 ? node.left.get() : node.right.get();
  KDTree::KDNode *far = ray.direction().Axis(split.axis) >= 0 ? node.right.get() : node.left.get();

  if (t_mid >= t_out) {
    return TraceNode(*near, t_in, t_out, ray, intersection);
  } else if (t_mid <= t_in) {
    return TraceNode(*far, t_in, t_out, ray, intersection);
  } else {
    auto target = TraceNode(*near, t_in, t_mid, ray, intersection);
    if (!target || Length(*intersection - ray.origin()) > t_mid) {
      Point intersection2;
      auto target2 = TraceNode(*far, t_mid, t_out, ray, &intersection2);
      if (target2 && !target || Length(intersection2 - ray.origin()) < Length(*intersection - ray.origin())) {
        *intersection = intersection2;
        return target2;
      }
    }
    return target;
  }
}

Entity * RayTracer::BruteTrace(Ray ray, const std::vector<Entity *> &entities,
                               Point *intersection) const {
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
  Color base = Color::Black;

  Point intersection;
  Entity *entity = Trace(ray, &intersection);
  if (!entity)
    return base;
  auto normal = entity->Normal(intersection);

  double opacity = entity->GetOpacity();
  double reflectance = entity->GetReflectance();
  double refractive_index = entity->GetRefractiveIndex();

  if (opacity > 0.0 && reflectance < 1.0 || depth == 0) {
    double illuminance = GetIlluminance(intersection, normal).value();
    base = illuminance * entity->GetColor(intersection);
    if (depth == 0) {
      return base;
    }
  }

  Color refracted_color;
  if (opacity < 1.0) {
    Real cos_phi = Dot(ray.direction(), normal);
    double k; // outside / inside
    if (cos_phi < 0) {
      cos_phi = -cos_phi;
      k = refractive_index;
    } else {
      k = 1 / refractive_index;
      normal = -normal;
    }

    Real cos2_psi = 1 - k * k * (1 - cos_phi * cos_phi);
    if (cos2_psi > 0) {
      auto refracted = k * ray.direction() + (cos_phi * k - cos2_psi.Sqrt()) * normal;
      Ray refracted_ray(intersection, refracted);
      refracted_color = GetColor(refracted_ray, depth - 1);
    } else {
      opacity = 1.0;
      reflectance = 1.0;
    }
  }

  if (reflectance > 0.0) {
    Color mirrored_color;
    Ray mirrored_ray = ray.Mirror(intersection, normal);
    mirrored_color = GetColor(mirrored_ray, depth - 1);
    base = (1.0 - reflectance) * base + reflectance * mirrored_color;
  }

  return opacity * base + (1 - opacity) * refracted_color;
}
