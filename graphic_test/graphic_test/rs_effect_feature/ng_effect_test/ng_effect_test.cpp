/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"
#include "../ng_filter_test/ng_sdf_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
using MaskCreator = std::function<std::shared_ptr<RSNGMaskBase>()>;
using MaskConvertor = std::function<std::shared_ptr<RSNGMaskBase>(std::shared_ptr<MaskPara>)>;
const std::string g_foregroundImagePath = "/data/local/tmp/fg_test.jpg";
const std::string g_backgroundImagePath = "/data/local/tmp/bg_test.jpg";

static std::unordered_map<RSNGEffectType, MaskCreator> creatorMask = {
    {RSNGEffectType::DOUBLE_RIPPLE_MASK,
     [] {
         return std::make_shared<RSNGDoubleRippleMask>();
     }},
    {RSNGEffectType::NOISY_FRAME_GRADIENT_MASK,
     [] {
         return std::make_shared<RSNGNoisyFrameGradientMask>();
     }},
};

std::shared_ptr<RSNGMaskBase> CreateMask(RSNGEffectType type)
{
    auto it = creatorMask.find(type);
    return it != creatorMask.end() ? it->second() : nullptr;
}

// Param arrays
constexpr int DOUBLE_RIPPLE_MASK_PARAMS_COUNT = 7;
std::vector<std::array<float, DOUBLE_RIPPLE_MASK_PARAMS_COUNT>> doubleRippleMaskParams = {
    {-20.0f, -20.0f, -20.0f, -20.0f, -1.0f, -1.0f, -1.0f},
    {0.15f, 0.15f, 0.15f, 0.15f, 0.4f, 0.35f, 0.75f},
    {0.25f, 0.25f, 0.75f, 0.75f, 0.5f, 0.5f, 0.5f},
    {0.15f, 0.15f, 0.65f, 0.65f, 0.8f, 0.15f, 0.35f},
    {10.0f, 10.0f, 0.75f, 0.75f, 0.5f, 0.5f, 12.0f},
    {20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f},
};

