/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"

using namespace OHOS;
using namespace OHOS::Rosen;

constexpr uint32_t SLEEP_TIME = 3;
constexpr uint32_t PARENT_WIDTH = 100;
constexpr uint32_t PARENT_HEIGHT = 200;
constexpr uint32_t CHILD_WIDTH = 50;
constexpr uint32_t CHILD_HEIGHT = 50;
constexpr uint32_t STRIDE = 64;

void Resize(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t width, int32_t height)
{
    width = (width / STRIDE + 1) * STRIDE;
    height = (height / STRIDE + 1) * STRIDE;
    surfaceNode->SetBoundsWidth(width);
    surfaceNode->SetBoundsHeight(height);
}

bool InitSurface(std::shared_ptr<RSSurfaceNode>& surfaceNode, const std::string& surfaceName,
    int32_t width, int32_t height, uint32_t color)
{
    std::cout << "InitSurface" << std::endl;
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = surfaceName;
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    std::cout << "surface::Create" <<std::endl;
    surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (!surfaceNode) {
        return false;
    }
    std::cout << "SetFrameGravity" <<std::endl;
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
    float position_z;
    uint32_t left;
    uint32_t top;
    if (surfaceName.find("parentNode") != std::string::npos) {
        position_z = RSSurfaceNode::POINTER_WINDOW_POSITION_Z;
        left = 100;  // 100 left
        top = 300;   // 300 top
    } else {
        position_z = RSSurfaceNode::POINTER_WINDOW_POSITION_Z + 1;
        left = 0;
        top = 0;
    }
    surfaceNode->SetPositionZ(position_z);
    surfaceNode->SetBounds(left, top, (width / STRIDE + 1) * STRIDE, (height / STRIDE + 1) * STRIDE);
    surfaceNode->SetBackgroundColor(color);
    std::cout << "InitSurface success" << std::endl;
    return true;
}

int main()
{
    std::cout << "rs pointer window demo start!" << std::endl;
    std::cout << "rs pointer window demo stage 1 Init" << std::endl;
    DisplayId displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    std::shared_ptr<RSSurfaceNode> parentSurfaceNode;
    std::shared_ptr<RSSurfaceNode> childSurfaceNode;
    if (!InitSurface(parentSurfaceNode, "parentNode", PARENT_WIDTH, PARENT_HEIGHT, SK_ColorGREEN)) {
        return 0;
    }
    if (!InitSurface(childSurfaceNode, "childNode", CHILD_WIDTH, CHILD_HEIGHT, SK_ColorBLUE)) {
        return 0;
    }

    parentSurfaceNode->AddChild(childSurfaceNode, 0);
    // Attach RSSurfaceNode to RSDisplayNode through the AttachToDisplay interface of RSSurfaceNode
    std::cout << "GetDisplayById: " << std::endl;
    uint64_t screenId = DisplayManager::GetInstance().GetDisplayById(displayId)->GetId();
    std::cout << "ScreenId: " << screenId << std::endl;
    parentSurfaceNode->AttachToDisplay(screenId);

    std::cout << "RSTranscation::FlushImplicitTransaction" << std::endl;
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);

    // Attach and Detach
    std::cout << "rs pointer window demo stage 2 Detach" << std::endl;
    parentSurfaceNode->DetachToDisplay(screenId);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);
    std::cout << "rs pointer window demo stage 3 Attach" << std::endl;
    parentSurfaceNode->AttachToDisplay(screenId);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);

    // Resize
    std::cout << "rs pointer window demo stage 4 Resize" << std::endl;
    Resize(parentSurfaceNode, PARENT_WIDTH / 2, PARENT_HEIGHT / 2);  // 2 resize 1/2
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);

    std::cout << "rs pointer window demo end!" << std::endl;
    return 0;
}