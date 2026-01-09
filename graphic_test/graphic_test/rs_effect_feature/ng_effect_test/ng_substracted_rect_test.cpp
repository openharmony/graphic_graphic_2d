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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
using MaskCreator = std::function<std::shared_ptr<RSNGMaskBase>()>;
using MaskConvertor = std::function<std::shared_ptr<RSNGMaskBase>(std::shared_ptr<MaskPara>)>;
const std::string g_foregroundImagePath = "/data/local/tmp/fg_test.jpg";

static std::unordered_map<RSNGEffectType, MaskCreator> creatorMask = {
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

struct ColorGradientEffectParams {
    Vector4f color0;
    Vector4f color1;
    Vector4f color2;
    Vector4f color3;
    Vector2f position0;
    Vector2f position1;
    Vector2f position2;
    Vector2f position3;
    float strength0;
    float strength1;
    float strength2;
    float strength3;
    float colorNumber;
    float blend;
    float blendK;
};

struct NoisyFrameGradientMaskParams {
    Vector4f gradientBezierControlPoints;
    float cornerRadius;
    Vector2f innerFrameWidth;
    Vector2f middleFrameWidth;
    Vector2f outsideFrameWidth;
    Vector2f rrectWH;
    Vector2f rrectPos;
    float slope;
    float progress;
};

std::vector<ColorGradientEffectParams> colorGradientEffectParams = {
    {
        .color0 = {0.0f, 0.0f, 1.0f, 1.0f},
        .color1 = {0.0f, 0.0f, 1.0f, 1.0f},
        .color2 = {1.0f, 0.0f, 1.0f, 1.0f},
        .color3 = {1.0f, 0.0f, 1.0f, 1.0f},
        .position0 = {0.0f, 1.0f},
        .position1 = {1.0f, 0.0f},
        .position2 = {0.0f, 0.0f},
        .position3 = {1.0f, 1.0f},
        .strength0 = 5.4f,
        .strength1 = 5.4f,
        .strength2 = 5.4f,
        .strength3 = 5.4f,
        .colorNumber = 4.0f,
        .blend = 6.0f,
        .blendK = 20.0f
    }
};

std::vector<NoisyFrameGradientMaskParams> noisyFrameGradientMaskParams = {
    // test large corner radius
    {
        .gradientBezierControlPoints = Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        .cornerRadius = 1.0f,
        .innerFrameWidth = Vector2f{0.0f, 0.01f},
        .middleFrameWidth = Vector2f{0.0f, 0.01f},
        .outsideFrameWidth = Vector2f{0.0f, 0.01f},
        .rrectWH = Vector2f{0.5f, 0.5f},
        .rrectPos = Vector2f{0.0f, 0.0f},
        .slope = 0.0f,
        .progress = 0.0f
    },
    // test width bigger than height
    {
        .gradientBezierControlPoints = Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        .cornerRadius = 0.1f,
        .innerFrameWidth = Vector2f{0.0f, 0.01f},
        .middleFrameWidth = Vector2f{0.0f, 0.01f},
        .outsideFrameWidth = Vector2f{0.0f, 0.01f},
        .rrectWH = Vector2f{1.0f, 0.3f},
        .rrectPos = Vector2f{0.0f, 0.0f},
        .slope = 0.0f,
        .progress = 0.0f
    },
    
    // test height bigger than width
    {
        .gradientBezierControlPoints = Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        .cornerRadius = 0.1f,
        .innerFrameWidth = Vector2f{0.0f, 0.01f},
        .middleFrameWidth = Vector2f{0.0f, 0.01f},
        .outsideFrameWidth = Vector2f{0.0f, 0.01f},
        .rrectWH = Vector2f{0.3f, 1.0f},
        .rrectPos = Vector2f{0.0f, 0.0f},
        .slope = 0.0f,
        .progress = 0.0f
    },
    
    // test large frame width
    {
        .gradientBezierControlPoints = Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        .cornerRadius = 0.01f,
        .innerFrameWidth = Vector2f{0.0f, 1.0f},
        .middleFrameWidth = Vector2f{0.0f, 1.0f},
        .outsideFrameWidth = Vector2f{0.0f, 1.0f},
        .rrectWH = Vector2f{0.5f, 0.5f},
        .rrectPos = Vector2f{0.0f, 0.0f},
        .slope = 0.0f,
        .progress = 0.0f
    },
    
    // test rect not included in canvas
    {
        .gradientBezierControlPoints = Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        .cornerRadius = 0.1f,
        .innerFrameWidth = Vector2f{0.0f, 0.1f},
        .middleFrameWidth = Vector2f{0.0f, 0.1f},
        .outsideFrameWidth = Vector2f{0.0f, 0.1f},
        .rrectWH = Vector2f{1.0f, 1.0f},
        .rrectPos = Vector2f{1.1f, 1.0f},
        .slope = 0.0f,
        .progress = 0.0f
    },
    
    // test no corner radius
    {
        .gradientBezierControlPoints = Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        .cornerRadius = 0.0f,
        .innerFrameWidth = Vector2f{0.0f, 0.1f},
        .middleFrameWidth = Vector2f{0.0f, 0.1f},
        .outsideFrameWidth = Vector2f{0.0f, 0.1f},
        .rrectWH = Vector2f{1.0f, 1.0f},
        .rrectPos = Vector2f{0.0f, 0.0f},
        .slope = 0.0f,
        .progress = 0.0f
    }
};
}

class NGSubstractedRectTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

void SetColorGradientEffectParams(const std::shared_ptr<RSNGColorGradientEffect>& colorGradient, int index)
{
    if (!colorGradient) {
        return;
    }
    const auto& params = colorGradientEffectParams[index];
    colorGradient->Setter<ColorGradientEffectColor0Tag>(params.color0);
    colorGradient->Setter<ColorGradientEffectColor1Tag>(params.color1);
    colorGradient->Setter<ColorGradientEffectColor2Tag>(params.color2);
    colorGradient->Setter<ColorGradientEffectColor3Tag>(params.color3);
    colorGradient->Setter<ColorGradientEffectPosition0Tag>(params.position0);
    colorGradient->Setter<ColorGradientEffectPosition1Tag>(params.position1);
    colorGradient->Setter<ColorGradientEffectPosition2Tag>(params.position2);
    colorGradient->Setter<ColorGradientEffectPosition3Tag>(params.position3);
    colorGradient->Setter<ColorGradientEffectStrength0Tag>(params.strength0);
    colorGradient->Setter<ColorGradientEffectStrength1Tag>(params.strength1);
    colorGradient->Setter<ColorGradientEffectStrength2Tag>(params.strength2);
    colorGradient->Setter<ColorGradientEffectStrength3Tag>(params.strength3);
    colorGradient->Setter<ColorGradientEffectColorNumberTag>(params.colorNumber);
    colorGradient->Setter<ColorGradientEffectBlendTag>(params.blend);
    colorGradient->Setter<ColorGradientEffectBlendKTag>(params.blendK);
}


void SetNoisyFrameGradientMaskParams(const std::shared_ptr<RSNGNoisyFrameGradientMask>& mask, int index, float height)
{
    if (!mask) {
        return;
    }
    const auto params = noisyFrameGradientMaskParams[index];
    
    mask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(params.gradientBezierControlPoints);
    mask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(params.cornerRadius * height / 2.0f);
    Vector2f adjustedInnerFramerWidth = params.innerFrameWidth;
    adjustedInnerFramerWidth.x_ *= height / 2.0f;
    adjustedInnerFramerWidth.y_ *= height / 2.0f;
    mask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(adjustedInnerFramerWidth);
    Vector2f adjustedMiddleFrameWidth = params.middleFrameWidth;
    adjustedMiddleFrameWidth.x_ *= height / 2.0f;
    adjustedMiddleFrameWidth.y_ *= height / 2.0f;
    mask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(adjustedMiddleFrameWidth);
    Vector2f adjustedOutsideFramerWidth = params.outsideFrameWidth;
    adjustedOutsideFramerWidth.x_ *= height / 2.0f;
    adjustedOutsideFramerWidth.y_ *= height / 2.0f;
    mask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(adjustedOutsideFramerWidth);
    mask->Setter<NoisyFrameGradientMaskRRectWHTag>(params.rrectWH);
    mask->Setter<NoisyFrameGradientMaskRRectPosTag>(params.rrectPos);
    mask->Setter<NoisyFrameGradientMaskSlopeTag>(params.slope);
    mask->Setter<NoisyFrameGradientMaskProgressTag>(params.progress);
}

// Test Foreground Noisy Frame Gradient Mask and Color Gradient Effect
GRAPHIC_TEST(NGSubstractedRectTest, EFFECT_TEST, Set_Substracted_Rect_NG_Mask_Noisy_Frame_Gradient_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = 6;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        // Create Noisy Frame Gradient Mask
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameGradientMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);
        SetNoisyFrameGradientMaskParams(noisyFrameGradientMask, i, sizeY);

        // Create Color Gradient Effect
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        SetColorGradientEffectParams(colorGradient, 0);
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

}  // namespace OHOS::Rosen
