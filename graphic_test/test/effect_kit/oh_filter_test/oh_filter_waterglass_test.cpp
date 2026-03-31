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
        const auto& data = waterGlassParams[i];
        params.speed = data[0]; // 0 index
        params.distortSpeed = data[1]; // 1 index
        params.refractionSpeed = { data[2],  data[3]}; // 2, 3 index
        params.progress = data[4]; // 4 index
        params.shakingDirection1 = {data[5], data[6]}; // 5, 6 index
        params.shakingDirection2 = {data[7], data[8]}; // 7, 8 idnex
        params.waveDensityXY = {data[9], data[10]}; // 9, 10 index
        params.waveStrength = data[11]; // 11 index
        params.waveRefraction = data[12]; // 12 index
        params.waveSpecular = data[13]; // 13 index
        params.waveFrequency = data[14]; // 14 index
        params.waveShapeDistortion = data[15]; // 15 index
        params.waveDistortionAngle = data[16]; // 16 index
        params.rippleXWave = data[17]; // 17 index
        params.rippleYWave = data[18]; // 18 index
        params.borderRadius = data[19]; // 19 index
        params.borderThickness = data[20]; // 20 index
        params.waveInnerMaskXY = {data[21], data[22]}; // 21, 22 index
        params.waveInnerMaskRadius = data[23]; // 23 index
        params.waveInnerMaskSmoothness = data[24]; // 24 index
        params.waveOuterMaskPadding = data[25]; // 25 index
        params.waveSpecularPower = data[26]; // 26 index
        params.refractionDetailDark = data[27]; // 27 index
        params.refractionDetailWhite = data[28]; // 28 index
        params.detailStrength = data[29]; // 29 index

        OH_Filter_WaterGlass(ohFilter, &params);
        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

}  // namespace OHOS::Rosen
