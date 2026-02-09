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
#include "ui_effect/filter/include/filter_edge_light_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;

const std::vector<float> alphaParams = {
    -1.0f, 0.0f, 0.3f, 0.7f, 1.0f, 2.0f
};

const std::vector<Vector4f> colorParams = {
    Vector4f{-0.5f, 0.0f, 0.0f, 1.0f}, Vector4f{0.0f, 0.0f, 0.0f, 1.0f},
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f}, Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f}, Vector4f{1.5f, 1.0f, 1.0f, 1.0f}
};

const std::vector<bool> bloomParams = {true, false, true, false, true, false};

const std::vector<bool> useRawColorParams = {true, false, false, true, true, false};
}

class NGFilterEdgeLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void SetBgAndChildNodes(const size_t i, const size_t columnCount, const size_t sizeX,
        const size_t sizeY, std::shared_ptr<RSNGEdgeLightFilter>& edgeLightFilter)
    {
        const size_t x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        const size_t y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        auto backgroundTestNode =
            SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(edgeLightFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
};

GRAPHIC_TEST(NGFilterEdgeLightTest, EFFECT_TEST, Set_EdgeLight_Alpha_Color_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = alphaParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (size_t i = 0; i < rowCount; i++) {
        auto edgeLightFilter = std::make_shared<RSNGEdgeLightFilter>();
        edgeLightFilter->Setter<EdgeLightAlphaTag>(alphaParams[i]);
        edgeLightFilter->Setter<EdgeLightColorTag>(colorParams[i]);
        edgeLightFilter->Setter<EdgeLightBloomTag>(true);
        edgeLightFilter->Setter<EdgeLightUseRawColorTag>(false);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, edgeLightFilter);
    }
}

GRAPHIC_TEST(NGFilterEdgeLightTest, EFFECT_TEST, Set_EdgeLight_Bloom_UseRawColor_Mask_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = bloomParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (size_t i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::DOUBLE_RIPPLE_MASK);
        auto doubleRippleMask = std::static_pointer_cast<RSNGDoubleRippleMask>(mask);
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(
            Vector2f{doubleRippleMaskParams[i][0], doubleRippleMaskParams[i][1]});
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(
            Vector2f{doubleRippleMaskParams[i][2], doubleRippleMaskParams[i][3]});
        doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(doubleRippleMaskParams[i][4]);
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(doubleRippleMaskParams[i][5]);
        doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(doubleRippleMaskParams[i][6]);

        auto edgeLightFilter = std::make_shared<RSNGEdgeLightFilter>();
        edgeLightFilter->Setter<EdgeLightAlphaTag>(0.8f);
        edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        edgeLightFilter->Setter<EdgeLightBloomTag>(static_cast<bool>(bloomParams[i]));
        edgeLightFilter->Setter<EdgeLightUseRawColorTag>(static_cast<bool>(useRawColorParams[i]));
        edgeLightFilter->Setter<EdgeLightMaskTag>(mask);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, edgeLightFilter);
    }
}
}  // namespace OHOS::Rosen
