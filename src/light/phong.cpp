#include "phong.h"
#include "shape/intersecter.h"
#include "utils/imagereader.h"
#include "light/sampletexture.h"
#include "raytracer/raytracerconfig.h"
// //////////////////////////////Phong illumination ///////////////////////////////////

#include <unordered_map>
#include <string>

std::unordered_map<std::string, Image*> textureCache;
std::unordered_map<std::string, std::vector<Image*>> texturePyramidCache;


RGBA phong::toRGBA(const glm::vec4 illumination) {
    std::uint8_t r = 255* fmax(0.f,fmin(illumination[0],1.f));
    std::uint8_t g = 255* fmax(0.f,fmin(illumination[1],1.f));
    std::uint8_t b = 255* fmax(0.f,fmin(illumination[2],1.f));
    //std::uint8_t a = 255; is default
    return RGBA{r, g, b};
}

// Calculates the RGBA of a pixel from intersection infomation and globally-defined coefficients
glm::vec4 phong::getColor(HitRecord& hit, glm::vec3  normal,
                           glm::vec3  directionToCamera,
                           SceneGlobalData globalData,
                           SceneMaterial material,
                           std::vector<SceneLightData>& lights, std::vector<RenderShapeData>& shapes, glm::vec2 uv) {

    glm::vec4 illumination(0.f, 0.f, 0.f, 1.f);
    phong::addAmbientTerm(illumination, globalData, material);

    if (glm::dot(normal, directionToCamera) < 0.0f) {
        normal = -normal; //enabling being inside textures
    }

    // One specular and diffuse term per light
    for (const SceneLightData &light : lights) {

        switch(light.type){
            case (LightType::LIGHT_DIRECTIONAL):
                phong::addDirectionalLight(hit,normal,directionToCamera,globalData,material,light, illumination, shapes, uv);
                break;

            case (LightType::LIGHT_POINT):
                phong::addPointLight(hit, normal, directionToCamera,globalData,material,light,illumination, shapes, uv);
                break;
            case( LightType::LIGHT_SPOT):
                phong::addSpotLight(hit, normal, directionToCamera, globalData, material, light, illumination, shapes, uv);
                break;
        }

    }

    return illumination;
}

float dist(glm::vec3 a, glm::vec3 b){
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}

float phong::getFatt(HitRecord& hit, SceneLightData light){
    float c1 = light.function[0];
    float c2 = light.function[1];
    float c3 = light.function[2];
    float distance = dist(light.pos, hit.pos);
    float fatt =fmin(1,1/(c1+c2*distance+c3*distance*distance));
    return fatt;
}

void phong::addDirectionalLight(HitRecord& hit, glm::vec3  normal,
                    glm::vec3  directionToCamera,
                    SceneGlobalData globalData,
                    SceneMaterial material,
                    SceneLightData light, glm::vec4 &illumination, std::vector<RenderShapeData>& shapes, glm::vec2 uv){
    float fatt = 1.0f;
    glm::vec3 to_light = glm::normalize(-light.dir);


    addDiffuse_Specular(hit, illumination, globalData, material, fatt, normal,to_light, directionToCamera, light, shapes, hit.pos, uv);

}

void phong::addPointLight(HitRecord& hit, glm::vec3  normal,
                          glm::vec3  directionToCamera,
                          SceneGlobalData globalData,
                          SceneMaterial material,
                          SceneLightData light, glm::vec4 &illumination, std::vector<RenderShapeData>& shapes, glm::vec2 uv){
    float fatt = phong::getFatt(hit, light);
    glm::vec3 to_light = glm::normalize(glm::vec3(light.pos) - hit.pos );
    addDiffuse_Specular(hit, illumination, globalData, material, fatt, normal,to_light, directionToCamera, light, shapes, hit.pos, uv);
}

float getfalloff(float theta, float inner, float outer){
    float q = (theta - inner) / (outer -inner);
    return -2.f * q*q*q +3* q*q;
}

float AngleBetween(const glm::vec3& a, const glm::vec3& b) {
    float magA = glm::length(a);
    float magB = glm::length(b);
    if (magA == 0.0f || magB == 0.0f) {
        return 0.0f;
    }

    float cosTheta = glm::dot(a, b) / (magA * magB);
    cosTheta = glm::clamp(cosTheta, -1.0f, 1.0f);
    float angle = std::acos(cosTheta);
    return angle; // in radians
}

void phong::addSpotLight(HitRecord& hit, glm::vec3  normal,
                          glm::vec3  directionToCamera,
                          SceneGlobalData globalData,
                          SceneMaterial material,
                          SceneLightData light, glm::vec4 &illumination, std::vector<RenderShapeData>& shapes, glm::vec2 uv){

    float fatt = phong::getFatt(hit, light);

    glm::vec3 to_light = glm::normalize(glm::vec3(light.pos) - hit.pos );

    float theta = AngleBetween(to_light , -light.dir);
    float inner = light.angle - light.penumbra;
    float outer = light.angle;

    if (theta<=inner){
        float fatt_falloff = fatt;
        addDiffuse_Specular(hit, illumination, globalData, material, fatt_falloff, normal,to_light, directionToCamera, light, shapes, hit.pos, uv);

    }
    if (inner< theta && theta <= outer ){
        float fatt_falloff = fatt * (1-getfalloff(theta, inner, outer));
        addDiffuse_Specular(hit, illumination, globalData, material, fatt_falloff, normal,to_light, directionToCamera, light, shapes, hit.pos, uv);

    }

}



