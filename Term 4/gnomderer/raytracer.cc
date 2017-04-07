#include "raytracer.h"

#include <functional>
#include <limits>
#include <cmath>
#include <iostream>
#include <thread>
#include <atomic>

#include <SFML/Graphics/Image.hpp>

#include "geometry.h"

RayTracer::RayTracer(std::vector<Entity *> scene, Real base_illuminance)
   : scene_(scene), base_illuminance_(base_illuminance), kd_tree_(scene) {
     std::cout << "Built kd" << std::endl;
   }

void RayTracer::AddLightSource(Point position, Real intensity) {
  light_sources_.push_back(LightSource{position, intensity});
}

sf::Image RayTracer::Render(Point observer, Screen screen,
                            unsigned int width, unsigned int height,
                            unsigned int antialiasing) {
  auto raw_image = Render(observer, screen,
                          antialiasing * width, antialiasing * height);

  std::vector<Color> image_data(width * height);
  std::atomic<int> pixel_id(0);

  int concurrency = 0;// std::thread::hardware_concurrency(); 

  auto worker = [&] {
    int my_pixel = 0;
    std::vector<Color> colors(antialiasing * antialiasing);
    while ((my_pixel = pixel_id++) < width * height) {
      unsigned int x = pixel_id % width;
      unsigned int y = pixel_id / width;
      y = pixel_id / width;
      for (int i = 0; i < antialiasing; ++i) {
        for (int j = 0; j < antialiasing; ++j) {
          if (antialiasing * x + i >= antialiasing * width)
            std::cout << "Bang x!" << x << std::endl;
          if (antialiasing * y + j >= antialiasing * height) {
            y = my_pixel / width;
            std::cout << "Bang y!" << y << ' ' << width << ' ' << my_pixel << std::endl;
          }
          colors[i * antialiasing + j] = raw_image.getPixel(
              antialiasing * x + i, antialiasing * y + j);
        }
      }

      image_data[my_pixel] = Average(begin(colors), end(colors));
    }
  };

  std::vector<std::thread> workers;
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

sf::Image RayTracer::Render(Point observer, Screen screen,
                            unsigned int width, unsigned int height) {
  std::vector<Color> image_data(width * height);
  std::atomic<int> pixel_id(0);

  int concurrency = std::thread::hardware_concurrency(); 

  auto worker = [&] {
    int my_pixel = 0;
    while ((my_pixel = pixel_id++) < width * height) {
      unsigned int x = pixel_id % width;
      unsigned int y = pixel_id / width;
      Point pixel = screen.position + screen.x  * ((1 + 2 * x) / (2. * width))
                                    + screen.y * ((1 + 2 * y) / (2. * height));
      Ray ray(pixel, pixel - observer);
      image_data[my_pixel] = GetColor(ray, 4);
    }
  };

  std::vector<std::thread> workers;
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
      if (!target2) {
        return target;
      } else if (!target || Length(intersection2 - ray.origin()) < Length(*intersection - ray.origin())) {
        *intersection = intersection2;
        return target2;
      }
    } else {
      return target;
    }
  }
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
