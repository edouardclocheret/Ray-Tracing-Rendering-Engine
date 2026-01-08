#include "intersecter.h"
#include <iostream>
#include "light/texture.h"



void HitRecord::populateHits(float t, glm::vec3 n_obj, glm::vec3 p_obj,
                             std::vector<HitRecord>& hits, RenderShapeData& shape, glm::vec3 d_world){
    glm::mat4 inv_ctm = shape.i_ctm;
    glm::mat3 inv_ctm3 = glm::mat3(inv_ctm);
    glm::vec3 n_world = glm::normalize(glm::transpose(inv_ctm3) * n_obj);

    glm::vec2 uv = texture::computeUV(shape.primitive.type, p_obj, n_obj);
    hits.push_back({t, &shape, n_world, uv, n_obj, p_obj, d_world});
}



// //////////////   Helpers for intersections
std::vector<float> solveQuadraticPosR(float a, float b, float c) {
    std::vector<float> roots;

    if (std::abs(a) < 1e-6f) { //linear case bx + c = 0
        if (std::abs(b) > 1e-6f) {
            if (-c/b>0) roots.push_back(-c / b);
        }
        return roots;
    }

    float disc = b*b - 4*a*c;
    if (disc < 0.0f) {
        return roots; // no real roots
    }

    float sqrtDisc = std::sqrt(disc);
    float t1 = (-b - sqrtDisc) / (2*a);
    float t2 = (-b + sqrtDisc) / (2*a);

    if (t1>0) {
        roots.push_back(t1);
    }
    if (std::abs(t2 - t1) > 1e-6f){
        if (t2>0) roots.push_back(t2);
    }
    return roots;
}

void intersectWithPlane(float x, int i, glm::vec3& cam_obj, glm::vec3& d_obj, std::vector<HitRecord>& hits, RenderShapeData& shape, glm::vec3 d_world){
    float t = (x-cam_obj[i])/d_obj[i]; //solution of for given x
    float y = cam_obj[(i+1)%3] +t*d_obj[(i+1)%3];
    float z = cam_obj[(i+2)%3] +t*d_obj[(i+2)%3];

    if (-0.5f< y && y <0.5f && -0.5f< z && z <0.5f){
        //normal in obj space
        glm::vec3 n_obj = glm::vec3(0.f);
        n_obj[i] = glm::sign(x);
        glm::vec3 p_obj = cam_obj + t * d_obj;
        HitRecord::populateHits(t, n_obj,p_obj, hits, shape,d_world);
    }
}

void intersectWithDisk(float r, float d, glm::vec3& cam_obj, glm::vec3& d_obj, std::vector<HitRecord>& hits, RenderShapeData& shape, glm::vec3 d_world){
    float t = (d - cam_obj.y) / d_obj.y;
    float x = cam_obj.x + t * d_obj.x;
    float z = cam_obj.z + t * d_obj.z;
    if (x*x + z*z <= r*r) {
        //normal in obj space
        glm::vec3 n_obj = glm::normalize(glm::vec3(0.f, d, 0.f));
        glm::vec3 p_obj = cam_obj + t * d_obj;
        HitRecord::populateHits(t, n_obj,p_obj, hits, shape,d_world);
    }
}

void intersectWithCylinder(float r, glm::vec3& cam_obj, glm::vec3& d_obj, std::vector<HitRecord>& hits, RenderShapeData& shape, glm::vec3 d_world){
    float a = d_obj.x*d_obj.x + d_obj.z*d_obj.z;
    float b = 2.f *(d_obj.x*cam_obj.x + d_obj.z*cam_obj.z);
    float c = cam_obj.x *cam_obj.x + cam_obj.z *cam_obj.z- r*r;

    std::vector<float> sphereRoots = solveQuadraticPosR(a, b, c);
    for (float r : sphereRoots) {
        float y_val = cam_obj.y + r * d_obj.y;
        if (y_val >= -0.5f && y_val <= 0.5f) {
            //normal in obj space
            glm::vec3 p_obj = cam_obj + r * d_obj;
            glm::vec3 n_obj = glm::normalize(glm::vec3(2.f*p_obj.x, 0.f, 2.f*p_obj.z));
            HitRecord::populateHits(r, n_obj,p_obj, hits, shape,d_world);

        }
    }
    for (float d : {-0.5, 0.5}){
        intersectWithDisk(r, d, cam_obj, d_obj, hits, shape,d_world);
    }
}



