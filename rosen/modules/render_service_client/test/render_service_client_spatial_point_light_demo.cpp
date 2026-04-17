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

#include <iostream>

#include "draw/canvas.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

using namespace OHOS;
using namespace OHOS::Rosen;

constexpr uint64_t SCREEN_ID = 0;

namespace {
// Window configuration
constexpr int WINDOW_OFFSET_Y = 200;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 2560;

// Node dimensions
constexpr int NODE_WIDTH = 400;
constexpr int NODE_HEIGHT = 400;

// Node positions
constexpr int NODE1_POS_X = 200;
constexpr int NODE1_POS_Y = 200;
constexpr int NODE2_POS_X = 700;
constexpr int NODE2_POS_Y = 200;
constexpr int NODE3_POS_X = 200;
constexpr int NODE3_POS_Y = 700;
constexpr int NODE4_POS_X = 700;
constexpr int NODE4_POS_Y = 700;

// Light parameters
constexpr float DEFAULT_LIGHT_INTENSITY = 1.0f;
constexpr float RED_LIGHT_INTENSITY = 1.5f;
constexpr float GREEN_LIGHT_INTENSITY = 2.0f;
constexpr float DEFAULT_LIGHT_ATTENUATION = 0.5f;
constexpr float RED_LIGHT_ATTENUATION = 0.3f;
constexpr float GREEN_LIGHT_ATTENUATION = 0.8f;
constexpr float LIGHT_DEPTH = 50.0f;

// Light positions
constexpr float LIGHT1_POS_X = 400.0f;
constexpr float LIGHT1_POS_Y = 400.0f;
constexpr float LIGHT2_POS_X = 900.0f;
constexpr float LIGHT2_POS_Y = 400.0f;
constexpr float LIGHT3_POS_X = 400.0f;
constexpr float LIGHT3_POS_Y = 900.0f;

// BorderLight cascade parameters
constexpr float BORDER_LIGHT_INTENSITY = 0.8f;
constexpr float BORDER_LIGHT_WIDTH = 5.0f;
constexpr float BORDER_LIGHT_CORNER_RADIUS = 30.0f;
constexpr float BORDER_LIGHT_POS_X = 0.5f;
constexpr float BORDER_LIGHT_POS_Y = 0.5f;

// Animation parameters
constexpr int ANIMATION_FRAME_COUNT = 60;
constexpr int ANIMATION_INTERVAL_US = 50000;
constexpr float ANIMATION_PHASE_STEP = 0.05f;
constexpr float ANIMATION_INTENSITY_BASE = 0.5f;
constexpr float ANIMATION_INTENSITY_RANGE = 0.5f;

// Timing
constexpr float WAIT_TIME_SHORT = 1.0f;
constexpr float WAIT_TIME_LONG = 2.0f;

// Colors
constexpr uint32_t COLOR_BLACK = 0xFF000000;
constexpr uint32_t COLOR_GRAY = 0xFF808080;
constexpr float COLOR_WHITE_R = 1.0f;
constexpr float COLOR_WHITE_G = 1.0f;
constexpr float COLOR_WHITE_B = 1.0f;
constexpr float COLOR_WHITE_A = 1.0f;
constexpr float COLOR_RED_R = 1.0f;
constexpr float COLOR_RED_G = 0.0f;
constexpr float COLOR_RED_B = 0.0f;
constexpr float COLOR_RED_A = 1.0f;
constexpr float COLOR_GREEN_R = 0.0f;
constexpr float COLOR_GREEN_G = 1.0f;
constexpr float COLOR_GREEN_B = 0.0f;
constexpr float COLOR_GREEN_A = 1.0f;
constexpr float COLOR_BORDER_GREEN_R = 0.0f;
constexpr float COLOR_BORDER_GREEN_G = 1.0f;
constexpr float COLOR_BORDER_GREEN_B = 0.0f;
constexpr float COLOR_BORDER_GREEN_A = 1.0f;

const std::string SURFACE_NODE_NAME = "spatial_point_light_demo";

std::shared_ptr<RSNGSpatialPointLight> CreateSpatialPointLightShader(
    const Vector3f& lightPos, const Vector4f& lightColor, float intensity, float attenuation)
{
    auto shader = std::make_shared<RSNGSpatialPointLight>();
    shader->Setter<SpatialPointLightLightIntensityTag>(intensity);
    shader->Setter<SpatialPointLightLightPositionTag>(lightPos);
    shader->Setter<SpatialPointLightLightColorTag>(lightColor);
    shader->Setter<SpatialPointLightAttenuationTag>(attenuation);
    return shader;
}

std::shared_ptr<RSNGBorderLight> CreateBorderLightShader()
{
    auto shader = std::make_shared<RSNGBorderLight>();
    shader->Setter<BorderLightPositionTag>(Vector3f(BORDER_LIGHT_POS_X, BORDER_LIGHT_POS_Y, 0.0f));
    shader->Setter<BorderLightColorTag>(
        Vector4f(COLOR_BORDER_GREEN_R, COLOR_BORDER_GREEN_G, COLOR_BORDER_GREEN_B, COLOR_BORDER_GREEN_A));
    shader->Setter<BorderLightIntensityTag>(BORDER_LIGHT_INTENSITY);
    shader->Setter<BorderLightWidthTag>(BORDER_LIGHT_WIDTH);
    shader->Setter<BorderLightCornerRadiusTag>(BORDER_LIGHT_CORNER_RADIUS);
    return shader;
}

class SpatialPointLightDemo {
public:
    SpatialPointLightDemo()
    {
        rsUiDirector_ = RSUIDirector::Create();
        if (rsUiDirector_ == nullptr) {
            std::cout << "Failed to create ui director!" << std::endl;
            return;
        }
        rsUiDirector_->Init(true, true);
        uiContext_ = rsUiDirector_->GetRSUIContext();
        if (uiContext_ == nullptr) {
            std::cout << "ui context is nullptr!" << std::endl;
            return;
        }

        if (!InitSurfaceNode()) {
            std::cout << "Failed to init surface node!" << std::endl;
            return;
        }

        if (!InitRootNode()) {
            std::cout << "Failed to init root node!" << std::endl;
            return;
        }

        if (!InitCanvasNodes()) {
            std::cout << "Failed to init canvas nodes!" << std::endl;
            return;
        }

        transaction_ = uiContext_->GetRSTransaction();
        if (transaction_ == nullptr) {
            std::cout << "rs transaction is nullptr" << std::endl;
            return;
        }

        transaction_->FlushImplicitTransaction();
        std::cout << "SpatialPointLight demo init success" << std::endl;
        initSuccess_ = true;
    }

