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

#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;
uint64_t screenId = 0;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height,
    std::vector<std::shared_ptr<RSCanvasNode>>& nodes, std::shared_ptr<RSNode>& rootNode)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    rootNode = RSRootNode::Create(false, false, rsUIContext);
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[0]->SetBounds(0, 0, 100, 100);
    nodes[0]->SetFrame(0, 0, 100, 100);
    nodes[0]->SetBackgroundColor(Drawing::Color::COLOR_BLUE);

    rootNode->AddChild(nodes[0], -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}

int main()
{
    std::cout << "render service client first frame callback demo start!" << std::endl;
    std::shared_ptr<RSNode> rootNode;
    std::vector<std::shared_ptr<RSCanvasNode>> nodes;
    std::shared_ptr<RSUIDirector> rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init(true, true);
    auto uiContext = rsUiDirector->GetRSUIContext();
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "AppMain_Window";
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
    surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode->SetBounds(0, 0, 600, 600);
    std::cout << "rs app demo stage 1 " << std::endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, 300, 300, nodes, rootNode);
    surfaceNode->AttachToDisplay(screenId);
    auto transaction = uiContext->GetRSTransaction();
    if (!transaction) {
        std::cout << "!transaction " << std::endl;
        return -1;
    }
    transaction->FlushImplicitTransaction();

    sleep(2);
    std::cout << "rs app demo stage 2 " << std::endl;
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);
    transaction->FlushImplicitTransaction();
    sleep(2);

    std::cout << "rs app demo stage 3 " << std::endl;
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLUE);
    transaction->FlushImplicitTransaction();
    sleep(2);

    std::cout << "rs app demo stage 4 bufferAvailble" << std::endl;
    surfaceNode->SetIsNotifyUIBufferAvailable(false);
    surfaceNode->SetBufferAvailableCallback([]() {
        std::cout << "SetBufferAvailableCallback 1" << std::endl;
    });
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);
    transaction->FlushImplicitTransaction();
    sleep(2);

    std::cout << "rs app demo stage 5 bufferAvailble " << std::endl;
    surfaceNode->SetIsNotifyUIBufferAvailable(false);
    surfaceNode->SetBufferAvailableCallback([]() {
        std::cout << "SetBufferAvailableCallback 2" << std::endl;
    });
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLUE);
    transaction->FlushImplicitTransaction();
    sleep(4);

    return 0;
}
