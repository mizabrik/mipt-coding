#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <iostream>

#include "geometry.h"
#include "sphere.h"
#include "triangle.h"
#include "quadrangle.h"
#include "raytracer.h"

#include <limits>

sf::Image draw() {
  RayTracer::Screen screen{Point{-10, 10, 10}, Vector{20, 0, 0}, Vector{0, 0, -15}};

  std::vector<Entity *> scene(2);
  scene[0] = new Sphere(Point{5, 20, 0}, 5, sf::Color(0, 0, 255));
  scene[1] = new Quadrangle(Point{10, -30, -30}, Point{10, -30, 30},
                                 Point{10, 30, 30}, Point{10, 30, -30}, sf::Color::White);
  scene[1]->alpha = 0.5;

  RayTracer tracer(scene, 1);
  //tracer.AddLightSource(Point{-10, 0, 0}, 5);

  return tracer.Render(Point{0, -10, 2.5}, screen, 800, 600);
}

int main(int argc, char **argv) {
  unsigned int w = 800, h = 600;

  sf::RenderWindow window(sf::VideoMode(w, h), "Gnomderer");

  std::vector<Entity *> scene;
  //scene.push_back(new Sphere(Point{0, 0, 20}, 4, sf::Color::Green));
  //scene.push_back(new Sphere(Point{-2, 0, 10}, 1, sf::Color::Red));
  //scene.push_back(new Triangle(Point{2, 2, 5}, Point{4, 2, 5}, Point{5, 2, 10}, sf::Color::Blue));
  scene.push_back(new Sphere(Point{0, -1, 10}, 1, sf::Color::Red));
  //scene.push_back(new Quadrangle(Point{-1, -2, 10}, Point{1, -2, 10},
  //                               Point{1, 0, 10}, Point{-1, 0, 10}, sf::Color(200, 200, 200)));
  scene.back()->refraction = 0.5;
  scene.push_back(new Quadrangle(Point{5, -2, 15}, Point{-5, -2, 15},
                                 Point{-5, -2, -5}, Point{5, -2, -5}, sf::Color(200, 200, 200)));
  scene.back()->alpha = 0;
  scene.push_back(new Quadrangle(Point{5, -2, 15}, Point{-5, -2, 15},
                                 Point{-5, 10, 15}, Point{5, 10, 15}, sf::Color::Green));
  scene.push_back(new Quadrangle(Point{-5, -2, 15}, Point{-5, 10, 15},
                                 Point{-5, 10, -5}, Point{-5, -2, -5}, sf::Color::Red));
  scene.push_back(new Quadrangle(Point{5, -2, 15}, Point{5, 10, 15},
                                 Point{5, 10, -5}, Point{5, -2, -5}, sf::Color::Blue));
  scene.push_back(new Quadrangle(Point{5, -2, -5}, Point{-5, -2, -5},
                                 Point{-5, 10, -5}, Point{5, 10, -5}, sf::Color::Yellow));

  for (int i = 0; i < 1000; ++i) {
    scene.push_back(new Triangle(Point{-5. + 0.01 * i, 2, 20}, Point{-5.1 + 0.01 * i, 2, 20}, Point{-5+0.01*i, 1, 20}, sf::Color::Blue));
  }

  Point observer{0, 0, -12};
  RayTracer::Screen screen{Point{-4, 3, 0}, Vector{8, 0, 0}, Vector{0, -6, 0}};

  RayTracer tracer(scene, 0.3);
  //tracer.AddLightSource(Point{-4, 0, 0}, 0.7);
  //tracer.AddLightSource(Point{-3, 0, 0}, 0.5);
  tracer.AddLightSource(Point{-5, 3, 0}, 2);

  auto image = tracer.Render(observer, screen, w, h, 2);
  //auto image = draw();
  sf::Texture texture;
  texture.loadFromImage(image);
  sf::Sprite sprite(texture);

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    window.clear();
    window.draw(sprite);
    window.display();
  }

  return 0;
}
