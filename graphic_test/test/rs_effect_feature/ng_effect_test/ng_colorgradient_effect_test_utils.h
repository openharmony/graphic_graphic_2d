/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef NG_COLORGRADIENT_EFFECT_TEST_UTILS_H
#define NG_COLORGRADIENT_EFFECT_TEST_UTILS_H

namespace OHOS::Rosen {
const int32_t SCREEN_WIDTH = 1200;
const int32_t SCREEN_HEIGHT = 2000;
const float BLUR_RADIUS = 10.f;
const size_t GRADIENT_EFFECT_MAX_SZIE = 12;
const std::string g_backgroundImagePath = "/data/local/tmp/Images/backGroundImage.jpg";

// Test parameters for ColorGradientEffect
// Ensuring colors, positions, and strengths have consistent dimensions
struct ColorGradientEffectTestParams {
    std::vector<Vector4f> colors;
    std::vector<Vector2f> positions;
    std::vector<float> strengths;
    float colorNumber;
    float blend;
    float blendK;
    float brightness;
};

// Predefined test parameters with different dimensions (< 12)

// Single color (dimension = 1)
const ColorGradientEffectTestParams SINGLECOLORPARAMS = {
    .colors = {Vector4f{1.0f, 0.0f, 0.0f, 1.0f}},
    .positions = {Vector2f{0.5f, 0.5f}},
    .strengths = {5.0f},
    .colorNumber = 1.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Two colors (dimension = 2)
const ColorGradientEffectTestParams TWOCOLORSPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.5f},
        Vector2f{1.0f, 0.5f}
    },
    .strengths = {5.0f, 5.0f},
    .colorNumber = 2.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Three colors (dimension = 3)
const ColorGradientEffectTestParams THREECOLORSPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.5f},
        Vector2f{0.5f, 0.5f},
        Vector2f{1.0f, 0.5f}
    },
    .strengths = {5.0f, 5.0f, 5.0f},
    .colorNumber = 3.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Four colors (dimension = 4)
const ColorGradientEffectTestParams FOURCOLORSPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.0f},
        Vector2f{1.0f, 0.0f},
        Vector2f{0.0f, 1.0f},
        Vector2f{1.0f, 1.0f}
    },
    .strengths = {5.0f, 5.0f, 5.0f, 5.0f},
    .colorNumber = 4.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Five colors (dimension = 5)
const ColorGradientEffectTestParams FIVECOLORSPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.0f},
        Vector2f{0.25f, 0.0f},
        Vector2f{0.5f, 0.0f},
        Vector2f{0.75f, 0.0f},
        Vector2f{1.0f, 0.0f}
    },
    .strengths = {5.0f, 5.0f, 5.0f, 5.0f, 5.0f},
    .colorNumber = 5.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Six colors (dimension = 6)
const ColorGradientEffectTestParams SIXCOLORSPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.5f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.0f},
        Vector2f{0.2f, 0.0f},
        Vector2f{0.4f, 0.0f},
        Vector2f{0.6f, 0.0f},
        Vector2f{0.8f, 0.0f},
        Vector2f{1.0f, 0.0f}
    },
    .strengths = {5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f},
    .colorNumber = 6.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Eight colors (dimension = 8)
const ColorGradientEffectTestParams EIGHTCOLORSPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.5f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.5f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.5f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.0f},
        Vector2f{0.14f, 0.0f},
        Vector2f{0.28f, 0.0f},
        Vector2f{0.42f, 0.0f},
        Vector2f{0.57f, 0.0f},
        Vector2f{0.71f, 0.0f},
        Vector2f{0.85f, 0.0f},
        Vector2f{1.0f, 0.0f}
    },
    .strengths = {5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f},
    .colorNumber = 8.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Eleven colors (dimension = 11, max < 12)
const ColorGradientEffectTestParams ELEVENCOLORSPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{1.0f, 0.5f, 0.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.5f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.5f, 1.0f},
        Vector4f{0.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.5f, 1.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{0.5f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.0f},
        Vector2f{0.1f, 0.0f},
        Vector2f{0.2f, 0.0f},
        Vector2f{0.3f, 0.0f},
        Vector2f{0.4f, 0.0f},
        Vector2f{0.5f, 0.0f},
        Vector2f{0.6f, 0.0f},
        Vector2f{0.7f, 0.0f},
        Vector2f{0.8f, 0.0f},
        Vector2f{0.9f, 0.0f},
        Vector2f{1.0f, 0.0f}
    },
    .strengths = {5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f},
    .colorNumber = 11.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Invalid color values (dimension = 2, testing edge cases)
