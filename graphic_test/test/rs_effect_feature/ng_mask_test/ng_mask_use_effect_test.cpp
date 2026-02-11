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

// Constants for Harmonium effect
const std::vector<bool> useEffectVec = {true, false, false, true};
const std::vector<float> cornerRadiusVec = {-100.f, 100.f, 1000.f, 10000.f};

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
    effectChildNode->SetCornerRadius(cornerRadiusVec[i]);
    effectChildNode->SetBackgroundNGShader(harmoniumEffect);
    effectNode->AddChild(effectChildNode);
    return effectChildNode;
}

class NGMaskUseEffectTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

void SetUseEffectMask(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, bool useEffect)
{
    auto useEffectMask = std::make_shared<RSNGUseEffectMask>();
    useEffectMask->Setter<UseEffectMaskUseEffectTag>(useEffect);
    harmoniumEffect->Setter<HarmoniumEffectUseEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(useEffectMask));
}

GRAPHIC_TEST(NGMaskUseEffectTest, EFFECT_TEST, Set_NG_Mask_USE_EFFECT_Test)
{
    auto backgroundTestNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    auto effectNode = RSEffectNode::Create();
    effectNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    effectNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    std::shared_ptr<Rosen::RSFilter> backFilter = Rosen::RSFilter::CreateMaterialFilter(20., 1, 1, 0,
        BLUR_COLOR_MODE::DEFAULT, true);
    effectNode->SetBackgroundFilter(backFilter);
    effectNode->SetClipToBounds(true);
    GetRootNode()->AddChild(backgroundTestNode);
    backgroundTestNode->AddChild(effectNode);
    RegisterNode(effectNode);
    RegisterNode(backgroundTestNode);

    const int columnCount = 2;
    const int rowCount = 4;
    for (int i = 0; i < rowCount; i++) {
        auto harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
        InitHarmoniumEffect(harmoniumEffect);
        SetUseEffectMask(harmoniumEffect, useEffectVec[i]);
        auto effectChildNode = CreateEffectChildNode(i, columnCount, rowCount, effectNode, harmoniumEffect);
        RegisterNode(effectChildNode);
    }
}

}