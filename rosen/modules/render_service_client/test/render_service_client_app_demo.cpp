/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "wm/window.h"


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
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_RED);

    rsUiDirector->SetRoot(rootNode->GetId());
}

int main()
{
    std::cout << "rs app demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, 2560, 112});
    auto window = Window::Create("app_demo", option);
    
    window->Show();
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs app demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("app_demo", option);
        window->Show();
        rect = window->GetRect();
    }
    std::cout << "rs app demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    std::cout << "rs app demo stage 1 " << std::endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs app demo stage 2 " << std::endl;
    int resizeH = 1600;
    window->Resize(2560, resizeH);
    rootNode->SetBounds(0, 0, 2560, resizeH);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);
    rsUiDirector->SendMessages();
    sleep(4);

    std::cout << "rs app demo stage 3 " << std::endl;
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLUE);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs app demo stage 3 " << std::endl;
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs app demo stage 3 " << std::endl;
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLUE);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs app demo start dump test --> " << std::endl;
    rootNode->SetRotation(20.f);
    rootNode->SetAlpha(0.5f);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_RED);
    rsUiDirector->SendMessages();
    sleep(1);

    std::string dumpInfo = rootNode->GetStagingProperties().Dump();
    std::cout << "dumpInfo: " << dumpInfo.c_str() << std::endl;
    sleep(1);

    std::cout << "rs app demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}
