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

#include <algorithm>
#include <unistd.h>

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100 ms

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

const std::vector<ContourDiagonalFlowLightParams> contourDiagonalFlowLightParams = {
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
    shader->Setter<ContourDiagonalFlowLightLine1StartTag>(std::clamp(params.line1Start, 0.0f, 1.0f));
    shader->Setter<ContourDiagonalFlowLightLine1LengthTag>(std::clamp(params.line1Length, 0.1f, 1.0f));
    shader->Setter<ContourDiagonalFlowLightLine1ColorTag>(Vector4f {
        std::clamp(params.line1Color.x_, 0.0f, 1.0f),
        std::clamp(params.line1Color.y_, 0.0f, 1.0f),
        std::clamp(params.line1Color.z_, 0.0f, 1.0f),
        std::clamp(params.line1Color.w_, 0.0f, 1.0f) });
    shader->Setter<ContourDiagonalFlowLightLine2StartTag>(std::clamp(params.line2Start, 0.0f, 1.0f));
    shader->Setter<ContourDiagonalFlowLightLine2LengthTag>(std::clamp(params.line2Length, 0.1f, 1.0f));
    shader->Setter<ContourDiagonalFlowLightLine2ColorTag>(Vector4f {
        std::clamp(params.line2Color.x_, 0.0f, 1.0f),
        std::clamp(params.line2Color.y_, 0.0f, 1.0f),
        std::clamp(params.line2Color.z_, 0.0f, 1.0f),
        std::clamp(params.line2Color.w_, 0.0f, 1.0f) });
    shader->Setter<ContourDiagonalFlowLightThicknessTag>(std::clamp(params.thickness, 0.01f, 0.08f));
    shader->Setter<ContourDiagonalFlowLightHaloRadiusTag>(std::clamp(params.haloRadius, 0.05f, 0.2f));
    shader->Setter<ContourDiagonalFlowLightLightWeightTag>(std::clamp(params.lightWeight, 0.2f, 0.8f));
    shader->Setter<ContourDiagonalFlowLightHaloWeightTag>(std::clamp(params.haloWeight, 0.2f, 0.7f));
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
        node->SetBackgroundColor(0xff1a1a1a);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
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
        shader->Setter<ContourDiagonalFlowLightLightWeightTag>(0.5f);
        shader->Setter<ContourDiagonalFlowLightHaloWeightTag>(0.35f);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto containerNode = RSCanvasNode::Create();
        containerNode->SetBounds({x, y, sizeX, sizeY});
        containerNode->SetFrame({x, y, sizeX, sizeY});
        containerNode->SetBackgroundColor(0xff182028);

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
        backgroundNode->SetBounds({0, 0, sizeX, sizeY});
        backgroundNode->SetFrame({0, 0, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);
        containerNode->AddChild(backgroundNode);
        GetRootNode()->AddChild(containerNode);
        RegisterNode(backgroundNode);
        RegisterNode(containerNode);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

GRAPHIC_TEST(ContourDiagonalFlowLightTest, EFFECT_TEST, Set_Contour_Diagonal_Flow_Light_Validity_Check)
{
    const int sizeX = screenWidth_ / 2;
    const int sizeY = screenHeight_ / 2;

    auto leftNode = RSCanvasNode::Create();
    leftNode->SetBounds({0, 0, sizeX, sizeY});
    leftNode->SetFrame({0, 0, sizeX, sizeY});
    leftNode->SetBackgroundColor(0xff182028);
    auto contourShader = std::make_shared<RSNGContourDiagonalFlowLight>();
    SetContourDiagonalFlowLightParams(contourShader, contourDiagonalFlowLightParams[0]);
    contourShader->Setter<ContourDiagonalFlowLightLightWeightTag>(0.8f);
    contourShader->Setter<ContourDiagonalFlowLightHaloWeightTag>(0.6f);
    leftNode->SetBackgroundNGShader(contourShader);
    GetRootNode()->AddChild(leftNode);
    RegisterNode(leftNode);

    auto rightNode = RSCanvasNode::Create();
    rightNode->SetBounds({sizeX, 0, sizeX, sizeY});
    rightNode->SetFrame({sizeX, 0, sizeX, sizeY});
    rightNode->SetBackgroundColor(0xff102040);
    auto aiBarGlowShader = std::make_shared<RSNGAIBarGlow>();
    aiBarGlowShader->Setter<AIBarGlowLTWHTag>(Vector4f {0.1f, 0.1f, 0.8f, 0.8f});
    aiBarGlowShader->Setter<AIBarGlowStretchFactorTag>(1.0f);
    aiBarGlowShader->Setter<AIBarGlowBarAngleTag>(0.0f);
    aiBarGlowShader->Setter<AIBarGlowColor0Tag>(Vector4f {1.0f, 0.0f, 0.0f, 1.0f});
    aiBarGlowShader->Setter<AIBarGlowColor1Tag>(Vector4f {0.0f, 1.0f, 0.0f, 1.0f});
    aiBarGlowShader->Setter<AIBarGlowColor2Tag>(Vector4f {0.0f, 0.0f, 1.0f, 1.0f});
    aiBarGlowShader->Setter<AIBarGlowColor3Tag>(Vector4f {1.0f, 1.0f, 0.0f, 1.0f});
    aiBarGlowShader->Setter<AIBarGlowPosition0Tag>(Vector2f {0.0f, 0.0f});
    aiBarGlowShader->Setter<AIBarGlowPosition1Tag>(Vector2f {0.33f, 0.0f});
    aiBarGlowShader->Setter<AIBarGlowPosition2Tag>(Vector2f {0.66f, 0.0f});
    aiBarGlowShader->Setter<AIBarGlowPosition3Tag>(Vector2f {1.0f, 0.0f});
    aiBarGlowShader->Setter<AIBarGlowStrengthTag>(Vector4f {0.3f, 0.4f, 0.5f, 0.6f});
    aiBarGlowShader->Setter<AIBarGlowBrightnessTag>(1.0f);
    aiBarGlowShader->Setter<AIBarGlowProgressTag>(0.5f);
    rightNode->SetBackgroundNGShader(aiBarGlowShader);
    GetRootNode()->AddChild(rightNode);
    RegisterNode(rightNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

} // namespace OHOS::Rosen
