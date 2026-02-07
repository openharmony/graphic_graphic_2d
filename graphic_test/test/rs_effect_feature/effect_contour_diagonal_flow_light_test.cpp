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
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;

struct ContourDiagonalFlowLightParams {
    std::vector<Vector2f> contour;
    float line1Start;
    float line1Length;
    Vector4f line1Color;
    float line2Start;
    float line2Length;
    Vector4f line2Color;
    float thickness;
    float haloRadius;
    float lightWeight;
    float haloWeight;
};

std::vector<ContourDiagonalFlowLightParams> contourDiagonalFlowLightParams = {
    // Test basic parameters
    {
        .contour = {{0.1f, 0.1f}, {0.9f, 0.1f}, {0.9f, 0.9f}, {0.1f, 0.9f}},
        .line1Start = 0.0f,
        .line1Length = 0.5f,
        .line1Color = {1.0f, 0.0f, 0.0f, 1.0f},
        .line2Start = 0.5f,
        .line2Length = 0.5f,
        .line2Color = {0.0f, 0.0f, 1.0f, 1.0f},
        .thickness = 0.02f,
        .haloRadius = 0.1f,
        .lightWeight = 0.8f,
        .haloWeight = 0.5f
    },
    // Test different colors
    {
        .contour = {{0.2f, 0.2f}, {0.8f, 0.2f}, {0.8f, 0.8f}, {0.2f, 0.8f}},
        .line1Start = 0.0f,
        .line1Length = 0.3f,
        .line1Color = {0.0f, 1.0f, 0.0f, 1.0f},
        .line2Start = 0.3f,
        .line2Length = 0.7f,
        .line2Color = {1.0f, 1.0f, 0.0f, 1.0f},
        .thickness = 0.03f,
        .haloRadius = 0.15f,
        .lightWeight = 0.6f,
        .haloWeight = 0.4f
    },
    // Test different thickness and halo
    {
        .contour = {{0.15f, 0.15f}, {0.85f, 0.15f}, {0.85f, 0.85f}, {0.15f, 0.85f}},
        .line1Start = 0.1f,
        .line1Length = 0.4f,
        .line1Color = {1.0f, 0.5f, 0.0f, 1.0f},
        .line2Start = 0.4f,
        .line2Length = 0.6f,
        .line2Color = {0.5f, 0.0f, 1.0f, 1.0f},
        .thickness = 0.05f,
        .haloRadius = 0.2f,
        .lightWeight = 0.9f,
        .haloWeight = 0.7f
    },
    // Test different weights
    {
        .contour = {{0.1f, 0.1f}, {0.9f, 0.1f}, {0.9f, 0.9f}, {0.1f, 0.9f}},
        .line1Start = 0.0f,
        .line1Length = 0.5f,
        .line1Color = {0.0f, 1.0f, 1.0f, 1.0f},
        .line2Start = 0.5f,
        .line2Length = 0.5f,
        .line2Color = {1.0f, 0.0f, 1.0f, 1.0f},
        .thickness = 0.02f,
        .haloRadius = 0.1f,
        .lightWeight = 0.3f,
        .haloWeight = 0.9f
    },
    // Test invalid/negative values
    {
        .contour = {{0.1f, 0.1f}, {0.9f, 0.1f}, {0.9f, 0.9f}, {0.1f, 0.9f}},
        .line1Start = -0.1f,
        .line1Length = 1.5f,
        .line1Color = {-0.1f, 0.0f, 0.0f, 1.0f},
        .line2Start = -0.2f,
        .line2Length = 2.0f,
        .line2Color = {0.0f, -0.1f, 0.0f, 1.0f},
        .thickness = -0.01f,
        .haloRadius = -0.1f,
        .lightWeight = -0.5f,
        .haloWeight = -0.3f
    },
    // Test different contour shapes
    {
        .contour = {{0.3f, 0.3f}, {0.7f, 0.3f}, {0.7f, 0.7f}, {0.3f, 0.7f}},
        .line1Start = 0.0f,
        .line1Length = 0.5f,
        .line1Color = {1.0f, 1.0f, 1.0f, 1.0f},
        .line2Start = 0.5f,
        .line2Length = 0.5f,
        .line2Color = {0.5f, 0.5f, 0.5f, 1.0f},
        .thickness = 0.04f,
        .haloRadius = 0.12f,
        .lightWeight = 0.7f,
        .haloWeight = 0.6f
    }
};

} // namespace

class ContourDiagonalFlowLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
};

void SetContourDiagonalFlowLightParams(const std::shared_ptr<RSNGContourDiagonalFlowLight>& shader,
    const ContourDiagonalFlowLightParams& params)
{
    if (!shader) {
        return;
    }
    shader->Setter<ContourDiagonalFlowLightContourTag>(params.contour);
    shader->Setter<ContourDiagonalFlowLightLine1StartTag>(params.line1Start);
    shader->Setter<ContourDiagonalFlowLightLine1LengthTag>(params.line1Length);
    shader->Setter<ContourDiagonalFlowLightLine1ColorTag>(params.line1Color);
    shader->Setter<ContourDiagonalFlowLightLine2StartTag>(params.line2Start);
    shader->Setter<ContourDiagonalFlowLightLine2LengthTag>(params.line2Length);
    shader->Setter<ContourDiagonalFlowLightLine2ColorTag>(params.line2Color);
    shader->Setter<ContourDiagonalFlowLightThicknessTag>(params.thickness);
    shader->Setter<ContourDiagonalFlowLightHaloRadiusTag>(params.haloRadius);
    shader->Setter<ContourDiagonalFlowLightLightWeightTag>(params.lightWeight);
    shader->Setter<ContourDiagonalFlowLightHaloWeightTag>(params.haloWeight);
}

GRAPHIC_TEST(ContourDiagonalFlowLightTest, EFFECT_TEST, Set_Contour_Diagonal_Flow_Light_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = contourDiagonalFlowLightParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < contourDiagonalFlowLightParams.size(); ++i) {
        auto shader = std::make_shared<RSNGContourDiagonalFlowLight>();
        SetContourDiagonalFlowLightParams(shader, contourDiagonalFlowLightParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto node = RSCanvasNode::Create();
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000000);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(ContourDiagonalFlowLightTest, EFFECT_TEST, Set_Contour_Diagonal_Flow_Light_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = contourDiagonalFlowLightParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < contourDiagonalFlowLightParams.size(); ++i) {
        auto shader = std::make_shared<RSNGContourDiagonalFlowLight>();
        SetContourDiagonalFlowLightParams(shader, contourDiagonalFlowLightParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {x, y, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

} // namespace OHOS::Rosen
