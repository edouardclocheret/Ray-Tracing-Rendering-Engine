#ifndef SAMPLETEXTURE_H
#define SAMPLETEXTURE_H


#include <glm/glm.hpp>
#include "utils/rgba.h"
#include "utils/scenedata.h"
#include "utils/imagereader.h"


class SampleTexture
{
public:
    static RGBA NNSampleTexture(Image* img, SceneFileMap& map, glm::vec2 uv);
    static RGBA BilinSampleTexture(Image* img, SceneFileMap& map, glm::vec2 uv);


};

#endif // SAMPLETEXTURE_H
