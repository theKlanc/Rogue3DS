/**
 * @file    SimplexNoise.h
 * @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D, 4D).
 *
 * Copyright (c) 2014-2015 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <cstddef>  // size_t

/**
 * @brief A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D, 4D).
 */
class SimplexNoise {
public:
    // 1D Perlin simplex noise
    static float noise(float x);
    // 2D Perlin simplex noise
    static float noise(float x, float y);

};

float fbmNoise(float x, float y, float frequency, int octaves, float lacunarity, float gain);