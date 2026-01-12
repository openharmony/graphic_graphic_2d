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
#include "ui_effect/filter/include/filter_content_light_para.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";
const std::string BG_TEST_JPG_PATH = "/data/local/tmp/bg_test.jpg";

const size_t screenWidth = 1200;
const size_t screenHeight = 2000;

// (x, y, z): [-10, 10]
const std::vector<Vector3f> positionParams = {
    Vector3f{0.1f, 0.2f, 1.0f},
    Vector3f{-0.1f, -0.2f, 2.0f},
    Vector3f{100.0f, 100.0f, 100.0f}, // invalid
    Vector3f{-100.0f, -100.0f, -100.0f} // invalid
};

const std::vector<Vector4f> colorParams = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
    Vector4f{1.0f, 1.0f, 1.0f, 0.0f}
};

// intensity: [0.0, 10.0], out of range internal truncation, -100.0, 100.0 invalid
const std::vector<float> intensityParams = {-100.0f, 0.0f, 1.0f, 100.0f};

// default value
const Vector3f DEFAULT_CONTENT_LIGHT_POSITION = Vector3f(0.0f, 0.0f, 2.0f);
const Vector4f DEFAULT_CONTENT_LIGHT_COLOR = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
const float DEFAULT_CONTENT_LIGHT_INTENSITY = 10.0f;
}

class NGFilterContentLightTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void SetBgAndChildNodes(const size_t i, const size_t columnCount, const size_t sizeX,
        const size_t sizeY, std::shared_ptr<RSNGContentLightFilter>& contentLightFilter)
    {
        const size_t x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        const size_t y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        // set background node
        auto backgroundTestNode =
            SetUpNodeBgImage(BG_TEST_JPG_PATH, {x, y, sizeX, sizeY});
        backgroundTestNode->SetBackgroundNGFilter(contentLightFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
};

static void InitContentLightFilter(std::shared_ptr<RSNGContentLightFilter>& contentLightFilter)
{
    contentLightFilter->Setter<ContentLightPositionTag>(DEFAULT_CONTENT_LIGHT_POSITION);
    contentLightFilter->Setter<ContentLightColorTag>(DEFAULT_CONTENT_LIGHT_COLOR);
    contentLightFilter->Setter<ContentLightIntensityTag>(DEFAULT_CONTENT_LIGHT_INTENSITY);
}

GRAPHIC_TEST(NGFilterContentLightTest, EFFECT_TEST, Set_DefaultTest)
{
    auto contentLightFilter = std::make_shared<RSNGContentLightFilter>();
    InitContentLightFilter(contentLightFilter);
    SetBgAndChildNodes(0, 1, screenWidth, screenHeight, contentLightFilter);
}

GRAPHIC_TEST(NGFilterContentLightTest, EFFECT_TEST, Set_PositionTest)
{
    const size_t columnCount = 2;
    const size_t rowCount = positionParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto contentLightFilter = std::make_shared<RSNGContentLightFilter>();
        InitContentLightFilter(contentLightFilter);
        contentLightFilter->Setter<ContentLightPositionTag>(positionParams[i]);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, contentLightFilter);
    }
}

GRAPHIC_TEST(NGFilterContentLightTest, EFFECT_TEST, Set_ColorTest)
{
    const size_t columnCount = 2;
    const size_t rowCount = colorParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto contentLightFilter = std::make_shared<RSNGContentLightFilter>();
        InitContentLightFilter(contentLightFilter);
        contentLightFilter->Setter<ContentLightColorTag>(colorParams[i]);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, contentLightFilter);
    }
}

GRAPHIC_TEST(NGFilterContentLightTest, EFFECT_TEST, Set_IntensityTest)
{
    const size_t columnCount = 2;
    const size_t rowCount = intensityParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < rowCount; i++) {
        auto contentLightFilter = std::make_shared<RSNGContentLightFilter>();
        InitContentLightFilter(contentLightFilter);
        contentLightFilter->Setter<ContentLightIntensityTag>(intensityParams[i]);
        SetBgAndChildNodes(i, columnCount, sizeX, sizeY, contentLightFilter);
    }
}
}  // namespace OHOS::Rosen