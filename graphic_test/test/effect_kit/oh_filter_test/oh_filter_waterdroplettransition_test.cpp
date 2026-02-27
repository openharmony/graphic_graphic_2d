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

#include "oh_filter_test_utils.h"
#include "oh_filter_test_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string BG_TEST_JPG_PATH = "/data/local/tmp/bg_test.jpg";
const std::string FG_TEST_JPG_PATH = "/data/local/tmp/fg_test.jpg";
}

class OHFilterWaterDropletTransitionTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

void DrawBackgroundNodeOHPixelMap(OH_PixelmapNative* pixelMapNative, const Rosen::Vector4f bounds)
{
    auto pixelmap = pixelMapNative->GetInnerPixelmap();
    auto image = std::make_shared<Rosen::RSImage>();
    image->SetPixelMap(pixelmap);
    image->SetImageFit((int)ImageFit::FILL);
    auto node = Rosen::RSCanvasNode::Create();
    node->SetBounds(bounds);
    node->SetFrame(bounds);
    node->SetBgImageSize(bounds[WIDTH_INDEX], bounds[HEIGHT_INDEX]);
    node->SetBgImage(image);
    GetRootNode()->AddChild(node);
    RegisterNode(node);
}

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

GRAPHIC_TEST(OHFilterWaterDropletTransitionTest, EFFECT_TEST, WaterDropletTransitionTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(OHFilterTestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        auto bgPixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        auto fgPixelMapNative = CreateTestPixelMap(FG_TEST_JPG_PATH);
        auto ohFilter = CreateFilter(bgPixelMapNative);

        OH_Filter_WaterDropletParams params = {};
        params.radius = waterDropletParams[i][0];
        params.transitionFadeWidth = waterDropletParams[i][1];
        params.distortionIntensity = waterDropletParams[i][2];
        params.distortionThickness = waterDropletParams[i][3];
        params.lightStrength = waterDropletParams[i][4];
        params.lightSoftness = waterDropletParams[i][5];
        params.noiseScaleX = waterDropletParams[i][6];
        params.noiseScaleY = waterDropletParams[i][7];
        params.noiseStrengthX = waterDropletParams[i][8];
        params.noiseStrengthY = waterDropletParams[i][9];
        params.progress = waterDropletProgressParams[i];

        bool inverse = (i < 3) ? false : true;
        OH_Filter_WaterDropletTransition(ohFilter, fgPixelMapNative, &params, inverse);
        OH_Filter_GetEffectPixelMap(ohFilter, &bgPixelMapNative);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        DrawBackgroundNodeOHPixelMap(bgPixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

}  // namespace OHOS::Rosen
