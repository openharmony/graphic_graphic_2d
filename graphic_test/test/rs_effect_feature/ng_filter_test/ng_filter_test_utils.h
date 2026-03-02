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

#ifndef NG_FILTER_TEST_UTILS_H
#define NG_FILTER_TEST_UTILS_H

#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"

namespace OHOS::Rosen {
std::shared_ptr<RSNGMaskBase> CreateMask(RSNGEffectType type);
std::shared_ptr<RSNGFilterBase> CreateFilter(RSNGEffectType type);

const std::string APPEARANCE_TEST_JPG_PATH = "/data/local/tmp/appearance_test.jpg";
const std::string BG_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";
const std::string MASK_RGB_PATH = "/data/local/tmp/Images/maskRGB.png";
const std::string MASK_SHADOW_PATH = "/data/local/tmp/Images/maskShadow.png";

// Default values
const Vector2f DEFAULT_BLUR_PARAMS = Vector2f(20.0f, 3.0f);
const Vector2f DEFAULT_WEIGHTS_EMBOSS = Vector2f(1.0f, 0.5f); // (envLight, sd)
const Vector2f DEFAULT_WEIGHTS_EDL = Vector2f(1.0f, 1.0f); // (envLight, sd)
const Vector2f DEFAULT_BG_RATES = Vector2f(-1.8792225f, 2.7626955f);
const Vector3f DEFAULT_BG_KBS = Vector3f(0.0073494f, 0.0998859f, 1.2f);
const Vector3f DEFAULT_BG_POS = Vector3f(0.3f, 0.5f, 0.5f);
const Vector3f DEFAULT_BG_NEG = Vector3f(0.5f, 1.0f, 1.0f);
const Vector3f DEFAULT_REFRACT_PARAMS = Vector3f(1.0f, 0.3f, 0.3f);
const Vector3f DEFAULT_SD_PARAMS = Vector3f(-50.0f, 6.0f, 6.62f);
const Vector2f DEFAULT_SD_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_SD_KBS = Vector3f(0.9f, 0.0f, 1.0f);
const Vector3f DEFAULT_SD_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_SD_NEG = Vector3f(3.0f, 2.0f, 1.0f);
const Vector3f DEFAULT_ENV_LIGHT_PARAMS = Vector3f(50.0f, 20.0f, 5.0f);
const Vector2f DEFAULT_ENV_LIGHT_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_ENV_LIGHT_KBS = Vector3f(0.8f, 0.27451f, 2.0f);
const Vector3f DEFAULT_ENV_LIGHT_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_ENV_LIGHT_NEG = Vector3f(3.0f, 2.0f, 1.0f);
const Vector2f DEFAULT_ED_LIGHT_PARAMS = Vector2f(2.0f, 2.0f);
const Vector2f DEFAULT_ED_LIGHT_ANGLES = Vector2f(40.0f, 20.0f);
const Vector2f DEFAULT_ED_LIGHT_DIR = Vector2f(2.5f, 2.5f);
const Vector2f DEFAULT_ED_LIGHT_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_ED_LIGHT_KBS = Vector3f(0.6027f, 0.627451f, 2.0f);
const Vector3f DEFAULT_ED_LIGHT_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_ED_LIGHT_NEG = Vector3f(3.2f, 2.0f, 1.0f);
const Vector4f DEFAULT_MATERIAL_COLOR = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
const bool DEFAULT_BASE_VIBRANCY_ENABLED = true;
const float DEFAULT_SAMPLING_SCALE = 1.0f;
 
// FrostedGlass Params
// 1. blurParams: [1e-6, 200], [1, 20]
const std::vector<Vector2f> blurparamsParams = {
    Vector2f{1e-6f, 1.0f},
    Vector2f{1e-6f, 5.0f},
    Vector2f{1e-6f, 20.0f},
    Vector2f{5.0f, 5.0f},
    Vector2f{200.0f, 1.0f},
    Vector2f{200.0f, 20.0f}
};

// 2. weightsEmboss: [0, 5]
const std::vector<Vector2f> weightsEmbossParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.0f, 5.0f},
    Vector2f{5.0f, 0.0f},
    Vector2f{3.0f, 3.0f},
    Vector2f{3.0f, 5.0f},
    Vector2f{5.0f, 5.0f}
};

