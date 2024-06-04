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

#include "window.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs node demo Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorWHITE);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(400, 200, 400, 400);
    nodes[0]->SetFrame(400, 200, 400, 400);
    nodes[0]->SetBackgroundColor(SK_ColorBLUE);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[1]->SetBounds(400, 650, 400, 400);
    nodes[1]->SetFrame(400, 650, 400, 400);
    nodes[1]->SetBackgroundColor(SK_ColorBLUE);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[2]->SetBounds(200, 1400, 400, 400);
    nodes[2]->SetFrame(400, 1400, 400, 400);
    nodes[2]->SetBackgroundColor(SK_ColorBLUE);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[3]->SetBounds(700, 1400, 400, 400);
    nodes[3]->SetFrame(700, 1400, 400, 400);
    nodes[3]->SetBackgroundColor(SK_ColorBLUE);

    rootNode->AddChild(nodes[0], -1);
    rootNode->AddChild(nodes[1], -1);
    rootNode->AddChild(nodes[2], -1);
    rootNode->AddChild(nodes[3], -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}

int main()
{
    std::cout << "rs node demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, 1280, 2560});
    auto window = Window::Create("node_demo", option);
    window->Show();
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs node demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("node_demo", option);
        window->Show();
        rect = window->GetRect();
    }
    std::cout << "rs node demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    std::cout << "rs node demo stage 1 " << std::endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs node demo stage 2 " << std::endl;
    nodes[0]->SetRotationY(0);
    nodes[1]->SetRotationY(0);
    nodes[1]->SetPivotZ(200);
    nodes[2]->SetRotationX(0);
    nodes[3]->SetRotationX(0);
    nodes[3]->SetPivotZ(200);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(3000);
    RSNode::Animate(protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        nodes[0]->SetRotationY(360.f);
        nodes[1]->SetRotationY(360.f);
        nodes[2]->SetRotationX(360.f);
        nodes[3]->SetRotationX(360.f);
    }, []() {
        std::cout<<"custom animation 2 finish callback"<<std::endl;
    });
    rsUiDirector->SendMessages();
    sleep(5);

    std::cout << "rs node demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}
