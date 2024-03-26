/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    rootNode->SetAlphaOffscreen(true);
    rootNode->SetAlpha(0.5);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(100, 100, 800, 800);
    nodes[0]->SetFrame(100, 100, 800, 800);
    nodes[0]->SetBackgroundColor(Drawing::Color::COLOR_WHITE);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[1]->SetBounds(200, 200, 600, 600);
    nodes[1]->SetFrame(200, 200, 600, 600);
    nodes[1]->SetBackgroundColor(Drawing::Color::COLOR_BLACK);

    rootNode->AddChild(nodes[0], -1);
    nodes[0]->AddChild(nodes[1], -1);

    rsUiDirector->SetRoot(rootNode->GetId());
}

int main()
{
    std::cout << "rs client alpha offscreen demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, 1000, 1000});
    auto window = Window::Create("alpha_demo", option);
    window->Show();
    sleep(3);
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs client alpha offscreen demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("app_demo", option);
        window->Show();
        rect = window->GetRect();
    }
    std::cout << "rs client alpha offscreen demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    std::cout << "rs client alpha offscreen demo setoffscreen alpha " << std::endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(4);

    std::cout << "rs client alpha offscreen demo don't setoffscreen alpha " << std::endl;
    rootNode->SetAlphaOffscreen(false);
    rootNode->SetAlpha(0.5);
    rsUiDirector->SendMessages();
    sleep(4);

    std::cout << "rs client alpha offscreen demo end" << std::endl;
    sleep(1);

    return 0;
}
