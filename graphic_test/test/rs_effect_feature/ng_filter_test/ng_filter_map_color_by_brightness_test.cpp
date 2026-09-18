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
#include "ui_effect/filter/include/filter_map_color_by_brightness_para.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";

const size_t screenWidth = 1200;
const size_t screenHeight = 2000;

constexpr size_t COLUMN_COUNT = 2;
constexpr size_t TOTAL_CELLS = 6;

struct MapColorByBrightnessParams {
    std::vector<Vector4f> colors;
    std::vector<float> positions;
};

const std::vector<MapColorByBrightnessParams> normalParams = {
    {{{0.96f, 0.7f, 0.74f, 1.0f}}, {0.5f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}}, {0.2f, 0.8f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f}},
     {0.1f, 0.5f, 0.9f}},
    {{{0.5f, 0.5f, 0.5f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.2f, 0.2f, 0.2f, 1.0f},
      {1.0f, 1.0f, 1.0f, 1.0f}},
     {0.0f, 0.33f, 0.66f, 1.0f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f},
      {0.87f, 0.86f, 0.87f, 1.0f}, {0.99f, 0.22f, 0.87f, 1.0f}},
     {0.1f, 0.3f, 0.5f, 0.7f, 0.9f}},
    {{{0.639f, 0.77f, 0.95f, 1.0f}, {0.97f, 0.72f, 0.23f, 1.0f}, {0.90f, 0.34f, 0.0f, 1.0f}},
     {0.1f, 0.5f, 0.9f}}
};

const std::vector<MapColorByBrightnessParams> boundaryPositionParams = {
    {{{0.96f, 0.7f, 0.74f, 1.0f}}, {0.0f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}}, {1.0f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}}, {0.0f, 1.0f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}}, {-0.5f, 1.5f}},
    {{{0.5f, 0.5f, 0.5f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.2f, 0.2f, 0.2f, 1.0f}},
     {0.49f, 0.5f, 0.51f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}}, {0.5f, 0.5f}}
};

const std::vector<MapColorByBrightnessParams> abnormalParams = {
    {{}, {}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f}},
     {0.2f, 0.8f}},
    {{{-0.5f, -0.5f, -0.5f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}}, {0.2f, 0.8f}},
    {{{2.0f, 2.0f, 2.0f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}}, {0.2f, 0.8f}},
    {{{0.0f, 0.0f, 0.0f, 0.0f}}, {0.5f}},
    {{{1.0f, 1.0f, 1.0f, 1.0f}}, {0.5f}}
};
}

class NGFilterMapColorByBrightnessTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void SetBgAndChildNodes(const size_t i, const size_t columnCount, const size_t sizeX,
        const size_t sizeY, std::shared_ptr<RSNGMapColorByBrightnessFilter>& filter)
    {
        const size_t x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        const size_t y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        auto backgroundTestNode = SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(filter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
};

GRAPHIC_TEST(NGFilterMapColorByBrightnessTest, EFFECT_TEST, Set_NormalColorsTest)
{
    auto sizeX = screenWidth / COLUMN_COUNT;
    auto sizeY = screenHeight * COLUMN_COUNT / TOTAL_CELLS;
    for (size_t i = 0; i < TOTAL_CELLS; i++) {
        auto filter = std::make_shared<RSNGMapColorByBrightnessFilter>();
        filter->Setter<MapColorByBrightnessColorsTag>(normalParams[i].colors);
        filter->Setter<MapColorByBrightnessPositionsTag>(normalParams[i].positions);
        SetBgAndChildNodes(i, COLUMN_COUNT, sizeX, sizeY, filter);
    }
}

GRAPHIC_TEST(NGFilterMapColorByBrightnessTest, EFFECT_TEST, Set_BoundaryPositionsTest)
{
    auto sizeX = screenWidth / COLUMN_COUNT;
    auto sizeY = screenHeight * COLUMN_COUNT / TOTAL_CELLS;
    for (size_t i = 0; i < TOTAL_CELLS; i++) {
        auto filter = std::make_shared<RSNGMapColorByBrightnessFilter>();
        filter->Setter<MapColorByBrightnessColorsTag>(boundaryPositionParams[i].colors);
        filter->Setter<MapColorByBrightnessPositionsTag>(boundaryPositionParams[i].positions);
        SetBgAndChildNodes(i, COLUMN_COUNT, sizeX, sizeY, filter);
    }
}

GRAPHIC_TEST(NGFilterMapColorByBrightnessTest, EFFECT_TEST, Set_AbnormalParamsTest)
{
    auto sizeX = screenWidth / COLUMN_COUNT;
    auto sizeY = screenHeight * COLUMN_COUNT / TOTAL_CELLS;
    for (size_t i = 0; i < TOTAL_CELLS; i++) {
        auto filter = std::make_shared<RSNGMapColorByBrightnessFilter>();
        filter->Setter<MapColorByBrightnessColorsTag>(abnormalParams[i].colors);
        filter->Setter<MapColorByBrightnessPositionsTag>(abnormalParams[i].positions);
        SetBgAndChildNodes(i, COLUMN_COUNT, sizeX, sizeY, filter);
    }
}
}  // namespace OHOS::Rosen
