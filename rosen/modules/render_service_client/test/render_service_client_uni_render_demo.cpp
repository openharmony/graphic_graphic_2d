/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window.h"

#include "platform/common/rs_system_properties.h"
#include "render/rs_filter.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

static std::shared_ptr<RSNode> rootNode;
static std::shared_ptr<RSCanvasNode> canvasNode;

static void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs uni render demo Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_RED);

    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(100, 100, 300, 200);
    canvasNode->SetFrame(100, 100, 300, 200);
    canvasNode->SetBackgroundColor(0x6600ff00);

    rootNode->AddChild(canvasNode, -1);

    rsUiDirector->SetRoot(rootNode->GetId());
}

int main()
{
    std::cout << "rs uni render demo start!" << std::endl;
    RSSystemProperties::GetUniRenderEnabled();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 500, 800 });
    auto window = Window::Create("uni_render_demo", option);

    window->Show();
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs uni render demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("uni_render_demo", option);
        window->Show();
        rect = window->GetRect();
    }

    std::cout << "rs uni render demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();
    if (!RSSystemProperties::GetUniRenderEnabled() || surfaceNode->GetSurface() != nullptr) {
        std::cout << "rs uni render demo not in uni render mode, exit! " << std::endl;
        return 0;
    }

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    std::cout << "rs uni render demo stage 1 " << std::endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs uni render demo stage 2 drawTextBlob" << std::endl;
    std::cout << "Drawing does not support TextBlob" << std::endl;
    canvasNode->FinishRecording();
    rsUiDirector->SendMessages();
    sleep(2);

    std::cout << "rs uni render demo stage 3 SetFilter" << std::endl;
    auto filter = RSFilter::CreateBlurFilter(5.f, 5.f);
    canvasNode->SetFilter(filter);
    rsUiDirector->SendMessages();
    sleep(2);

    std::cout << "rs uni render demo stage 4 SetBounds" << std::endl;
    surfaceNode->SetBoundsWidth(250);
    surfaceNode->SetBoundsHeight(400);
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "rs uni render demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}
