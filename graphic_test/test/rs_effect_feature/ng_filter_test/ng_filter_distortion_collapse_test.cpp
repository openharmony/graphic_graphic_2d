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

#include "ng_filter_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class NGFilterDistortionCollapseTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    const int filterParaTypeCount = static_cast<int>(FilterPara::ParaType::CONTENT_LIGHT);
};

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapse_Test_01)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{0.5f, 0.5f, 0.5f, 0.5f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapse_Test_02)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.1f, 0.1f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.1f, 0.1f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.1f, 0.1f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.1f, 0.1f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{0.2f, 0.2f, 0.2f, 0.2f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_03)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.3f, 0.3f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.3f, 0.3f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.3f, 0.3f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.3f, 0.3f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{0.8f, 0.8f, 0.8f, 0.8f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_04)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.4f, 0.4f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.4f, 0.4f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.4f, 0.4f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.4f, 0.4f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_05)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{0.0f, 0.0f, 0.0f, 0.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_06)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{-0.5f, -0.5f, -0.5f, -0.5f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_07)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.1f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.2f, 0.1f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.1f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.2f, 0.1f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{0.1f, 0.2f, 0.3f, 0.4f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_08)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.0f, 0.0f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.0f, 0.0f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.0f, 0.0f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.0f, 0.0f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{0.5f, 0.5f, 0.5f, 0.5f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_09)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.8f, 0.8f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.8f, 0.8f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.8f, 0.8f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.8f, 0.8f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{0.5f, 0.5f, 0.5f, 0.5f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGFilterDistortionCollapseTest, EFFECT_TEST, Set_NG_Filter_DistortionCollapseTest_10)
{
    float sizeX = screenWidth;
    float sizeY = screenWidth;
    auto filter = CreateFilter(RSNGEffectType::DISTORTION_COLLAPSE);
    auto distortionCollapseFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);

    distortionCollapseFilter->Setter<DistortionCollapseLUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRUCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseRBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseLBCornerTag>(Vector2f{0.2f, 0.2f});
    distortionCollapseFilter->Setter<DistortionCollapseBarrelDistortionTag>(Vector4f{-1.0f, 0.0f, 1.0f, 1.0f});

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
    backgroundTestNode->SetBackgroundNGFilter(distortionCollapseFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

}  // namespace OHOS::Rosen
