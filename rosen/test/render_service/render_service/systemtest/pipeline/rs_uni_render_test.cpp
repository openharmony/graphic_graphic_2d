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
#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "surface.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_render_service_listener.h"
#include "wm/window.h"
#include "draw/color.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_filter.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height);

    std::shared_ptr<RSNode> rootNode;
    std::shared_ptr<RSCanvasNode> canvasNode;
    static constexpr int SLEEP_TIME_US = 100000;
};

void RSUniRenderTest::Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs uni render ST Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_RED);

    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(100, 100, 300, 200); // canvasnode bounds: {L: 100, T:100, W: 300, H: 200}
    canvasNode->SetFrame(100, 100, 300, 200); // canvasnode Framebounds: {L: 100, T:100, W: 300, H: 200}
    canvasNode->SetBackgroundColor(0x6600ff00);

    rootNode->AddChild(canvasNode, -1);

    rsUiDirector->SetRoot(rootNode->GetId());
}

void RSUniRenderTest::SetUpTestCase()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "6");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
}

void RSUniRenderTest::TearDownTestCase()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

void RSUniRenderTest::SetUp() {}
void RSUniRenderTest::TearDown() {}

/**
 * @tc.name: RSUniRenderTest001
 * @tc.desc: rs_uni_render_visitor system test.
 */
HWTEST_F(RSUniRenderTest, RSUniRenderTest001, TestSize.Level2)
{
    std::cout << "rs uni render demo start!" << std::endl;
    RSSystemProperties::GetUniRenderEnabled();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 500, 800 });
    auto window = Window::Create("uni_render_demo1", option);
    ASSERT_NE(nullptr, window);
    
    window->Show();
    usleep(SLEEP_TIME_US);
    auto rect = window->GetRect();
    int failureTimes = 0;
    while (rect.width_ == 0 && rect.height_ == 0) {
        if (failureTimes == 10) {
            return;
        }
        failureTimes++;
        std::cout << "rs uni render demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("uni_render_demo", option);
        window->Show();
        usleep(SLEEP_TIME_US);
        rect = window->GetRect();
    }


    std::cout << "rs uni render demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        std::cout << "rs uni render demo not in uni render mode, exit! " << std::endl;
        return;
    }
    system::SetParameter("rosen.uni.partialrender.enabled", "0");
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    std::cout << "rs uni render demo stage 1 " << std::endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    surfaceNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    std::cout << "rs uni render demo stage 2 SetFilter" << std::endl;
    auto filter = RSFilter::CreateBlurFilter(5.f, 5.f);
    canvasNode->SetFilter(filter);
    rsUiDirector->SendMessages();
    sleep(2);

    std::cout << "rs uni render demo stage 3 SetBounds" << std::endl;
    surfaceNode->SetBoundsWidth(250);
    surfaceNode->SetBoundsHeight(400);
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    std::cout << "rs uni render demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
}

/**
 * @tc.name: RSUniRenderTest002
 * @tc.desc: rs_uni_render_visitor system test.
 */
HWTEST_F(RSUniRenderTest, RSUniRenderTest002, TestSize.Level2)
{
    RSSystemProperties::GetUniRenderEnabled();
    sptr<WindowOption> option1 = new WindowOption();
    option1->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option1->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option1->SetWindowRect({ 0, 0, 500, 900 });
    auto window1 = Window::Create("uni_render_demo2", option1);
    ASSERT_NE(nullptr, window1);

    sptr<WindowOption> option2 = new WindowOption();
    option2->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option2->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option2->SetWindowRect({ 0, 0, 600, 800 });
    auto window2 = Window::Create("uni_render_demo3", option2);
    ASSERT_NE(nullptr, window2);

    window1->Show();
    window2->Show();
    auto rect1 = window1->GetRect();
    auto rect2 = window2->GetRect();
    auto surfaceNode1 = window1->GetSurfaceNode();
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
    auto rsUiDirector1 = RSUIDirector::Create();
    rsUiDirector1->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);
    rsUiDirector1->SetRSSurfaceNode(surfaceNode1);
    Init(rsUiDirector1, rect1.width_, rect1.height_);
    rsUiDirector1->SendMessages();
    sleep(1);
    auto surfaceNode2 = window2->GetSurfaceNode();
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
    auto rsUiDirector2 = RSUIDirector::Create();
    rsUiDirector2->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);
    rsUiDirector2->SetRSSurfaceNode(surfaceNode2);
    Init(rsUiDirector2, rect2.width_, rect2.height_);
    rsUiDirector2->SendMessages();
    sleep(1);
    window1->Hide();
    window1->Destroy();
    window2->Hide();
    window2->Destroy();
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
}
} // namespace OHOS::Rosen