void phong::addAmbientTerm(glm::vec4 &illumination, SceneGlobalData globalData, SceneMaterial material){
    float ka = globalData.ka;
        illumination.x += ka*material.cAmbient.x;
        illumination.y += ka*material.cAmbient.y;
        illumination.z += ka*material.cAmbient.z;

    }


void phong::addDiffuseTerm(HitRecord& hit,glm::vec4 &illumination,
                           SceneGlobalData globalData, SceneMaterial material,
                           float fatt, glm::vec3 normal, glm::vec3 to_light,
                           SceneLightData light, glm::vec2 uv) {

    float kd = globalData.kd;

    glm::vec3 fatt_I_costheta = fatt * glm::dot(normal, to_light) * light.color;
    glm::vec3 d_illumination;
    d_illumination.x = fatt_I_costheta.x * kd * material.cDiffuse.x;
    d_illumination.y = fatt_I_costheta.y * kd * material.cDiffuse.y;
    d_illumination.z = fatt_I_costheta.z * kd * material.cDiffuse.z;

    if (g_m_Config.enableTextureMap && material.textureMap.isUsed) {
        Image* img = nullptr;
        auto it = textureCache.find(material.textureMap.filename);
        if (it != textureCache.end()) {
            img = it->second;
        } else {
            img = loadImageFromFile(material.textureMap.filename);
            if (img) textureCache[material.textureMap.filename] = img;
        }

        Image* imgToSample = img;
        Image* imgToSample_lp1 = img;
        float alpha = 0;

        if (g_m_Config.enableMipMapping) {
            std::vector<Image*> mipmaps;
            auto mit = texturePyramidCache.find(material.textureMap.filename);
            if (mit != texturePyramidCache.end()) {
                mipmaps = mit->second;
            } else {
                mipmaps = texture::buildTexturePyramid(img);
                texturePyramidCache[material.textureMap.filename] = mipmaps;
            }
            float S = mipmap::getLevel(hit, img->width, img->height);
            float level_f = glm::log2(S);
            int level = int(glm::floor(level_f));
            alpha = level_f - float(level);

            level = glm::clamp(level, 0, (int)mipmaps.size() - 1);
            int level_lp1 = glm::clamp(level + 1, 0, (int)mipmaps.size() - 1);

            imgToSample = mipmaps[level];
            imgToSample_lp1 = mipmaps[level_lp1];


        }

        RGBA texturePixel;

        switch (g_m_Config.textureFilterType) {
        case TextureFilterType::Nearest:
            texturePixel = SampleTexture::NNSampleTexture(imgToSample, material.textureMap, uv);
            break;

        case TextureFilterType::Bilinear:
            texturePixel = SampleTexture::BilinSampleTexture(imgToSample, material.textureMap, uv);
            break;

        case TextureFilterType::Trilinear: {
            RGBA tex_l = SampleTexture::BilinSampleTexture(imgToSample, material.textureMap, uv);
            RGBA tex_lp1 = SampleTexture::BilinSampleTexture(imgToSample_lp1, material.textureMap, uv);

            texturePixel.r = (1.0f - alpha) * tex_l.r + alpha * tex_lp1.r;
            texturePixel.g = (1.0f - alpha) * tex_l.g + alpha * tex_lp1.g;
            texturePixel.b = (1.0f - alpha) * tex_l.b + alpha * tex_lp1.b;
            break;
        }

        default:
            break;
        }

        illumination.x += (1 - material.blend) * d_illumination.x + fatt_I_costheta.x * material.blend * texturePixel.r / 255.f;
        illumination.y += (1 - material.blend) * d_illumination.y + fatt_I_costheta.y * material.blend * texturePixel.g / 255.f;
        illumination.z += (1 - material.blend) * d_illumination.z + fatt_I_costheta.z * material.blend * texturePixel.b / 255.f;
    } else {
        illumination.x += d_illumination.x;
        illumination.y += d_illumination.y;
        illumination.z += d_illumination.z;
    }

}


void phong::addSpecularTerm(glm::vec4 &illumination, SceneGlobalData globalData, SceneMaterial material, float fatt, glm::vec3  normal,glm::vec3 to_light, glm::vec3  directionToCamera, SceneLightData light){
    float ks = globalData.ks;
    glm::vec3 R = -glm::reflect(to_light, normal);
    float cosAlpha = fmax(0.f, glm::dot(directionToCamera, R));
    glm::vec3 singamma_n = std::pow(cosAlpha, material.shininess) * light.color;

    illumination.x += fatt *ks * singamma_n.x * material.cSpecular.x ;
    illumination.y += fatt *ks * singamma_n.y * material.cSpecular.y ;
    illumination.z += fatt *ks * singamma_n.z * material.cSpecular.z ;
}

void phong::addDiffuse_Specular(HitRecord& hit,glm::vec4 &illumination, SceneGlobalData globalData, SceneMaterial material, float fatt, glm::vec3  normal,glm::vec3 to_light, glm::vec3  directionToCamera, SceneLightData light, std::vector<RenderShapeData>& shapes, glm::vec3 hit_pos, glm::vec2 uv){

    if (glm::dot(normal, to_light)>0 ){//surface is facing towards the light source

        //We only check for blocking objects if the surface was oriented towards the light source
        glm::vec3 d_world = to_light;
        glm::vec3 shadow_origin = hit_pos + 1e-6f * normal;
        glm::vec3 light_pos = light.pos;
        bool no_object_blocking = intersecter::no_object_between_point_and_light(shadow_origin, light_pos,d_world, shapes);
            if (no_object_blocking){
            addDiffuseTerm(hit,illumination, globalData, material, fatt, normal,to_light, light, uv);
            addSpecularTerm(illumination, globalData, material, fatt, normal,to_light, directionToCamera, light);
        }
    }
}


