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
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
}  // namespace

class NGFilterBlurExtendTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Test Blur DisableSystemAdaptation
GRAPHIC_TEST(NGFilterBlurExtendTest, EFFECT_TEST, Set_NG_Filter_Blur_DisableSystemAdaptation_Test)
{
    auto blurPtr = CreateFilter(RSNGEffectType::BLUR);
    auto blurFilter = std::static_pointer_cast<RSNGBlurFilter>(blurPtr);
    ASSERT_NE(blurFilter, nullptr);

    // Set DisableSystemAdaptation tag
    blurFilter->Setter<BlurDisableSystemAdaptationTag>(true);

    int nodeWidth = 460;
    int nodeHeight = 1400;
    int startX = 120;
    int startY = 300;
    int gap = 40;

    for (int i = 0; i < 2; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {startX + (nodeWidth + gap) * i, startY, nodeWidth, nodeHeight});
        blurFilter->Setter<BlurDisableSystemAdaptationTag>(i == 0);
        backgroundNode->SetBackgroundNGFilter(blurFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen
