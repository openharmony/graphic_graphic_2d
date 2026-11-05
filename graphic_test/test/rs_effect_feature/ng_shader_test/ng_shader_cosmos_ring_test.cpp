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

#include <array>
#include <cfloat>
#include <limits>
#include <tuple>
#include <vector>

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"

#include "ui/rs_effect_node.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {

const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/AllWhite.jpg";
const int32_t SCREEN_WIDTH = 1200;
const int32_t SCREEN_HEIGHT = 2000;
const int32_t NODE_SIZE = 350;

// Colours used by the colour sweeps, picked so that each sweep has four distinct tones.
const Vector4f DARK_DESIGN_COLOR(0.10f, 0.00f, 0.30f, 1.0f);
const Vector4f MID_DESIGN_COLOR(0x0B / 255.0f, 0x5C / 255.0f, 0xFF / 255.0f, 1.0f);
const Vector4f BRIGHT_DESIGN_COLOR(1.00f, 0.75f, 0.30f, 1.0f);
const Vector4f PARTICLE_DESIGN_COLOR(0.30f, 0.85f, 0.45f, 1.0f);

// Four arc parameters share one pattern so a span or brightness sweep can enable one arc at a time.
struct HLArcPattern {
    float p1Outer;
    float p1Inner;
    float p2Outer;
    float p2Inner;
};

