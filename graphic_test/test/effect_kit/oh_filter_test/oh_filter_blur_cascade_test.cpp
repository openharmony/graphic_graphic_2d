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
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
}

class OHFilterBlurCascadeTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void DrawBackgroundNodeOHPixelMap(OH_PixelmapNative* pixelMapNative,
        const Rosen::Vector4f bounds)
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

    bool SetupWaterGlassParams(OH_Filter_WaterGlassDataParams& params, size_t index)
    {
        if (index >= waterGlassParams.size()) {
            return false;
        }
        const auto& data = waterGlassParams[index];
        params.waveCenter = {data[0], data[1]};
        params.waveSourceXY = {data[2], data[3]};
        params.waveDistortXY = {data[4], data[5]};
        params.waveDensityXY = {data[6], data[7]};
        params.waveStrength = data[8];
        params.waveLightStrength = data[9];
        params.waveRefraction = data[10];
        params.waveSpecular = data[11];
        params.waveFrequency = data[12];
        params.waveShapeDistortion = data[13];
        params.waveNoiseStrength = data[14];
        params.waveMaskSize = {data[15], data[16]};
        params.waveMaskRadius = data[17];
        params.borderRadius = data[18];
        params.borderThickness = data[19];
        params.borderScope = data[20];
        params.borderStrength = data[21];
        params.progress = data[22];
        return true;
    }

    bool SetupReededGlassParams(OH_Filter_ReededGlassDataParams& params, size_t index)
    {
        if (index >= reededGlassParams.size()) {
            return false;
        }
        const auto& data = reededGlassParams[index];
        params.refractionFactor = data[0];
        params.dispersionStrength = data[1];
        params.roughness = data[2];
        params.noiseFrequency = data[3];
        params.horizontalPatternNumber = static_cast<uint8_t>(data[4]);
        params.saturationFactor = data[5];
        params.borderLightStrength = data[6];
        params.borderLightWidth = data[7];
        params.pointLightColor = {data[8], data[9], data[10], data[11]};
        params.pointLight1Position = {data[12], data[13]};
        params.pointLight1Strength = data[14];
        params.pointLight2Position = {data[15], data[16]};
        params.pointLight2Strength = data[17];
        return true;
    }
};

/*
 * Test Blur cascaded with GammaCorrection effect
 */