// 3. weightsEdl: [0, 5]
const std::vector<Vector2f> weightsEdlParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.0f, 5.0f},
    Vector2f{5.0f, 0.0f},
    Vector2f{3.0f, 3.0f},
    Vector2f{1.0f, 1.0f},
    Vector2f{5.0f, 5.0f}
};

// 4. bgRates: [-20, 20]
const std::vector<Vector2f> bgRatesParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{-1.8f, 2.8f},
    Vector2f{-1.8f, 15.0f},
    Vector2f{-10.0f, 2.5f},
    Vector2f{-20.0f, -20.0f},
    Vector2f{20.0f, 20.0f}
};

// 5. sdRates: [-20, 20]
const std::vector<Vector2f> sdRatesParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.0f, 1.0f},
    Vector2f{1.0f, 0.0f},
    Vector2f{10.0f, 0.0f},
    Vector2f{-20.0f, -20.0f},
    Vector2f{20.0f, 20.0f}
};

// 6. envLightRates: [-20, 20]
const std::vector<Vector2f> envLightRatesParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.0f, 1.0f},
    Vector2f{1.0f, 0.0f},
    Vector2f{10.0f, 0.0f},
    Vector2f{-20.0f, -20.0f},
    Vector2f{20.0f, 20.0f}
};

// 7. edLightRates: [-20, 20]
const std::vector<Vector2f> edLightRatesParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.0f, 1.0f},
    Vector2f{1.0f, 0.0f},
    Vector2f{10.0f, 0.0f},
    Vector2f{-20.0f, -20.0f},
    Vector2f{20.0f, 20.0f}
};