// ---- Requirement 1: base ring ----
const std::vector<float> progressValues = { 0.0f, 0.35f, 0.75f, 1.0f };
const std::vector<float> radiusValues = { 20.0f, 87.0f, 150.0f, 300.0f };
const std::vector<float> widthValues = { 1.0f, 15.0f, 30.0f, 40.0f };
const std::vector<float> featherValues = { 0.0f, 5.0f, 14.7f, 40.0f };
const std::vector<float> noiseDensityValues = { 10.0f, 25.5f, 50.0f, 100.0f };
const std::vector<int> noisePeriodValues = { 1, 4, 10, 50 };
const std::vector<float> noiseContrastValues = { 0.5f, 1.0f, 2.56f, 10.0f };
const std::vector<float> noise2DensityValues = { 5.0f, 10.5f, 25.0f, 100.0f };
const std::vector<int> noise2PeriodValues = { 1, 3, 7, 32 };
const std::vector<float> noise2ContrastValues = { 0.6f, 1.28f, 2.0f, 10.0f };
const std::vector<int> noiseSeedValues = { 0, 29, 5000, 10000 };
const std::vector<Vector4f> darkColorValues = {
    Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
    Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
    DARK_DESIGN_COLOR,
    Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
};
const std::vector<Vector4f> midColorValues = {
    MID_DESIGN_COLOR,
    Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
    Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
    Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
};
const std::vector<Vector4f> brightColorValues = {
    Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
    BRIGHT_DESIGN_COLOR,
    Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
    Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
};
const std::vector<std::tuple<float, float>> radiusWidthCombos = {
    { 50.0f, 10.0f },
    { 87.0f, 20.0f },
    { 150.0f, 40.0f },
    { 300.0f, 40.0f },
};
const std::vector<std::tuple<float, float>> radiusFeatherCombos = {
    { 50.0f, 0.0f },
    { 150.0f, 5.0f },
    { 300.0f, 20.0f },
    { 150.0f, 60.0f },
};
const std::vector<std::tuple<float, float>> progressRadiusCombos = {
    { 0.0f, 87.0f },
    { 0.33f, 150.0f },
    { 0.66f, 300.0f },
    { 1.0f, 87.0f },
};
const std::vector<std::tuple<float, float>> progressWidthCombos = {
    { 0.0f, 10.0f },
    { 0.33f, 40.0f },
    { 0.66f, 40.0f },
    { 1.0f, 10.0f },
};
const std::vector<std::tuple<float, int>> noiseDensityPeriodCombos = {
    { 10.0f, 4 },
    { 25.5f, 10 },
    { 50.0f, 50 },
    { 100.0f, 100 },
};
const std::vector<std::tuple<int, float>> noise2PeriodContrastCombos = {
    { 3, 0.5f },
    { 7, 1.28f },
    { 32, 5.0f },
    { 100, 10.0f },
};
const std::vector<std::tuple<int, float>> noiseSeedContrastCombos = {
    { 0, 1.0f },
    { 0, 5.0f },
    { 10000, 1.0f },
    { 10000, 5.0f },
};
const std::vector<std::tuple<float, float>> noiseDensityContrastCombos = {
    { 10.0f, 1.0f },
    { 50.0f, 1.0f },
    { 100.0f, 5.0f },
    { 100.0f, 10.0f },
};
// ---- Requirement 2: chroma dispersion ----
const std::vector<float> chromaStrengthValues = { 0.0f, 0.35f, 0.69f, 10.0f };
const std::vector<std::tuple<float, Vector4f>> chromaColorCombos = {
    { 0.0f, MID_DESIGN_COLOR },
    { 0.69f, MID_DESIGN_COLOR },
    { 5.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f) },
    { 10.0f, Vector4f(0.0f, 0.0f, 0.0f, 0.0f) },
};
const std::vector<std::tuple<float, Vector4f>> chromaBrightColorCombos = {
    { 0.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f) },
    { 0.69f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f) },
    { 5.0f, BRIGHT_DESIGN_COLOR },
    { 10.0f, BRIGHT_DESIGN_COLOR },
};
// ---- Requirement 3: highlight ----
const std::array<bool, 2> hlEnableValues = { true, false };
const std::vector<float> hlOuterRadiusValues = { 40.0f, 78.0f, 97.0f, 300.0f };
const std::vector<float> hlInnerRadiusValues = { 30.0f, 78.0f, 135.0f, 260.0f };
const std::vector<float> hlOuterWidthValues = { 0.0f, 1.0f, 6.11f, 20.0f };
const std::vector<float> hlInnerWidthValues = { 0.0f, 1.0f, 3.75f, 20.0f };
const std::vector<float> hlDispersionStrengthValues = { 0.0f, 0.32f, 1.0f, 10.0f };
const std::vector<float> hlRotationAngleValues = { 0.0f, 100.0f, 180.0f, 270.0f };
const std::vector<float> hlRadialFeatherValues = { 0.0f, 1.0f, 10.0f, 30.0f };
const std::vector<float> hlTangentFeatherValues = { 0.0f, 10.0f, 50.0f, 120.0f };
const std::vector<float> hlAngularFadeValues = { 0.0f, 0.1f, 0.3f, 0.5f };
const std::vector<float> centerAngleValues = { 0.0f, 45.0f, 180.0f, 270.0f };
const std::vector<float> arcSpanValues = { 0.0f, 90.0f, 220.0f, 360.0f };
const std::vector<float> arcBrightnessValues = { 0.0f, 0.5f, 1.0f, 2.0f };
const std::vector<Vector2f> dispScalesValues = {
    Vector2f(0.0f, 0.0f),
    Vector2f(0.8f, 1.4f),
    Vector2f(1.0f, 1.0f),
    Vector2f(2.0f, 2.0f),
};
const std::vector<std::tuple<float, float>> hlOuterInnerRadiusCombos = {
    { 135.0f, 110.0f },
    { 165.0f, 135.0f },
    { 200.0f, 300.0f },
    { 300.0f, 135.0f },
};
const std::vector<std::tuple<float, float>> hlOuterInnerWidthCombos = {
    { 1.0f, 0.0f },
    { 1.0f, 5.78f },
    { 5.78f, 1.0f },
    { 20.0f, 20.0f },
};
const std::vector<std::tuple<float, float>> hlRadialTangentCombos = {
    { 0.0f, 0.0f },
    { 8.0f, 35.0f },
    { 30.0f, 120.0f },
    { 60.0f, 360.0f },
};
const std::vector<std::tuple<float, float>> hlAngularFadeRotationCombos = {
    { 0.0f, 0.0f },
    { 0.2f, 90.0f },
    { 0.5f, 180.0f },
    { 1.0f, 270.0f },
};
const std::vector<std::tuple<float, float>> hlRotationCenterAngleCombos = {
    { 0.0f, 0.0f },
    { 90.0f, 45.0f },
    { 180.0f, 204.0f },
    { 270.0f, 270.0f },
};
const std::vector<HLArcPattern> hlArcSpanPatterns = {
    { 220.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 260.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 210.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 190.0f },
};
const std::vector<HLArcPattern> hlArcBrightnessPatterns = {
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 2.0f, 2.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 2.0f, 2.0f },
    { 2.0f, 0.0f, 0.0f, 0.0f },
};
const std::vector<std::tuple<float, float>> hlP1P2BrightnessCombos = {
    { 0.0f, 0.0f },
    { 2.0f, 0.0f },
    { 0.0f, 2.0f },
    { 2.0f, 2.0f },
};
const std::vector<std::tuple<Vector2f, float>> hlDispScalesStrengthCombos = {
    { Vector2f(0.8f, 1.4f), 0.0f },
    { Vector2f(0.8f, 1.4f), 5.0f },
    { Vector2f(10.0f, 10.0f), 0.0f },
    { Vector2f(10.0f, 10.0f), 5.0f },
};
// ---- Requirement 4: particles ----
const std::vector<float> particleCountValues = { 1.0f, 10.0f, 65.0f, 150.0f };
const std::vector<float> particleSizeValues = { 0.5f, 1.0f, 3.7f, 10.0f };
const std::vector<float> particleDispersionValues = { 0.0f, 0.01f, 0.2f, 1.0f };
const std::vector<float> particleScaleValues = { 0.25f, 0.5f, 0.925f, 1.0f };
const std::vector<float> particleInnerRadiusValues = { 20.0f, 65.0f, 84.175f, 150.0f };
const std::vector<float> particleOuterRadiusValues = { 50.0f, 102.305f, 150.0f, 300.0f };
const std::vector<float> particlePathLengthValues = { 10.0f, 31.08f, 90.0f, 360.0f };
const std::vector<Vector4f> particleMidColorValues = {
    MID_DESIGN_COLOR,
    Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
    Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
    Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
};
const std::vector<float> particleTintStrengthValues = { 0.0f, 0.2f, 0.6f, 1.0f };
const std::vector<float> particleBrightnessValues = { 0.0f, 0.5f, 1.0f, 4.0f };
const std::vector<float> particleBirthPeakValues = { 0.0f, 0.15f, 0.3f, 1.0f };
const std::vector<float> particleOpacityValues = { 0.0f, 0.2f, 0.6f, 1.0f };
const std::vector<std::tuple<float, float>> particleCountSizeCombos = {
    { 10.0f, 1.0f },
    { 65.0f, 2.0f },
    { 150.0f, 2.0f },
    { 500.0f, 10.0f },
};
const std::vector<std::tuple<float, float>> particleRadiusRangeCombos = {
    { 65.0f, 102.0f },
    { 84.0f, 150.0f },
    { 65.0f, 300.0f },
    { 300.0f, 150.0f },
};
const std::vector<std::tuple<float, float>> particleDispersionPathCombos = {
    { 0.0f, 0.0f },
    { 0.0f, 360.0f },
    { 1.0f, 0.0f },
    { 1.0f, 360.0f },
};
const std::vector<std::tuple<float, float>> particleBrightnessBirthPeakCombos = {
    { 0.0f, 0.3f },
    { 1.0f, 0.0f },
    { 4.0f, 0.3f },
    { 1.0f, 1.0f },
};
const std::vector<std::tuple<float, Vector4f>> particleTintColorCombos = {
    { 0.0f, PARTICLE_DESIGN_COLOR },
    { 0.6f, PARTICLE_DESIGN_COLOR },
    { 1.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f) },
    { 1.0f, Vector4f(0.0f, 1.0f, 0.0f, 1.0f) },
};
const std::vector<std::tuple<float, float>> particleScaleOpacityCombos = {
    { 0.25f, 0.2f },
    { 1.0f, 1.0f },
    { 0.25f, 1.0f },
    { 1.0f, 0.2f },
};
const std::vector<std::tuple<float, float>> particleCountOpacityCombos = {
    { 1.0f, 0.2f },
    { 65.0f, 1.0f },
    { 500.0f, 0.2f },
    { 500.0f, 1.0f },
};
const std::vector<std::tuple<float, float>> particleSizeInnerRadiusCombos = {
    { 1.0f, 65.0f },
    { 10.0f, 65.0f },
    { 1.0f, 300.0f },
    { 10.0f, 300.0f },
};
const std::vector<std::tuple<float, float>> particleSizeDispersionCombos = {
    { 0.5f, 0.0f },
    { 3.7f, 0.04f },
    { 10.0f, 0.04f },
    { 10.0f, 1.0f },
};
// ---- Requirement 5: glow ----
const std::vector<Vector2f> glowRadiusValues = {
    Vector2f(0.0f, 0.0f),
    Vector2f(18.0f, 90.0f),
    Vector2f(50.0f, 200.0f),
    Vector2f(500.0f, 1000.0f),
};
const std::vector<Vector2f> glowStrengthValues = {
    Vector2f(0.0f, 0.0f),
    Vector2f(0.12f, 0.43f),
    Vector2f(0.45f, 0.25f),
    Vector2f(1.0f, 1.0f),
};
const std::vector<Vector2f> glowThresholdValues = {
    Vector2f(-1.0f, -1.0f),
    Vector2f(-0.26f, 0.0f),
    Vector2f(0.0f, 0.0f),
    Vector2f(1.0f, 1.0f),
};
const std::vector<float> glowIntensityValues = { 0.0f, 0.41f, 0.85f, 1.0f };
const std::vector<float> glowSaturationValues = { 0.0f, 0.2f, 0.9f, 1.0f };
const std::vector<float> glowExposureValues = { 0.5f, 1.45f, 6.59f, 10.0f };
const std::vector<std::tuple<Vector2f, Vector2f>> glowRadiusStrengthCombos = {
    { Vector2f(18.0f, 90.0f), Vector2f(0.12f, 0.43f) },
    { Vector2f(18.0f, 90.0f), Vector2f(1.0f, 1.0f) },
    { Vector2f(500.0f, 1000.0f), Vector2f(0.12f, 0.43f) },
    { Vector2f(500.0f, 1000.0f), Vector2f(1.0f, 1.0f) },
};
const std::vector<std::tuple<Vector2f, float>> glowThresholdIntensityCombos = {
    { Vector2f(-0.26f, 0.0f), 0.41f },
    { Vector2f(-0.26f, 0.0f), 1.0f },
    { Vector2f(0.0f, 0.0f), 0.41f },
    { Vector2f(1.0f, 1.0f), 1.0f },
};
const std::vector<std::tuple<float, float>> glowIntensitySaturationCombos = {
    { 0.0f, 0.0f },
    { 0.85f, 0.0f },
    { 0.85f, 1.0f },
    { 1.0f, 1.0f },
};
const std::vector<std::tuple<float, float>> glowSaturationExposureCombos = {
    { 0.0f, 0.5f },
    { 0.0f, 1.45f },
    { 1.0f, 6.59f },
    { 1.0f, 10.0f },
};
const std::vector<std::tuple<Vector2f, Vector2f>> glowRadiusThresholdCombos = {
    { Vector2f(18.0f, 90.0f), Vector2f(-1.0f, -1.0f) },
    { Vector2f(18.0f, 90.0f), Vector2f(1.0f, 1.0f) },
    { Vector2f(500.0f, 1000.0f), Vector2f(0.0f, 0.0f) },
    { Vector2f(500.0f, 1000.0f), Vector2f(1.0f, 1.0f) },
};
const std::vector<std::tuple<float, float>> glowExposureIntensityCombos = {
    { 0.5f, 0.41f },
    { 1.45f, 1.0f },
    { 6.59f, 0.41f },
    { 10.0f, 1.0f },
};
// ---- Boundary, extreme and invalid values shared by several parameters ----
const std::vector<float> progressBoundaryValues = { -0.5f, 0.0f, 1.0f, 1.5f };
const std::vector<float> pixelBoundaryValues = { -1.0f, 0.0f, 4096.0f, 4097.0f };
const std::vector<float> positivePixelBoundaryValues = { 0.0f, 1.0f, 4096.0f, 4097.0f };
const std::vector<float> percentBoundaryValues = { -1.0f, 0.0f, 100.0f, 101.0f };
const std::vector<int> intPercentBoundaryValues = { -1, 0, 100, 101 };
const std::vector<int> noiseSeedBoundaryValues = { -1, 0, 10000, 10001 };
const std::vector<int> noisePeriodBoundaryValues = { -1, 1, 100, 101 };
const std::vector<float> contrastBoundaryValues = { -1.0f, 0.0f, 10.0f, 11.0f };
const std::vector<float> angleBoundaryValues = { -1.0f, 0.0f, 360.0f, 361.0f };
const std::vector<float> unitBoundaryValues = { -1.0f, 0.0f, 1.0f, 2.0f };
const std::vector<float> countBoundaryValues = { -1.0f, 0.0f, 1000.0f, 1001.0f };
const std::vector<float> extremeFloatValues = { -FLT_MAX, FLT_MAX, -1.0e9f, 1.0e9f };
const std::vector<float> invalidFloatValues = {
    std::numeric_limits<float>::quiet_NaN(),
    std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::denorm_min(),
};
const std::vector<Vector2f> pixelBoundaryVector2fValues = {
    Vector2f(-1.0f, -1.0f),
    Vector2f(0.0f, 0.0f),
    Vector2f(4096.0f, 4096.0f),
    Vector2f(4097.0f, 4097.0f),
};
const std::vector<Vector2f> unitBoundaryVector2fValues = {
    Vector2f(-1.0f, -1.0f),
    Vector2f(0.0f, 0.0f),
    Vector2f(1.0f, 1.0f),
    Vector2f(2.0f, 2.0f),
};
const std::vector<Vector2f> extremeVector2fValues = {
    Vector2f(FLT_MAX, FLT_MAX),
    Vector2f(-FLT_MAX, -FLT_MAX),
    Vector2f(1.0e9f, 0.0f),
    Vector2f(0.0f, 1.0e9f),
};
const std::vector<Vector2f> invalidVector2fValues = {
    Vector2f(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()),
    Vector2f(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
    Vector2f(-std::numeric_limits<float>::infinity(), 0.0f),
    Vector2f(std::numeric_limits<float>::denorm_min(), std::numeric_limits<float>::denorm_min()),
};

} // namespace

