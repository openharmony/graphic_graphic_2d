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

#include "rs_interface_client_frame_test.h"

constexpr size_t VIEWPORT_WIDTH = 500;
constexpr size_t VIEWPORT_HEIGHT = 200;
constexpr uint32_t BACKGROUND_COLOR = 0XFFFF0000;
constexpr uint32_t CANVAS_BACKGROUND_COLOR = 0XFFFF0000;
constexpr uint32_t SLEPP_10S = 100000;
constexpr float MAX_ZORDER = 1e5f;
using namespace OHOS;
using namespace OHOS::Rosen;
InterfaceClientFrameTest::InterfaceClientFrameTest()
{
    auto& rsInterefaces = RSInterfaces::GetInstance();
    auto screenId = rsInterefaces.GetDefaultScreenId();

    RSSurfaceNodeConfig config {
        .SurfaceNodeName = "InterfaceSurfaceTest",
        .isSync = true,
    };

    RSSurfaceNodeType type = RSSurfaceNodeType::APP_WINDOW_NODE;

    rootSurfaceNode_ = RSSurfaceNode::Create(config, type, false, false);
    if (rootSurfaceNode_ == nullptr) {
        std::cout << "RenderServiceClinetFrameTest surfaceNode is nullptr" << std::endl;
        return;
    }
    rootSurfaceNode_->AttachToDisplay(screenId);
    rootSurfaceNode_->SetPositionZ(MAX_ZORDER);
    rootSurfaceNode_->SetBounds({0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT});
    rootSurfaceNode_->SetBackgroundColor(BACKGROUND_COLOR);

    rootCanvasNode_ = Rosen::RSCanvasNode::Create(true, false);

    rootCanvasNode_->SetBounds({0, 0, VIEWPORT_WIDTH / 2, VIEWPORT_HEIGHT / 2});
    rootCanvasNode_->SetBackgroundColor(CANVAS_BACKGROUND_COLOR);
    rootCanvasNode_->SetPositionZ(MAX_ZORDER);

    rootSurfaceNode_->AddChild(rootCanvasNode_, 0);
    std::cout << "RenderServiceClinetFrameTest Init Success" << std::endl;
    RSTransaction::FlushImplicitTransaction();
    usleep(SLEPP_10S);
}

void InterfaceClientFrameTest::Run()
{
    for (auto &[code, func] : execFuncMap) {
        func(*this);
    }
}

InterfaceClientFrameTest &InterfaceClientFrameTest::GetInstance()
{
    static InterfaceClientFrameTest instance;
    return instance;
}