GRAPHIC_TEST(OHFilterBlurCascadeTest, EFFECT_TEST, Blur_With_GammaCorrection_Test)
{
    const size_t testCount = 6;
    const int columnCount = 3;
    const int rowCount = 2;

    for (size_t i = 0; i < testCount; i++) {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        if (!pixelMapNative) {
            continue;
        }
        auto ohFilter = CreateFilter(pixelMapNative);
        if (!ohFilter) {
            continue;
        }

        OH_Filter_Blur(ohFilter, blurParams[i]);
        OH_Filter_GammaCorrection(ohFilter, gammaCorrectionParams[i]);
        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

/*
 * Test Blur cascaded with MapColorByBrightness effect
 */
GRAPHIC_TEST(OHFilterBlurCascadeTest, EFFECT_TEST, Blur_With_MapColorByBrightness_Test)
{
    const size_t testCount = 6;
    const int columnCount = 3;
    const int rowCount = 2;

    for (size_t i = 0; i < testCount; i++) {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        if (!pixelMapNative) {
            continue;
        }
        auto ohFilter = CreateFilter(pixelMapNative);
        if (!ohFilter) {
            continue;
        }

        OH_Filter_Blur(ohFilter, blurParams[i]);

        OH_Filter_MapColorByBrightnessParams mapColorParams = {};
        const auto& mapData = mapColorByBrightnessParams1[i];
        mapColorParams.colors = const_cast<OH_Filter_Color*>(mapData.colors.data());
        mapColorParams.positions = const_cast<float*>(mapData.positions.data());
        mapColorParams.colorsNum = mapData.colors.size();
        OH_Filter_MapColorByBrightness(ohFilter, &mapColorParams);

        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

/*
 * Test Blur cascaded with WaterGlass effect
 */
GRAPHIC_TEST(OHFilterBlurCascadeTest, EFFECT_TEST, Blur_With_WaterGlass_Test)
{
    const size_t testCount = 6;
    const int columnCount = 3;
    const int rowCount = 2;

    for (size_t i = 0; i < testCount; i++) {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        if (!pixelMapNative) {
            continue;
        }
        auto ohFilter = CreateFilter(pixelMapNative);
        if (!ohFilter) {
            continue;
        }

        OH_Filter_Blur(ohFilter, blurParams[i]);

        OH_Filter_WaterGlassDataParams waterGlassParams = {};
        if (!SetupWaterGlassParams(waterGlassParams, i)) {
            OH_Filter_Release(ohFilter);
            continue;
        }
        OH_Filter_WaterGlass(ohFilter, &waterGlassParams);
        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

/*
 * Test Blur cascaded with ReededGlass effect
 */
GRAPHIC_TEST(OHFilterBlurCascadeTest, EFFECT_TEST, Blur_With_ReededGlass_Test)
{
    const size_t testCount = 6;
    const int columnCount = 3;
    const int rowCount = 2;

    for (size_t i = 0; i < testCount; i++) {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        if (!pixelMapNative) {
            continue;
        }
        auto ohFilter = CreateFilter(pixelMapNative);
        if (!ohFilter) {
            continue;
        }

        OH_Filter_Blur(ohFilter, blurParams[i]);

        OH_Filter_ReededGlassDataParams reededGlassParams = {};
        if (!SetupReededGlassParams(reededGlassParams, i)) {
            OH_Filter_Release(ohFilter);
            continue;
        }
        OH_Filter_ReededGlass(ohFilter, &reededGlassParams);
        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

/*
 * Test Blur cascaded with MaskTransition effect
 */
GRAPHIC_TEST(OHFilterBlurCascadeTest, EFFECT_TEST, Blur_With_MaskTransition_Test)
{
    const size_t testCount = 6;
    const int columnCount = 3;
    const int rowCount = 2;

    for (size_t i = 0; i < testCount; i++) {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        if (!pixelMapNative) {
            continue;
        }
        auto ohFilter = CreateFilter(pixelMapNative);
        if (!ohFilter) {
            continue;
        }

        OH_Filter_Blur(ohFilter, blurParams[i]);

        auto fgPixelMapNative = CreateTestPixelMap(FG_TEST_JPG_PATH);
        if (!fgPixelMapNative) {
            OH_Filter_Release(ohFilter);
            continue;
        }

        const auto& maskData = linearGradientMaskParams[i];
        OH_Filter_LinearGradientMask linearGradientMask = {};
        linearGradientMask.startPosition = maskData.startPosition;
        linearGradientMask.endPosition = maskData.endPosition;
        linearGradientMask.fractionStops =
            const_cast<OH_Filter_Vec2*>(maskData.fractionStops.data());
        linearGradientMask.fractionStopsLength = maskData.fractionStops.size();

        OH_Filter_MaskTransition(ohFilter, fgPixelMapNative, &linearGradientMask,
            EffectMaskType::LINEAR_GRADIENT_MASK, maskData.factor, maskData.inverse);

        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

/*
 * Test Blur cascaded with WaterDropletTransition effect
 */
GRAPHIC_TEST(OHFilterBlurCascadeTest, EFFECT_TEST, Blur_With_WaterDropletTransition_Test)
{
    const size_t testCount = 6;
    const int columnCount = 3;
    const int rowCount = 2;

    for (size_t i = 0; i < testCount; i++) {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto pixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        if (!pixelMapNative) {
            continue;
        }
        auto ohFilter = CreateFilter(pixelMapNative);
        if (!ohFilter) {
            continue;
        }

        OH_Filter_Blur(ohFilter, blurParams[i]);

        auto fgPixelMapNative = CreateTestPixelMap(FG_TEST_JPG_PATH);
        if (!fgPixelMapNative) {
            OH_Filter_Release(ohFilter);
            continue;
        }

        OH_Filter_WaterDropletParams params = {};
        const auto& dropletData = waterDropletParams[i];
        params.radius = dropletData[0];
        params.transitionFadeWidth = dropletData[1];
        params.distortionIntensity = dropletData[2];
        params.distortionThickness = dropletData[3];
        params.lightStrength = dropletData[4];
        params.lightSoftness = dropletData[5];
        params.noiseScaleX = dropletData[6];
        params.noiseScaleY = dropletData[7];
        params.noiseStrengthX = dropletData[8];
        params.noiseStrengthY = dropletData[9];
        params.progress = waterDropletProgressParams[i];

        OH_Filter_WaterDropletTransition(ohFilter, fgPixelMapNative,
            &params, false);

        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);

        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

}  // namespace OHOS::Rosen
