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
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Binocular mask parameter variations
const std::vector<float> radiusXs = {0.1f, 0.28f, 0.5f};
const std::vector<float> radiusYs = {0.2f, 0.48f, 0.8f};
const std::vector<float> gaps = {0.3f, 0.52f, 0.7f};
const std::vector<float> softnesses = {0.05f, 0.20f, 0.4f};
}

class NGMaskBinocularTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const std::shared_ptr<RSNGBinocularMask>& binocularMask)
    {
        const size_t sizeX = SCREEN_WIDTH;
        const size_t sizeY = SCREEN_HEIGHT;

        auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        parentNode->SetFrame({0, 0, sizeX, sizeY});
        GetRootNode()->AddChild(parentNode);
        RegisterNode(parentNode);

        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        effectNode->SetBounds({0, 0, sizeX, sizeY});
        effectNode->SetFrame({0, 0, sizeX, sizeY});
        effectNode->SetBackgroundColor(0xffff0000);
        parentNode->AddChild(effectNode);
        RegisterNode(effectNode);

        auto harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
        harmoniumEffect->Setter<HarmoniumEffectMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(binocularMask));
        harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(1.0f);

        auto effectChildNode =
            RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        effectChildNode->SetBounds({0, 0, sizeX, sizeY});
        effectChildNode->SetFrame({0, 0, sizeX, sizeY});
        effectChildNode->SetBackgroundNGShader(harmoniumEffect);
        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }
};

/*
 * Test binocular mask with default parameters
 * Default: RX=0.28, RY=0.48, GAP=0.52, SOFT=0.20
 */
GRAPHIC_TEST(NGMaskBinocularTest, EFFECT_TEST, Set_Binocular_Mask_Default_Test)
{
    auto binocularMask = std::make_shared<RSNGBinocularMask>();
    SetUpTestNode(binocularMask);
}

/*
 * Test binocular mask with different radiusX values
 */
GRAPHIC_TEST(NGMaskBinocularTest, EFFECT_TEST, Set_Binocular_Mask_RadiusX_Test)
{
    for (size_t i = 0; i < radiusXs.size(); i++) {
        auto binocularMask = std::make_shared<RSNGBinocularMask>();
        binocularMask->Setter<BinocularMaskRadiusXTag>(radiusXs[i]);
        SetUpTestNode(binocularMask);
    }
}

/*
 * Test binocular mask with different radiusY values
 */
GRAPHIC_TEST(NGMaskBinocularTest, EFFECT_TEST, Set_Binocular_Mask_RadiusY_Test)
{
    for (size_t i = 0; i < radiusYs.size(); i++) {
        auto binocularMask = std::make_shared<RSNGBinocularMask>();
        binocularMask->Setter<BinocularMaskRadiusYTag>(radiusYs[i]);
        SetUpTestNode(binocularMask);
    }
}

/*
 * Test binocular mask with different gap values
 */
GRAPHIC_TEST(NGMaskBinocularTest, EFFECT_TEST, Set_Binocular_Mask_Gap_Test)
{
    for (size_t i = 0; i < gaps.size(); i++) {
        auto binocularMask = std::make_shared<RSNGBinocularMask>();
        binocularMask->Setter<BinocularMaskGapTag>(gaps[i]);
        SetUpTestNode(binocularMask);
    }
}

/*
 * Test binocular mask with different softness values
 */
GRAPHIC_TEST(NGMaskBinocularTest, EFFECT_TEST, Set_Binocular_Mask_Softness_Test)
{
    for (size_t i = 0; i < softnesses.size(); i++) {
        auto binocularMask = std::make_shared<RSNGBinocularMask>();
        binocularMask->Setter<BinocularMaskSoftnessTag>(softnesses[i]);
        SetUpTestNode(binocularMask);
    }
}

/*
 * Test binocular mask creation with multiple instances
 * Verifies that multiple binocular masks can coexist without conflict
 */
GRAPHIC_TEST(NGMaskBinocularTest, EFFECT_TEST, Set_Binocular_Mask_Multiple_Instances_Test)
{
    constexpr int instanceCount = 3;
    for (int i = 0; i < instanceCount; i++) {
        auto binocularMask = std::make_shared<RSNGBinocularMask>();
        binocularMask->Setter<BinocularMaskRadiusXTag>(radiusXs[i % radiusXs.size()]);
        binocularMask->Setter<BinocularMaskRadiusYTag>(radiusYs[i % radiusYs.size()]);
        SetUpTestNode(binocularMask);
    }
}

} // namespace OHOS::Rosen