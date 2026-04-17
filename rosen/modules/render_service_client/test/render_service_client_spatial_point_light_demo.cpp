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
#include "ui_effect/property/include/rs_ui_shape_base.h"

using namespace OHOS;
using namespace OHOS::Rosen;

constexpr uint64_t SCREEN_ID = 0;

namespace {
constexpr int WINDOW_OFFSET = 200;
constexpr int WINDOW_WIDTH = 1500;
constexpr int WINDOW_HEIGHT = 2000;

constexpr float WAIT_TIME = 5.0f;

constexpr float LIGHT_INTENSITY = 1.0f;
constexpr float LIGHT_ATTENUATION = 0.5f;
constexpr float LIGHT_Z = 50.0f;

const std::string SURFACE_NODE_NAME = "spatial_point_light_demo";

std::shared_ptr<RSNGSpatialPointLight> CreateSpatialPointLightShader()
{
    auto shader = std::make_shared<RSNGSpatialPointLight>();
    shader->Setter<SpatialPointLightLightIntensityTag>(LIGHT_INTENSITY);
    shader->Setter<SpatialPointLightLightPositionTag>(Vector3f(750.0f, 1000.0f, LIGHT_Z));
    shader->Setter<SpatialPointLightLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    shader->Setter<SpatialPointLightAttenuationTag>(LIGHT_ATTENUATION);
    return shader;
}

std::shared_ptr<RSNGSDFRRectShape> CreateSDFRRectShape()
{
    auto shape = std::make_shared<RSNGSDFRRectShape>();
    constexpr float shapeWidth = 1000.0f;
    constexpr float shapeHeight = 500.0f;
    constexpr float cornerRadius = 50.0f;
    RRect rrect = RRect(RectT<float>(250, 750, shapeWidth, shapeHeight), cornerRadius, cornerRadius);
    shape->Setter<SDFRRectShapeRRectTag>(rrect);
    return shape;
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

        if (!InitCanvasNode()) {
            std::cout << "Failed to init canvas node!" << std::endl;
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
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = SURFACE_NODE_NAME;
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        surfaceNode_ = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext_);
        if (!surfaceNode_) {
            std::cout << "Failed to create surface node!" << std::endl;
            return false;
        }
        surfaceNode_->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode_->SetBounds(0, WINDOW_OFFSET, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetFrame(0, WINDOW_OFFSET, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode_->SetBackgroundColor(SK_ColorWHITE);
        surfaceNode_->AttachToDisplay(SCREEN_ID);
        rsUiDirector_->SetRSSurfaceNode(surfaceNode_);
        return true;
    }

    bool InitRootNode()
    {
        rootNode_ = RSRootNode::Create(false, false, uiContext_);
        if (rootNode_ == nullptr) {
            std::cout << "Failed to create root node!" << std::endl;
            return false;
        }
        rootNode_->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetBackgroundColor(0xFF000000);
        rsUiDirector_->SetRSRootNode(rootNode_->ReinterpretCastTo<RSRootNode>());
        return true;
    }

    bool InitCanvasNode()
    {
        canvasNode_ = RSCanvasNode::Create(false, false, uiContext_);
        if (canvasNode_ == nullptr) {
            std::cout << "Failed to create canvas node!" << std::endl;
            return false;
        }
        canvasNode_->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        canvasNode_->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        canvasNode_->SetSDFShape(CreateSDFRRectShape());
        canvasNode_->SetMaterialNGFilter(CreateSpatialPointLightShader());
        rootNode_->AddChild(canvasNode_, -1);
        return true;
    }

    ~SpatialPointLightDemo() noexcept
    {
        if (initSuccess_) {
            surfaceNode_->DetachToDisplay(SCREEN_ID);
            transaction_->FlushImplicitTransaction();
        }
    }

    void Run()
    {
        if (!initSuccess_) {
            std::cout << "SpatialPointLight demo init failed, can't run" << std::endl;
            return;
        }
        transaction_->FlushImplicitTransaction();
        rsUiDirector_->SendMessages();
        rsUiDirector_->FlushModifier();
        std::cout << "test spatial point light success" << std::endl;
        sleep(WAIT_TIME);
    }

private:
    std::shared_ptr<RSUIDirector> rsUiDirector_;
    std::shared_ptr<RSUIContext> uiContext_;
    std::shared_ptr<RSTransactionHandler> transaction_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSNode> rootNode_;
    RSCanvasNode::SharedPtr canvasNode_;
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