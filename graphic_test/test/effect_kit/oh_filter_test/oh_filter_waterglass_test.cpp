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
}

class OHFilterWaterGlassTest : public RSGraphicTest {
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

GRAPHIC_TEST(OHFilterWaterGlassTest, EFFECT_TEST, WaterGlassTest)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(OHFilterTestDataGroupParamsType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;

    for (int i = 0; i < rowCount; i++) {
        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        auto ohFilter = CreateFilter(pixelMapNative);

        OH_Filter_WaterGlassDataParams params = {};
        params.waveCenter = {waterGlassParams[i][0], waterGlassParams[i][1]};
        params.waveSourceXY = {waterGlassParams[i][2], waterGlassParams[i][3]};
        params.waveDistortXY = {waterGlassParams[i][4], waterGlassParams[i][5]};
        params.waveDensityXY = {waterGlassParams[i][6], waterGlassParams[i][7]};
        params.waveStrength = waterGlassParams[i][8];
        params.waveLightStrength = waterGlassParams[i][9];
        params.waveRefraction = waterGlassParams[i][10];
        params.waveSpecular = waterGlassParams[i][11];
        params.waveFrequency = waterGlassParams[i][12];
        params.waveShapeDistortion = waterGlassParams[i][13];
        params.waveNoiseStrength = waterGlassParams[i][14];
        params.waveMaskSize = {waterGlassParams[i][15], waterGlassParams[i][16]};
        params.waveMaskRadius = waterGlassParams[i][17];
        params.borderRadius = waterGlassParams[i][18];
        params.borderThickness = waterGlassParams[i][19];
        params.borderScope = waterGlassParams[i][20];
        params.borderStrength = waterGlassParams[i][21];
        params.progress = waterGlassParams[i][22];

        OH_Filter_WaterGlass(ohFilter, &params);
        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

}  // namespace OHOS::Rosen
