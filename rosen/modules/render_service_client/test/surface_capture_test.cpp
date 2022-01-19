/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "display_type.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "pixel_map.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::unique_ptr<RSSurfaceFrame> framePtr;

void DrawSurface(
    SkRect surfaceGeometry, uint32_t color, SkRect shapeGeometry, std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto x = surfaceGeometry.x();
    auto y = surfaceGeometry.y();
    auto width = surfaceGeometry.width();
    auto height = surfaceGeometry.height();
    surfaceNode->SetBounds(x, y, width, height);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        return;
    }
    auto frame = rsSurface->RequestFrame(width, height);
    framePtr = std::move(frame);
    auto canvas = framePtr->GetCanvas();
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeWidth(20);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);

    canvas->drawRect(shapeGeometry, paint);
    framePtr->SetDamageRegion(0, 0, width, height);
    auto framePtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framePtr1);
}

void DrawSurfaceToCapture(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    SkRect surfaceGeometry = SkRect::MakeXYWH(100, 100, 256, 256);
    SkRect shapeGeometry = SkRect::MakeXYWH(40, 60, 100, 120);
    auto x = surfaceGeometry.x();
    auto y = surfaceGeometry.y();
    auto width = surfaceGeometry.width();
    auto height = surfaceGeometry.height();
    surfaceNode->SetBounds(x, y, width, height);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        ROSEN_LOGE("***SurfaceCaptureTest*** DrawSurfaceToCapture: rsSurface == nullptr");
        return;
    }
    auto frame = rsSurface->RequestFrame(width, height);
    framePtr = std::move(frame);

    auto canvas = framePtr->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("***SurfaceCaptureTest*** DrawSurfaceToCapture: canvas == nullptr");
        return;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeWidth(20);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(0xffff0000);
    SkPath path;
    path.cubicTo(20, 20, 50, 160, 120, 160);
    SkPaint pathPaint;
    pathPaint.setAntiAlias(true);
    pathPaint.setStyle(SkPaint::kFill_Style);
    pathPaint.setStrokeWidth(5);
    pathPaint.setStrokeJoin(SkPaint::kRound_Join);
    pathPaint.setColor(0xffFFD700);
    canvas->drawRect(shapeGeometry, paint);
    canvas->drawPath(path, pathPaint);
    framePtr->SetDamageRegion(0, 0, width, height);
    auto framePtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framePtr1);
}

void DrawPixelmap(std::shared_ptr<RSSurfaceNode> surfaceNode, std::shared_ptr<Media::PixelMap> pixelmap)
{
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        ROSEN_LOGE("***SurfaceCaptureTest*** : rsSurface == nullptr");
        return;
    }
    if (pixelmap == nullptr) {
        return;
    }
    int width = pixelmap->GetWidth();
    int height = pixelmap->GetHeight();
    int sWidth = surfaceNode->GetStagingProperties().GetBoundsWidth();
    int sHeight = surfaceNode->GetStagingProperties().GetBoundsHeight();
    ROSEN_LOGD("SurfaceCaptureTest: DrawPxielmap [%u][%u][%u][%u]", width, height, sWidth, sHeight);
    auto frame = rsSurface->RequestFrame(sWidth, sHeight);
    if (frame == nullptr) {
        ROSEN_LOGE("***SurfaceCaptureTest*** : frame == nullptr");
        return;
    }
    framePtr = std::move(frame);
    auto canvas = framePtr->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("***SurfaceCaptureTest*** : canvas == nullptr");
        return;
    }
    canvas->drawColor(0xFF87CEEB);
    SkImageInfo layerInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    auto addr = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (addr == nullptr) {
        ROSEN_LOGE("***SurfaceCaptureTest*** : addr == nullptr");
        return;
    }
    SkPixmap pixmap(layerInfo, addr, pixelmap->GetRowBytes());
    SkBitmap bitmap;
    if (bitmap.installPixels(pixmap)) {
        canvas->drawBitmapRect(bitmap, SkRect::MakeXYWH(0, 0, sWidth, sHeight), nullptr);
    }
    framePtr->SetDamageRegion(0, 0, sWidth, sHeight);
    auto framePtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framePtr1);
}


std::shared_ptr<RSSurfaceNode> CreateSurface()
{
    RSSurfaceNodeConfig config;
    return RSSurfaceNode::Create(config);
}

