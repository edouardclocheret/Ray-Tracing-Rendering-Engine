#include "sampletexture.h"



RGBA SampleTexture::NNSampleTexture(Image* img, SceneFileMap& map, glm::vec2 uv) {
    float u = fmod(uv.x * map.repeatU, 1.0f);
    float v = fmod(uv.y * map.repeatV, 1.0f);
    if (u < 0) u += 1.0f;
    if (v < 0) v += 1.0f;

    int x = std::min(int(u * img->width), img->width - 1);
    int y = std::min(int(v * img->height), img->height - 1);
    int index = y * img->width + x;
    return img->data[index];
}


RGBA SampleTexture::BilinSampleTexture(Image* img, SceneFileMap& map, glm::vec2 uv) {
    float u = fmod(uv.x * map.repeatU, 1.0f);
    float v = fmod(uv.y * map.repeatV, 1.0f);
    if (u < 0) u += 1.0f;
    if (v < 0) v += 1.0f;

    // convert to pixel coordinates (v downwards)
    float x = u * img->width  - 0.5f;
    float y = v * img->height - 0.5f;

    int x_left   = static_cast<int>(floor(x));
    int y_top    = static_cast<int>(floor(y));
    int x_right  = x_left + 1;
    int y_bottom = y_top + 1;

    x_left   = (x_left   % img->width  + img->width)  % img->width;
    x_right  = (x_right  % img->width  + img->width)  % img->width;
    y_top    = (y_top    % img->height + img->height) % img->height;
    y_bottom = (y_bottom % img->height + img->height) % img->height;

    float alpha_x = x - floor(x);
    float alpha_y = y - floor(y);

    const RGBA& TL = img->data[y_top    * img->width + x_left];
    const RGBA& TR = img->data[y_top    * img->width + x_right];
    const RGBA& BL = img->data[y_bottom * img->width + x_left];
    const RGBA& BR = img->data[y_bottom * img->width + x_right];

    RGBA I_top;
    I_top.r = (1 - alpha_x) * TL.r + alpha_x * TR.r;
    I_top.g = (1 - alpha_x) * TL.g + alpha_x * TR.g;
    I_top.b = (1 - alpha_x) * TL.b + alpha_x * TR.b;

    RGBA I_bottom;
    I_bottom.r = (1 - alpha_x) * BL.r + alpha_x * BR.r;
    I_bottom.g = (1 - alpha_x) * BL.g + alpha_x * BR.g;
    I_bottom.b = (1 - alpha_x) * BL.b + alpha_x * BR.b;

    RGBA I_final;
    I_final.r = (1 - alpha_y) * I_top.r + alpha_y * I_bottom.r;
    I_final.g = (1 - alpha_y) * I_top.g + alpha_y * I_bottom.g;
    I_final.b = (1 - alpha_y) * I_top.b + alpha_y * I_bottom.b;

    return I_final;
}
