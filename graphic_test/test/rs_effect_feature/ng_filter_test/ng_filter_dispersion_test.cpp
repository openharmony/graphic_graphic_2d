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
#include "ui_effect/filter/include/filter_dispersion_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;

const std::vector<float> opacityParams = {
    -1.0f, 0.0f, 0.5f, 1.0f, 0.8f, 2.0f
};

const std::vector<Vector2f> offsetParams = {
    Vector2f{-2.0f, -2.0f}, Vector2f{-1.0f, -1.0f}, Vector2f{0.7f, 0.7f},
    Vector2f{1.0f, 1.0f}, Vector2f{0.5f, 0.5f}, Vector2f{2.0f, 2.0f}
};
}

class NGFilterDispersionTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void SetBgAndChildNodes(const size_t i, const size_t columnCount, const size_t sizeX,
        const size_t sizeY, std::shared_ptr<RSNGDispersionFilter>& dispersionFilter)
    {
        const size_t x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        const size_t y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        auto backgroundTestNode =
            SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(dispersionFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
};

GRAPHIC_TEST(NGFilterDispersionTest, EFFECT_TEST, Set_Dispersion_Offset_With_Mask_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = offsetParams.size();
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

        auto dispersionFilter = std::make_shared<RSNGDispersionFilter>();
        dispersionFilter->Setter<DispersionOpacityTag>(0.8f);
        dispersionFilter->Setter<DispersionRedOffsetTag>(offsetParams[i]);
        dispersionFilter->Setter<DispersionGreenOffsetTag>(offsetParams[i]);
        dispersionFilter->Setter<DispersionBlueOffsetTag>(offsetParams[i]);
        dispersionFilter->Setter<DispersionMaskTag>(mask);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, dispersionFilter);
    }
}
}  // namespace OHOS::Rosen