constexpr int CIRCLE_FLOWLIGHT_PARAMS_COUNT = 12;
std::vector<std::array<std::variant<float, Vector4f>, CIRCLE_FLOWLIGHT_PARAMS_COUNT>> circleFlowlightParams = {
    {
        Vector4f{-1.0f, -1.0f, -1.0f, -1.0f}, Vector4f{-1.0f, -1.0f, -1.0f, -1.0f},
        Vector4f{-1.0f, -1.0f, -1.0f, -1.0f}, Vector4f{-1.0f, -1.0f, -1.0f, -1.0f},
        Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f}, Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f},
        Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f}, Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f},
        Vector4f{1000.0f, 1000.0f, 1000.0f, 1000.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        4.0f, 1.0f
    },
    {
        Vector4f{0.9921f, 0.5020f, 0.0196f, 1.0f}, Vector4f{0.0039f, 0.5255f, 1.000f, 1.0f},
        Vector4f{0.0569f, 0.3429f, 1.000f, 1.0f}, Vector4f{0.5510f, 0.3412f, 0.5098f, 1.0f},
        Vector4f{0.500f, 0.600f, 0.300f, 0.5f}, Vector4f{0.300f, 0.300f, 0.300f, 0.3f},
        Vector4f{0.200f, 0.400f, 0.600f, 0.8f}, Vector4f{0.300f, 0.700f, 0.300f, 0.7f},
        Vector4f{0.300f, 0.300f, 0.700f, 0.7f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        4.0f, 1.0f
    },
    {
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{0.5f, 0.5f, 0.5f, 1.0f},
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{0.5f, 0.5f, 0.5f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        4.0f, 1.0f
    },
    {
        Vector4f{0.8f, 0.2f, 0.6f, 1.0f}, Vector4f{0.2f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.6f, 0.4f, 0.8f, 1.0f}, Vector4f{0.4f, 0.6f, 0.2f, 1.0f},
        Vector4f{2.0f, 0.5f, 1.5f, 0.8f}, Vector4f{0.5f, 2.0f, 0.8f, 1.5f},
        Vector4f{1.5f, 0.8f, 0.5f, 2.0f}, Vector4f{0.8f, 1.5f, 2.0f, 0.5f},
        Vector4f{0.5f, 1.5f, 0.8f, 2.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        4.0f, 1.0f
    },
    {
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{0.5f, 0.5f, 0.5f, 1.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        4.0f, 1.0f
    },
    {
        Vector4f{2.0f, 2.0f, 2.0f, 2.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{2.0f, 2.0f, 2.0f, 2.0f}, Vector4f{2.0f, 2.0f, 2.0f, 2.0f},
        Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f}, Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f},
        Vector4f{10000.0f, 10000.0f, 10000.0f, 10000.0f}, Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        4.0f, 1.0f
    },
};

constexpr int ROUNDED_RECT_FLOWLIGHT_PARAMS_COUNT = 12;
std::vector<std::array<std::variant<Vector2f, float, Vector4f>, ROUNDED_RECT_FLOWLIGHT_PARAMS_COUNT>>
    roundedRectFlowlightParams = {
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        -1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 0.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 0.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 1.0f, 1.0f},
        1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.8f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        1.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{2.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        2.0f
    },
    {
        Vector2f{0.0f, 1.0f},
        0.2f, 0.2f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        -1.0f
    }
};

constexpr int COLOR_GRADIENMT_EFFECT_PARAMS_COUNT = 15;
std::vector<std::array<std::variant<Vector2f, float, Vector4f>, COLOR_GRADIENMT_EFFECT_PARAMS_COUNT>>
    colorGradientEffectParams = {
    {
        Vector4f{0.0f, 0.0f, -1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector2f{0.0f, 1.0f},
        Vector2f{1.0f, 0.0f},
        Vector2f{0.0f, 0.0f},
        Vector2f{1.0f, 1.0f},
        5.4f, 5.4f, 5.4f, 5.4f, 4.0f, 6.0f, 20.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector2f{0.0f, 1.0f},
        Vector2f{1.0f, 0.0f},
        Vector2f{0.0f, 0.0f},
        Vector2f{1.0f, 1.0f},
        5.4f, 5.4f, 5.4f, 5.4f, 4.0f, 6.0f, 20.0f
    },
    {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector2f{0.0f, 0.5f},
        Vector2f{0.5f, 0.0f},
        Vector2f{1.0f, 0.5f},
        Vector2f{0.5f, 1.0f},
        5.4f, 5.4f, 5.4f, 5.4f, 4.0f, 6.0f, 20.0f
    },
    {
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector2f{0.0f, 0.5f},
        Vector2f{0.5f, 0.0f},
        Vector2f{1.0f, 0.5f},
        Vector2f{0.5f, 1.0f},
        5.4f, 5.4f, 5.4f, 5.4f, 4.0f, 6.0f, 20.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector2f{0.0f, -1.0f},
        Vector2f{1.0f, 0.0f},
        Vector2f{0.0f, 0.0f},
        Vector2f{1.0f, 1.0f},
        5.4f, 5.4f, 5.4f, 5.4f, 4.0f, 6.0f, 20.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f},
        Vector2f{0.0f, 1.0f},
        Vector2f{1.0f, 0.0f},
        Vector2f{0.0f, 0.0f},
        Vector2f{1.0f, 1.0f},
        5.4f, 1.0f, 5.4f, 1.0f, 4.0f, 6.0f, 20.0f
    }
};

constexpr int NOISY_FRAME_GRADIENT_MASK_PARAMS_COUNT = 9;
std::vector<std::array<std::variant<Vector2f, float, Vector4f>, NOISY_FRAME_GRADIENT_MASK_PARAMS_COUNT>>
    noisyFrameGradientMaskParams = {
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.0f, -1.0f},
        Vector2f{0.0f, -1.0f},
        Vector2f{0.0f, -1.0f},
        Vector2f{1.0f, 1.0f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{1.0f, 1.0f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.1f, 0.0f},
        Vector2f{0.1f, 0.0f},
        Vector2f{0.1f, 0.0f},
        Vector2f{1.0f, 1.0f},
        Vector2f{0.0f, 0.0f},
        0.2f, 0.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.4f,
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{0.7f, 0.7f},
        Vector2f{0.0f, 0.0f},
        0.0f, 0.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.1f, 0.0f},
        Vector2f{0.1f, 0.0f},
        Vector2f{0.1f, 0.0f},
        Vector2f{1.0f, 1.0f},
        Vector2f{0.0f, 0.0f},
        -1.0f, 0.0f
    },
    {
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        0.1f,
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{0.0f, 0.1f},
        Vector2f{-999.0f, -999.0f},
        Vector2f{999.0f, 999.0f},
        0.0f, 0.0f
    }
};

