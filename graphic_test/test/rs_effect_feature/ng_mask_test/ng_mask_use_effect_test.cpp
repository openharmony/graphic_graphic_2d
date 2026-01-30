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
#include "ng_mask_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
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
    auto backgroundTestNode = SetUpNodeBgImage(g_backgroundImagePath, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
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