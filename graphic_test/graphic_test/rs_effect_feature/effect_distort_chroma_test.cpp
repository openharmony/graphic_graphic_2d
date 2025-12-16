/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

class DistortChromaTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

struct DistortChromaInfo {
    Vector3f colorStrength = Vector3f(0.0f, 0.0f, 0.0f);
    float brightness = 0.0f;
    float dispersion = 0.0f;
    float saturation = 0.0f;
    Vector2f granularity = Vector2f(0.0f, 0.0f);
    float frequency = 0.0f;
    float sharpness = 0.0f;
    Vector2f distortFactor = Vector2f(0.0f, 0.0f);
    float moveDistance = 0.0f;
    float progress = 0.0f;
};

struct DupoliNoiseMaskInfo
{
    float granularity = 0.0f;
    float progress = 0.0f;
    float moveDistance =0.0f;
};

struct RippleMaskInfo
{
    Vector2f center = Vector2f(0.0f, 0.0f);
    float radius = 0.0f;
    float width = 0.0f;
    float centerOffset = 0.0f;
};

std::vector<DistortChromaInfo> DistortChromaParams = {
    // Strength brightness dispersion saturation granularity frequency sharpness distortFactor moveDistance progress
    {{0.3f, 0.5f, 0.3f}, 1.0f, 0.2f, 0.2f, {10.1f, 10.1f}, 2.1f, 1.0f, {0.1f, 0.1f}, 0.0f, 0.0f},
    {{0.2f, 0.2f, 0.2f}, 5.0f, 1.0f, 2.0f, {2.1f, 2.1f}, 1.0f, 1.5f, {0.2f, 0.3f},1.0f, 1.0f},
    {{1.0f, 1.3f, 1.3f}, 1.0f, 1.0f, 0.3f, {3.1f, 3.1f}, 0.8f, 1.0f, {0.3f, 0.4f},-1.0f, -1.0f},
    {{2.0f, 2.0f, 2.0f}, 3.0f, 2.0f, 3.0f, {4.1f,4.1f}, 2.0f, 2.0f, {0.5f, 0.6f},2.0f, 2.0f},
    {{0.5f, 0.5f, 0.5f}, 10.0f, 0.5f, 1.0f, {5.1f, 5.1f}, 0.5f, 0.5f, {-0.1f, -0.1f},0.5f, 0.5f},
    {{0.6f, 0.2f, 0.2f}, 1.2f, 0.2f, 0.2f, {6.1f, 6.1f}, 0.2f, 0.2f, {-0.2f, 0.3f},0.2f, 0.2f},
};

std::vector<DupoliNoiseMaskInfo> DupoliNoiseMaskParams = {
    {5.0f, -1.0f, 1.5f},
    {-2.0f, 2.0f, 2.7f},
    {20.5f, 3.0f, 1.0f},
    {10.0f, 0.4f, 5.0f},
    {3.0f, 5.0f, 2.0f},
    {4.0f, 1.0f, 1.0f},
};

std::vector<RippleMaskInfo> RippleMaskParams = {
    {{0.5f, 0.8f},0.5f,0.1f,0.0f},
    {{0.5f, 0.4f},0.8f,0.2f,0.2f},
    {{0.2f, 1.4f},0.7f,0.3f,0.0f},
    {{0.3f, 0.1f},1.5f,0.5f,0.1f},
    {{0.8f, 1.0f},0.2f,0.6f,0.3f},
    {{0.8f, 1.0f},0.5f,0.3f,0.0f},
};

void SetParameter(std::shared_ptr<RSNGDistortChroma> distortChroma, int index)
{
    auto mask = std::make_shared<RSNGDupoliNoiseMask>();
    mask->Setter<DupoliNoiseMaskGranularityTag>(DupoliNoiseMaskParams[index].granularity);
    mask->Setter<DupoliNoiseMaskProgressTag>(DupoliNoiseMaskParams[index].progress);
    mask->Setter<DupoliNoiseMaskVerticalMoveDistanceTag>(DupoliNoiseMaskParams[index].moveDistance);

    std::shared_ptr<RSNGMaskBase> noiseMask = std::static_pointer_cast<RSNGMaskBase>(mask);
    distortChroma->Setter<DistortChromaTurbulentMaskTag>(noiseMask);

    distortChroma->Setter<DistortChromaGranularityTag>(DistortChromaParams[index].granularity);
    distortChroma->Setter<DistortChromaFrequencyTag>(DistortChromaParams[index].frequency);
    distortChroma->Setter<DistortChromaSharpnessTag>(DistortChromaParams[index].sharpness);
    distortChroma->Setter<DistortChromaBrightnessTag>(DistortChromaParams[index].brightness);
    distortChroma->Setter<DistortChromaDispersionTag>(DistortChromaParams[index].dispersion);
    distortChroma->Setter<DistortChromaDistortFactorTag>(DistortChromaParams[index].distortFactor);
    distortChroma->Setter<DistortChromaSaturationTag>(DistortChromaParams[index].saturation);
    distortChroma->Setter<DistortChromaColorStrengthTag>(DistortChromaParams[index].colorStrength);
    distortChroma->Setter<DistortChromaVerticalMoveDistanceTag>(DistortChromaParams[index].moveDistance);
    distortChroma->Setter<DistortChromaProgressTag>(DistortChromaParams[index].progress);
}

void SetDistortChromaRippleMask(std::shared_ptr<RSNGDistortChroma> distortChroma, int index){

    auto mask = std::make_shared<RSNGRippleMask>();
    mask->Setter<RippleMaskCenterTag>(RippleMaskParams[index].center);
    mask->Setter<RippleMaskRadiusTag>(RippleMaskParams[index].radius);
    mask->Setter<RippleMaskWidthTag>(RippleMaskParams[index].width);
    mask->Setter<RippleMaskOffsetTag>(RippleMaskParams[index].centerOffset);
    std::shared_ptr<RSNGMaskBase> alphaMask = std::static_pointer_cast<RSNGMaskBase>(mask);
    distortChroma->Setter<DistortChromaMaskTag>(alphaMask);
}

GRAPHIC_TEST(DistortChromaTest, EFFECT_TEST, Set_DistortChroma_DupoliNoiseMask_Test)
{
    int columnCount = 2;
    int rowCount = DistortChromaParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t index = 0; index < DistortChromaParams.size(); ++index) {
        auto distortChroma = std::make_shared<RSNGDistortChroma>();
        SetParameter(distortChroma, index);
        int x = (index % columnCount) * sizeX;
        int y = (index / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetForegroundShader(distortChroma);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(DistortChromaTest, EFFECT_TEST, Set_DistortChroma_RippleMask_Test)
{
    int columnCount = 2;
    int rowCount = DistortChromaParams.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t index = 0; index < DistortChromaParams.size(); ++index) {
        auto distortChroma = std::make_shared<RSNGDistortChroma>();
        SetParameter(distortChroma, index);
        SetDistortChromaRippleMask(distortChroma, index);
        int x = (index % columnCount) * sizeX;
        int y = (index / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundTestNode->SetForegroundShader(distortChroma);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

} // namespace

} // namespace OHOS::Rosen