// 8. bgKBS: KB[-20,20], S[0,20]
const std::vector<Vector3f> bgKBSParams = {
    Vector3f{1.0f, 0.0f, 1.0f},
    Vector3f{1.0f, 10.0f, 1.0f},
    Vector3f{10.0f, 10.0f, 10.0f},
    Vector3f{-10.0f, 0.0f, 1.0f},
    Vector3f{-20.0f, -20.0f, 0.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 9. sdKBS: KB[-20,20], S[0,20]
const std::vector<Vector3f> sdKBSParams = {
    Vector3f{1.0f, 0.0f, 1.0f},
    Vector3f{1.0f, 10.0f, 1.0f},
    Vector3f{10.0f, 10.0f, 10.0f},
    Vector3f{-10.0f, 0.0f, 1.0f},
    Vector3f{20.0f, 20.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, 20.0f}
};

// 10. envLightKBS: KB[-20,20], S[0,20]
const std::vector<Vector3f> envLightKBSParams = {
    Vector3f{1.0f, 0.0f, 1.0f},
    Vector3f{1.0f, 10.0f, 1.0f},
    Vector3f{10.0f, 10.0f, 10.0f},
    Vector3f{-10.0f, 0.0f, 1.0f},
    Vector3f{20.0f, 20.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, 20.0f}
};

// 11. edLightKBS: KB[-20,20], S[0,20]
const std::vector<Vector3f> edLightKBSParams = {
    Vector3f{0.6f, 0.6f, 2.0f},
    Vector3f{0.6f, 10.0f, 2.0f},
    Vector3f{0.6f, 0.8f, 10.0f},
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, 0.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 12. bgPos: [-20,20]
const std::vector<Vector3f> bgPosParams = {
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{0.5f, 0.5f, 0.5f},
    Vector3f{10.0f, 0.5f, 0.5f},
    Vector3f{0.5f, 10.0f, 0.1f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 13. bgNeg: [-20,20]
const std::vector<Vector3f> bgNegParams = {
    Vector3f{3.2f, 2.0f, 1.2f},
    Vector3f{0.0f, 2.0f, 1.2f},
    Vector3f{3.0f, 0.0f, 1.0f},
    Vector3f{3.5f, 2.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 14. sdPos: [-20,20]
const std::vector<Vector3f> sdPosParams = {
    Vector3f{1.0f, 2.0f, 1.5f},
    Vector3f{0.0f, 2.0f, 1.5f},
    Vector3f{1.0f, 0.0f, 1.5f},
    Vector3f{1.0f, 2.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 15. sdNeg: [-20,20]
const std::vector<Vector3f> sdNegParams = {
    Vector3f{3.0f, 2.0f, 1.0f},
    Vector3f{0.0f, 2.0f, 1.0f},
    Vector3f{3.0f, 0.0f, 1.0f},
    Vector3f{3.0f, 2.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 16. envLightPos: [-20,20]
const std::vector<Vector3f> envLightPosParams = {
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{-10.0f, 0.0f, 0.0f},
    Vector3f{10.0f, 10.0f, 10.0f},
    Vector3f{0.0f, 10.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 17. envLightNeg: [-20,20]
const std::vector<Vector3f> envLightNegParams = {
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{-10.0f, 0.0f, 0.0f},
    Vector3f{10.0f, 10.0f, 10.0f},
    Vector3f{0.0f, 10.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 18. edLightPos: [-20,20]
const std::vector<Vector3f> edLightPosParams = {
    Vector3f{1.5f, 1.5f, 1.5f},
    Vector3f{0.0f, 1.5f, 1.5f},
    Vector3f{1.5f, 0.0f, 1.5f},
    Vector3f{1.5f, 1.5f, 0.0f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 19. edLightNeg: [-20,20]
const std::vector<Vector3f> edLightNegParams = {
    Vector3f{3.2f, 2.0f, 1.2f},
    Vector3f{0.0f, 2.0f, 1.2f},
    Vector3f{3.0f, 0.0f, 1.0f},
    Vector3f{3.5f, 2.0f, 0.0f},
    Vector3f{-20.0f, -20.0f, -20.0f},
    Vector3f{20.0f, 20.0f, 20.0f}
};

// 20. sdParams: [−500,500], [0,250], [0,250]
const std::vector<Vector3f> sdParamsParams = {
    Vector3f{-50.0f, 6.0f, 6.0f},
    Vector3f{-5.0f, 60.0f, 60.0f},
    Vector3f{-500.0f, 6.0f, 6.0f},
    Vector3f{500.0f, 60.0f, 6.0f},
    Vector3f{-50.0f, 0.0f, 60.0f},
    Vector3f{50.0f, 250.0f, 6.0f},
    Vector3f{50.0f, 60.0f, 0.0f},
    Vector3f{50.0f, 6.0f, 250.0f},
};

// 21. envLightParams: [0,500], [0,250], [0,250]
const std::vector<Vector3f> envLightParamsParams = {
    Vector3f{1.0f, 6.0f, 6.0f},
    Vector3f{10.0f, 60.0f, 60.0f},
    Vector3f{0.0f, 6.0f, 6.0f},
    Vector3f{500.0f, 60.0f, 6.0f},
    Vector3f{10.0f, 0.0f, 60.0f},
    Vector3f{10.0f, 250.0f, 60.0f},
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{0.0f, 0.0f, 250.0f}
};

// 22. refractParams: [-1,1], [0,10], [0,1]
const std::vector<Vector3f> refractParamsParams = {
    Vector3f{0.0f, 0.2f, 0.2f},
    Vector3f{1.0f, 0.2f, 0.2f},
    Vector3f{1.0f, 0.0f, 0.2f},
    Vector3f{-1.0f, 10.0f, 0.2f},
    Vector3f{-1.0f, 2.0f, 0.0f},
    Vector3f{-1.0f, 2.0f, 1.0f}
};

// 23. edLightParams: [0,250]
const std::vector<Vector2f> edLightParamsParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{10.0f, 10.0f},
    Vector2f{100.0f, 100.0f},
    Vector2f{250.0f, 250.0f},
    Vector2f{0.0f, 100.0f},
    Vector2f{100.0f, 0.0f}
};

// 24. edLightAngles: [0,180]
const std::vector<Vector2f> edLightAnglesParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{45.0f, 45.0f},
    Vector2f{90.0f, 90.0f},
    Vector2f{180.0f, 180.0f},
    Vector2f{0.0f, 180.0f},
    Vector2f{180.0f, 0.0f}
};

// 25. edLightDir: [-1,1]
const std::vector<Vector2f> edLightDirParams = {
    Vector2f{0.0f, 0.0f},
    Vector2f{1.0f, 1.0f},
    Vector2f{-1.0f, -1.0f},
    Vector2f{0.0f, 1.0f},
    Vector2f{1.0f, 0.0f},
    Vector2f{0.5f, 0.5f}
};

// 26. baseVibrancyEnabled: {true, false}
const std::vector<bool> baseVibrancyEnabledParams = {true, false};

// 27. samplingScale
const std::vector<float> samplingScaleParams = {-1.0f, 0.0f, 0.1f, 0.2f, 0.5f, 10.0f};

// 28. materialColor: [0.0, 1.0]
const std::vector<Vector4f> materialColorParams = {
    Vector4f{0.0f, 0.0f, 0.0f, 0.5f},
    Vector4f{1.0f, 0.0f, 0.0f, 0.5f},
    Vector4f{0.0f, 1.0f, 0.0f, 0.5f},
    Vector4f{0.0f, 0.0f, 1.0f, 0.5f},
    Vector4f{0.3f, 0.3f, 0.3f, 0.5f},
    Vector4f{1.0f, 1.0f, 1.0f, 0.5f}
};

constexpr int DOUBLE_RIPPLE_MASK_PARAMS_COUNT = 7;
const std::vector<std::array<float, DOUBLE_RIPPLE_MASK_PARAMS_COUNT>> doubleRippleMaskParams = {
    {-20.0f, -20.0f, -20.0f, -20.0f, -1.0f, -1.0f, -1.0f},
    {0.15f, 0.15f, 0.15f, 0.15f, 0.4f, 0.35f, 0.75f},
    {0.25f, 0.25f, 0.75f, 0.75f, 0.5f, 0.5f, 0.5f},
    {0.15f, 0.15f, 0.65f, 0.65f, 0.8f, 0.15f, 0.35f},
    {10.0f, 10.0f, 0.75f, 0.75f, 0.5f, 0.5f, 12.0f},
    {20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f}
};

constexpr int DISPLACEMENT_DISTORT_PARAMS_COUNT = 2;
const std::vector<std::array<float, DISPLACEMENT_DISTORT_PARAMS_COUNT>> displacementDistortParams = {
    {-1, -1}, {6.0f, 6.0f}, {6.0f, 6.0f}, {10.0f, 10.0f}, {3.0f, 7.0f}, {999.0f, 999.0f}
};

constexpr int COLOR_GRADIENT_PARAMS_COUNT = 7;
const std::vector<std::array<float, COLOR_GRADIENT_PARAMS_COUNT>> colorGradientParams = {
    {-1, -1, -1, -1, -1, -1, -1},
    {0.5f, 0.6f, 0.9f, 0.9f, 0.9f, 0.9f, 3.0f},
    {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 1.5f},
    {0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 3.0f},
    {0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f},
    {999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f}
};

constexpr int GRID_WARP_POINT_PARAMS_COUNT = 9;
const std::vector<std::array<Vector2f, GRID_WARP_POINT_PARAMS_COUNT>> gridWarpPointParams = {
    {
        Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f },
        Vector2f{ 0.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f },
        Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.5f, 0.5f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.5f, 0.0f }, Vector2f{ 1.0f, 0.0f },
        Vector2f{ 0.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 1.0f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 1.0f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.5f, -0.1f }, Vector2f{ 1.0f, 0.0f },
        Vector2f{ -0.1f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 1.1f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 1.1f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.5f, 0.1f }, Vector2f{ 1.0f, 0.0f },
        Vector2f{ 0.1f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 0.9f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 0.9f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 1.0f, 0.5f },
        Vector2f{ 0.0f, 1.0f }, Vector2f{ 0.5f, 1.0f }, Vector2f{ 1.0f, 1.0f }
    },
    {
        Vector2f{ -999.0f, 0.0f }, Vector2f{ 0.5f, 0.0f }, Vector2f{ 999.0f, 0.0f },
        Vector2f{ -999.0f, 0.5f }, Vector2f{ 0.5f, 0.5f }, Vector2f{ 999.0f, 0.5f },
        Vector2f{ -999.0f, 1.0f }, Vector2f{ 0.5f, 1.0f }, Vector2f{ 999.0f, 999.0f }
    }
};

constexpr int GRID_WARP_ANGLE_PARAMS_COUNT = 9;
const std::vector<std::array<Vector2f, GRID_WARP_ANGLE_PARAMS_COUNT>> gridWarpAngleParams = {
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ -999.0f, -999.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 90.0f, 90.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 90.0f, 90.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 90.0f, 90.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 90.0f, 90.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 90.0f, 90.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 90.0f, 90.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 90.0f, 90.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 90.0f, 90.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 999.0f, 999.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }
    },
    {
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 0.0f, 0.0f },
        Vector2f{ 999.0f, 999.0f }, Vector2f{ 0.0f, 0.0f }, Vector2f{ 999.0f, 999.0f },
        Vector2f{ 0.0f, 0.0f }, Vector2f{ 999.0f, 999.0f }, Vector2f{ 0.0f, 0.0f }
    }
};

enum class TestDataGroupParamsType {
    INVALID_DATA_MIN,
    VALID_DATA1,
    VALID_DATA2,
    VALID_DATA3,
    INVALID_AND_VALID_DATA,
    INVALID_DATA_MAX,
    COUNT
};

const std::vector<float> gasifyProgressVec = {-1.f, 0.01f, 0.2f, 1.5f};

// BlurFilter初始化函数（2个参数：RadiusX, RadiusY）
inline void InitBlurFilter(std::shared_ptr<RSNGBlurFilter>& blurFilter)
{
    blurFilter->Setter<BlurRadiusXTag>(20.0f);
    blurFilter->Setter<BlurRadiusYTag>(20.0f);
}

// EdgeLight初始化函数（4个参数：Alpha, Color, Bloom, UseRawColor）
inline void InitEdgeLight(std::shared_ptr<RSNGEdgeLightFilter>& edgeLight)
{
    edgeLight->Setter<EdgeLightAlphaTag>(1.0f);
    edgeLight->Setter<EdgeLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    edgeLight->Setter<EdgeLightBloomTag>(false);
    edgeLight->Setter<EdgeLightUseRawColorTag>(false);
}

// FrostedGlassBlurFilter初始化函数（2个参数：Radius, RadiusScaleK）
inline void InitFrostedGlassBlurFilter(std::shared_ptr<RSNGFrostedGlassBlurFilter>& frostedGlassBlurFilter)
{
    frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(20.0f);
    frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusScaleKTag>(1.0f);
}

// MaskTransition初始化函数（1个参数：Factor）
inline void InitMaskTransition(std::shared_ptr<RSNGMaskTransitionFilter>& maskTransition)
{
    maskTransition->Setter<MaskTransitionFactorTag>(0.5f);
}

// SDFEdgeLight初始化函数（4个参数：SpreadFactor, LightMaxIntensity, MinBorderWidth, MaxBorderWidth）
inline void InitSDFEdgeLight(std::shared_ptr<RSNGSDFEdgeLightFilter>& sdfEdgeLight)
{
    sdfEdgeLight->Setter<SDFEdgeLightSpreadFactorTag>(1.0f);
    sdfEdgeLight->Setter<SDFEdgeLightLightMaxIntensityTag>(2.0f);
    sdfEdgeLight->Setter<SDFEdgeLightMinBorderWidthTag>(1.0f);
    sdfEdgeLight->Setter<SDFEdgeLightMaxBorderWidthTag>(10.0f);
}

// VariableRadiusBlur初始化函数（1个参数：Radius）
inline void InitVariableRadiusBlur(std::shared_ptr<RSNGVariableRadiusBlurFilter>& variableRadiusBlur)
{
    variableRadiusBlur->Setter<VariableRadiusBlurRadiusTag>(20.0f);
}

}  // namespace OHOS::Rosen
#endif // NG_FILTER_TEST_UTILS_H