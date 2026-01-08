#pragma once

#include "utils/ini_utils.h"
#include "utils/rgba.h"
#include "raytracer/supersampler.h"

#define RAY_TRACE_MAX_DEPTH 4
#define RAY_TRACE_DEFAULT_SPP 64

struct RayTracerConfig {
    bool enableShadow        = false;
    bool enableReflection    = false;
    bool enableRefraction    = false;
    bool enableTextureMap    = false;
    TextureFilterType textureFilterType = TextureFilterType::Nearest;
    bool enableParallelism   = false;
    bool enableSuperSample   = false;
    bool enableAcceleration  = false;
    bool enableDepthOfField  = false;
    int maxRecursiveDepth    = RAY_TRACE_MAX_DEPTH;
    int samplesPerPixel      = RAY_TRACE_DEFAULT_SPP;
    SuperSamplerPattern superSamplerPattern = SuperSamplerPattern::Grid;
    bool onlyRenderNormals   = false;
    bool enableMipMapping    = false;
};


extern RayTracerConfig g_m_Config;