constexpr int ROUNDED_RECT_AIBAR_RECT_HALO_COUNT = 12;
std::vector<std::array<std::variant<Vector4f, Vector2f, float>, ROUNDED_RECT_AIBAR_RECT_HALO_COUNT>>
    aibarRectHaloParams = {
    {
        Vector4f{0.0f, -0.8f, 0.3f, 0.1f},
        Vector4f{0.5f, 0.3f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.6f, 0.5f, 1.0f},
        Vector4f{0.6f, 0.4f, 0.8f, 1.0f},
        Vector4f{1.0f, 0.95f, 0.9f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.5f
    },
    {
        Vector4f{0.0f, -0.8f, 0.5f, 0.1f},
        Vector4f{0.5f, 0.3f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.6f, 0.5f, 1.0f},
        Vector4f{0.6f, 0.4f, 0.8f, 1.0f},
        Vector4f{1.0f, 0.95f, 0.9f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.6f
    },
    {
        Vector4f{0.0f, -0.8f, 0.3f, 0.6f},
        Vector4f{0.3f, 1.0f, 0.5f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.6f, 1.0f},
        Vector4f{0.6f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.9f, 1.0f, 0.95f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.7f
    },
    {
        Vector4f{0.5f, -0.8f, 0.3f, 0.1f},
        Vector4f{0.3f, 1.0f, 0.5f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.6f, 1.0f},
        Vector4f{0.6f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.9f, 1.0f, 0.95f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.3f
    },
    {
        Vector4f{0.5f, -0.8f, 0.0f, 0.1f},
        Vector4f{0.3f, 1.0f, 0.5f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.6f, 1.0f},
        Vector4f{0.6f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.9f, 1.0f, 0.95f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.5f
    },
    {
        Vector4f{0.5f, -0.8f, 1.3f, 0.1f},
        Vector4f{0.3f, 1.0f, 0.5f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.6f, 1.0f},
        Vector4f{0.6f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.9f, 1.0f, 0.95f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.5f
    },
    {
        Vector4f{0.5f, -0.5f, 0.1f, 1.8f},
        Vector4f{0.3f, 1.0f, 0.5f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.6f, 1.0f},
        Vector4f{0.6f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.9f, 1.0f, 0.95f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.5f
    },
    {
        Vector4f{0.5f, -0.5f, 0.1f, -0.5f},
        Vector4f{0.3f, 1.0f, 0.5f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.6f, 1.0f},
        Vector4f{0.6f, 0.8f, 0.4f, 1.0f},
        Vector4f{0.9f, 1.0f, 0.95f, 1.0f},
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f},
        Vector4f{0.2f, 0.3f, 0.4f, 0.5f},
        5.0f, 0.5f
    }
};

// Frosted Glass Default values
const float DEFAULT_BLUR_PARAMS = 1.0f; // K times downsample
const Vector2f DEFAULT_WEIGHTS_EMBOSS = Vector2f(1.0f, 0.5f);
const Vector2f DEFAULT_WEIGHTS_EDL = Vector2f(1.0f, 1.0f);
// Background darken parameters
const Vector2f DEFAULT_BG_RATES = Vector2f(-1.8792225f, 2.7626955f);
const Vector3f DEFAULT_BG_KBS = Vector3f(0.0073494f, 0.0998859f, 1.2f);
const Vector3f DEFAULT_BG_POS = Vector3f(0.3f, 0.5f, 0.5f);
const Vector3f DEFAULT_BG_NEG = Vector3f(0.5f, 1.0f, 1.0f);
// Refraction parameters
const Vector3f DEFAULT_REFRACT_PARAMS = Vector3f(1.0f, 0.3f, 0.3f);
// Inner shadow parameters
const Vector3f DEFAULT_SD_PARAMS = Vector3f(-50.0f, 6.0f, 6.62f);
const Vector2f DEFAULT_SD_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_SD_KBS = Vector3f(0.9f, 0.0f, 1.0f);
const Vector3f DEFAULT_SD_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_SD_NEG = Vector3f(3.0f, 2.0f, 1.0f);
// Env refraction parameters
const Vector2f DEFAULT_ENV_LIGHT_PARAMS = Vector2f(20.0f, 5.0f); // envB, envS
const Vector2f DEFAULT_ENV_LIGHT_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_ENV_LIGHT_KBS = Vector3f(0.8f, 0.27451f, 2.0f);
const Vector3f DEFAULT_ENV_LIGHT_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_ENV_LIGHT_NEG = Vector3f(3.0f, 2.0f, 1.0f);
// Edge highlights parameters
const Vector2f DEFAULT_ED_LIGHT_PARAMS = Vector2f(2.0f, 2.0f);
const Vector2f DEFAULT_ED_LIGHT_ANGLES = Vector2f(40.0f, 20.0f);
const Vector2f DEFAULT_ED_LIGHT_DIR = Vector2f(2.5f, 2.5f);
const Vector2f DEFAULT_ED_LIGHT_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_ED_LIGHT_KBS = Vector3f(0.6027f, 0.627451f, 2.0f);
const Vector3f DEFAULT_ED_LIGHT_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_ED_LIGHT_NEG = Vector3f(3.2f, 2.0f, 1.0f);
const Vector4f DEFAULT_MATERIAL_COLOR = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
 
// Blur parameters
const float DEFAULT_BLUR_RADIUS = 48.0f;
const float DEFAULT_BLUR_REFRACT_OUT_PX = 0.010834f;

enum class TestDataGroupParamsType {
    INVALID_DATA_MIN,
    VALID_DATA1,
    VALID_DATA2,
    VALID_DATA3,
    INVALID_AND_VALID_DATA,
    INVALID_DATA_MAX,
    COUNT
};

constexpr uint32_t NUM_0 = 0;
constexpr uint32_t NUM_1 = 1;
constexpr uint32_t NUM_2 = 2;
constexpr uint32_t NUM_3 = 3;
constexpr uint32_t NUM_4 = 4;
constexpr uint32_t NUM_5 = 5;
constexpr uint32_t NUM_6 = 6;
constexpr uint32_t NUM_7 = 7;
constexpr uint32_t NUM_8 = 8;
constexpr uint32_t NUM_9 = 9;
constexpr uint32_t NUM_10 = 10;
constexpr uint32_t NUM_11 = 11;
constexpr uint32_t NUM_12 = 12;
constexpr uint32_t NUM_13 = 13;
constexpr uint32_t NUM_14 = 14;
}

class NGEffectTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    std::shared_ptr<Rosen::RSCanvasNode> SetCommonBackgroundNode()
    {
        // set background node
        auto backgroundTestNode = SetUpNodeBgImage(g_backgroundImagePath, {0, 0, screenWidth, screenHeight});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        return backgroundTestNode;
    }
 
    std::shared_ptr<RSEffectNode> SetDefaultFrostedGlassBlurEffectNode()
    {
        auto effectNode = RSEffectNode::Create();
        effectNode->SetBounds({0, 0, screenWidth, screenHeight});
        effectNode->SetFrame({0, 0, screenWidth, screenHeight});
 
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(DEFAULT_BLUR_RADIUS);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRefractOutPxTag>(DEFAULT_BLUR_REFRACT_OUT_PX);
 
        effectNode->SetBackgroundNGFilter(frostedGlassBlurFilter);
        effectNode->SetClipToBounds(true);
        GetRootNode()->AddChild(effectNode);
        RegisterNode(effectNode);
 
        return effectNode;
    }
 
    void SetEffectChildNode(const int i, const int columnCount, const int rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGFrostedGlassEffect>& frostedGlassEffect)
    {
        auto sizeX = (columnCount != 0) ? (screenWidth / columnCount) : screenWidth;
        auto sizeY = (rowCount != 0) ? (screenHeight * columnCount / rowCount) : screenHeight;

        int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;
 
        // set effect child node
        auto effectChildNode = RSCanvasNode::Create();
        effectChildNode->SetBounds(x, y, sizeX, sizeY);
        effectChildNode->SetFrame(x, y, sizeX, sizeY);
 
        //  apply sdf on effect effect child node
        const RRect defaultRectParam =
            {RectT<float>{sizeX / 4, sizeY / 4, sizeX / 2, sizeY / 2}, sizeX / 16, sizeX / 16};
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, defaultRectParam, defaultRectParam, 0.0);
        effectChildNode->SetSDFShape(sdfShape);
 
        // apply frostedGlassEffect on child node
        effectChildNode->SetBackgroundNGShader(frostedGlassEffect);
 
        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

void SetDoubleRippleMaskParams(const std::shared_ptr<RSNGDoubleRippleMask>& mask, int index)
{
    if (!mask) {
        return;
    }
    mask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f{ doubleRippleMaskParams[index][NUM_0], doubleRippleMaskParams[index][NUM_1] });
    mask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f{ doubleRippleMaskParams[index][NUM_2], doubleRippleMaskParams[index][NUM_3] });
    mask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[index][NUM_4]);
    mask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[index][NUM_5]);
    mask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[index][NUM_6]);
}

