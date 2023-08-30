/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <csignal>
#include <iostream>

#include "dm/display_manager.h"
#include "pipeline/rs_render_thread.h"
#include "ui/rs_surface_extractor.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

constexpr float X_RATIO = 0.5;
constexpr float Y_RATIO = 0.68;
constexpr float RADIUS = 200;
constexpr float MAX_ZORDER = 100000.0f;
constexpr int SLEEP_TIME = 5;
static volatile sig_atomic_t g_flag = 0;

static void Handler(int sig)
{
    g_flag = 1;
}

int main()
{
    signal(SIGINT, Handler);

    RSSurfaceNodeConfig config = {
        .SurfaceNodeName = "FingerprintSurfaceNodeTest"
    };

    auto renderContext = std::make_shared<RenderContext>();
    renderContext->InitializeEglContext();

    auto surfaceNode = RSSurfaceNode::Create(config, RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE);
    auto defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
    surfaceNode->SetBounds(0, 0, defaultDisplay->GetWidth(), defaultDisplay->GetHeight());

    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    rsSurface->SetRenderContext(renderContext.get());

    auto surfaceFrame = rsSurface->RequestFrame(defaultDisplay->GetWidth(), defaultDisplay->GetHeight());
    auto skSurface = surfaceFrame->GetSurface();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(skSurface.get());

    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorWHITE);
    canvas->drawCircle(SkPoint::Make(defaultDisplay->GetWidth() * X_RATIO,
        defaultDisplay->GetHeight() * Y_RATIO), RADIUS, paint);

    rsSurface->FlushFrame(surfaceFrame);
    surfaceNode->SetFingerprint(true);

    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    surfaceNode->SetPositionZ(MAX_ZORDER);
    surfaceNode->AttachToDisplay(id);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    transactionProxy->FlushImplicitTransaction();

    while (1) {
        if (g_flag) {
            cout << "Terminated by Ctrl+C." << endl;
            break;
        }
    }

    surfaceNode->DetachToDisplay(id);
    transactionProxy->FlushImplicitTransaction();
    sleep(SLEEP_TIME);
    return 0;
}
