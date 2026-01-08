#ifndef INTERSECTER_H
#define INTERSECTER_H

#include <glm/glm.hpp>
#include "utils/sceneparser.h"

class HitRecord {
    public:
        float root;       // t
        RenderShapeData* shape;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 n_obj;
        glm::vec3 p_obj;
        glm::vec3 d_world;
        glm::vec3 pos;
        static void populateHits(float t, glm::vec3 n_obj, glm::vec3 p_obj,std::vector<HitRecord>& hits, RenderShapeData& shape, glm::vec3 d_world);
};


class intersecter
{
public:
    static HitRecord getBestHit(glm::vec4 camPos, glm::vec3 d_world, std::vector<RenderShapeData>& shapes);

    static bool no_object_between_point_and_light(glm::vec3 hit, glm::vec3 light_pos,glm::vec3 d_world, std::vector<RenderShapeData>& shapes);

private :
    static std::vector<HitRecord> getHits(glm::vec4 camPos, glm::vec3 d_world, std::vector<RenderShapeData>& shapes);

};



#endif // INTERSECTER_H