void SetRoundedRectFlowlightParams(const std::shared_ptr<RSNGRoundedRectFlowlight>& flowlight, int index)
{
    if (!flowlight) {
        return;
    }
    flowlight->Setter<RoundedRectFlowlightStartEndPositionTag>(
        std::get<Vector2f>(roundedRectFlowlightParams[index][NUM_0]));
    flowlight->Setter<RoundedRectFlowlightWaveLengthTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_1]));
    flowlight->Setter<RoundedRectFlowlightWaveTopTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_2]));
    flowlight->Setter<RoundedRectFlowlightCornerRadiusTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_3]));
    flowlight->Setter<RoundedRectFlowlightBrightnessTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_4]));
    flowlight->Setter<RoundedRectFlowlightScaleTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_5]));
    flowlight->Setter<RoundedRectFlowlightSharpingTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_6]));
    flowlight->Setter<RoundedRectFlowlightFeatheringTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_7]));
    flowlight->Setter<RoundedRectFlowlightFeatheringBezierControlPointsTag>(
        std::get<Vector4f>(roundedRectFlowlightParams[index][NUM_8]));
    flowlight->Setter<RoundedRectFlowlightGradientBezierControlPointsTag>(
        std::get<Vector4f>(roundedRectFlowlightParams[index][NUM_9]));
    flowlight->Setter<RoundedRectFlowlightColorTag>(
        std::get<Vector4f>(roundedRectFlowlightParams[index][NUM_10]));
    flowlight->Setter<RoundedRectFlowlightProgressTag>(
        std::get<float>(roundedRectFlowlightParams[index][NUM_11]));
}

