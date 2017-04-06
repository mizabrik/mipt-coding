#include <SFML/Graphics.hpp>
#include <iostream>

#include "geometry.h"
#include "sphere.h"
#include "triangle.h"
#include "quadrangle.h"
#include "raytracer.h"

#include <limits>

int main(int argc, char **argv) {
  unsigned int w = 800, h = 600;
  sf::RenderWindow window(sf::VideoMode(w, h), "Gnomderer");

  std::vector<Entity *> scene;
  //scene.push_back(new Sphere(Point{0, 0, 20}, 4, sf::Color::Green));
  //scene.push_back(new Sphere(Point{-2, 0, 10}, 1, sf::Color::Red));
  //scene.push_back(new Triangle(Point{2, 2, 5}, Point{4, 2, 5}, Point{5, 2, 10}, sf::Color::Blue));
  scene.push_back(new Sphere(Point{0, 0, -5}, 2, sf::Color::Red));
  scene.push_back(new Quadrangle(Point{-2, -2, 5}, Point{0, -2, 5}, Point{0, 2, 5}, Point{-2, 2, 5}, sf::Color::Green));
  scene.push_back(new Quadrangle(Point{0, -2, 5}, Point{2, -2, 5}, Point{2, 2, 5}, Point{0, 2, 5}, sf::Color::Green));
  scene.back()->alpha = 0.8;

  Point observer{0, 0, -5};
  RayTracer::Screen screen{Point{-2, 1.5, 0}, Vector{4, 0, 0}, Vector{0, -3, 0}};

  RayTracer tracer(scene, 0.1);
  //tracer.AddLightSource(Point{-4, 0, 0}, 0.7);
  //tracer.AddLightSource(Point{-3, 0, 0}, 0.5);
  tracer.AddLightSource(Point{0, 0, 0}, 0.5);

  auto image = tracer.Render(observer, screen, w, h);
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
