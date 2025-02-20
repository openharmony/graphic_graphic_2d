/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in comliance with the License.
 * You may obtian a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef DDGR_TEST_DTK_DTK_CONSTANTS_H
#define DDGR_TEST_DTK_DTK_CONSTANTS_H

#include "dtk_test_ext.h"
#include <array>

namespace OHOS {
namespace Rosen {

    // 循环坐标计算常量
    constexpr int LEFT = 200; // 200 is the positions to draw rect
    constexpr int RIGHT = 350; // 350 is the positions to draw rect
    constexpr int VARIATION = 5; // 5 is variables at fixed intervals

    // 数组常量
    // blendModes array
    constexpr std::array<Drawing::BlendMode, 29> blendModes = {Drawing::BlendMode::CLEAR,
        Drawing::BlendMode::SRC, Drawing::BlendMode::DST, Drawing::BlendMode::SRC_OVER,
        Drawing::BlendMode::DST_OVER, Drawing::BlendMode::SRC_IN, Drawing::BlendMode::DST_IN,
        Drawing::BlendMode::SRC_OUT, Drawing::BlendMode::DST_OUT, Drawing::BlendMode::SRC_ATOP,
        Drawing::BlendMode::DST_ATOP, Drawing::BlendMode::XOR, Drawing::BlendMode::PLUS,
        Drawing::BlendMode::MODULATE, Drawing::BlendMode::SCREEN, Drawing::BlendMode::OVERLAY,
        Drawing::BlendMode::DARKEN, Drawing::BlendMode::LIGHTEN, Drawing::BlendMode::COLOR_DODGE,
        Drawing::BlendMode::COLOR_BURN, Drawing::BlendMode::HARD_LIGHT, Drawing::BlendMode::SOFT_LIGHT,
        Drawing::BlendMode::DIFFERENCE, Drawing::BlendMode::EXCLUSION, Drawing::BlendMode::MULTIPLY,
        Drawing::BlendMode::HUE, Drawing::BlendMode::STATURATION, Drawing::BlendMode::COLOR_MODE,
        Drawing::BlendMode::LUMINOSITY
    };

    // blurTypes array
    constexpr std::array<Drawing::BlurType, 4> blurTypes = {
        Drawing::BlurType::NORMAL,
        Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER,
        Drawing::BlurType::INNER
    };
    
    constexpr Drawing::CMSMatrix3x3 SRGBMatrix{{
            {0.436065674f, 0.385147095f, 0.143066406f},
            {0.222488403f, 0.716873169f, 0.060607910f},
            {0.013916016f, 0.097076416f, 0.714096069f}
        }
    };

    constexpr Drawing::CMSTransferFunction PQ = {
        -2.0f, -107 / 128.0f, 32 / 2523.0f, 2413 / 128.0f, -2392 / 128.0f, 8192 / 1305.0f
    };
    
    constexpr float ARR[] = {
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
}
}
#endif