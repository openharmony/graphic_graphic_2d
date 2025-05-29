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
#include <surface.h>

#include "foundation/window/window_manager/interfaces/innerkits/wm/window.h"

#include "image_source.h"
#include "pixel_map.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Rosen;

constexpr float MAX_ZORDER = 1e5f;
constexpr size_t BOUNDS_WIDTH = 800;
constexpr size_t BOUNDS_HEIGHT = 500;
int main()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();
    if (screenId == INVALID_SCREEN_ID) {
        std::cout << "GetDefaultScreenId failed" << std::endl;
        screenId = 0;
    }

    auto rsUiDirector = RSUIDirector::Create();
    if (rsUiDirector == nullptr) {
        std::cout << "rsUiDirector is nullptr" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Create RSSurfaceNode" << std::endl;
    RSSurfaceNodeConfig config {
        .SurfaceNodeName = "SurfaceLocalMagnificationTest",
        .isSync = true,
    };
    RSSurfaceNodeType type = RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = RSSurfaceNode::Create(config, type, true, false, rsUIContext);
    if (surfaceNode == nullptr) {
        std::cout << "surfaceNode is nullptr" << std::endl;
        return EXIT_FAILURE;
    }
    surfaceNode->AttachToDisplay(screenId);

    std::cout << "Local Magnification Case1: base case" << std::endl;
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->SetAbilityBGAlpha(254);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
    surfaceNode->SetRegionToBeMagnified({0, 0, 160, 100});
    surfaceNode->SetFrame({0, 0, BOUNDS_WIDTH, BOUNDS_HEIGHT});
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "Local Magnification Case2: move position" << std::endl;
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->SetAbilityBGAlpha(254);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
    surfaceNode->SetRegionToBeMagnified({500, 500, 160, 100});
    surfaceNode->SetFrame({500, 500, BOUNDS_WIDTH, BOUNDS_HEIGHT});
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "Local Magnification Case3: change scale" << std::endl;
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->SetAbilityBGAlpha(254);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
    surfaceNode->SetRegionToBeMagnified({500, 500, 400, 250});
    surfaceNode->SetFrame({500, 500, BOUNDS_WIDTH, BOUNDS_HEIGHT});
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "Local Magnification Case4: move position and change scale" << std::endl;
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->SetAbilityBGAlpha(254);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
    surfaceNode->SetRegionToBeMagnified({500, 1000, 100, 62.5});
    surfaceNode->SetFrame({500, 1000, BOUNDS_WIDTH, BOUNDS_HEIGHT});
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "Local Magnification Case5: abnormal case1" << std::endl;
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->SetAbilityBGAlpha(254);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
    surfaceNode->SetRegionToBeMagnified({500, 500, 0, 0});
    surfaceNode->SetFrame({500, 500, BOUNDS_WIDTH, BOUNDS_HEIGHT});
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "Local Magnification Case5: abnormal case2" << std::endl;
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->SetAbilityBGAlpha(254);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
    surfaceNode->SetRegionToBeMagnified({ 1300, 500, 400, 250 });
    surfaceNode->SetFrame({ 500, 500, BOUNDS_WIDTH, BOUNDS_HEIGHT });
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "Local Magnification Case7: sliding window" << std::endl;
    for (int i = 0; i < 15; i++) {
        surfaceNode->SetPositionZ(MAX_ZORDER);
        surfaceNode->SetAbilityBGAlpha(254);
        surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
        surfaceNode->SetRegionToBeMagnified({ 30 * i, 60 * i, 400, 250 });
        surfaceNode->SetFrame({ 30 * i, 60 * i, BOUNDS_WIDTH, BOUNDS_HEIGHT });
        RSTransaction::FlushImplicitTransaction();
        sleep(1);
    }

    std::cout << "Local Magnification Case8: dirty manager, click to the accessibility page" << std::endl;
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->SetAbilityBGAlpha(254);
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
    surfaceNode->SetRegionToBeMagnified({230, 1100, 400, 400});
    surfaceNode->SetFrame({230, 1100, 800, 800});
    RSTransaction::FlushImplicitTransaction();
    sleep(50);

    return EXIT_SUCCESS;
}