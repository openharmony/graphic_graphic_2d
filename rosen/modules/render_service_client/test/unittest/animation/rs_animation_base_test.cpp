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

#include "rs_animation_base_test.h"

#include "rs_animation_test_utils.h"

#include "event_handler.h"
#include "common/rs_color.h"
#include "transaction/rs_transaction.h"
#include "wm/window_option.h"

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
sptr<Window> RSAnimationBaseTest::window = nullptr;
std::shared_ptr<RSSurfaceNode> RSAnimationBaseTest::animationSurfaceNode = nullptr;
std::shared_ptr<RSNode> RSAnimationBaseTest::rootNode = nullptr;
std::shared_ptr<RSCanvasNode> RSAnimationBaseTest::canvasNode = nullptr;
std::shared_ptr<RSUIDirector> RSAnimationBaseTest::rsUiDirector = nullptr;

void RSAnimationBaseTest::SetUpTestCase()
{
    system("setenforce 0");
    InitAnimationWindow();
}

void RSAnimationBaseTest::TearDownTestCase()
{
    DestoryAnimationWindow();
}

void RSAnimationBaseTest::SetUp()
{
    ResetAnimationCanvasNode();
}

void RSAnimationBaseTest::TearDown()
{
    RemoveAnimationCanvasNode();
}

void RSAnimationBaseTest::InitNode(int width, int height)
{
    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorYELLOW);

    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(ANIMATION_START_BOUNDS);
    canvasNode->SetFrame(ANIMATION_START_BOUNDS);
    canvasNode->SetBackgroundColor(SK_ColorBLUE);
    rootNode->AddChild(canvasNode, -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}

void RSAnimationBaseTest::InitAnimationWindow()
{
    std::cout << "InitAnimationWindow start" << std::endl;
    if (window != nullptr) {
        return;
    }
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 720, 1280 });
    window = Window::Create("animation_ut", option);
    while (!window) {
        window = Window::Create("animation_ut", option);
    }
    window->Show();
    sleep(1);
    auto rect = window->GetRequestRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "animation_ut create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("animation_ut", option);
        window->Show();
        sleep(1);
        rect = window->GetRequestRect();
    }
    std::cout << "animation_ut create window " << rect.width_ << " " << rect.height_ << std::endl;
    animationSurfaceNode = window->GetSurfaceNode();
    rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    rsUiDirector->SetUITaskRunner(
        [handler](const std::function<void()>& task, uint32_t delay) { handler->PostTask(task); });
    runner->Run();
    RSTransaction::FlushImplicitTransaction();
    sleep(DELAY_TIME_ONE);
    rsUiDirector->SetRSSurfaceNode(animationSurfaceNode);
    InitNode(rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_THREE);
    std::cout << "InitAnimationWindow end " << std::endl;
}

void RSAnimationBaseTest::DestoryAnimationWindow()
{
    std::cout << "DestoryAnimationWindow start" << std::endl;
    window->Hide();
    window->Destroy();
    window = nullptr;
    animationSurfaceNode = nullptr;
    rootNode = nullptr;
    rsUiDirector->Destroy();
    rsUiDirector = nullptr;
    std::cout << "DestoryAnimationWindow end" << std::endl;
}

void RSAnimationBaseTest::RemoveAnimationCanvasNode()
{
    rootNode->RemoveChild(canvasNode);
    canvasNode = nullptr;
}

void RSAnimationBaseTest::ResetAnimationCanvasNode()
{
    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(ANIMATION_START_BOUNDS);
    canvasNode->SetFrame(ANIMATION_START_BOUNDS);
    canvasNode->SetBackgroundColor(SK_ColorBLUE);
    rootNode->AddChild(canvasNode, -1);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_TWO);
}

void RSAnimationBaseTest::NotifyStartAnimation()
{
    int64_t startNum = START_NUMBER;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(startNum);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        startNum += INCREASE_NUMBER;
        usleep(DELAY_TIME_REFRESH);
    }
    sleep(DELAY_TIME_ONE);
}
}
}
