// Copyright (C) 2009-2016, Panagiotis Christopoulos Charitos and contributors.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

// This file contains common code for all shaders. It's optional but it's
// recomended to include it

#ifndef ANKI_SHADERS_COMMON_GLSL
#define ANKI_SHADERS_COMMON_GLSL

// WORKAROUND
#if defined(ANKI_VENDOR_NVIDIA)
#define NVIDIA_LINK_ERROR_WORKAROUND 1
#else
#define NVIDIA_LINK_ERROR_WORKAROUND 0
#endif

// Default precision
#ifndef DEFAULT_FLOAT_PRECISION
#define DEFAULT_FLOAT_PRECISION highp
#endif

#ifndef DEFAULT_INT_PRECISION
#define DEFAULT_INT_PRECISION highp
#endif

precision DEFAULT_FLOAT_PRECISION float;
precision DEFAULT_FLOAT_PRECISION int;

const float EPSILON = 0.000001;
const float PI = 3.14159265358979323846;
const uint UBO_MAX_SIZE = 16384;

// Read from a render target texture
//#define textureRt(tex_, texc_) texture(tex_, texc_)
#define textureRt(tex_, texc_) textureLod(tex_, texc_, 0.0)

// Common locations
#define POSITION_LOCATION 0
#define TEXTURE_COORDINATE_LOCATION 1
#define NORMAL_LOCATION 2
#define TANGENT_LOCATION 3
#define SCALE_LOCATION 1
#define ALPHA_LOCATION 2

#endif
