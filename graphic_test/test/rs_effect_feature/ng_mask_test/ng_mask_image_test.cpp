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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const std::string DISTOR_IMAGE_PATH = "/data/local/tmp/Images/perlin_worley_noise_3d_64.png";
const std::string CLOCK_IMAGE_PATH = "/data/local/tmp/Images/clockEffect.png";

void InitHarmoniumEffect(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect)
{
    auto useEffectMask = std::make_shared<RSNGUseEffectMask>();
    useEffectMask->Setter<UseEffectMaskUseEffectTag>(true);
    harmoniumEffect->Setter<HarmoniumEffectUseEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(useEffectMask));
    harmoniumEffect->Setter<HarmoniumEffectRippleProgressTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectTintColorTag>(Vector4f{1.f, 1.f, 1.0f, 1.0f});
    harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectDistortFactorTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectReflectionFactorTag>(1.f);
    harmoniumEffect->Setter<HarmoniumEffectRefractionFactorTag>(0.4f);
    harmoniumEffect->Setter<HarmoniumEffectMaterialFactorTag>(1.f);
    harmoniumEffect->Setter<HarmoniumEffectRateTag>(1.f);
    harmoniumEffect->Setter<HarmoniumEffectLightUpDegreeTag>(2.f);
    harmoniumEffect->Setter<HarmoniumEffectCubicCoeffTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectQuadCoeffTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectSaturationTag>(1.5f);
    harmoniumEffect->Setter<HarmoniumEffectPosRGBTag>(Vector3f{0.4f, 1.f, 0.2f});
    harmoniumEffect->Setter<HarmoniumEffectNegRGBTag>(Vector3f{1.f, 1.f, 0.5f});
    harmoniumEffect->Setter<HarmoniumEffectFractionTag>(1.f);
}

std::shared_ptr<RSCanvasNode> CreateEffectChildNode(const int i, const int columnCount,
    const int rowCount, std::shared_ptr<RSEffectNode>& effectNode,
    std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect)
{
    auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
    auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;

    int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
    int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

    auto effectChildNode = RSCanvasNode::Create();
    effectChildNode->SetBounds(x, y, sizeX, sizeY);
    effectChildNode->SetFrame(x, y, sizeX, sizeY);
    effectChildNode->SetCornerRadius(0.f);
    effectChildNode->SetBackgroundNGShader(harmoniumEffect);
    effectNode->AddChild(effectChildNode);
    return effectChildNode;
}

class NGMaskImageTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

GRAPHIC_TEST(NGMaskImageTest, EFFECT_TEST, Set_NG_Mask_Image_Test)
{
    auto sizeX = SCREEN_WIDTH;
    auto sizeY = SCREEN_HEIGHT;

    auto parentNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { 0, 0, sizeX - 10, sizeY - 10 });
    parentNode->SetFrame({ 0, 0, sizeX - 10, sizeY - 10 });
    parentNode->SetBorderStyle(0, 0, 0, 0);
    parentNode->SetBorderWidth(5, 5, 5, 5);
    parentNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
    GetRootNode()->AddChild(parentNode);
    RegisterNode(parentNode);
    
    auto effectNode = RSEffectNode::Create();
    effectNode->SetBounds({ 0, 0, sizeX - 10, sizeY - 10 });
    effectNode->SetFrame({ 0, 0, sizeX - 10, sizeY - 10 });
    effectNode->SetBackgroundColor(0xffff0000);
    effectNode->SetBackgroundFilter(RSFilter::CreateBlurFilter(25, 25));
    parentNode->AddChild(effectNode);
    RegisterNode(effectNode);

    auto childNode = RSCanvasNode::Create();
    childNode->SetFrame({ (sizeX - 763*1.5) / 2, (sizeY - 243*1.5) / 2, 763*1.5, 243*1.5 });
    childNode->SetUseEffect(false);
    childNode->SetBorderStyle(0, 0, 0, 0);
    childNode->SetBorderWidth(5, 5, 5, 5);
    childNode->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
    effectNode->AddChild(childNode);
    RegisterNode(childNode);

    std::shared_ptr<Media::PixelMap> pixelMap =
        DecodePixelMap(DISTOR_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
    std::shared_ptr<RSNGImageMask> pixelMapMask = std::make_shared<RSNGImageMask>();
    pixelMapMask->Setter<ImageMaskImageTag>(pixelMap);

    std::shared_ptr<Media::PixelMap> pixelMapClock =
        DecodePixelMap(CLOCK_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
    std::shared_ptr<RSNGImageMask> pixelMapMaskClock = std::make_shared<RSNGImageMask>();
    pixelMapMaskClock->Setter<ImageMaskImageTag>(pixelMapClock);

    std::shared_ptr<RSNGHarmoniumEffect> harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
    InitHarmoniumEffect(harmoniumEffect);
    CreateEffectChildNode(0, 1, 1, effectNode, harmoniumEffect);
    harmoniumEffect->Setter<HarmoniumEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(pixelMapMask));
    harmoniumEffect->Setter<HarmoniumEffectMaskClockTag>(std::static_pointer_cast<RSNGMaskBase>(pixelMapMaskClock));
    harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(1.0f);
    childNode->SetBackgroundNGShader(harmoniumEffect);
}
}