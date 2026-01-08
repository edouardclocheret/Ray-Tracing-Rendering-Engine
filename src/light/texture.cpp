#include "texture.h"

glm::vec2 texture::computeUV(PrimitiveType type, const glm::vec3 &p_obj, const glm::vec3 &n_obj) {
    glm::vec2 uv(0.0f);
    switch (type) {
    case PrimitiveType::PRIMITIVE_SPHERE: {
        float theta = atan2(p_obj.z, p_obj.x);
        float phi = acos(glm::clamp(p_obj.y / 0.5f, -1.0f, 1.0f));
        theta = -theta + M_PI;
        phi = -phi + M_PI;
        uv.x = 0.5f + theta / (2 * M_PI);
        uv.y = 1.0f - (phi / M_PI);
        break;
    }

    case PrimitiveType::PRIMITIVE_CYLINDER: {
        float theta = atan2(p_obj.z, p_obj.x);
        theta = -theta + M_PI;
        if (glm::abs(glm::abs(p_obj.y) - 0.5f) < 0.01f) {
            // Top/bottom caps
            if (p_obj.y > 0) {
                uv.x = -p_obj.x + 0.5f;
                uv.y = -p_obj.z + 0.5f;
                uv.y = 1.0f - uv.y;
                uv.x = 1.0f - uv.x;
            } else {
                uv.x = p_obj.x + 0.5f;
                uv.y = p_obj.z + 0.5f;
                uv.y = 1.0f - uv.y;
            }
        } else {
            // Side
            uv.x = 0.5f + theta / (2 * M_PI);
            uv.y = 1.0f - (p_obj.y + 0.5f);
        }
        break;
    }

    case PrimitiveType::PRIMITIVE_CONE: {
        float theta = atan2(p_obj.z, p_obj.x);
        theta = -theta + M_PI;
        if (glm::abs(glm::abs(p_obj.y) - 0.5f) < 1e-6f) {
            if (p_obj.y > 0) {
                uv.x = -p_obj.x + 0.5f;
                uv.y = -p_obj.z + 0.5f;
            } else {
                uv.x = p_obj.x + 0.5f;
                uv.y = p_obj.z + 0.5f;
            }
            uv.y = 1.0f - uv.y;
        } else {
            uv.x = 0.5f + theta / (2 * M_PI);
            uv.y = 1.0f - (p_obj.y + 0.5f);
        }
        break;
    }

    case PrimitiveType::PRIMITIVE_CUBE: {
        glm::vec3 absN = glm::abs(n_obj);
        if (absN.x > absN.y && absN.x > absN.z) {
            if (n_obj.x > 0) {
                uv.x = -p_obj.z + 0.5f;
                uv.y = p_obj.y + 0.5f;
            } else {
                uv.x = p_obj.z + 0.5f;
                uv.y = p_obj.y + 0.5f;
            }
        } else if (absN.y > absN.x && absN.y > absN.z) {
            if (n_obj.y > 0) {
                uv.x = p_obj.x + 0.5f;
                uv.y = -p_obj.z + 0.5f;
            } else {
                uv.x = p_obj.x + 0.5f;
                uv.y = p_obj.z + 0.5f;
            }
        } else {
            if (n_obj.z > 0) {
                uv.x = p_obj.x + 0.5f;
                uv.y = p_obj.y + 0.5f;
            } else {
                uv.x = -p_obj.x + 0.5f;
                uv.y = p_obj.y + 0.5f;
            }
        }
        uv.y = 1.0f - uv.y;
        break;
    }

    default:
        break;
    }

    return uv;
}

double triangle(double x, double a){
    double radius;
    if (a<1){
        radius = 1.0f/a;
    }
    else{
        radius =1.0f;
    }

    if ((x<-radius)||(x>radius)){
        return 0.f;
    }
    else{
        return (1.f-fabs(x)/radius)/radius;
    }
}

std::vector<Image*> texture::buildTexturePyramid(Image* img) {
    std::vector<Image*> pyramid;
    pyramid.push_back(img);

    Image* current = img;
    while (current->width > 1 || current->height > 1) {
        int newWidth  = std::max(1, current->width / 2);
        int newHeight = std::max(1, current->height / 2);

        Image* downsampled = new Image{new RGBA[newWidth * newHeight], newWidth, newHeight};

        double a = 0.5; // triangle filter width scaling

        // precompute horizontal weights for each new pixel
        std::vector<std::vector<float>> hWeights(newWidth);
        std::vector<std::vector<int>> hIndices(newWidth);

        float radius = (a > 1) ? 1 : 1.0 / a;

        for (int x = 0; x < newWidth; x++) {
            float center = x / a + (1 - a) / (2 * a);
            int left = std::ceil(center - radius);
            int right = std::floor(center + radius);

            std::vector<float> w;
            std::vector<int> idx;

            for (int i = left; i <= right; i++) {
                int clamped = glm::clamp(i, 0, current->width - 1);
                w.push_back(triangle(i - center, a));
                idx.push_back(clamped);
            }

            hWeights[x] = std::move(w);
            hIndices[x] = std::move(idx);
        }

        // horizontal pass
        std::vector<glm::vec3> horizBuffer(newWidth * current->height, glm::vec3(0));
        for (int y = 0; y < current->height; y++) {
            for (int x = 0; x < newWidth; x++) {
                glm::vec3 sum(0);
                float wSum = 0.0f;
                for (size_t k = 0; k < hIndices[x].size(); k++) {
                    RGBA px = current->data[y * current->width + hIndices[x][k]];
                    sum += glm::vec3(px.r, px.g, px.b) * hWeights[x][k];
                    wSum += hWeights[x][k];
                }
                horizBuffer[y * newWidth + x] = sum / wSum;
            }
        }

        // vertical pass
        for (int y = 0; y < newHeight; y++) {
            float center = y / a + (1 - a) / (2 * a);
            int top = std::ceil(center - radius);
            int bottom = std::floor(center + radius);

            std::vector<int> vIdx;
            std::vector<float> vW;
            for (int i = top; i <= bottom; i++) {
                int clamped = glm::clamp(i, 0, current->height - 1);
                vIdx.push_back(clamped);
                vW.push_back(triangle(i - center, a));
            }

            for (int x = 0; x < newWidth; x++) {
                glm::vec3 sum(0);
                float wSum = 0.0f;
                for (size_t k = 0; k < vIdx.size(); k++) {
                    sum += horizBuffer[vIdx[k] * newWidth + x] * vW[k];
                    wSum += vW[k];
                }

                RGBA outPixel = {
                    (uint8_t)glm::clamp(sum.r / wSum, 0.0f, 255.0f),
                    (uint8_t)glm::clamp(sum.g / wSum, 0.0f, 255.0f),
                    (uint8_t)glm::clamp(sum.b / wSum, 0.0f, 255.0f),
                    255
                };
                downsampled->data[y * newWidth + x] = outPixel;
            }
        }

        pyramid.push_back(downsampled);
        current = downsampled;
    }

    return pyramid;
}



