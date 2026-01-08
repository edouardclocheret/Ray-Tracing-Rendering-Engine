#pragma once

#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    Camera(glm::vec3 posi, glm::vec3 look, glm::vec3 up, float ar, float hAng, float focal):
        aspectRatio(ar), heightAngle(hAng), focalLenght(focal)
    {
        glm::mat4 trans = glm::mat4(1.f,0.f,0.f,0.f,
                                    0.f,1.f,0.f,0.f,
                                    0.f,0.f,1.f,0.f,
                                    -posi[0], -posi[1], -posi[2], 1.f);
        glm::vec3 w = -normalize(look);
        glm::vec3 v = normalize(up - dot(up,w)*w);
        glm::vec3 u = cross(v,w);
        glm::mat4 rot = glm::mat4(u[0],v[0],w[0],0.f,
                                  u[1],v[1],w[1],0.f,
                                  u[2],v[2],w[2],0.f,
                                  0.f, 0.f, 0.f, 1.f);

        viewMat = rot*trans;
        pos.x = posi.x;
        pos.y = posi.y;
        pos.z = posi.z;
        pos[3] = 1;

        this->u = u;
        this->v = v;
    }

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::vec4 getPos() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    static glm::mat3 inv_viewMat;
    static float k;
    static float w;
    static float h;
    static glm::vec3 u;
    static glm::vec3 v;

private :
    glm::mat4 viewMat;
    float aspectRatio;
    float heightAngle;
    float focalLenght;
    glm::vec4 pos;


};
