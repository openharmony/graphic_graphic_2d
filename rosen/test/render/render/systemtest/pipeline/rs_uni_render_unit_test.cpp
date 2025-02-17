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

#include "accesstoken_kit.h"
#ifdef SUPPORT_ACCESS_TOKEN
#include "nativetoken_kit.h"
#include "token_setproc.h"
#endif
#include "gtest/gtest.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/rs_render_service_listener.h"
#include "limit_number.h"
#include "surface.h"
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
class RsUniRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height);

    std::shared_ptr<RSNode> rootNode;
    std::shared_ptr<RSCanvasNode> canvasNode;
};

void RsUniRenderTest::Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
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

void RsUniRenderTest::SetUpTestCase()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "6");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
}

void RsUniRenderTest::TearDownTestCase()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

void RsUniRenderTest::SetUp()
{
#ifdef SUPPORT_ACCESS_TOKEN
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.SYSTEM_FLOAT_WINDOW";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "test",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
#endif
}

void RsUniRenderTest::TearDown() {}

/**
 * @tc.name: RSUniRenderTest01
 * @tc.desc: rs_uni_render_visitor system test.
 */
HWTEST_F(RsUniRenderTest, RSUniRenderTest01, TestSize.Level2)
{
#ifdef SUPPORT_ACCESS_TOKEN
    RSSystemProperties::GetUniRenderEnabled();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 500, 800 });
    auto window = Window::Create("uni_render_demo1", option);
    ASSERT_NE(window, nullptr);
    
    window->Show();
    usleep(100000);
    auto rect = window->GetRect();
    int failureTimes = 0;
    while (rect.width_ == 0 && rect.height_ == 0) {
        if (failureTimes == 10) {
            return;
        }
        failureTimes++;
        window->Hide();
        window->Destroy();
        window = Window::Create("uni_render_demo", option);
        window->Show();
        usleep(100000);
        rect = window->GetRect();
    }
    auto surfaceNode = window->GetSurfaceNode();
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
    system::SetParameter("rosen.uni.partialrender.enabled", "0");
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    surfaceNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    auto filter = RSFilter::CreateBlurFilter(5.f, 5.f);
    canvasNode->SetFilter(filter);
    rsUiDirector->SendMessages();
    sleep(2);

    surfaceNode->SetBoundsWidth(250);
    surfaceNode->SetBoundsHeight(400);
    RSTransaction::FlushImplicitTransaction();
    sleep(2);

    window->Hide();
    window->Destroy();
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
#endif
}

/**
 * @tc.name: RSUniRenderTest02
 * @tc.desc: rs_uni_render_visitor system test.
 */
HWTEST_F(RsUniRenderTest, RSUniRenderTest02, TestSize.Level2)
{
#ifdef SUPPORT_ACCESS_TOKEN
    RSSystemProperties::GetUniRenderEnabled();
    sptr<WindowOption> option1 = new WindowOption();
    option1->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option1->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option1->SetWindowRect({ 0, 0, 500, 900 });
    auto window1 = Window::Create("uni_render_demo2", option1);
    ASSERT_NE(window1, nullptr);

    sptr<WindowOption> option2 = new WindowOption();
    option2->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option2->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option2->SetWindowRect({ 0, 0, 600, 800 });
    auto window2 = Window::Create("uni_render_demo3", option2);
    ASSERT_NE(window2, nullptr);

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
#endif
}
} // namespace OHOS::Rosen