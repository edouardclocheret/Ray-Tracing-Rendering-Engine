#include "raytracescene.h"
#include "utils/sceneparser.h"
#include <iostream>

RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData):
    w(width), h(height), globData(metaData.globalData),
    cam( glm::vec3(metaData.cameraData.pos),
          glm::vec3(metaData.cameraData.look),
          glm::vec3(metaData.cameraData.up),
          float(width) / float(height),
          metaData.cameraData.heightAngle,
          metaData.cameraData.focalLength
          ),
    shapes(metaData.shapes), lights(metaData.lights)
{}

const int& RayTraceScene::width() const {
    // Optional TODO: implement the getter or make your own design
    return this->w;
}

const int& RayTraceScene::height() const {
    // Optional TODO: implement the getter or make your own design
    return this->h;
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    // Optional TODO: implement the getter or make your own design
    return this->globData;
}

const Camera& RayTraceScene::getCamera() const {
    // Optional TODO: implement the getter or make your own design
    return this->cam;
}

const std::vector<RenderShapeData>& RayTraceScene::getShapes() const{
    return this->shapes;
}

const std::vector<SceneLightData>& RayTraceScene::getLights() const{
    return this->lights;
}

