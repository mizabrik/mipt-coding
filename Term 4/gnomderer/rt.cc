#include "rt.h"

#include "sphere.h"
#include "triangle.h"
#include "quadrangle.h"

#include <limits>

Rt::Rt(std::istream &input) {
  ReadViewport(input);
  ReadMaterials(input);
  ReadLightSources(input);
  ReadEntities(input);
}

std::vector<Entity *> Rt::GetScene() const {
  return scene_;
}

std::vector<RayTracer::LightSource> Rt::GetLightSources() const {
  return light_sources_;
}

RayTracer::Screen Rt::GetScreen() const {
  return screen_;
}

Point Rt::GetObserver() const {
  return observer_;
}

void Rt::CheckWord(std::string expected, std::string got) const {
  if (got != expected) {
    auto message = "Expected " + expected + ", but got " + got;
    throw std::runtime_error(message);
  }
}

std::string Rt::ReadWord(std::istream &input) const {
  std::string word;

  input >> word;
  while (word[0] == '#') {
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    input >> word;
  }

  return word;
}

void Rt::ReadWord(std::istream &input, const std::string &word) const {
  CheckWord(word, ReadWord(input));
}

Real Rt::ReadReal(std::istream &input, const std::string &name) const {
  ReadWord(input, name);
  Real x;
  input >> x;
  return x;
}

Point Rt::ReadPoint(std::istream &input, const std::string &name) const {
  ReadWord(input, name);
  Real x, y, z;
  input >> x >> y >> z;
  return Point{x, y, z};
}

Entity::Material Rt::ReadMaterial(std::istream &input) const {
  ReadWord(input, "material");
  std::string name;
  input >> name;
  auto it = materials_.find(name);
  if (it == materials_.end())
    throw std::runtime_error("Undefined material " + name);
  return it->second;
}

void Rt::ReadViewport(std::istream &input) {
  ReadWord(input, "viewport");

  observer_ = ReadPoint(input, "origin");

  Point origin = ReadPoint(input, "topleft");
  Vector y = ReadPoint(input, "bottomleft") - origin;
  Vector x = ReadPoint(input, "topright") - origin;
  screen_ = RayTracer::Screen{origin, x, y};

  ReadWord(input, "endviewport");
}

void Rt::ReadMaterials(std::istream &input) {
  ReadWord(input, "materials");

  std::string word;
  while ((word = ReadWord(input)) == "entry") {
    ReadWord(input, "name");
    std::string name;
    input >> name;
   
    Entity::Material material; 
    ReadWord(input, "color");
    std::string color_string;
    input >> color_string;
    int rgb[3];
    if (color_string.size() != 6)
      throw std::runtime_error("Incorrect color " + color_string);
    for (int i = 0; i < 3; ++i)
      rgb[i] = std::stoi(color_string.substr(2 * i, 2), nullptr, 16);
    material.color = Color(rgb[0], rgb[1], rgb[2]);

    ReadWord(input, "alpha");
    input >> material.opacity;
    ReadWord(input, "reflect");
    input >> material.reflectance;
    ReadWord(input, "refract");
    input >> material.refractive_index;

    word = ReadWord(input);
    if (word == "texture") {
      std::string path;
      input >> path;
      material.texture.loadFromFile(path);
      ReadWord(input, "endentry");
    } else {
      CheckWord("endentry", word);
    }

    materials_[name] = material;
  }

  CheckWord("endmaterials", word);
}

void Rt::ReadLightSources(std::istream &input) {
  ReadWord(input, "lights");

  ReadWord(input, "reference");
  Real power = ReadReal(input, "power");
  Real distance = ReadReal(input, "distance");
  Real k = distance * distance / power;
  ReadWord(input, "endreference");

  std::string word;
  while ((word = ReadWord(input)) == "point") {
    Point position = ReadPoint(input, "coords");
    ReadWord(input, "power");
    input >> power;
    light_sources_.push_back({position, power});
    ReadWord(input, "endpoint");
  }

  CheckWord("endlights", word);
}

void Rt::ReadEntities(std::istream &input) {
  ReadWord(input, "geometry");

  std::string type;
  while ((type = ReadWord(input)) != "endgeometry") {
    if (type == "sphere") {
      Point c = ReadPoint(input, "coords");
      Real r = ReadReal(input, "radius");
      entities_.emplace_back(new Sphere(c, r));
      entities_.back()->SetMaterial(ReadMaterial(input));
      ReadWord(input, "endsphere");
    } else if (type == "triangle") {
      Point a = ReadPoint(input, "vertex");
      Point b = ReadPoint(input, "vertex");
      Point c = ReadPoint(input, "vertex");
      entities_.emplace_back(new Triangle(a, b, c));
      entities_.back()->SetMaterial(ReadMaterial(input));
      ReadWord(input, "endtriangle");
    } else if (type == "quadrangle") {
      Point a = ReadPoint(input, "vertex");
      Point b = ReadPoint(input, "vertex");
      Point c = ReadPoint(input, "vertex");
      Point d = ReadPoint(input, "vertex");
      entities_.emplace_back(new Quadrangle(a, b, c, d));
      entities_.back()->SetMaterial(ReadMaterial(input));
      ReadWord(input, "endquadrangle");
    }
    scene_.push_back(entities_.back().get());
  }
}
