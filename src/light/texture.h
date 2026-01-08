#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/glm.hpp>
#include "utils/scenedata.h"
#include "utils/imagereader.h"



class texture
{
public:
    static glm::vec2 computeUV(PrimitiveType type, const glm::vec3 &p_obj, const glm::vec3 &n_obj);
    static std::vector<Image*> buildTexturePyramid(Image* img);
};

#endif // TEXTURE_H
