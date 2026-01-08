#ifndef PHONG_H
#define PHONG_H

#include <glm/glm.hpp>
#include "utils/rgba.h"
#include "utils/scenedata.h"
#include "raytracer/raytracescene.h"
#include "utils/ini_utils.h"
#include "texture.h"
#include "mipmap.h"


class phong
{
public:
    static RGBA toRGBA(const glm::vec4 illumination);
    static glm::vec4 getColor(HitRecord& hit, glm::vec3  normal,
                        glm::vec3  directionToCamera,
                        SceneGlobalData globalData,
                        SceneMaterial material,
                        std::vector<SceneLightData>& lights, std::vector<RenderShapeData>& shapes, glm::vec2 uv) ;
private:
    //these private functions are called by getColor only
    static void addDirectionalLight(HitRecord& hit, glm::vec3  normal,
                        glm::vec3  directionToCamera,
                        SceneGlobalData globalData,
                        SceneMaterial material,
                        SceneLightData light, glm::vec4 &illumination, std::vector<RenderShapeData>& shapes, glm::vec2 uv) ;

    static void addPointLight(HitRecord& hit, glm::vec3  normal, glm::vec3  directionToCamera, SceneGlobalData globalData,SceneMaterial material,SceneLightData light, glm::vec4 &illumination, std::vector<RenderShapeData>& shapes, glm::vec2 uv);

    static void addSpotLight(HitRecord& hit, glm::vec3  normal,glm::vec3  directionToCamera, SceneGlobalData globalData, SceneMaterial material,SceneLightData light, glm::vec4 &illumination, std::vector<RenderShapeData>& shapes, glm::vec2 uv);


    //these methods use
    static float getFatt(HitRecord& hit, SceneLightData light);
    static void addAmbientTerm(glm::vec4 &illumination, SceneGlobalData globalData, SceneMaterial material);
    static void addDiffuseTerm(HitRecord& hit,glm::vec4 &illumination, SceneGlobalData globalData, SceneMaterial material, float fatt, glm::vec3  normal,glm::vec3 to_light, SceneLightData light, glm::vec2 uv);
    static void addSpecularTerm(glm::vec4 &illumination, SceneGlobalData globalData, SceneMaterial material, float fatt, glm::vec3  normal,glm::vec3 to_light, glm::vec3  directionToCamera, SceneLightData light);

    // this function checks if
    // 1. the normals points towards the light
    // 2. there is a blocking object and doesn't add color if the light is blocked
    static void addDiffuse_Specular(HitRecord& hit,glm::vec4 &illumination, SceneGlobalData globalData, SceneMaterial material, float fatt, glm::vec3  normal,glm::vec3 to_light, glm::vec3  directionToCamera, SceneLightData light, std::vector<RenderShapeData>& shapes, glm::vec3 hit_pos, glm::vec2 uv);



};

#endif // PHONG_H
