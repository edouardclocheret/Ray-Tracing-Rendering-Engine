#include "supersampler.h"
#include <cmath>
#include <random>
#include "raytracer/raytracerconfig.h"

glm::vec4 Supersampler::RegularSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene){
    bool random_is_on = false;
    return superSample(random_is_on, i, j, h, w, k, thetaH, thetaW, maxDepth, shapes,lights,globalData, inv_viewMat,scene);
}
glm::vec4 Supersampler::RegularGridSuperSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene){
    bool random_is_on = false;
    return superSample(random_is_on, i, j, h, w, k, thetaH, thetaW, maxDepth, shapes,lights,globalData, inv_viewMat,scene);
}
glm::vec4 Supersampler::StratifiedSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene){
    bool random_is_on = true;
    return superSample(random_is_on, i, j, h, w, k, thetaH, thetaW, maxDepth, shapes,lights,globalData, inv_viewMat,scene);
}


glm::vec4 get_p_viewplane (float i, float j, int h, int w, float k, float thetaH, float thetaW){
    glm::vec4 p;
    p.x = 2.0f*k*tan(thetaW/2) * ((j+0.5f)/w -0.5f);
    p.y = 2.0f*k*tan(thetaH/2) * ((h-1-i+0.5f)/h -0.5f);
    p.z= -k;
    p[3] =1.0f;
    return p;
}


glm::vec4 average(std::vector<glm::vec4> &illuminations){
    float r=0.f;
    float g=0.f;
    float b =0.f;
    float a =255.f;
    float n = (float)illuminations.size();
    for (glm::vec4 illu : illuminations){
        r+=illu.r;
        g+=illu.g;
        b+=illu.b;
    }
    return glm::vec4(r/n,g/n,b/n,a);
}

glm::vec4 Supersampler::superSample(bool random_is_on, int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene){
    int num_samples = g_m_Config.samplesPerPixel;
    int grid_side = ceil(sqrt(num_samples));

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0, 1/(float)grid_side);

    glm::vec4 camPos = scene.getCamera().getPos();
    std::vector<glm::vec4> illuminations;


    float top = j-0.5f;
    float left = i-0.5f;
    float di = 1/(float)grid_side;
    float dj = 1/(float)grid_side;

    for(int l=0; l<grid_side; l++ ){
        for(int m=0; m<grid_side; m++ ){
            float ri = 0.f;
            float rj = 0.f;
            if (random_is_on){
                ri = dist(gen);
                rj = dist(gen);
            }

            float new_i = left + l*di +ri;
            float new_j = top + m*dj +rj;
            glm::vec4 p_viewplane = get_p_viewplane(new_i, new_j,h, w, k, thetaH, thetaW);//in camera space
            glm::vec3 p_world = glm::vec3(inv_viewMat * p_viewplane);
            glm::vec3 d_world = glm::normalize(p_world - glm::vec3(camPos)); //removed normalization

            int depth = 0;
            illuminations.push_back(Supersampler::getPixel(depth, camPos, d_world, shapes, lights, globalData, inv_viewMat));
        }
    }

    return average(illuminations);
}

glm::vec4 Supersampler::RandomSampling(int i, int j, int h, int w, float k, float thetaH, float thetaW, int maxDepth, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene){
    int num_samples = g_m_Config.samplesPerPixel;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    glm::vec4 camPos = scene.getCamera().getPos();
    std::vector<glm::vec4> illuminations;


    float top = j-0.5f;
    float left = i-0.5f;


    for(int n=0; n<num_samples; n++ ){

        float ri = dist(gen);
        float rj = dist(gen);


        float new_i = left +ri;
        float new_j = top  +rj;
        glm::vec4 p_viewplane = get_p_viewplane(new_i, new_j,h, w, k, thetaH, thetaW);//in camera space
        glm::vec3 p_world = glm::vec3(inv_viewMat * p_viewplane);
        glm::vec3 d_world = glm::normalize(p_world - glm::vec3(camPos)); //removed normalization

        int depth = 0;
        illuminations.push_back(Supersampler::getPixel(depth, camPos, d_world, shapes, lights, globalData, inv_viewMat));
    }


    return average(illuminations);
}


glm::vec4 Supersampler::getPixel(int depth, glm::vec3 origin, glm::vec3 direction,std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat){
    int maxDepth = g_m_Config.maxRecursiveDepth;
    if (!g_m_Config.enableReflection) maxDepth =1;

    glm::vec4 illumination (0.f,0.f,0.f,0.f);
    if (depth >= maxDepth){// black background
        return illumination;
    }


    glm::vec4 origin4 (origin,1);
    HitRecord hit = intersecter::getBestHit(origin4, direction, shapes);
    if (hit.root == -1){
        // black background
        return illumination;
    }
    else{
        glm::vec3 directionToCamera = -direction;
        SceneMaterial material = hit.shape->primitive.material;
        glm::vec3 normal = hit.normal;
        glm::vec2 uv = hit.uv;

        hit.pos =origin + hit.root* direction;
        illumination = phong::getColor(hit, normal,
                                       directionToCamera,
                                       globalData,
                                       material,
                                       lights, shapes, uv);

        //only if material is reflective, recursion
        if (material.cReflective.x > 0.0f || material.cReflective.y > 0.0f || material.cReflective.z > 0.0f) {


            //glm::vec3 reflectDir = glm::normalize(glm::reflect(direction, normal));
            glm::vec3 reflectDir = glm::normalize(direction - 2.f*glm::dot(direction,normal) *normal);


            glm::vec4 reflectionColor = Supersampler::getPixel(depth + 1, hit.pos + 0.01f * reflectDir,reflectDir,shapes,lights,globalData, inv_viewMat);

            float ks = globalData.ks;

            illumination.r += ks* material.cReflective.r * reflectionColor.r;
            illumination.g += ks* material.cReflective.g *reflectionColor.g;
            illumination.b += ks* material.cReflective.b *reflectionColor.b;
        }
        return illumination;
    }
}


glm::vec4 Supersampler::getIllumination(int i, int j, int h, int w, float k, float thetaH, float thetaW, std::vector<RenderShapeData>& shapes,std::vector<SceneLightData>& lights,SceneGlobalData& globalData, glm::mat4 inv_viewMat, const RayTraceScene &scene){
    int maxDepth = g_m_Config.maxRecursiveDepth;
    if (!g_m_Config.enableReflection) maxDepth =1;
    if (!g_m_Config.enableSuperSample) g_m_Config.samplesPerPixel =1;
    glm::vec4 illumination;
    if (g_m_Config.enableSuperSample){
        switch (g_m_Config.superSamplerPattern) {
        case SuperSamplerPattern::Grid:
            illumination = Supersampler::RegularGridSuperSampling(i, j, h, w, k, thetaH, thetaW, maxDepth, shapes,lights,globalData, inv_viewMat,scene);
            break;
        case SuperSamplerPattern::Stratified:
            illumination = Supersampler::StratifiedSampling(i, j, h, w, k, thetaH, thetaW, maxDepth, shapes,lights,globalData, inv_viewMat,scene);
            break;
        case SuperSamplerPattern::Random:
            illumination = Supersampler::RandomSampling(i, j, h, w, k, thetaH, thetaW, maxDepth, shapes,lights,globalData, inv_viewMat,scene);
            break;
        default:
            break;
        }
    }
    else illumination = Supersampler::RegularSampling(i, j, h, w, k, thetaH, thetaW, maxDepth, shapes,lights,globalData, inv_viewMat,scene);
    return illumination;
}
