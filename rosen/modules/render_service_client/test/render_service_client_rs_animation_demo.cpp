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

#include <event_handler.h>
#include <iostream>

#include "animation/rs_curve_animation.h"
#include "animation/rs_transition.h"

#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"
#include "wm/window.h"
#include "wm/window_scene.h"

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

std::unique_ptr<RSSurfaceFrame> framePtr;
RenderContext* rc_ = nullptr;

void DrawSurface(Drawing::Rect surfaceGeometry,
    uint32_t color, Drawing::Rect shapeGeometry, std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto x = surfaceGeometry.GetLeft();
    auto y = surfaceGeometry.GetTop();
    auto width = surfaceGeometry.GetWidth();
    auto height = surfaceGeometry.GetHeight();
    surfaceNode->SetBounds(x, y, width, height);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        return;
    }
    if (rc_) {
        rsSurface->SetRenderContext(rc_);
    }
    auto frame = rsSurface->RequestFrame(width, height);
    framePtr = std::move(frame);
    if (!framePtr) {
        printf("DrawSurface frameptr is nullptr");
        return;
    }
    auto canvas = framePtr->GetCanvas();
    if (!canvas) {
        printf("DrawSurface canvas is nullptr");
        return;
    }
    Drawing::Brush brush;
    brush.SetAntiAilas(true);
    brush.SetColor(color);

    canvas->AttachBrush(brush);
    canvas->DrawRect(shapeGeometry);
    canvas->DetachBrush();
    framePtr->SetDamageRegion(0, 0, width, height);
    auto framePtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framePtr1);
}

int main()
{
    std::cout << "rs app demo start!" << std::endl;

    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowRect({0, 0, 2560, 112});

    auto scene = new WindowScene();

    std::shared_ptr<AbilityRuntime::Context> context = nullptr;
    sptr<IWindowLifeCycle> listener = nullptr;
    scene->Init(0, context, listener, option);
    auto window = scene->GetMainWindow();
    scene->GoForeground();
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    rsUiDirector->SetUITaskRunner(
        [handler](const std::function<void()>& task) {
            handler->PostTask(task);
        });
    runner->Run();

    RSTransaction::FlushImplicitTransaction();
    DrawSurface(Drawing::Rect(0, 0, 2800, 1600), 0xffffe4c4, Drawing::Rect(0, 0, 2800, 1600), surfaceNode);
    std::cout << "rs app demo set up finished!" << std::endl;
    RSTransaction::FlushImplicitTransaction();
    sleep(5);

    std::cout << "adding animation" << std::endl;

    RSTransaction::FlushImplicitTransaction();
    sleep(5);

    std::cout << "adding transition" << std::endl;
    auto animation2 = std::make_shared<RSTransition>(RSTransitionEffect::OPACITY, true);
    animation2->SetDuration(100);
    animation2->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    animation2->SetFinishCallback([]() {
        std::cout << "animation2 finish" << std::endl;
    });
    surfaceNode->AddAnimation(animation2);

    RSTransaction::FlushImplicitTransaction();
    sleep(5);

    std::cout << "rs app demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}