void SetCircleFlowlightParams(const std::shared_ptr<RSNGCircleFlowlight>& flowlight, int index)
{
    if (!flowlight) {
        return;
    }
    flowlight->Setter<CircleFlowlightColor0Tag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_0]));
    flowlight->Setter<CircleFlowlightColor1Tag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_1]));
    flowlight->Setter<CircleFlowlightColor2Tag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_2]));
    flowlight->Setter<CircleFlowlightColor3Tag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_3]));
    flowlight->Setter<CircleFlowlightRotationFrequencyTag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_4]));
    flowlight->Setter<CircleFlowlightRotationAmplitudeTag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_5]));
    flowlight->Setter<CircleFlowlightRotationSeedTag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_6]));
    flowlight->Setter<CircleFlowlightGradientXTag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_7]));
    flowlight->Setter<CircleFlowlightGradientYTag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_8]));
    flowlight->Setter<CircleFlowlightStrengthTag>(std::get<Vector4f>(circleFlowlightParams[index][NUM_9]));
    flowlight->Setter<CircleFlowlightDistortStrengthTag>(std::get<float>(circleFlowlightParams[index][NUM_10]));
    flowlight->Setter<CircleFlowlightBlendGradientTag>(std::get<float>(circleFlowlightParams[index][NUM_11]));
    flowlight->Setter<CircleFlowlightProgressTag>(0.0f);
}