std::vector<HitRecord> intersecter::getHits(glm::vec4 camPos, glm::vec3 d_world, std::vector<RenderShapeData>& shapes){
    std::vector<HitRecord> hits;
    for(RenderShapeData& shape : shapes){

        glm::mat4 inv_ctm = shape.i_ctm;
        //in object space
        glm::vec4 d_obj4 = inv_ctm * glm::vec4(d_world.x, d_world.y, d_world.z, 0.0f);
        glm::vec4 cam_obj4 = inv_ctm * glm::vec4(camPos.x, camPos.y, camPos.z, 1.0f);
        glm::vec3 d_obj = d_obj4;
        glm::vec3 cam_obj = cam_obj4;


        switch(shape.primitive.type){
        case (PrimitiveType::PRIMITIVE_CUBE):{
            for(int i: {0,1,2}){//in x, y, z
                for (float d : {-0.5f, 0.5f} ){ //2 sides per coordinate
                    intersectWithPlane(d, i, cam_obj, d_obj, hits, shape,d_world);
                }
            }
            break;
        }
        case (PrimitiveType::PRIMITIVE_CONE):{

            // Conical top
            {
                float a = d_obj.x*d_obj.x + d_obj.z*d_obj.z - d_obj.y*d_obj.y/4.f;
                float b = 2.f*(d_obj.x*cam_obj.x + d_obj.z*cam_obj.z) - d_obj.y*cam_obj.y/2.f + d_obj.y/4.f;
                float c = cam_obj.x*cam_obj.x + cam_obj.z*cam_obj.z - cam_obj.y*cam_obj.y/4.f + cam_obj.y/4.f - 0.0625f;

                std::vector<float> sideRoots = solveQuadraticPosR(a, b, c);

                // Keep only roots inside the valid y-range
                for (float r : sideRoots) {
                    float y_val = cam_obj.y + r * d_obj.y;
                    if (y_val >= -0.5f && y_val <= 0.5f) {
                        //normal in obj space
                        glm::vec3 p_obj = cam_obj + r * d_obj;
                        glm::vec3 n_obj = glm::normalize(glm::vec3(2.f*p_obj.x, 0.25f-0.5f*p_obj.y, 2.f*p_obj.z));
                        HitRecord::populateHits(r, n_obj,p_obj, hits, shape,d_world);
                    }
                }

                // Flat base with r =-0.5 at y = -0.5
                intersectWithDisk(0.5f, -0.5f, cam_obj,d_obj,hits, shape,d_world);
            }

            break;
        }
        case (PrimitiveType::PRIMITIVE_CYLINDER):{
            intersectWithCylinder(0.5f, cam_obj, d_obj, hits, shape,d_world);
            break;
        }

        case (PrimitiveType::PRIMITIVE_SPHERE):
        {
            float a = glm::dot(d_obj, d_obj);
            float b = 2.f * glm::dot(d_obj, cam_obj);
            float c = glm::dot(cam_obj, cam_obj) - 0.25f;

            std::vector<float> sphereRoots = solveQuadraticPosR(a, b, c);

            for (float r : sphereRoots) {
                //normal in obj space
                glm::vec3 p_obj = cam_obj + r * d_obj;
                glm::vec3 n_obj = glm::normalize(p_obj);
                HitRecord::populateHits(r, n_obj,p_obj, hits, shape,d_world);
            }
            break;
        }
        default:
            std::cout<< "Unsupported primitive"<<std::endl;
        }

    }
    return hits;
}

bool intersecter::no_object_between_point_and_light(glm::vec3 hit, glm::vec3 light_pos, glm::vec3 d_world, std::vector<RenderShapeData>& shapes) {
    glm::vec4 hit4(hit, 1.0f);
    std::vector<HitRecord> hits = intersecter::getHits(hit4, d_world, shapes);

    float dist_to_light = glm::length(light_pos - hit);

    for (const HitRecord& rec : hits) {
        if (rec.root > 0.001f && rec.root < dist_to_light) {
            // Object is between hit point and light
            return false;
        }
    }
    return true;
}

HitRecord intersecter::getBestHit(glm::vec4 camPos, glm::vec3 d_world, std::vector<RenderShapeData>& shapes)
{

    std::vector<HitRecord> hits = intersecter::getHits(camPos, d_world, shapes);
    int bestIndex = -1;
    float minRoot = std::numeric_limits<float>::infinity();

    for (int i = 0; i < (int)hits.size(); i++) {
        if (hits[i].root >= 0.f && hits[i].root < minRoot) {
            minRoot = hits[i].root;
            bestIndex = i;
        }
    }

    if (bestIndex == -1){
        HitRecord hit;
        hit.root =-1;
        return hit; //no hit
    }
    return hits[bestIndex];

}




