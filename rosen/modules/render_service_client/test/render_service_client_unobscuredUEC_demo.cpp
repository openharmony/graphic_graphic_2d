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
#include <iostream>

#include "dm/display_manager.h"
#include "securec.h"

#include "draw/canvas.h"
#include "image/bitmap.h"

#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;

constexpr uint32_t SLEEP_TIME = 3;
constexpr uint32_t LIMIT = 100;
constexpr uint32_t POINTER_WIDTH = 100;
constexpr uint32_t POINTER_HEIGHT = 200;
constexpr uint32_t POINTER_WINDOW_INIT_SIZE = 64;
constexpr uint32_t NODE_POSITION_X = 100;
constexpr uint32_t NODE_POSITION_Y = 300;
constexpr uint32_t UIEXTENSION_POSITION_X = 150;
constexpr uint32_t UIEXTENSION_POSITION_Y = 350;
std::shared_ptr<RSSurfaceNode> surfaceNode;
uint64_t screenId = 0;

namespace {

bool InitSurface()
{
    std::cout << "InitSurface" << std::endl;
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "AppMain_Window";
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    std::cout << "RSSurfaceNode::Create" <<std::endl;
    surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (!surfaceNode) {
        return false;
    }

    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
    surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    int width = (POINTER_WIDTH / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE * 3;
    int height = (POINTER_HEIGHT / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE * 3;
    surfaceNode->SetBounds(NODE_POSITION_X, NODE_POSITION_Y, width, height);
    surfaceNode->SetBackgroundColor(SK_ColorGREEN);

    std::shared_ptr<RSNode> rootNode = RSRootNode::Create();
    auto node1 = RSCanvasNode::Create();
    auto node2 = RSCanvasNode::Create();
    auto node3 = RSCanvasNode::Create();
    auto node4 = RSCanvasNode::Create();
    node4->SetBounds(NODE_POSITION_X, 0, width, height);
    node4->SetBackgroundColor(SK_ColorYELLOW);
    rootNode->AddChild(node1);
    rootNode->AddChild(node2);
    node1->AddChild(node3);
    node3->AddChild(node4);
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    rsUiDirector->SendMessages();
    sleep(1);

    RSSurfaceNodeConfig config;
    surfaceNodeConfig.SurfaceNodeName = "UIExtension";
    RSSurfaceNodeType nodeType = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    std::shared_ptr<RSSurfaceNode> uiExtension = RSSurfaceNode::Create(surfaceNodeConfig, nodeType, true, true);
    int width1 = (POINTER_WIDTH / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE * 2;
    int height1 = (POINTER_HEIGHT / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE * 2;
    uiExtension->SetBounds(NODE_POSITION_X, NODE_POSITION_Y, width1, height1);
    uiExtension->SetBackgroundColor(SK_ColorBLUE);
    node4->AddChild(uiExtension);

    surfaceNodeConfig.SurfaceNodeName = "SubUIExtension";
    std::shared_ptr<RSSurfaceNode> subUIExtension = RSSurfaceNode::Create(surfaceNodeConfig, nodeType, true, true);
    int width2 = (POINTER_WIDTH / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    int height2 = (POINTER_HEIGHT / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    subUIExtension->SetBounds(UIEXTENSION_POSITION_X, UIEXTENSION_POSITION_Y, width2, height2);
    subUIExtension->SetBackgroundColor(SK_ColorRED);
    auto node5 = RSCanvasNode::Create();
    uiExtension->AddChild(node5, -1);
    node5->AddChild(subUIExtension);

    std::cout << "ScreenId: " << screenId << std::endl;
    surfaceNode->AttachToDisplay(screenId);
    RSTransaction::FlushImplicitTransaction();
    std::cout << "RSTranscation::FlushImplicitTransaction" << std::endl;
    sleep(SLEEP_TIME);

    node4->RemoveChild(uiExtension);
    RSTransaction::FlushImplicitTransaction();
    std::cout << "RSTranscation::removechild" << std::endl;
    sleep(SLEEP_TIME);

    for (int i = 0; i < LIMIT; i++) {
        node4->AddChild(uiExtension);
        RSTransaction::FlushImplicitTransaction();
        sleep(SLEEP_TIME);
        node4->RemoveChild(uiExtension);
        RSTransaction::FlushImplicitTransaction();
        sleep(SLEEP_TIME);
    }
    return true;
}
}

int main()
{
    std::cout << "rs pointer window demo start!" << std::endl;
    InitSurface();
    std::cout << "rs pointer window demo end!" << std::endl;
    return 0;
}