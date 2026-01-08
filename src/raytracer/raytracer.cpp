#include "raytracer.h"
#include "raytracescene.h"
#include<cmath>
#include "raytracerconfig.h"
//#include <iostream>

RayTracerConfig g_m_Config;

RayTracer::RayTracer(Config config) :
    m_config(config)
{
    g_m_Config = config; // Synchronising my "global" variable
}


int indexFromMatIndices (int i, int j, int w){
    return i*w + j;
}



void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    // Note that we're passing `data` as a pointer (to its first element)
    // Recall from Lab 1 that you can access its elements like this: `data[i]`

    // TODO: Implement the ray tracing algorithm. Good luck!
    float k = 1.f; // depth of the view plane from the camera position
    Camera::k =k;
    float w = scene.width();
    float h = scene.height();
    Camera::w = w;
    Camera::h =h;
    glm::mat4 viewMat = scene.getCamera().getViewMatrix();
    glm::mat4 inv_viewMat = glm::inverse(viewMat);
    float thetaH = scene.getCamera().getHeightAngle();
    float thetaW = 2* atan(scene.getCamera().getAspectRatio()*tan(thetaH/2));
    std::vector<RenderShapeData> shapes = scene.getShapes();
    SceneGlobalData globalData = scene.getGlobalData();
    std::vector<SceneLightData> lights = scene.getLights();




    #pragma omp parallel for collapse(2) //for parrallel computation
    for (int i =0; i< h; i++){
        for (int j=0; j<w; j++){

            glm::vec4 illumination = Supersampler::getIllumination(i, j, h, w, k, thetaH, thetaW, shapes,lights,globalData, inv_viewMat,scene);

            imageData[indexFromMatIndices(i,j,w)]= phong::toRGBA(illumination);
        }
    }
}








