#ifndef RT_H_
#define RT_H_

#include <map>
#include <vector>

#include "raytracer.h"

class Rt {
 public:
  Rt(std::istream &input);

  std::vector<Entity *> GetScene();

  std::vector<RayTracer::LightSource> GetLightSources();

  RayTracer::Screen GetScreen();

  Point GetObserver();

 private:
  std::string ReadWord(std::istream &input);

  void ReadViewport(std::istream &input);
  
  void ReadMaterials(std::istream &input);

  void ReadMaterial(std::istream &input);
  
  void ReadLightSources(std::istream &input);

  void ReadLightSource(std::istream &input);
  
  void ReadEntities(std::istream &input);

  void ReadEntity(std::istream &input);

  std::vector<Entity> entities_;
  std::vector<Entity *> scene_;

  std::vector<LightSource> light_sources_;
};

#endif // RT_H_
