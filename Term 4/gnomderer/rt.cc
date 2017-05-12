#include "rt.h"

Rt::Rt(std::istream &input) {
}

std::vector<Entity *> GetObjects();

std::vector<RayTracer::LightSource> Rt::GetLightSources();

RayTracer::Screen Rt::GetScreen();

Point Rt::GetObserver();

std::string Rt::ReadWord(std::istream &input);

void Rt::ReadViewport(std::istream &input);

void Rt::ReadMaterials(std::istream &input);

void Rt::ReadMaterial(std::istream &input);

void Rt::ReadLightSources(std::istream &input);

void Rt::ReadLightSource(std::istream &input);

void Rt::ReadEntities(std::istream &input);

void Rt::ReadEntity(std::istream &input);
