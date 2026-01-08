#ifndef MIPMAP_H
#define MIPMAP_H

#include <glm/glm.hpp>
#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "shape/intersecter.h"
#include "camera/camera.h"
#include <cmath>
#include "utils/imagereader.h"
#include "raytracer/raytracerconfig.h"

class mipmap
{
public:
    static float getLevel(HitRecord hit, int w, int h);

};

#endif // MIPMAP_H
