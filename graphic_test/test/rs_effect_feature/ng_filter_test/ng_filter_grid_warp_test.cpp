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
constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;
}

class NGFilterGridWarpTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void SetBgAndChildNodes(const size_t i, const size_t columnCount, const size_t sizeX,
        const size_t sizeY, std::shared_ptr<RSNGGridWarpFilter>& gridWarpFilter)
    {
        const size_t x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        const size_t y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        auto backgroundTestNode =
            SetUpNodeBgImage(FG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(gridWarpFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
};

GRAPHIC_TEST(NGFilterGridWarpTest, EFFECT_TEST, Set_GridWarp_GridPoint_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = gridWarpPointParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (size_t i = 0; i < rowCount; i++) {
        auto gridWarpFilter = std::make_shared<RSNGGridWarpFilter>();
        gridWarpFilter->Setter<GridWarpGridPoint0Tag>(gridWarpPointParams[i][0]);
        gridWarpFilter->Setter<GridWarpGridPoint1Tag>(gridWarpPointParams[i][1]);
        gridWarpFilter->Setter<GridWarpGridPoint2Tag>(gridWarpPointParams[i][2]);
        gridWarpFilter->Setter<GridWarpGridPoint3Tag>(gridWarpPointParams[i][3]);
        gridWarpFilter->Setter<GridWarpGridPoint4Tag>(gridWarpPointParams[i][4]);
        gridWarpFilter->Setter<GridWarpGridPoint5Tag>(gridWarpPointParams[i][5]);
        gridWarpFilter->Setter<GridWarpGridPoint6Tag>(gridWarpPointParams[i][6]);
        gridWarpFilter->Setter<GridWarpGridPoint7Tag>(gridWarpPointParams[i][7]);
        gridWarpFilter->Setter<GridWarpGridPoint8Tag>(gridWarpPointParams[i][8]);
        gridWarpFilter->Setter<GridWarpRotationAngle0Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle1Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle2Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle3Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle4Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle5Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle6Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle7Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle8Tag>(Vector2f{0.0f, 0.0f});
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, gridWarpFilter);
    }
}

GRAPHIC_TEST(NGFilterGridWarpTest, EFFECT_TEST, Set_GridWarp_RotationAngle_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = gridWarpAngleParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (size_t i = 0; i < rowCount; i++) {
        auto gridWarpFilter = std::make_shared<RSNGGridWarpFilter>();
        gridWarpFilter->Setter<GridWarpGridPoint0Tag>(Vector2f{0.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpGridPoint1Tag>(Vector2f{0.5f, 0.0f});
        gridWarpFilter->Setter<GridWarpGridPoint2Tag>(Vector2f{1.0f, 0.0f});
        gridWarpFilter->Setter<GridWarpGridPoint3Tag>(Vector2f{0.0f, 0.5f});
        gridWarpFilter->Setter<GridWarpGridPoint4Tag>(Vector2f{0.5f, 0.5f});
        gridWarpFilter->Setter<GridWarpGridPoint5Tag>(Vector2f{1.0f, 0.5f});
        gridWarpFilter->Setter<GridWarpGridPoint6Tag>(Vector2f{0.0f, 1.0f});
        gridWarpFilter->Setter<GridWarpGridPoint7Tag>(Vector2f{0.5f, 1.0f});
        gridWarpFilter->Setter<GridWarpGridPoint8Tag>(Vector2f{1.0f, 1.0f});
        gridWarpFilter->Setter<GridWarpRotationAngle0Tag>(gridWarpAngleParams[i][0]);
        gridWarpFilter->Setter<GridWarpRotationAngle1Tag>(gridWarpAngleParams[i][1]);
        gridWarpFilter->Setter<GridWarpRotationAngle2Tag>(gridWarpAngleParams[i][2]);
        gridWarpFilter->Setter<GridWarpRotationAngle3Tag>(gridWarpAngleParams[i][3]);
        gridWarpFilter->Setter<GridWarpRotationAngle4Tag>(gridWarpAngleParams[i][4]);
        gridWarpFilter->Setter<GridWarpRotationAngle5Tag>(gridWarpAngleParams[i][5]);
        gridWarpFilter->Setter<GridWarpRotationAngle6Tag>(gridWarpAngleParams[i][6]);
        gridWarpFilter->Setter<GridWarpRotationAngle7Tag>(gridWarpAngleParams[i][7]);
        gridWarpFilter->Setter<GridWarpRotationAngle8Tag>(gridWarpAngleParams[i][8]);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, gridWarpFilter);
    }
}
}  // namespace OHOS::Rosen