    bool InitSurfaceNode()
    {
        if (rsUiDirector_ == nullptr) {
            return false;
        }
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = SURFACE_NODE_NAME;
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        surfaceNode_ = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext_);
        if (!surfaceNode_) {
            std::cout << "Failed to create surface node!" << std::endl;
            return false;
        }
        surfaceNode_->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode_->SetBounds(0, WINDOW_OFFSET_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetFrame(0, WINDOW_OFFSET_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode_->SetBackgroundColor(SK_ColorWHITE);
        surfaceNode_->AttachToDisplay(SCREEN_ID);
        rsUiDirector_->SetRSSurfaceNode(surfaceNode_);
        return true;
    }

    bool InitRootNode()
    {
        if (rsUiDirector_ == nullptr) {
            return false;
        }
        rootNode_ = RSRootNode::Create(false, false, uiContext_);
        if (rootNode_ == nullptr) {
            std::cout << "Failed to create root node!" << std::endl;
            return false;
        }
        rootNode_->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetBackgroundColor(COLOR_BLACK);
        rsUiDirector_->SetRSRootNode(rootNode_->ReinterpretCastTo<RSRootNode>());
        return true;
    }

    bool InitCanvasNodes()
    {
        if (rootNode_ == nullptr) {
            return false;
        }

        // Node 1: Basic spatial point light shader (white light)
        auto node1 = CreateCanvasNode(NODE1_POS_X, NODE1_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto shader1 = CreateSpatialPointLightShader(
            Vector3f(LIGHT1_POS_X, LIGHT1_POS_Y, LIGHT_DEPTH),
            Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
            DEFAULT_LIGHT_INTENSITY,
            DEFAULT_LIGHT_ATTENUATION);
        node1->SetBackgroundNGShader(shader1);
        canvasNodes_.push_back(node1);
        rootNode_->AddChild(node1, -1);

        // Node 2: Red point light
        auto node2 = CreateCanvasNode(NODE2_POS_X, NODE2_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto shader2 = CreateSpatialPointLightShader(
            Vector3f(LIGHT2_POS_X, LIGHT2_POS_Y, LIGHT_DEPTH),
            Vector4f(COLOR_RED_R, COLOR_RED_G, COLOR_RED_B, COLOR_RED_A),
            RED_LIGHT_INTENSITY,
            RED_LIGHT_ATTENUATION);
        node2->SetBackgroundNGShader(shader2);
        canvasNodes_.push_back(node2);
        rootNode_->AddChild(node2, -1);

        // Node 3: Green point light
        auto node3 = CreateCanvasNode(NODE3_POS_X, NODE3_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto shader3 = CreateSpatialPointLightShader(
            Vector3f(LIGHT3_POS_X, LIGHT3_POS_Y, LIGHT_DEPTH),
            Vector4f(COLOR_GREEN_R, COLOR_GREEN_G, COLOR_GREEN_B, COLOR_GREEN_A),
            GREEN_LIGHT_INTENSITY,
            GREEN_LIGHT_ATTENUATION);
        node3->SetBackgroundNGShader(shader3);
        canvasNodes_.push_back(node3);
        rootNode_->AddChild(node3, -1);

        // Node 4: Cascade shader (SpatialPointLight + BorderLight)
        auto node4 = CreateCanvasNode(NODE4_POS_X, NODE4_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto spatialShader = CreateSpatialPointLightShader(
            Vector3f(LIGHT1_POS_X, LIGHT1_POS_Y, LIGHT_DEPTH),
            Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
            DEFAULT_LIGHT_INTENSITY,
            DEFAULT_LIGHT_ATTENUATION);
        auto borderShader = CreateBorderLightShader();
        spatialShader->Append(borderShader);
        node4->SetBackgroundNGShader(spatialShader);
        canvasNodes_.push_back(node4);
        rootNode_->AddChild(node4, -1);

        return true;
    }

    RSCanvasNode::SharedPtr CreateCanvasNode(int x, int y, int width, int height)
    {
        auto node = RSCanvasNode::Create(false, false, uiContext_);
        if (node == nullptr) {
            return nullptr;
        }
        node->SetBounds(x, y, width, height);
        node->SetFrame(x, y, width, height);
        node->SetBackgroundColor(COLOR_GRAY);
        return node;
    }

    ~SpatialPointLightDemo() noexcept
    {
        if (initSuccess_) {
            surfaceNode_->DetachToDisplay(SCREEN_ID);
            transaction_->FlushImplicitTransaction();
        }
    }

    void TestSpatialPointLight()
    {
        if (!initSuccess_) {
            std::cout << "SpatialPointLight demo init failed, can't run" << std::endl;
            return;
        }
        transaction_->FlushImplicitTransaction();
        rsUiDirector_->SendMessages();
        rsUiDirector_->FlushModifier();
        std::cout << "test spatial point light success" << std::endl;
        sleep(WAIT_TIME_LONG);
    }

    void RunIntensityAnimation()
    {
        if (!initSuccess_ || canvasNodes_.empty()) {
            std::cout << "SpatialPointLight demo init failed, can't run animation" << std::endl;
            return;
        }

        std::cout << "Running intensity animation..." << std::endl;
        float phase = 0.0f;
        for (int frame = 0; frame < ANIMATION_FRAME_COUNT; frame++) {
            phase += ANIMATION_PHASE_STEP;
            float intensity = ANIMATION_INTENSITY_BASE + ANIMATION_INTENSITY_RANGE * sin(phase);

            auto shader = CreateSpatialPointLightShader(
                Vector3f(LIGHT1_POS_X, LIGHT1_POS_Y, LIGHT_DEPTH),
                Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
                intensity,
                DEFAULT_LIGHT_ATTENUATION);
            canvasNodes_[0]->SetBackgroundNGShader(shader);

            transaction_->FlushImplicitTransaction();
            rsUiDirector_->SendMessages();
            usleep(ANIMATION_INTERVAL_US);
        }
        std::cout << "Animation completed" << std::endl;
        sleep(WAIT_TIME_SHORT);
    }

    void Run()
    {
        TestSpatialPointLight();
        RunIntensityAnimation();
    }

private:
    std::shared_ptr<RSUIDirector> rsUiDirector_;
    std::shared_ptr<RSUIContext> uiContext_;
    std::shared_ptr<RSTransactionHandler> transaction_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSNode> rootNode_;
    std::vector<RSCanvasNode::SharedPtr> canvasNodes_;
    bool initSuccess_ = false;
};
} // namespace

int main()
{
    {
        SpatialPointLightDemo demo;
        demo.Run();
    }
    return 0;
}