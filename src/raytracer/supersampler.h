#ifndef SUPERSAMPLER_H
#define SUPERSAMPLER_H

#include <glm/glm.hpp>
#include "raytracescene.h"
#include "shape/intersecter.h"
#include "light/phong.h"
#include "utils/ini_utils.h"

class Supersampler
{
private:
    static glm::vec4 getPixel(int depth, glm::vec3 origin, glm::vec3 direction,std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat);
    static glm::vec4 superSample(bool random_is_on, int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene);

    static glm::vec4 RegularGridSuperSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene);
    static glm::vec4 RandomSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene);
    static glm::vec4 StratifiedSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene);
    static glm::vec4 RegularSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene);

public:
    static glm::vec4 getIllumination(int i, int j, int h, int w, float k, float thetaH, float thetaW, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene);


};

#endif // SUPERSAMPLER_H
