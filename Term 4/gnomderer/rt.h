#ifndef RT_H_
#define RT_H_

#include <map>
#include <memory>
#include <vector>

#include "entity.h"
#include "raytracer.h"

class Rt {
 public:
  Rt(std::istream &input);

  Rt(const std::string &filename);

  std::vector<Entity *> GetScene() const;

  std::vector<RayTracer::LightSource> GetLightSources() const;

  RayTracer::Screen GetScreen() const;

  Point GetObserver() const;

 private:
  void CheckWord(std::string expected, std::string got) const;

  std::string ReadWord(std::istream &input) const;

  void ReadWord(std::istream &input, const std::string &word) const;

  Real ReadReal(std::istream &input, const std::string &name) const;

  Point ReadPoint(std::istream &input, const std::string &name) const;

  Entity::Material ReadMaterial(std::istream &input) const;

  void ReadViewport(std::istream &input);
  
  void ReadMaterials(std::istream &input);
  
  void ReadLightSources(std::istream &input);
  
  void ReadEntities(std::istream &input);

  void ReadEntity(std::istream &input);

  std::vector<std::unique_ptr<Entity>> entities_;
  std::vector<Entity *> scene_;

  std::vector<RayTracer::LightSource> light_sources_;

  Point observer_;
  RayTracer::Screen screen_;

  std::map<std::string, Entity::Material> materials_;
};

#endif // RT_H_
