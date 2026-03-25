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

class OHFilterCascadeTest : public RSGraphicTest {
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
        params.speed = data[0];
        params.distortSpeed = data[1];
        params.refractionSpeed = { data[2],  data[3]};
        params.progress = data[4];
        params.shakingDirection1 = {data[5], data[6]};
        params.shakingDirection2 = {data[7], data[8]};
        params.waveDensityXY = {data[9], data[10]};
        params.waveStrength = data[11];
        params.waveRefraction = data[12];
        params.waveSpecular = data[13];
        params.waveFrequency = data[14];
        params.waveShapeDistortion = data[15];
        params.waveDistortionAngle = data[16];
        params.rippleXWave = data[17];
        params.rippleYWave = data[18];
        params.borderRadius = data[19];
        params.borderThickness = data[20];
        params.waveInnerMaskXY = {data[21], data[22]};
        params.waveInnerMaskRadius = data[23];
        params.waveInnerMaskSmoothness = data[24];
        params.waveOuterMaskPadding = data[25];
        params.waveSpecularPower = data[26];
        params.refractionDetailDark = data[27];
        params.refractionDetailWhite = data[28];
        params.detailStrength = data[29];
        return true;
    }

    bool SetupReededGlassParams(OH_Filter_ReededGlassDataParams& params, size_t index)
    {
        if (index >= reededGlassParams.size()) {
            return false;
        }
        const auto& data = reededGlassParams[index];
        params.refractionFactor = data[REFRACTION_FACTOR_INDEX];
        params.horizontalPatternNumber = static_cast<uint8_t>(data[HORIZONTAL_PATTERN_NUMBER_INDEX]);
        params.gridLightStrength = data[GRID_LIGHT_STRENGTH_INDEX];
        params.gridLightPositionStart = data[GRID_LIGHT_POSITION_START_INDEX];
        params.gridLightPositionEnd = data[GRID_LIGHT_POSITION_END_INDEX];
        params.gridShadowStrength = data[GRID_SHADOW_STRENGTH_INDEX];
        params.gridShadowPositionStart = data[GRID_SHADOW_POSITION_START_INDEX];
        params.gridShadowPositionEnd = data[GRID_SHADOW_POSITION_END_INDEX];
        params.portalLightSize = {data[PORTAL_LIGHT_SIZE_X_INDEX], data[PORTAL_LIGHT_SIZE_Y_INDEX]};
        params.portalLightTilt = {data[PORTAL_LIGHT_TILT_X_INDEX], data[PORTAL_LIGHT_TILT_Y_INDEX]};
        params.portalLightPosition = {data[PORTAL_LIGHT_POSITION_X_INDEX], data[PORTAL_LIGHT_POSITION_Y_INDEX]};
        params.portalLightDisperseAttenuation = data[PORTAL_LIGHT_DISPERSE_ATTENUATION_INDEX];
        params.portalLightDisperse = data[PORTAL_LIGHT_DISPERSE_INDEX];
        params.portalLightSmoothBorder = data[PORTAL_LIGHT_SMOOTH_BORDER_INDEX];
        params.portalLightShadowBorder = data[PORTAL_LIGHT_SHADOW_BORDER_INDEX];
        params.portalLightShadowPositionShift = data[PORTAL_LIGHT_SHADOW_POSITION_SHIFT_INDEX];
        params.portalLightStrength = data[PORTAL_LIGHT_STRENGTH_INDEX];
        return true;
    }
};

/*
 * Test all 7 effects cascaded together with 6 parameter groups
 */
GRAPHIC_TEST(OHFilterCascadeTest, EFFECT_TEST, All_Effects_Cascade_Test)
{
    const size_t testCount = 6;
    const int columnCount = 2;
    const int rowCount = 3;

    for (size_t i = 0; i < testCount; i++) {
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto pixelMapNative = CreateTestPixelMap(FG_TEST_JPG_PATH);
        if (!pixelMapNative) {
            continue;
        }
        auto ohFilter = CreateFilter(pixelMapNative);
        if (!ohFilter) {
            continue;
        }

        OH_Filter_GammaCorrection(ohFilter, gammaCorrectionParams[i]);

        OH_Filter_MapColorByBrightnessParams mapColorParams = {};
        const auto& mapData = mapColorByBrightnessParams1[i];
        mapColorParams.colors = const_cast<OH_Filter_Color*>(mapData.colors.data());
        mapColorParams.positions = const_cast<float*>(mapData.positions.data());
        mapColorParams.colorsNum = mapData.colors.size();
        OH_Filter_MapColorByBrightness(ohFilter, &mapColorParams);

        OH_Filter_WaterGlassDataParams waterGlassParams = {};
        if (!SetupWaterGlassParams(waterGlassParams, i)) {
            OH_Filter_Release(ohFilter);
            continue;
        }
        OH_Filter_WaterGlass(ohFilter, &waterGlassParams);

        OH_Filter_ReededGlassDataParams reededGlassParams = {};
        if (!SetupReededGlassParams(reededGlassParams, i)) {
            OH_Filter_Release(ohFilter);
            continue;
        }
        OH_Filter_ReededGlass(ohFilter, &reededGlassParams);

        auto bgPixelMapNative = CreateTestPixelMap(BG_TEST_JPG_PATH);
        if (!bgPixelMapNative) {
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

        OH_Filter_MaskTransition(ohFilter, bgPixelMapNative, &linearGradientMask,
            EffectMaskType::LINEAR_GRADIENT_MASK, maskData.factor, maskData.inverse);

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

        OH_Filter_WaterDropletTransition(ohFilter, bgPixelMapNative,
            &params, false);

        OH_Filter_GetEffectPixelMap(ohFilter, &pixelMapNative);
        DrawBackgroundNodeOHPixelMap(pixelMapNative, {x, y, sizeX, sizeY});
        OH_Filter_Release(ohFilter);
    }
}

}  // namespace OHOS::Rosen