void SetColorGradientEffectParams(const std::shared_ptr<RSNGColorGradientEffect>& colorGradient, int index)
{
    if (!colorGradient) {
        return;
    }
    colorGradient->Setter<ColorGradientEffectColor0Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_0]));
    colorGradient->Setter<ColorGradientEffectColor1Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_1]));
    colorGradient->Setter<ColorGradientEffectColor2Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_2]));
    colorGradient->Setter<ColorGradientEffectColor3Tag>(
        std::get<Vector4f>(colorGradientEffectParams[index][NUM_3]));
    colorGradient->Setter<ColorGradientEffectPosition0Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_4]));
    colorGradient->Setter<ColorGradientEffectPosition1Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_5]));
    colorGradient->Setter<ColorGradientEffectPosition2Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_6]));
    colorGradient->Setter<ColorGradientEffectPosition3Tag>(
        std::get<Vector2f>(colorGradientEffectParams[index][NUM_7]));
    colorGradient->Setter<ColorGradientEffectStrength0Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_8]));
    colorGradient->Setter<ColorGradientEffectStrength1Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_9]));
    colorGradient->Setter<ColorGradientEffectStrength2Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_10]));
    colorGradient->Setter<ColorGradientEffectStrength3Tag>(
        std::get<float>(colorGradientEffectParams[index][NUM_11]));
    colorGradient->Setter<ColorGradientEffectColorNumberTag>(
        std::get<float>(colorGradientEffectParams[index][NUM_12]));
    colorGradient->Setter<ColorGradientEffectBlendTag>(
        std::get<float>(colorGradientEffectParams[index][NUM_13]));
    colorGradient->Setter<ColorGradientEffectBlendKTag>(
        std::get<float>(colorGradientEffectParams[index][NUM_14]));
}

void SetNoisyFrameGradientMaskParams(const std::shared_ptr<RSNGNoisyFrameGradientMask>& mask, int index)
{
    if (!mask) {
        return;
    }
    mask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(
        std::get<Vector4f>(noisyFrameGradientMaskParams[index][NUM_0]));
    mask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(
        std::get<float>(noisyFrameGradientMaskParams[index][NUM_1]));
    mask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_2]));
    mask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_3]));
    mask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_4]));
    mask->Setter<NoisyFrameGradientMaskRRectWHTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_5]));
    mask->Setter<NoisyFrameGradientMaskRRectPosTag>(
        std::get<Vector2f>(noisyFrameGradientMaskParams[index][NUM_6]));
    mask->Setter<NoisyFrameGradientMaskSlopeTag>(
        std::get<float>(noisyFrameGradientMaskParams[index][NUM_7]));
    mask->Setter<NoisyFrameGradientMaskProgressTag>(
        std::get<float>(noisyFrameGradientMaskParams[index][NUM_8]));
}

