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
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <securec.h>
#include <sstream>
#include <unistd.h>

#include "wm/window.h"
#include "wm/window_scene.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

#include "media_callback.h"

#include "player.h"
#include "nocopyable.h"
#include "media_data_source.h"

const char* SURFACE_STRIDE_ALIGNMENT = "8";
constexpr int32_t SURFACE_QUEUE_SIZE = 3;

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::map<std::string, std::function<int32_t()>> playerTable;

// If you want compile this demo, please add
// "//foundation/graphic/graphic_2d/rosen/modules/render_service_client/test:render_service_client_surface_node_demo",
// to bundle.json.
// and add "multimedia_player_framework:media_client" to external_deps in BUILD.gn.
// Attention: Before use this demo, please push any mp4 file which must be renamed "H264_Main.mp4" to /data,
// otherwise the demo would stop unnormally.

void RegisterTable(std::shared_ptr<OHOS::Media::Player> player)
{
    (void)playerTable.emplace("prepare", std::bind(&OHOS::Media::Player::Prepare, player));
    (void)playerTable.emplace("prepareasync", std::bind(&OHOS::Media::Player::PrepareAsync, player));
    (void)playerTable.emplace("", std::bind(&OHOS::Media::Player::Play, player)); // ENTER -> play
    (void)playerTable.emplace("play", std::bind(&OHOS::Media::Player::Play, player));
    (void)playerTable.emplace("pause", std::bind(&OHOS::Media::Player::Pause, player));
    (void)playerTable.emplace("stop", std::bind(&OHOS::Media::Player::Stop, player));
    (void)playerTable.emplace("reset", std::bind(&OHOS::Media::Player::Reset, player));
    (void)playerTable.emplace("release", std::bind(&OHOS::Media::Player::Release, player));
}

void DoNext()
{
    cout << "Enter your step:" << endl;
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        auto iter = playerTable.find(cmd);
        if (iter != playerTable.end()) {
            auto func = iter->second;
            if (func() != 0) {
                cout << "Operation error" << endl;
            }
            continue;
        } else if (cmd.find("quit") != std::string::npos || cmd == "q") {
            break;
        }
    }
}


void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs SurfaceNode demo Init Rosen Backend!" << std::endl;

    if (!rsUiDirector) {
        return;
    }
    rsUiDirector->Init();
    std::cout << "Init Rosen Backend" << std::endl;

    auto rootNode = RSRootNode::Create();
    cout << "RootNode id = " << rootNode->GetId() << endl;
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_WHITE);
    rsUiDirector->SetRoot(rootNode->GetId());

    auto canvasNode = RSCanvasNode::Create();
    cout << "canvasNode id = " << canvasNode->GetId() << endl;
    // SetFrame also can be (100, 100, 960, 1000)
    canvasNode->SetFrame(10, 10, 100, 100);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_RED);
    rootNode->AddChild(canvasNode, -1);

    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;

    // Create surfaceView node
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, false);

    // Create abilityView node
    // "auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);"
    // "surfaceNode->CreateNodeInRenderThread();"

    cout << "surfaceNode id = " << surfaceNode->GetId() << endl;
    // SetBounds also can be (300, 300, 960, 540);
    surfaceNode->SetBounds(30, 30, 512, 256);
    surfaceNode->SetBufferAvailableCallback([]() {
         cout << "SetBufferAvailableCallback" << endl;
    });

    canvasNode->AddChild(surfaceNode, -1);
    rsUiDirector->SendMessages();
    auto player = OHOS::Media::PlayerFactory::CreatePlayer();
    if (player == nullptr) {
        cout << "player is null" << endl;
        return;
    }
    RegisterTable(player);
    std::shared_ptr<OHOS::Ace::MediaCallback> cb = std::make_shared<OHOS::Ace::MediaCallback>();
    int32_t media_ret = -1;
    media_ret = player->SetPlayerCallback(cb);
    if (media_ret != 0) {
        cout << "SetPlayerCallback fail" << endl;
    }
    // Use Local file source here
    // path is /data/H264_Main.mp4
    media_ret = player->SetSource("/data/H264_Main.mp4");
    if (media_ret != 0) {
        cout << "SetSource fail" << endl;
        return;
    }
    sptr<Surface> producerSurface = nullptr;
    producerSurface = surfaceNode->GetSurface();
    if (producerSurface != nullptr) {
        producerSurface->SetQueueSize(SURFACE_QUEUE_SIZE);
        producerSurface->SetUserData("SURFACE_STRIDE_ALIGNMENT", SURFACE_STRIDE_ALIGNMENT);
        producerSurface->SetUserData("SURFACE_FORMAT", std::to_string(GRAPHIC_PIXEL_FMT_RGBA_8888));
        media_ret = player->SetVideoSurface(producerSurface);
        if (media_ret != 0) {
            cout << "SetVideoSurface fail" << endl;
        }
    }

    media_ret = player->PrepareAsync();
        if (media_ret !=  0) {
            cout << "PrepareAsync fail" << endl;
            return;
        }
    DoNext();
    sleep(10);
}

int main()
{
    std::cout << "rs SurfaceNode demo start!" << std::endl;

    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // SetWindowRect also can be {200, 200, 1501, 1200}
    option->SetWindowRect({120, 120, 512, 512});

    auto scene = new WindowScene();

    std::shared_ptr<AbilityRuntime::Context> context = nullptr;
    sptr<IWindowLifeCycle> listener = nullptr;
    scene->Init(0, context, listener, option);
    auto window = scene->GetMainWindow();
    scene->GoForeground();

    auto rect = window->GetRect();
    std::cout << "rs SurfaceNode demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto windowSurfaceNode = window->GetSurfaceNode();
    cout << "windowSurfaceNode id = " << windowSurfaceNode->GetId() << endl;

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    rsUiDirector->SetRSSurfaceNode(windowSurfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    std::cout << "rs SurfaceNode demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}