void InitCosmosRing(std::shared_ptr<RSNGCosmosRing>& cosmosRing)
{
    if (!cosmosRing) {
        return;
    }
    // Base ring: shape and two noise layers
    cosmosRing->Setter<CosmosRingProgressTag>(0.5f);
    cosmosRing->Setter<CosmosRingRadiusTag>(87.0f);
    cosmosRing->Setter<CosmosRingWidthTag>(30.0f);
    cosmosRing->Setter<CosmosRingFeatherTag>(14.7f);
    cosmosRing->Setter<CosmosRingNoiseDensityTag>(25.5f);
    cosmosRing->Setter<CosmosRingNoisePeriodTag>(10);
    cosmosRing->Setter<CosmosRingNoiseContrastTag>(1.0f);
    cosmosRing->Setter<CosmosRingNoise2DensityTag>(10.5f);
    cosmosRing->Setter<CosmosRingNoise2PeriodTag>(7);
    cosmosRing->Setter<CosmosRingNoise2ContrastTag>(0.6f);
    cosmosRing->Setter<CosmosRingNoiseSeedTag>(29);
    // Chroma dispersion and colour ramp
    cosmosRing->Setter<CosmosRingDarkColorTag>(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
    cosmosRing->Setter<CosmosRingMidColorTag>(Vector4f(0x0B / 255.0f, 0x5C / 255.0f, 0xFF / 255.0f, 1.0f));
    cosmosRing->Setter<CosmosRingBrightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    cosmosRing->Setter<CosmosRingChromaStrengthTag>(0.69f);
    // Highlight ring
    cosmosRing->Setter<CosmosRingHLEnableTag>(true);
    cosmosRing->Setter<CosmosRingHLOuterRadiusTag>(97.0f);
    cosmosRing->Setter<CosmosRingHLInnerRadiusTag>(78.0f);
    cosmosRing->Setter<CosmosRingHLOuterWidthTag>(6.11f);
    cosmosRing->Setter<CosmosRingHLInnerWidthTag>(3.75f);
    cosmosRing->Setter<CosmosRingHLDispersionStrengthTag>(0.32f);
    cosmosRing->Setter<CosmosRingHLRotationAngleTag>(100.0f);
    cosmosRing->Setter<CosmosRingHLP1OuterCenterAngleTag>(204.0f);
    cosmosRing->Setter<CosmosRingHLP1OuterArcSpanTag>(220.0f);
    cosmosRing->Setter<CosmosRingHLP1OuterBrightnessTag>(1.0f);
    cosmosRing->Setter<CosmosRingHLP1OuterDispScalesTag>(Vector2f(1.5f, 1.5f));
    cosmosRing->Setter<CosmosRingHLP1InnerCenterAngleTag>(270.0f);
    cosmosRing->Setter<CosmosRingHLP1InnerArcSpanTag>(260.0f);
    cosmosRing->Setter<CosmosRingHLP1InnerBrightnessTag>(1.0f);
    cosmosRing->Setter<CosmosRingHLP1InnerDispScalesTag>(Vector2f(1.0f, 1.0f));
    cosmosRing->Setter<CosmosRingHLP2OuterCenterAngleTag>(75.0f);
    cosmosRing->Setter<CosmosRingHLP2OuterArcSpanTag>(210.0f);
    cosmosRing->Setter<CosmosRingHLP2OuterBrightnessTag>(1.0f);
    cosmosRing->Setter<CosmosRingHLP2OuterDispScalesTag>(Vector2f(1.2f, 1.2f));
    cosmosRing->Setter<CosmosRingHLP2InnerCenterAngleTag>(70.0f);
    cosmosRing->Setter<CosmosRingHLP2InnerArcSpanTag>(190.0f);
    cosmosRing->Setter<CosmosRingHLP2InnerBrightnessTag>(1.0f);
    cosmosRing->Setter<CosmosRingHLP2InnerDispScalesTag>(Vector2f(1.0f, 1.0f));
    cosmosRing->Setter<CosmosRingHLRadialFeatherPxTag>(10.0f);
    cosmosRing->Setter<CosmosRingHLTangentFeatherDegTag>(50.0f);
    cosmosRing->Setter<CosmosRingHLAngularFadeTag>(0.3f);
    // Particles
    cosmosRing->Setter<CosmosRingParticleCountTag>(65.0f);
    cosmosRing->Setter<CosmosRingParticleSizeTag>(3.7f);
    cosmosRing->Setter<CosmosRingParticleDispersionStrengthTag>(0.01f);
    cosmosRing->Setter<CosmosRingParticleScaleTag>(1.0f);
    cosmosRing->Setter<CosmosRingParticleInnerRadiusTag>(65.0f);
    cosmosRing->Setter<CosmosRingParticleOuterRadiusTag>(113.0f);
    cosmosRing->Setter<CosmosRingParticlePathLengthTag>(45.0f);
    cosmosRing->Setter<CosmosRingParticleMidColorTag>(Vector4f(0x0B / 255.0f, 0x5C / 255.0f, 0xFF / 255.0f, 1.0f));
    cosmosRing->Setter<CosmosRingParticleTintStrengthTag>(0.55f);
    cosmosRing->Setter<CosmosRingParticleBrightnessTag>(2.04f);
    cosmosRing->Setter<CosmosRingParticleOpacityTag>(1.0f);
    cosmosRing->Setter<CosmosRingParticleBirthPeakTag>(0.15f);
    // Glow
    cosmosRing->Setter<CosmosRingGlowRadiusTag>(Vector2f(18.0f, 90.0f));
    cosmosRing->Setter<CosmosRingGlowStrengthTag>(Vector2f(0.12f, 0.43f));
    cosmosRing->Setter<CosmosRingGlowThresholdTag>(Vector2f(-0.26f, 0.0f));
    cosmosRing->Setter<CosmosRingGlowIntensityTag>(0.41f);
    cosmosRing->Setter<CosmosRingGlowSaturationTag>(1.0f);
    cosmosRing->Setter<CosmosRingGlowExposureTag>(6.59f);
}

void DisableCosmosRingChroma(const std::shared_ptr<RSNGCosmosRing>& cosmosRing)
{
    if (cosmosRing) {
        cosmosRing->Setter<CosmosRingChromaStrengthTag>(0.0f);
    }
}

void DisableCosmosRingHighlight(const std::shared_ptr<RSNGCosmosRing>& cosmosRing)
{
    if (cosmosRing) {
        cosmosRing->Setter<CosmosRingHLEnableTag>(false);
    }
}

void DisableCosmosRingParticles(const std::shared_ptr<RSNGCosmosRing>& cosmosRing)
{
    if (cosmosRing) {
        cosmosRing->Setter<CosmosRingParticleCountTag>(0.0f);
    }
}

void DisableCosmosRingGlow(const std::shared_ptr<RSNGCosmosRing>& cosmosRing)
{
    if (cosmosRing) {
        cosmosRing->Setter<CosmosRingGlowStrengthTag>(Vector2f(0.0f, 0.0f));
    }
}

class NGShaderCosmosRingTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    std::shared_ptr<RSEffectNode> SetUpEffectNode()
    {
        auto backgroundTestNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!backgroundTestNode || !effectNode) {
            return nullptr;
        }
        effectNode->SetBounds({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        effectNode->SetFrame({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        effectNode->SetClipToBounds(false);
        GetRootNode()->AddChild(backgroundTestNode);
        backgroundTestNode->AddChild(effectNode);
        RegisterNode(backgroundTestNode);
        RegisterNode(effectNode);
        return effectNode;
    }

    void SetUpCell(const size_t i, const size_t columns, const size_t rows, std::shared_ptr<RSEffectNode>& effectNode,
        const std::shared_ptr<RSNGShaderBase>& shader)
    {
        if (!effectNode || !shader || columns == 0 || rows == 0) {
            return;
        }
        const int32_t cellWidth = SCREEN_WIDTH / static_cast<int32_t>(columns);
        const int32_t cellHeight = SCREEN_HEIGHT / static_cast<int32_t>(rows);
        const int32_t x = static_cast<int32_t>(i % columns) * cellWidth + (cellWidth - NODE_SIZE) / 2;
        const int32_t y = static_cast<int32_t>(i / columns) * cellHeight + (cellHeight - NODE_SIZE) / 2;

        auto cellNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!cellNode) {
            return;
        }
        cellNode->SetBounds({ x, y, NODE_SIZE, NODE_SIZE });
        cellNode->SetFrame({ x, y, NODE_SIZE, NODE_SIZE });
        cellNode->SetBackgroundNGShader(shader);
        effectNode->AddChild(cellNode);
        RegisterNode(cellNode);
    }
};

// ---- Requirement 1: base ring: cases ----

/*
 * Test the CosmosRing animation phase
 * Zero and one are the seamless ends of one loop
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Progress_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < progressValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingProgressTag>(progressValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the base ring radius
 * Large radii are clipped by the node bounds
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Radius_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < radiusValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingRadiusTag>(radiusValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the ring band width
 * A width close to the radius collapses the inner hole
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Width_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < widthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingWidthTag>(widthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the ring edge feather
 * Zero keeps a hard edge while large values blur the band
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Feather_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < featherValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingFeatherTag>(featherValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the first noise layer density
 * Low density gives broad warps, high density fine jitter
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoiseDensity_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noiseDensityValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseDensityTag>(noiseDensityValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the first noise layer period
 * The period changes how often the warp repeats around the ring
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoisePeriod_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noisePeriodValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoisePeriodTag>(noisePeriodValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the first noise layer contrast
 * Contrast controls the amplitude of the warp
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoiseContrast_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noiseContrastValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseContrastTag>(noiseContrastValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the second noise layer density
 * The second layer adds detail on top of the first
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Noise2Density_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noise2DensityValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoise2DensityTag>(noise2DensityValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the second noise layer period
 * A different period keeps the two layers from masking each other
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Noise2Period_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noise2PeriodValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoise2PeriodTag>(noise2PeriodValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the second noise layer contrast
 * Low contrast perturbs, high contrast shows a visible texture
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Noise2Contrast_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noise2ContrastValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoise2ContrastTag>(noise2ContrastValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the noise seed
 * The same seed must reproduce, a new seed must change the pattern
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoiseSeed_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noiseSeedValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseSeedTag>(noiseSeedValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the dark tone of the luminance ramp
 * The dark tone should only appear in low luminance areas
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_DarkColor_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < darkColorValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingDarkColorTag>(darkColorValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the mid tone of the luminance ramp
 * Check where the colour transition lands on the band
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_MidColor_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < midColorValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingMidColorTag>(midColorValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the bright tone of the luminance ramp
 * The brightest area and the glow should follow this colour
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_BrightColor_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < brightColorValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingBrightColorTag>(brightColorValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Radius and width combinations
 * The band collapses when the width approaches the radius
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Radius_Width_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < radiusWidthCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingRadiusTag>(std::get<0>(radiusWidthCombos[i]));
        cosmosRing->Setter<CosmosRingWidthTag>(std::get<1>(radiusWidthCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Radius and feather combinations
 * Feather must not change the geometric extent of the band
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Radius_Feather_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < radiusFeatherCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingRadiusTag>(std::get<0>(radiusFeatherCombos[i]));
        cosmosRing->Setter<CosmosRingFeatherTag>(std::get<1>(radiusFeatherCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Progress and radius combinations
 * The warp should not accumulate with the phase
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Progress_Radius_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < progressRadiusCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingProgressTag>(std::get<0>(progressRadiusCombos[i]));
        cosmosRing->Setter<CosmosRingRadiusTag>(std::get<1>(progressRadiusCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Progress and width combinations
 * The seamless ends must hold for every band width
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Progress_Width_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < progressWidthCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingProgressTag>(std::get<0>(progressWidthCombos[i]));
        cosmosRing->Setter<CosmosRingWidthTag>(std::get<1>(progressWidthCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * First layer density and period combinations
 * Both scale the warp, find where they mask each other
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Noise_Density_Period_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noiseDensityPeriodCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseDensityTag>(std::get<0>(noiseDensityPeriodCombos[i]));
        cosmosRing->Setter<CosmosRingNoisePeriodTag>(std::get<1>(noiseDensityPeriodCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Second layer period and contrast combinations
 * Low frequency with high contrast against the reverse
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Noise2_Period_Contrast_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noise2PeriodContrastCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoise2PeriodTag>(std::get<0>(noise2PeriodContrastCombos[i]));
        cosmosRing->Setter<CosmosRingNoise2ContrastTag>(std::get<1>(noise2PeriodContrastCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Seed and contrast combinations
 * Same seed keeps the style, different contrast changes the strength
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Noise_Seed_Contrast_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noiseSeedContrastCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseSeedTag>(std::get<0>(noiseSeedContrastCombos[i]));
        cosmosRing->Setter<CosmosRingNoiseContrastTag>(std::get<1>(noiseSeedContrastCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Density and contrast combinations
 * Both shape the graininess of the warp
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Noise_Density_Contrast_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noiseDensityContrastCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseDensityTag>(std::get<0>(noiseDensityContrastCombos[i]));
        cosmosRing->Setter<CosmosRingNoiseContrastTag>(std::get<1>(noiseDensityContrastCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the progress range boundaries
 * An out-of-range phase must collapse onto the nearest end
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Progress_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < progressBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingProgressTag>(progressBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the radius range boundaries
 * An out-of-range radius must match the nearest boundary
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Radius_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < positivePixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingRadiusTag>(positivePixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the width range boundaries
 * A zero width must be invisible, an over-range width must clamp
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Width_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < positivePixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingWidthTag>(positivePixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the feather range boundaries
 * A negative feather must clamp to a hard edge
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Feather_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < pixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingFeatherTag>(pixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the noise density range boundaries
 * The upper bound is one hundred, beyond it must clamp
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoiseDensity_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < percentBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseDensityTag>(percentBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the noise period range boundaries
 * The lower bound is one, zero would divide by zero
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoisePeriod_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noisePeriodBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoisePeriodTag>(noisePeriodBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the noise contrast range boundaries
 * The upper bound is ten
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoiseContrast_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < contrastBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseContrastTag>(contrastBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the noise seed range boundaries
 * The seed range is zero to ten thousand
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoiseSeed_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < noiseSeedBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseSeedTag>(noiseSeedBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the radius with extreme values
 * Extreme inputs must clamp instead of breaking the frame
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Radius_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingRadiusTag>(extremeFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the width with extreme values
 * Extreme inputs must not paint outside the node
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Width_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingWidthTag>(extremeFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the feather with extreme values
 * Extreme feather must not turn the whole cell translucent
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Feather_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingFeatherTag>(extremeFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the noise contrast with extreme values
 * Extreme contrast must not produce pure noise
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_NoiseContrast_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingNoiseContrastTag>(extremeFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the progress with invalid values
 * Invalid inputs must not crash or leak into neighbour cells
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Progress_Invalid_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < invalidFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingProgressTag>(invalidFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the radius with invalid values
 * Invalid inputs must not crash or leak into neighbour cells
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Radius_Invalid_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < invalidFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingRadiusTag>(invalidFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

// ---- Requirement 2: chroma dispersion: cases ----

/*
 * Test the chroma dispersion strength
 * Zero disables the spectrum, high values spread it widely
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_ChromaStrength_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < chromaStrengthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingChromaStrengthTag>(chromaStrengthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Chroma strength and mid colour combinations
 * Dispersion must rotate luminance instead of replacing the colour
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Chroma_Color_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < chromaColorCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingChromaStrengthTag>(std::get<0>(chromaColorCombos[i]));
        cosmosRing->Setter<CosmosRingMidColorTag>(std::get<1>(chromaColorCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Chroma strength and bright colour combinations
 * The dispersion colour must follow the brightest tone
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Chroma_Strength_BrightColor_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < chromaBrightColorCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingChromaStrengthTag>(std::get<0>(chromaBrightColorCombos[i]));
        cosmosRing->Setter<CosmosRingBrightColorTag>(std::get<1>(chromaBrightColorCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the chroma strength range boundaries
 * The upper bound is ten
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_ChromaStrength_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < contrastBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingChromaStrengthTag>(contrastBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

// ---- Requirement 3: highlight: cases ----

/*
 * Test the highlight switch
 * Disabling the highlight must not affect the other layers
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_Enable_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlEnableValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLEnableTag>(hlEnableValues[i]);

        SetUpCell(i, 1, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight outer radius
 * The outer contour is independent of the base ring geometry
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_OuterRadius_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlOuterRadiusValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLOuterRadiusTag>(hlOuterRadiusValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight inner radius
 * An inner radius above the outer radius is expected to degenerate
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_InnerRadius_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlInnerRadiusValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLInnerRadiusTag>(hlInnerRadiusValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight outer width
 * Zero width hides the segment, large width smears it
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_OuterWidth_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlOuterWidthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLOuterWidthTag>(hlOuterWidthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight inner width
 * The inner contour is easier to cover by the outer one
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_InnerWidth_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlInnerWidthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLInnerWidthTag>(hlInnerWidthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight dispersion strength
 * The dispersion must stay on the contour, not the base ring
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_DispersionStrength_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlDispersionStrengthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLDispersionStrengthTag>(hlDispersionStrengthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight rotation
 * All four arcs must rotate together around the ring
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_RotationAngle_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlRotationAngleValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLRotationAngleTag>(hlRotationAngleValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight radial feather
 * Zero keeps a hard edge
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_RadialFeatherPx_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlRadialFeatherValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLRadialFeatherPxTag>(hlRadialFeatherValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight tangent feather
 * Large values fade both ends of every arc
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_TangentFeatherDeg_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlTangentFeatherValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLTangentFeatherDegTag>(hlTangentFeatherValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight angular fade
 * The ends of each arc must fade out
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_AngularFade_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlAngularFadeValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLAngularFadeTag>(hlAngularFadeValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 outer centre angle
 * The arc must follow the requested angle
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Outer_CenterAngle_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < centerAngleValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterCenterAngleTag>(centerAngleValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 inner centre angle
 * The inner arc must follow the requested angle
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Inner_CenterAngle_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < centerAngleValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1InnerCenterAngleTag>(centerAngleValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 outer centre angle
 * The second pair must use the same angle convention
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Outer_CenterAngle_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < centerAngleValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2OuterCenterAngleTag>(centerAngleValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 inner centre angle
 * The second inner arc must follow the requested angle
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Inner_CenterAngle_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < centerAngleValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2InnerCenterAngleTag>(centerAngleValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 outer arc span
 * Zero hides the arc, three hundred sixty closes the loop
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Outer_ArcSpan_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcSpanValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterArcSpanTag>(arcSpanValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 inner arc span
 * Zero hides the arc, three hundred sixty closes the loop
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Inner_ArcSpan_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcSpanValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1InnerArcSpanTag>(arcSpanValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 outer arc span
 * Zero hides the arc, three hundred sixty closes the loop
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Outer_ArcSpan_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcSpanValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2OuterArcSpanTag>(arcSpanValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 inner arc span
 * Zero hides the arc, three hundred sixty closes the loop
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Inner_ArcSpan_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcSpanValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2InnerArcSpanTag>(arcSpanValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 outer brightness
 * Zero hides the arc, values above one may over expose
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Outer_Brightness_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcBrightnessValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterBrightnessTag>(arcBrightnessValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 inner brightness
 * Zero hides the arc, values above one may over expose
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Inner_Brightness_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcBrightnessValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1InnerBrightnessTag>(arcBrightnessValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 outer brightness
 * Zero hides the arc, values above one may over expose
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Outer_Brightness_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcBrightnessValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2OuterBrightnessTag>(arcBrightnessValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 inner brightness
 * Zero hides the arc, values above one may over expose
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Inner_Brightness_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < arcBrightnessValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2InnerBrightnessTag>(arcBrightnessValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 outer dispersion scales
 * The two components must take effect independently
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Outer_DispScales_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < dispScalesValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterDispScalesTag>(dispScalesValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P1 inner dispersion scales
 * The two components must take effect independently
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1Inner_DispScales_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < dispScalesValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1InnerDispScalesTag>(dispScalesValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 outer dispersion scales
 * The two components must take effect independently
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Outer_DispScales_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < dispScalesValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2OuterDispScalesTag>(dispScalesValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the P2 inner dispersion scales
 * The two components must take effect independently
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P2Inner_DispScales_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < dispScalesValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP2InnerDispScalesTag>(dispScalesValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Highlight outer and inner radius combinations
 * Cover inner smaller than, equal to and larger than outer
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_OuterInner_Radius_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlOuterInnerRadiusCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLOuterRadiusTag>(std::get<0>(hlOuterInnerRadiusCombos[i]));
        cosmosRing->Setter<CosmosRingHLInnerRadiusTag>(std::get<1>(hlOuterInnerRadiusCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Highlight outer and inner width combinations
 * A zero width segment must vanish without dragging the other
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_OuterInner_Width_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlOuterInnerWidthCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLOuterWidthTag>(std::get<0>(hlOuterInnerWidthCombos[i]));
        cosmosRing->Setter<CosmosRingHLInnerWidthTag>(std::get<1>(hlOuterInnerWidthCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Radial and tangent feather combinations
 * Blur the edge radially and taper the arc ends at the same time
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_Radial_Tangent_Feather_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlRadialTangentCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLRadialFeatherPxTag>(std::get<0>(hlRadialTangentCombos[i]));
        cosmosRing->Setter<CosmosRingHLTangentFeatherDegTag>(std::get<1>(hlRadialTangentCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Angular fade and rotation combinations
 * The fade must rotate with the highlight, not stay screen aligned
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_AngularFade_Rotation_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlAngularFadeRotationCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLAngularFadeTag>(std::get<0>(hlAngularFadeRotationCombos[i]));
        cosmosRing->Setter<CosmosRingHLRotationAngleTag>(std::get<1>(hlAngularFadeRotationCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Rotation and centre angle combinations
 * Rotation must add to the centre angle instead of replacing it
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_Rotation_CenterAngle_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlRotationCenterAngleCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLRotationAngleTag>(std::get<0>(hlRotationCenterAngleCombos[i]));
        cosmosRing->Setter<CosmosRingHLP1OuterCenterAngleTag>(std::get<1>(hlRotationCenterAngleCombos[i]));
        cosmosRing->Setter<CosmosRingHLP1InnerCenterAngleTag>(std::get<1>(hlRotationCenterAngleCombos[i]));
        cosmosRing->Setter<CosmosRingHLP2OuterCenterAngleTag>(std::get<1>(hlRotationCenterAngleCombos[i]));
        cosmosRing->Setter<CosmosRingHLP2InnerCenterAngleTag>(std::get<1>(hlRotationCenterAngleCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Four arc span combinations
 * Switch on one arc at a time to locate a missing segment
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_FourArc_Span_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlArcSpanPatterns.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterArcSpanTag>(hlArcSpanPatterns[i].p1Outer);
        cosmosRing->Setter<CosmosRingHLP1InnerArcSpanTag>(hlArcSpanPatterns[i].p1Inner);
        cosmosRing->Setter<CosmosRingHLP2OuterArcSpanTag>(hlArcSpanPatterns[i].p2Outer);
        cosmosRing->Setter<CosmosRingHLP2InnerArcSpanTag>(hlArcSpanPatterns[i].p2Inner);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Four arc brightness combinations
 * Brightness must be independent per segment
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_FourArc_Brightness_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlArcBrightnessPatterns.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterBrightnessTag>(hlArcBrightnessPatterns[i].p1Outer);
        cosmosRing->Setter<CosmosRingHLP1InnerBrightnessTag>(hlArcBrightnessPatterns[i].p1Inner);
        cosmosRing->Setter<CosmosRingHLP2OuterBrightnessTag>(hlArcBrightnessPatterns[i].p2Outer);
        cosmosRing->Setter<CosmosRingHLP2InnerBrightnessTag>(hlArcBrightnessPatterns[i].p2Inner);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * First and second pair brightness combinations
 * The two pairs must not share a single gain
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_P1P2_Brightness_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlP1P2BrightnessCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterBrightnessTag>(std::get<0>(hlP1P2BrightnessCombos[i]));
        cosmosRing->Setter<CosmosRingHLP1InnerBrightnessTag>(std::get<0>(hlP1P2BrightnessCombos[i]));
        cosmosRing->Setter<CosmosRingHLP2OuterBrightnessTag>(std::get<1>(hlP1P2BrightnessCombos[i]));
        cosmosRing->Setter<CosmosRingHLP2InnerBrightnessTag>(std::get<1>(hlP1P2BrightnessCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Dispersion scales and strength combinations
 * The scales only matter once the dispersion strength is non zero
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_DispScales_DispersionStrength_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < hlDispScalesStrengthCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLDispersionStrengthTag>(std::get<1>(hlDispScalesStrengthCombos[i]));
        cosmosRing->Setter<CosmosRingHLP1OuterDispScalesTag>(std::get<0>(hlDispScalesStrengthCombos[i]));
        cosmosRing->Setter<CosmosRingHLP1InnerDispScalesTag>(std::get<0>(hlDispScalesStrengthCombos[i]));
        cosmosRing->Setter<CosmosRingHLP2OuterDispScalesTag>(std::get<0>(hlDispScalesStrengthCombos[i]));
        cosmosRing->Setter<CosmosRingHLP2InnerDispScalesTag>(std::get<0>(hlDispScalesStrengthCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight outer radius boundaries
 * An out-of-range radius must match the nearest boundary
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_OuterRadius_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < pixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLOuterRadiusTag>(pixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight inner radius boundaries
 * A zero inner radius must hide the inner contour
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_InnerRadius_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < pixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLInnerRadiusTag>(pixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight outer width boundaries
 * A zero width must be invisible
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_OuterWidth_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < pixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLOuterWidthTag>(pixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight inner width boundaries
 * A zero width must be invisible
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_InnerWidth_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < pixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLInnerWidthTag>(pixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight dispersion strength boundaries
 * The upper bound is ten
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_DispersionStrength_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < contrastBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLDispersionStrengthTag>(contrastBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight rotation boundaries
 * Three hundred sixty must equal zero
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_RotationAngle_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < angleBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLRotationAngleTag>(angleBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight tangent feather boundaries
 * The upper bound is three hundred sixty degrees
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_TangentFeather_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < angleBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLTangentFeatherDegTag>(angleBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight dispersion scales with extreme values
 * Extreme inputs must not paint the whole cell white
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_DispScales_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeVector2fValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterDispScalesTag>(extremeVector2fValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the highlight dispersion scales with invalid values
 * Invalid inputs must not crash or leak into neighbour cells
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_HL_DispScales_Invalid_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < invalidVector2fValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingHLP1OuterDispScalesTag>(invalidVector2fValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

// ---- Requirement 4: particles: cases ----

/*
 * Test the particle count
 * Few particles, the baseline and the upper range
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Count_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleCountValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleCountTag>(particleCountValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle size
 * From sub pixel dots to clumps that touch each other
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Size_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleSizeValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleSizeTag>(particleSizeValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle dispersion strength
 * Zero keeps every particle on the same orbit
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_DispersionStrength_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleDispersionValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleDispersionStrengthTag>(particleDispersionValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle scale
 * The scale changes the whole band, the size only one particle
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Scale_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleScaleValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleScaleTag>(particleScaleValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle band inner radius
 * The band must follow the base ring geometry
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_InnerRadius_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleInnerRadiusValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleInnerRadiusTag>(particleInnerRadiusValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle band outer radius
 * An inner radius above the outer is expected to degenerate
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_OuterRadius_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleOuterRadiusValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleOuterRadiusTag>(particleOuterRadiusValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle path length
 * Ten degrees groups the particles, three hundred sixty wraps the ring
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_PathLength_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particlePathLengthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticlePathLengthTag>(particlePathLengthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle base colour
 * The particle colour must be independent of the ring mid colour
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_MidColor_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleMidColorValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleMidColorTag>(particleMidColorValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle tint strength
 * Zero keeps the particle colour, one takes the ring colour
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_TintStrength_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleTintStrengthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleTintStrengthTag>(particleTintStrengthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle brightness
 * Zero hides the particles, high values also feed the glow
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Brightness_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleBrightnessValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleBrightnessTag>(particleBrightnessValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle birth peak
 * The peak position changes the size and brightness distribution
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_BirthPeak_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleBirthPeakValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleBirthPeakTag>(particleBirthPeakValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle opacity
 * Zero is fully transparent, one is fully opaque
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Opacity_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleOpacityValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleOpacityTag>(particleOpacityValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle count and size combinations
 * Many small particles against few large ones
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Count_Size_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleCountSizeCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleCountTag>(std::get<0>(particleCountSizeCombos[i]));
        cosmosRing->Setter<CosmosRingParticleSizeTag>(std::get<1>(particleCountSizeCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle radius range combinations
 * The band widens, and degenerates when the inner radius passes the outer
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Radius_Range_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleRadiusRangeCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleInnerRadiusTag>(std::get<0>(particleRadiusRangeCombos[i]));
        cosmosRing->Setter<CosmosRingParticleOuterRadiusTag>(std::get<1>(particleRadiusRangeCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle dispersion and path combinations
 * Spread and angular range shape the visible arc together
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Dispersion_Path_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleDispersionPathCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleDispersionStrengthTag>(std::get<0>(particleDispersionPathCombos[i]));
        cosmosRing->Setter<CosmosRingParticlePathLengthTag>(std::get<1>(particleDispersionPathCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle brightness and birth peak combinations
 * The peak position changes the brightness distribution on screen
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Brightness_BirthPeak_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleBrightnessBirthPeakCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleBrightnessTag>(std::get<0>(particleBrightnessBirthPeakCombos[i]));
        cosmosRing->Setter<CosmosRingParticleBirthPeakTag>(std::get<1>(particleBrightnessBirthPeakCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle tint and colour combinations
 * Full tint must equal the ring colour, zero tint the particle colour
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_TintStrength_MidColor_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleTintColorCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleTintStrengthTag>(std::get<0>(particleTintColorCombos[i]));
        cosmosRing->Setter<CosmosRingParticleMidColorTag>(std::get<1>(particleTintColorCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle scale and opacity combinations
 * A shrunken translucent layer must fade without vanishing
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Scale_Opacity_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleScaleOpacityCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleScaleTag>(std::get<0>(particleScaleOpacityCombos[i]));
        cosmosRing->Setter<CosmosRingParticleOpacityTag>(std::get<1>(particleScaleOpacityCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle count and opacity combinations
 * Sparse translucent against dense opaque should look comparable
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Count_Opacity_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleCountOpacityCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleCountTag>(std::get<0>(particleCountOpacityCombos[i]));
        cosmosRing->Setter<CosmosRingParticleOpacityTag>(std::get<1>(particleCountOpacityCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle size and inner radius combinations
 * Large particles at a large inner radius may be clipped by the band
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Size_InnerRadius_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleSizeInnerRadiusCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleSizeTag>(std::get<0>(particleSizeInnerRadiusCombos[i]));
        cosmosRing->Setter<CosmosRingParticleInnerRadiusTag>(std::get<1>(particleSizeInnerRadiusCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Particle size and dispersion combinations
 * Large particles with a strong spread can merge into clumps
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Size_Dispersion_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < particleSizeDispersionCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleSizeTag>(std::get<0>(particleSizeDispersionCombos[i]));
        cosmosRing->Setter<CosmosRingParticleDispersionStrengthTag>(std::get<1>(particleSizeDispersionCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle count boundaries
 * The upper bound is one thousand, zero hides the particles
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Count_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < countBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleCountTag>(countBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle size boundaries
 * A zero size must not draw anything
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Size_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < pixelBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleSizeTag>(pixelBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle path length boundaries
 * Zero groups the particles, three hundred sixty wraps the ring
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_PathLength_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < angleBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticlePathLengthTag>(angleBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle opacity boundaries
 * Out-of-range opacity must clamp to zero or one
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Opacity_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < unitBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleOpacityTag>(unitBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle count with extreme values
 * Extreme counts must not freeze the render or paint the cell white
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Count_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleCountTag>(extremeFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle size with extreme values
 * Extreme sizes must not paint the whole cell white
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Size_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleSizeTag>(extremeFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the particle count with invalid values
 * Invalid inputs must not crash or leak into neighbour cells
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Particle_Count_Invalid_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < invalidFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingParticleCountTag>(invalidFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

// ---- Requirement 5: glow: cases ----

/*
 * Test the glow radius
 * The two components are near and far field radii
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Radius_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowRadiusValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowRadiusTag>(glowRadiusValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow strength
 * Zero disables the glow, high values saturate
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Strength_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowStrengthValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowStrengthTag>(glowStrengthValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow luminance threshold
 * Negative thresholds also light the dark areas
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Threshold_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowThresholdValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowThresholdTag>(glowThresholdValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow intensity
 * The total gain, independent of the per field strength
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Intensity_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowIntensityValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowIntensityTag>(glowIntensityValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow saturation
 * Zero turns the glow grey white, one keeps the source colour
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Saturation_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowSaturationValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowSaturationTag>(glowSaturationValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow exposure
 * The core parameter of the high dynamic range tail
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Exposure_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowExposureValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowExposureTag>(glowExposureValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Glow radius and strength combinations
 * A wide weak glow against a narrow strong one
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Radius_Strength_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowRadiusStrengthCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowRadiusTag>(std::get<0>(glowRadiusStrengthCombos[i]));
        cosmosRing->Setter<CosmosRingGlowStrengthTag>(std::get<1>(glowRadiusStrengthCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Glow threshold and intensity combinations
 * The threshold selects the area, the intensity sets the brightness
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Threshold_Intensity_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowThresholdIntensityCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowThresholdTag>(std::get<0>(glowThresholdIntensityCombos[i]));
        cosmosRing->Setter<CosmosRingGlowIntensityTag>(std::get<1>(glowThresholdIntensityCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Glow intensity and saturation combinations
 * High gain with zero saturation must turn the glow grey
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Intensity_Saturation_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowIntensitySaturationCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowIntensityTag>(std::get<0>(glowIntensitySaturationCombos[i]));
        cosmosRing->Setter<CosmosRingGlowSaturationTag>(std::get<1>(glowIntensitySaturationCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Glow saturation and exposure combinations
 * Saturation after the exposure pushes the highlights to white
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Saturation_Exposure_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowSaturationExposureCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowSaturationTag>(std::get<0>(glowSaturationExposureCombos[i]));
        cosmosRing->Setter<CosmosRingGlowExposureTag>(std::get<1>(glowSaturationExposureCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Glow radius and threshold combinations
 * Both decide how far the halo reaches
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Radius_Threshold_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowRadiusThresholdCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowRadiusTag>(std::get<0>(glowRadiusThresholdCombos[i]));
        cosmosRing->Setter<CosmosRingGlowThresholdTag>(std::get<1>(glowRadiusThresholdCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Glow exposure and intensity combinations
 * Exposure and gain together decide whether the highlights saturate
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Exposure_Intensity_Combination_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < glowExposureIntensityCombos.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowExposureTag>(std::get<0>(glowExposureIntensityCombos[i]));
        cosmosRing->Setter<CosmosRingGlowIntensityTag>(std::get<1>(glowExposureIntensityCombos[i]));

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow radius boundaries
 * Both components share the same range
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Radius_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < pixelBoundaryVector2fValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowRadiusTag>(pixelBoundaryVector2fValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow strength boundaries
 * Both components share the same range
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Strength_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < unitBoundaryVector2fValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowStrengthTag>(unitBoundaryVector2fValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow intensity boundaries
 * The range is zero to one
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Intensity_Boundary_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < unitBoundaryValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowIntensityTag>(unitBoundaryValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow radius with extreme values
 * Extreme radii must not paint the whole cell white
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Radius_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeVector2fValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowRadiusTag>(extremeVector2fValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow exposure with extreme values
 * Extreme exposure must clamp to the range
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Exposure_Extreme_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeFloatValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowExposureTag>(extremeFloatValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Test the glow strength with invalid values
 * Invalid inputs must not crash or leak into neighbour cells
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Glow_Strength_Invalid_Values_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < invalidVector2fValues.size(); i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        cosmosRing->Setter<CosmosRingGlowStrengthTag>(invalidVector2fValues[i]);

        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

// ---- Layer comparison: cases ----

/*
 * Compare the layers one by one
 * The first cell is the base ring and each next cell adds one layer
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Layer_Base_Comparison_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < 4; i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        if (i < 3) {
            DisableCosmosRingParticles(cosmosRing);
        }
        if (i < 2) {
            DisableCosmosRingHighlight(cosmosRing);
        }
        if (i < 1) {
            DisableCosmosRingChroma(cosmosRing);
        }
        DisableCosmosRingGlow(cosmosRing);
        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

/*
 * Compare the glow layer and the single layer switches
 * The second cell keeps every layer enabled
 */
GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Cosmos_Ring_Layer_Glow_Comparison_Test)
{
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < 4; i++) {
        auto cosmosRing = std::make_shared<RSNGCosmosRing>();
        InitCosmosRing(cosmosRing);
        switch (i) {
            case 0:
                DisableCosmosRingChroma(cosmosRing);
                DisableCosmosRingHighlight(cosmosRing);
                DisableCosmosRingParticles(cosmosRing);
                break;
            case 2:
                DisableCosmosRingHighlight(cosmosRing);
                break;
            case 3:
                DisableCosmosRingParticles(cosmosRing);
                break;
            default:
                break;
        }
        SetUpCell(i, 2, 2, effectNode, cosmosRing);
    }
}

} // namespace OHOS::Rosen