static void SetAIBarRectHaloParams(const std::shared_ptr<RSNGAIBarRectHalo>& aiBarRectHalo, int index)
{
    aiBarRectHalo->Setter<AIBarRectHaloLTWHTag>(std::get<Vector4f>(aibarRectHaloParams[index][NUM_0]));
    aiBarRectHalo->Setter<AIBarRectHaloColor0Tag>(std::get<Vector4f>(aibarRectHaloParams[index][NUM_1]));
    aiBarRectHalo->Setter<AIBarRectHaloColor1Tag>(std::get<Vector4f>(aibarRectHaloParams[index][NUM_2]));
    aiBarRectHalo->Setter<AIBarRectHaloColor2Tag>(std::get<Vector4f>(aibarRectHaloParams[index][NUM_3]));
    aiBarRectHalo->Setter<AIBarRectHaloColor3Tag>(std::get<Vector4f>(aibarRectHaloParams[index][NUM_4]));
    aiBarRectHalo->Setter<AIBarRectHaloPosition0Tag>(std::get<Vector2f>(aibarRectHaloParams[index][NUM_5]));
    aiBarRectHalo->Setter<AIBarRectHaloPosition1Tag>(std::get<Vector2f>(aibarRectHaloParams[index][NUM_6]));
    aiBarRectHalo->Setter<AIBarRectHaloPosition2Tag>(std::get<Vector2f>(aibarRectHaloParams[index][NUM_7]));
    aiBarRectHalo->Setter<AIBarRectHaloPosition3Tag>(std::get<Vector2f>(aibarRectHaloParams[index][NUM_8]));
    aiBarRectHalo->Setter<AIBarRectHaloStrengthTag>(std::get<Vector4f>(aibarRectHaloParams[index][NUM_9]));
    aiBarRectHalo->Setter<AIBarRectHaloBrightnessTag>(std::get<float>(aibarRectHaloParams[index][NUM_10]));
    aiBarRectHalo->Setter<AIBarRectHaloProgressTag>(std::get<float>(aibarRectHaloParams[index][NUM_11]));
}

