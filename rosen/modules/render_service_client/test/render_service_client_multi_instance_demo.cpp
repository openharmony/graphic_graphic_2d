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

#include <event_handler.h>
#include <iostream>
#include <surface.h>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "rs_trace.h"
#include "token_setproc.h"
#include "wm/window.h"

#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "render/rs_border.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    rootNode = RSRootNode::Create(false, false, rsUIContext);
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[0]->SetBounds(0, 0, 100, 100);
    nodes[0]->SetFrame(0, 0, 100, 100);
    nodes[0]->SetBackgroundColor(Drawing::Color::COLOR_BLUE);

    rootNode->AddChild(nodes[0], -1);

    nodes.emplace_back(RSCanvasNode::Create(false, false, rsUIContext));
    nodes[1]->SetBounds(0, 200, 200, 200);
    nodes[1]->SetFrame(0, 200, 200, 200);
    nodes[1]->SetBackgroundColor(Drawing::Color::COLOR_GREEN);

    rootNode->AddChild(nodes[0], -1);
    rootNode->AddChild(nodes[1], -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}
class SubThread {
public:
    SubThread(uint32_t threadIndex) : threadIndex_(threadIndex) {}
    ~SubThread() = default;
    void Start()
    {
        std::string name = "SubThread" + std::to_string(threadIndex_);
        runner_ = AppExecFwk::EventRunner::Create(name);
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    }
    void PostTask(const std::function<void()>& task, const std::string& name = std::string())
    {
        if (handler_) {
            handler_->PostImmediateTask(task, name);
        }
    }

private:
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    uint32_t threadIndex_ = 0;
};

void InitNativeTokenInfo()
{
    uint64_t tokenId;
    const char* perms[1];
    perms[0] = "ohos.permission.SYSTEM_FLOAT_WINDOW";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "rs_uni_render_pixelmap_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

int main()
{
    InitNativeTokenInfo();
    SubThread subThread1 = SubThread(1);
    SubThread subThread2 = SubThread(2);
    subThread1.Start();
    subThread2.Start();

    int cnt = 0;
    std::cout << "rs app demo start!";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 720, 1280 });
    auto window = Window::Create("app_demo", option);
    std::shared_ptr<RSUIDirector> rsUiDirector;
    // Init demo env
    std::function<void()> task = [&cnt, &window, &option, &rsUiDirector]() {
        window->Show();
        sleep(2);
        auto rect = window->GetRect();
        while (rect.width_ == 0 && rect.height_ == 0) {
            std::cout << "rs app demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
            window->Hide();
            window->Destroy();
            window = Window::Create("app_demo", option);
            window->Show();
            rect = window->GetRect();
        }
        std::cout << "rs app demo create window: " << rect.width_ << " " << rect.height_ << std::endl;
        auto surfaceNode = window->GetSurfaceNode();
        // Build rosen renderThread & create nodes
        std::cout << "rs app demo stage: " << cnt++ << std::endl;
        rsUiDirector = RSUIDirector::Create();
        rsUiDirector->Init(true, true);
        auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
        auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
        rsUiDirector->SetUITaskRunner(
            [handler](const std::function<void()>& task, uint32_t delay) { handler->PostTask(task); });
        runner->Run();
        auto uiContext = rsUiDirector->GetRSUIContext();
        surfaceNode->SetRSUIContext(uiContext);
        rsUiDirector->SetRSSurfaceNode(surfaceNode);
        Init(rsUiDirector, rect.width_, rect.height_);
        // RSTransaction::FlushImplicitTransaction();
        if (!uiContext) {
            std::cout << "rs app demo stage error" << std::endl;
            return;
        }
        uiContext->GetRSTransaction()->FlushImplicitTransaction();
        // change property in nodes [setter using modifier]
        std::cout << "rs app demo stage" << cnt++ << std::endl;
        nodes[0]->SetBounds(0, 0, 200, 200);
        nodes[0]->SetFrame(0, 0, 200, 200);
        nodes[0]->SetBorderColor(Drawing::Color::COLOR_BLACK);
        nodes[0]->SetBorderWidth(10);
        nodes[0]->SetBorderStyle((uint32_t)BorderStyle::SOLID);

        auto transaction = uiContext->GetRSTransaction();
        if (!transaction) {
            std::cout << "!transaction" << cnt++ << std::endl;
            return;
        }
        transaction->FlushImplicitTransaction();
        // rsUiDirector->SendMessages();
    };
    subThread1.PostTask(task, "create");
    sleep(3);
    std::function<void()> task2 = [&cnt]() {
        std::cout << "rs app demo stage " << cnt++ << std::endl;
        nodes[0]->SetBounds(0, 0, 400, 200);
        nodes[0]->SetFrame(0, 0, 400, 200);
        nodes[0]->SetBorderColor(Drawing::Color::COLOR_BLACK);
        nodes[0]->SetBorderWidth(10);
        nodes[0]->SetBorderStyle((uint32_t)BorderStyle::SOLID);

        // rsUiDirector->SendMessages();
        auto transaction = nodes[0]->GetRSUIContext()->GetRSTransaction();
        if (!transaction) {
            std::cout << "!transaction" << cnt++ << std::endl;
            return;
        }
        transaction->FlushImplicitTransaction();
    };
    subThread2.PostTask(task2, "change");

    sleep(3);

    // dump property via modifiers
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    std::cout << nodes[0]->GetStagingProperties().Dump() << std::endl;
    std::cout << "rs app demo end!" << std::endl;
    sleep(3);

    window->Hide();
    window->Destroy();
    return 0;
}