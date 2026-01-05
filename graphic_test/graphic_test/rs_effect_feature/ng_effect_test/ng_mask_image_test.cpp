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
#include "ng_harmonium_effect_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class NGMaskImageTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(NGMaskImageTest, EFFECT_TEST, Set_NG_Mask_Image_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;

    auto parentNode = SetUpNodeBgImage(g_backgroundImagePath, { 0, 0, sizeX - 10, sizeY - 10 });
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
        DecodePixelMap(g_distorImagePath, Media::AllocatorType::SHARE_MEM_ALLOC);
    std::shared_ptr<Rosen::RSNGImageMask> pixelMapMask = std::make_shared<Rosen::RSNGImageMask>();
    pixelMapMask->Setter<Rosen::ImageMaskImageTag>(pixelMap);

    std::shared_ptr<Media::PixelMap> pixelMapClock =
        DecodePixelMap(g_clockImagePath, Media::AllocatorType::SHARE_MEM_ALLOC);
    std::shared_ptr<Rosen::RSNGImageMask> pixelMapMaskClock = std::make_shared<Rosen::RSNGImageMask>();
    pixelMapMaskClock->Setter<Rosen::ImageMaskImageTag>(pixelMapClock);

    std::shared_ptr<RSNGHarmoniumEffect> harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
    InitHarmoniumEffect(harmoniumEffect);
    CreateEffectChildNode(0, 1, 1, effectNode, harmoniumEffect);
    harmoniumEffect->Setter<Rosen::HarmoniumEffectMaskTag>(std::static_pointer_cast<Rosen::RSNGMaskBase>(pixelMapMask));
    harmoniumEffect->Setter<Rosen::HarmoniumEffectMaskClockTag>(std::static_pointer_cast<Rosen::RSNGMaskBase>(pixelMapMaskClock));
    harmoniumEffect->Setter<Rosen::HarmoniumEffectDistortProgressTag>(1.0f);
    childNode->SetBackgroundNGShader(harmoniumEffect);
}
}