static void InitFrostedGlassEffect(std::shared_ptr<RSNGFrostedGlassEffect>& frostedGlassEffect)
{
    frostedGlassEffect->Setter<FrostedGlassEffectBlurParamTag>(DEFAULT_BLUR_PARAMS);
    frostedGlassEffect->Setter<FrostedGlassEffectWeightsEmbossTag>(DEFAULT_WEIGHTS_EMBOSS);
    frostedGlassEffect->Setter<FrostedGlassEffectWeightsEdlTag>(DEFAULT_WEIGHTS_EDL);
    frostedGlassEffect->Setter<FrostedGlassEffectBgRatesTag>(DEFAULT_BG_RATES);
    frostedGlassEffect->Setter<FrostedGlassEffectBgKBSTag>(DEFAULT_BG_KBS);
    frostedGlassEffect->Setter<FrostedGlassEffectBgPosTag>(DEFAULT_BG_POS);
    frostedGlassEffect->Setter<FrostedGlassEffectBgNegTag>(DEFAULT_BG_NEG);
    frostedGlassEffect->Setter<FrostedGlassEffectRefractParamsTag>(DEFAULT_REFRACT_PARAMS);
    frostedGlassEffect->Setter<FrostedGlassEffectSdParamsTag>(DEFAULT_SD_PARAMS);
    frostedGlassEffect->Setter<FrostedGlassEffectSdRatesTag>(DEFAULT_SD_RATES);
    frostedGlassEffect->Setter<FrostedGlassEffectSdKBSTag>(DEFAULT_SD_KBS);
    frostedGlassEffect->Setter<FrostedGlassEffectSdPosTag>(DEFAULT_SD_POS);
    frostedGlassEffect->Setter<FrostedGlassEffectSdNegTag>(DEFAULT_SD_NEG);
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightParamsTag>(DEFAULT_ENV_LIGHT_PARAMS);
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightRatesTag>(DEFAULT_ENV_LIGHT_RATES);
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightKBSTag>(DEFAULT_ENV_LIGHT_KBS);
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightPosTag>(DEFAULT_ENV_LIGHT_POS);
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightNegTag>(DEFAULT_ENV_LIGHT_NEG);
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightParamsTag>(DEFAULT_ED_LIGHT_PARAMS);
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightAnglesTag>(DEFAULT_ED_LIGHT_ANGLES);
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightDirTag>(DEFAULT_ED_LIGHT_DIR);
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightRatesTag>(DEFAULT_ED_LIGHT_RATES);
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightKBSTag>(DEFAULT_ED_LIGHT_KBS);
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightPosTag>(DEFAULT_ED_LIGHT_POS);
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightNegTag>(DEFAULT_ED_LIGHT_NEG);
    frostedGlassEffect->Setter<FrostedGlassEffectMaterialColorTag>(DEFAULT_MATERIAL_COLOR);
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_Circle_Flowlight_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage(g_foregroundImagePath, { x, y, sizeX, sizeY });
        backgroundTestNode->SetForegroundShader(circleFlowlight);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_Circle_Flowlight_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);
        
        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(circleFlowlight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_First_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);
        
        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        circleFlowlight->Append(roundedRectFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage(g_foregroundImagePath, { x, y, sizeX, sizeY });
        backgroundTestNode->SetForegroundShader(circleFlowlight);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_Second_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));
        
        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        roundedRectFlowlight->Append(circleFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage(g_foregroundImagePath, { x, y, sizeX, sizeY });
        backgroundTestNode->SetForegroundShader(roundedRectFlowlight);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_First_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        circleFlowlight->Append(roundedRectFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(circleFlowlight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_Multi_NG_Effect_Circle_Flowlight_Second_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        SetDoubleRippleMaskParams(doubleRippleMask, i);

        // Create rounded rect flowlight effect
        auto roundedRectFlowlight = std::make_shared<RSNGRoundedRectFlowlight>();
        SetRoundedRectFlowlightParams(roundedRectFlowlight, i);

        // Create circle flowLight effect
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        SetCircleFlowlightParams(circleFlowlight, i);
        circleFlowlight->Setter<CircleFlowlightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        roundedRectFlowlight->Append(circleFlowlight);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(roundedRectFlowlight);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

// Test Background Noisy Frame Gradient Mask and Color Gradient Effect
GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Mask_Noisy_Frame_Gradient_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create Noisy Frame Gradient Mask
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameGradientMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);
        SetNoisyFrameGradientMaskParams(noisyFrameGradientMask, i);

        // Create Color Gradient Effect
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        SetColorGradientEffectParams(colorGradient, i);
        colorGradient->Setter<ColorGradientEffectMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(noisyFrameGradientMask));
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(colorGradient);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_AIBAR_RECT_HALO_Test_1)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {

        // Create circle flowLight effect
        auto aiBarRectHalo = std::make_shared<RSNGAIBarRectHalo>();
        SetAIBarRectHaloParams(aiBarRectHalo, i);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(aiBarRectHalo);
        node->SetBackgroundColor(0x88888888);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

// Test Foreground Noisy Frame Gradient Mask and Color Gradient Effect
GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Mask_Noisy_Frame_Gradient_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = static_cast<int>(TestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create Noisy Frame Gradient Mask
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameGradientMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);
        SetNoisyFrameGradientMaskParams(noisyFrameGradientMask, i);

        // Create Color Gradient Effect
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        SetColorGradientEffectParams(colorGradient, i);
        colorGradient->Setter<ColorGradientEffectMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(noisyFrameGradientMask));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage(g_foregroundImagePath, { x, y, sizeX, sizeY });
        foregroundTestNode->SetForegroundShader(colorGradient);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_AIBAR_RECT_HALO_Test_2)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {

        // Create circle flowLight effect
        auto aiBarRectHalo = std::make_shared<RSNGAIBarRectHalo>();
        SetAIBarRectHaloParams(aiBarRectHalo, 4 + i); // invalid LTWH params
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetBackgroundNGShader(aiBarRectHalo);
        node->SetBackgroundColor(0x88888888);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_AIBAR_RECT_HALO_Foreground_Test)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {

        // Create circle flowLight effect
        auto aiBarRectHalo = std::make_shared<RSNGAIBarRectHalo>();
        SetAIBarRectHaloParams(aiBarRectHalo, i);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto node = Rosen::RSCanvasNode::Create();
        node->SetBounds({ x, y, sizeX, sizeY });
        node->SetFrame({ x, y, sizeX, sizeY });
        node->SetForegroundShader(aiBarRectHalo);
        node->SetBackgroundColor(0x88888888);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

// Frosted_Glass Effect tests
GRAPHIC_TEST(NGEffectTest, EFFECT_TEST, Set_NG_Effect_Frosted_Glass_DefaultTest)
{
    const int columnCount = 2;
    const int rowCount = 4;
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
 
    for (int i = 0; i < rowCount; i++) {
        auto frostedGlassEffect = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlassEffect);
        SetEffectChildNode(i, columnCount, rowCount, effectNode, frostedGlassEffect);
    }
}

}  // namespace OHOS::Rosen