// Toy DMS.
using DisplayId = ScreenId;
class MyDMS {
public:
    MyDMS() : rsInterface_(RSInterfaces::GetInstance())
    {
        Init();
    }
    ~MyDMS() noexcept = default;
    RSInterfaces& rsInterface_;

    DisplayId GetDefaultDisplayId()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return defaultDisplayId_;
    }

    std::optional<RSScreenModeInfo> GetDisplayActiveMode(DisplayId id) const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            cout << "MyDMS: No display " << id << "!" << endl;
            return {};
        }
        return displays_.at(id).activeMode;
    }

    void OnDisplayConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displays_[id] = Display { id, rsInterface_.GetScreenActiveMode(id) };
        std::cout << "MyDMS: Display " << id << " connected." << endl;
    }

    void OnDisplayDisConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            cout << "MyDMS: No display " << id << "!" << endl;
        } else {
            std::cout << "MyDMS: Display " << id << " disconnected." << endl;
            displays_.erase(id);
            if (id == defaultDisplayId_) {
                defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
                std::cout << "MyDMS: DefaultDisplayId changed, new DefaultDisplayId is" << defaultDisplayId_ << "."
                          << endl;
            }
        }
    }

    private:
    struct Display {
        DisplayId id;
        RSScreenModeInfo activeMode;
    };
    std::unordered_map<DisplayId, Display> displays_;
    mutable std::recursive_mutex mutex_;
    DisplayId defaultDisplayId_;

    void Init()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        rsInterface_.SetScreenChangeCallback([this](ScreenId id, ScreenEvent event) {
            switch (event) {
                case ScreenEvent::CONNECTED: {
                    this->OnDisplayConnected(id);
                    break;
                }
                case ScreenEvent::DISCONNECTED: {
                    this->OnDisplayDisConnected(id);
                    break;
                }
                default:
                    break;
            }
        });

        defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
        displays_[defaultDisplayId_] =
            Display { defaultDisplayId_, rsInterface_.GetScreenActiveMode(defaultDisplayId_) };
    }
};
MyDMS g_dms;

int main()
{
    DisplayId id = g_dms.GetDefaultDisplayId();
    cout << "RS default screen id is " << id << ".\n";
    ROSEN_LOGD("***SurfaceCaptureTest*** main: begin");

    auto surfaceLauncher = CreateSurface();
    auto surfaceNode1 = CreateSurface();
    auto surfaceNode2 = CreateSurface();
    auto surfaceNode3 = CreateSurface();
    DrawSurface(SkRect::MakeXYWH(0, 0, 1400, 1200), 0xFFF0FFF0, SkRect::MakeXYWH(0, 0, 1400, 1200), surfaceLauncher);
    DrawSurface(SkRect::MakeXYWH(500, 100, 512, 512), 0xFF8B008B, SkRect::MakeXYWH(20, 20, 230, 230), surfaceNode1);
    DrawSurface(SkRect::MakeXYWH(1700, 100, 512, 512), 0xFF00FF40, SkRect::MakeXYWH(20, 20, 230, 230), surfaceNode3);
    DrawSurfaceToCapture(surfaceNode2);
    RSDisplayNodeConfig config;
    config.screenId = id;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    displayNode->AddChild(surfaceLauncher, -1);
    displayNode->AddChild(surfaceNode1, -1);
    displayNode->AddChild(surfaceNode2, -1);
    displayNode->AddChild(surfaceNode3, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();

    class TestSurfaceCapture : public SurfaceCaptureCallback {
    public:
        TestSurfaceCapture(std::shared_ptr<RSSurfaceNode> surfaceNode) : showNode_(surfaceNode) {}
        ~TestSurfaceCapture() override {}
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
        {
            DrawPixelmap(showNode_, pixelmap);
        }
    private:
        std::shared_ptr<RSSurfaceNode> showNode_;
    };
    sleep(2);
    std::shared_ptr<SurfaceCaptureCallback> cb = make_shared<TestSurfaceCapture>(surfaceNode1);
    g_dms.rsInterface_.TakeSurfaceCapture(surfaceNode2, cb);
    sleep(2);
    std::shared_ptr<SurfaceCaptureCallback> cb2 = make_shared<TestSurfaceCapture>(surfaceNode3);
    g_dms.rsInterface_.TakeSurfaceCapture(displayNode, cb2);
    ROSEN_LOGD("***SurfaceCaptureTest*** main: end");
    return 0;
}