const ColorGradientEffectTestParams INVALIDCOLORPARAMS = {
    .colors = {
        Vector4f{-1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{2.0f, 0.0f, 0.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.5f},
        Vector2f{1.0f, 0.5f}
    },
    .strengths = {5.0f, 5.0f},
    .colorNumber = 2.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Extreme position values (dimension = 3)
const ColorGradientEffectTestParams EXTREMEPOSITIONPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{-1.0f, -1.0f},
        Vector2f{0.5f, 0.5f},
        Vector2f{2.0f, 2.0f}
    },
    .strengths = {5.0f, 5.0f, 5.0f},
    .colorNumber = 3.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Varying strengths (dimension = 4)
const ColorGradientEffectTestParams VARYINGSTRENGTHPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.0f},
        Vector2f{0.33f, 0.33f},
        Vector2f{0.67f, 0.67f},
        Vector2f{1.0f, 1.0f}
    },
    .strengths = {1.0f, 5.0f, 10.0f, 20.0f},
    .colorNumber = 4.0f,
    .blend = 2.0f,
    .blendK = 10.0f,
    .brightness = 0.0f
};

// Different blend modes (dimension = 3)
const ColorGradientEffectTestParams BLENDMODEPARAMS = {
    .colors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f}
    },
    .positions = {
        Vector2f{0.0f, 0.5f},
        Vector2f{0.5f, 0.5f},
        Vector2f{1.0f, 0.5f}
    },
    .strengths = {5.0f, 5.0f, 5.0f},
    .colorNumber = 3.0f,
    .blend = 1.0f,
    .blendK = 2.0f,
    .brightness = 0.0f
};

// Arrays of setter function pointers for colors
using ColorSetter = void(RSNGColorGradientEffect::*)(const Vector4f&);
constexpr ColorSetter COLORSETTERS[] = {
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor0Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor1Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor2Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor3Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor4Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor5Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor6Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor7Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor8Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor9Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor10Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectColor11Tag>
};

// Arrays of setter function pointers for positions
using PositionSetter = void(RSNGColorGradientEffect::*)(const Vector2f&);
constexpr PositionSetter POSITIONSETTERS[] = {
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition0Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition1Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition2Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition3Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition4Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition5Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition6Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition7Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition8Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition9Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition10Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectPosition11Tag>
};

// Arrays of setter function pointers for strengths
using StrengthSetter = void(RSNGColorGradientEffect::*)(const float&);
constexpr StrengthSetter STRENGTHSETTERS[] = {
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength0Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength1Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength2Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength3Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength4Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength5Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength6Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength7Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength8Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength9Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength10Tag>,
    &RSNGColorGradientEffect::Setter<ColorGradientEffectStrength11Tag>
};

// Arrays of setter function pointers - NO SWITCH CASE
void InitColorGradientEffect(std::shared_ptr<RSNGColorGradientEffect>& colorGradientEffect,
    const ColorGradientEffectTestParams& params)
{
    // Set colors - dimensions must be consistent with positions and strengths
    for (size_t i = 0; i < params.colors.size() && i < GRADIENT_EFFECT_MAX_SZIE; ++i) {
        (colorGradientEffect.get()->*(COLORSETTERS[i]))(params.colors[i]);
    }

    // Set positions - dimensions must match colors
    for (size_t i = 0; i < params.positions.size() && i < GRADIENT_EFFECT_MAX_SZIE; ++i) {
        (colorGradientEffect.get()->*(POSITIONSETTERS[i]))(params.positions[i]);
    }

    // Set strengths - dimensions must match colors and positions
    for (size_t i = 0; i < params.strengths.size() && i < GRADIENT_EFFECT_MAX_SZIE; ++i) {
        (colorGradientEffect.get()->*(STRENGTHSETTERS[i]))(params.strengths[i]);
    }

    // Set other parameters
    colorGradientEffect->Setter<ColorGradientEffectColorNumberTag>(params.colorNumber);
    colorGradientEffect->Setter<ColorGradientEffectBlendTag>(params.blend);
    colorGradientEffect->Setter<ColorGradientEffectBlendKTag>(params.blendK);
    colorGradientEffect->Setter<ColorGradientEffectBrightnessTag>(params.brightness);
}

}  // namespace OHOS::Rosen
#endif // NG_COLORGRADIENT_EFFECT_TEST_UTILS_H
