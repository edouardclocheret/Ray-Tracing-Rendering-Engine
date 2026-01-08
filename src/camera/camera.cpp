#include <stdexcept>
#include "camera.h"

glm::mat3 Camera::inv_viewMat;
float Camera::k;
float Camera::w;
float Camera::h;
glm::vec3 Camera::u;
glm::vec3 Camera::v;

glm::mat4 Camera::getViewMatrix() const{
    return viewMat;
}

glm::vec4 Camera::getPos() const{
    return pos;
}

float Camera::getAspectRatio() const {
    // Optional TODO: implement the getter or make your own design
    return aspectRatio;
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return heightAngle;
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    return focalLenght;
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}
