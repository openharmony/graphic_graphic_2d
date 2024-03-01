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

#include "skia_framework.h"

#include <chrono>
#include <cmath>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkPath.h>
#include <include/core/SkSurface.h>
#include <include/core/SkTextBlob.h>

#include <hitrace_meter.h>
#include <input_manager.h>
#include <securec.h>
#include <transaction/rs_interfaces.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_node.h>
#include <utils/log.h>

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std::chrono_literals;
using MMIPE = MMI::PointerEvent;

#define TRACE_BEGIN(str) StartTrace(HITRACE_TAG_GRAPHIC_AGP, str)
#define TRACE_END() FinishTrace(HITRACE_TAG_GRAPHIC_AGP)
#define TRACE_COUNT(str, val) CountTrace(HITRACE_TAG_GRAPHIC_AGP, str, val)
#define TRACE_SCOPE(str) HitraceScoped trace(HITRACE_TAG_GRAPHIC_AGP, str)

struct TestData {
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<VSyncReceiver> receiver = nullptr;
    std::shared_ptr<RSSurfaceNode> sNode = nullptr;
    std::shared_ptr<RSDisplayNode> dNode = nullptr;
    std::function<void(void)> onVsync = nullptr;
    int64_t lastTime = 0;

    void RequestVsync();
};

void TestData::RequestVsync()
{
    TRACE_COUNT("RequestVsync", 1);
    auto func = [this](int64_t ts, void *data) {
        TRACE_COUNT("RequestVsync", 0);
        (void) ts;
        (void) data;
        if (this->onVsync != nullptr) {
            this->onVsync();
        }
    };
    receiver->RequestNextVSync({.callback_ = func});
}

class PointerFilter : public OHOS::MMI::IInputEventFilter {
public:
    PointerFilter(SkiaFramework* skiaFramework) : sf(skiaFramework) {}

    bool OnInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) const override {return false;}
    bool OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const override;

private:
    void ProcessSinglePointerEvent(const std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const;
    void ProcessPointerEvents(const std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const;
    void ProcessSingleAction(const uint32_t action, int x, int y) const;

    SkiaFramework* sf = nullptr;
};

bool PointerFilter::OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const
{
    TRACE_SCOPE("HandleInput");
    std::lock_guard<std::mutex> lock(sf->propsMutex_);

    TRACE_BEGIN("HandleInputLocked");
    auto &rsdata = *reinterpret_cast<struct TestData *>(sf->data_);
    const auto &ids = pointerEvent->GetPointerIds();

    if (ids.size() == 1) {
        ProcessSinglePointerEvent(pointerEvent);
    }

    if (ids.size() >= 2) {  // 2 means not single pointer event
        ProcessPointerEvents(pointerEvent);
    }

    rsdata.RequestVsync();
    pointerEvent->MarkProcessed();
    TRACE_END();
    return true;
}

void PointerFilter::ProcessSinglePointerEvent(const std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const
{
    const auto &ids = pointerEvent->GetPointerIds();
    const auto &action = pointerEvent->GetPointerAction();
    MMI::PointerEvent::PointerItem firstPointer = {};
    pointerEvent->GetPointerItem(ids[0], firstPointer);
    const auto &x = firstPointer.GetDisplayX();
    const auto &y = firstPointer.GetDisplayY();

    if (action == MMIPE::POINTER_ACTION_DOWN) {
        auto &rsdata = *reinterpret_cast<struct TestData*>(sf->data_);
        // 400000 means 400 * 1000 is the discriminant time for consecutive clicks
        if (pointerEvent->GetActionTime() - rsdata.lastTime <= 400000) {
            sf->right_ = false;
            sf->left_ = true;
        }

        if (sf->right_ == false && sf->left_ == false) {
            sf->right_ = true;
        }
        rsdata.lastTime = pointerEvent->GetActionTime();
    }
    ProcessSingleAction(action, x, y);
}

void PointerFilter::ProcessSingleAction(const uint32_t action, int x, int y) const
{
    if (sf->left_ && action == MMIPE::POINTER_ACTION_DOWN) {
        sf->dirty_ = true;
        sf->clickx_ = x;
        sf->clicky_ = y;
    }
    if (sf->left_ && action == MMIPE::POINTER_ACTION_MOVE) {
        sf->dirty_ = true;
        sf->x_ = x;
        sf->y_ = y;
    }
    if (sf->left_ && action == MMIPE::POINTER_ACTION_UP) {
        sf->dirty_ = true;
        sf->left_ = false;
    }

    if (sf->right_ && action == MMIPE::POINTER_ACTION_DOWN) {
        sf->dirty_ = true;
        sf->downRX_ = x;
        sf->downRY_ = y;
    }
    if (sf->right_ && action == MMIPE::POINTER_ACTION_MOVE) {
        sf->dirty_ = true;
        sf->x_ = x;
        sf->y_ = y;

        sf->mat_ = Drawing::Matrix();
        sf->mat_.Translate(-sf->diffx_, -sf->diffy_);
        sf->mat_.PreConcat(sf->mat_);
        sf->diffx_ = sf->x_ - sf->downRX_;
        sf->diffy_ = sf->y_ - sf->downRY_;
        sf->mat_.Translate(sf->diffx_, sf->diffy_);
        sf->mat_.PreConcat(sf->mat_);
        sf->UpdateInvertMatrix();
    }
    if (sf->right_ && action == MMIPE::POINTER_ACTION_UP) {
        sf->dirty_ = true;
        sf->right_ = false;
        sf->mat_ = Drawing::Matrix();
        sf->mat_.Translate(-sf->diffx_, -sf->diffy_);
        sf->mat_.PreConcat(sf->mat_);
        sf->mat_.Translate(x - sf->downRX_, y - sf->downRY_);
        sf->mat_.PreConcat(sf->mat_);
        sf->UpdateInvertMatrix();
        sf->diffx_ = sf->diffy_ = 0;
    }
}

void PointerFilter::ProcessPointerEvents(const std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const
{
    const auto &ids = pointerEvent->GetPointerIds();
    const auto &action = pointerEvent->GetPointerAction();
    MMI::PointerEvent::PointerItem firstPointer = {};
    MMI::PointerEvent::PointerItem secondPointer = {};
    sf->right_ = false;
    sf->left_ = false;

    pointerEvent->GetPointerItem(ids[0], firstPointer);
    pointerEvent->GetPointerItem(ids[1], secondPointer);
    const auto &x1 = firstPointer.GetDisplayX();
    const auto &y1 = firstPointer.GetDisplayY();
    const auto &x2 = secondPointer.GetDisplayX();
    const auto &y2 = secondPointer.GetDisplayY();

    if (action == MMIPE::POINTER_ACTION_DOWN) {
        // 2 is to compute the middle position
        sf->scalex_ = (x1 + x2) / 2;
        sf->scaley_ = (y1 + y2) / 2;    // 2 means divisor, get the middle position of Y axle
        sf->scalediff_ = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
        sf->scaleMat_ = sf->mat_;
    }
    if (action == MMIPE::POINTER_ACTION_MOVE || action == MMIPE::POINTER_ACTION_UP) {
        sf->dirty_ = true;
        auto scalediff = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
        double scale = 1.0;
        double epsilon = 1e-10;
        if (fabs(sf->scalediff_.load()) > epsilon) {
            scale = (scalediff / sf->scalediff_.load()) == 0 ?
                1 : scalediff / sf->scalediff_.load();
        }
        Drawing::Matrix scaleMatrix;
        scaleMatrix.SetScale(scale, scale);
        sf->mat_.PreConcat(scaleMatrix);
        sf->UpdateInvertMatrix();
    }
}

SkiaFramework::SkiaFramework()
{
    UpdateTraceLabel();
    data_ = new struct TestData();
}

SkiaFramework::~SkiaFramework()
{
    auto &rsdata = *reinterpret_cast<struct TestData *>(data_);
    rsdata.dNode->RemoveChild(rsdata.sNode);
    rsdata.sNode = nullptr;
    rsdata.dNode = nullptr;

    if (auto tp = RSTransactionProxy::GetInstance(); tp != nullptr) {
        tp->FlushImplicitTransaction();
    }
    delete reinterpret_cast<struct TestData *>(data_);
}

void SkiaFramework::SetDrawFunc(const DrawFunc &onDraw)
{
    onDraw_ = onDraw;
}

void SkiaFramework::SetResizeFunc(const ResizeFunc &onResize)
{
    onResize_ = onResize;
}

void SkiaFramework::SetGPUAllowance(bool allow)
{
    allowGPU_ = allow;
}

void SkiaFramework::SetWindowWidth(int width)
{
    windowWidth_ = width;
}

void SkiaFramework::SetWindowHeight(int height)
{
    windowHeight_ = height;
}

void SkiaFramework::SetWindowScale(double scale)
{
    windowScale_ = scale;
}

void SkiaFramework::SetWindowTitle(const std::string &title)
{
    windowTitle_ = title;
}

int SkiaFramework::GetWindowWidth() const
{
    return windowWidth_;
}

int SkiaFramework::GetWindowHeight() const
{
    return windowHeight_;
}

double SkiaFramework::GetWindowScale() const
{
    return windowScale_;
}

void SkiaFramework::Run()
{
    TRACE_BEGIN("Run");
    auto &rsdata = *reinterpret_cast<struct TestData *>(data_);
    // vsync
    rsdata.runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(rsdata.runner);
    rsdata.receiver = RSInterfaces::GetInstance().CreateVSyncReceiver("vsync", handler);
    rsdata.receiver->Init();

    // input
    auto mmi = MMI::InputManager::GetInstance();
    if (mmi == nullptr) {
       return;
    }

    auto filter = std::make_shared<PointerFilter>(this);
    uint32_t touchTags = CapabilityToTags(OHOS::MMI::InputDeviceCapability::INPUT_DEV_CAP_MAX);
    mmi->AddInputEventFilter(filter, 220, touchTags); // the priority is 220
    rsdata.sNode = RSSurfaceNode::Create({});
    rsdata.dNode = RSDisplayNode::Create({});
    if (rsdata.sNode == nullptr || rsdata.dNode == nullptr) {
        return;
    }
    rsdata.sNode->SetBounds(0, 0, windowWidth_, windowHeight_);
    rsdata.sNode->SetAlpha(1); // 1 is opaque
    rsdata.dNode->AddChild(rsdata.sNode, -1); //child index is -1
    if (auto tp = RSTransactionProxy::GetInstance(); tp != nullptr) {
        tp->FlushImplicitTransaction();
    } else {
        return;
    }

    PrepareVsyncFunc();
    Drawing::Matrix scaleMatrix;
    scaleMatrix.SetScale(windowScale_, windowScale_);
    mat_.PreConcat(scaleMatrix);
    rsdata.RequestVsync();
    TRACE_END();
    rsdata.runner->Run();
}

void SkiaFramework::PrepareVsyncFunc()
{
    auto &rsdata = *reinterpret_cast<struct TestData *>(data_);
    rsdata.onVsync = [this]() {
        if (dirty_ == false) {
            return;
        }
        dirty_ = false;
        TRACE_SCOPE("OnVsync");
        auto &rsdata = *reinterpret_cast<struct TestData *>(data_);
        sptr<Surface> surface = rsdata.sNode->GetSurface();
        if (surface == nullptr) {
            return;
        }

        sptr<SurfaceBuffer> buffer;
        int32_t releaseFence;
        BufferRequestConfig config = {
            .width = windowWidth_,
            .height = windowHeight_,
            .strideAlignment = 0x8,
            .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        };

        SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
        LOGI("request buffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());

        if (buffer == nullptr) {
            LOGE("request buffer failed: buffer is nullptr");
            return;
        }
        if (buffer->GetVirAddr() == nullptr) {
            LOGE("get virAddr failed: virAddr is nullptr");
            return;
        }

        auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
        LOGI("buffer width:%{public}d, height:%{public}d", buffer->GetWidth(), buffer->GetHeight());
        Drawing::Bitmap bitmap;
        ProcessBitmap(bitmap, buffer);
        constexpr uint32_t stride = 4;
        uint32_t addrSize = buffer->GetWidth() * buffer->GetHeight() * stride;
        void* bitmapAddr = bitmap.GetPixels();
        if (auto res = memcpy_s(addr, addrSize, bitmapAddr, addrSize); res != EOK) {
            LOGI("memcpy_s failed");
        }

        BufferFlushConfig flushConfig = { .damage = { .w = buffer->GetWidth(), .h = buffer->GetHeight(), }, };
        ret = surface->FlushBuffer(buffer, -1, flushConfig);
        LOGI("flushBuffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());
    };
}
void SkiaFramework::ProcessBitmap(Drawing::Bitmap &bitmap, const sptr<SurfaceBuffer> buffer)
{
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE};
    bitmap.Build(buffer->GetWidth(), buffer->GetHeight(), format);
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.ResetMatrix();
    canvas.Save();
    canvas.Clear(SK_ColorWHITE);
    canvas.SetMatrix(mat_);
    DrawBefore(canvas);
    if (onDraw_) {
        TRACE_SCOPE("OnDraw");
        onDraw_(canvas);
    }

    canvas.Restore();
    DrawColorPicker(canvas, bitmap);
    DrawAfter(canvas);
}

void SkiaFramework::UpdateInvertMatrix()
{
    if (auto ret = mat_.Invert(invmat_); ret == false) {
        invmat_ = Drawing::Matrix();
    }
}

void SkiaFramework::DrawString(RSCanvas &canvas, const std::string &str, double x, double y)
{
    RSBrush textbrush;
    textbrush.SetAntiAlias(true);
    textbrush.SetColor(0xff0066ff); // color is 0xff0066ff

    RSFont font;
    font.SetTypeface(RSTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.SetSize(16); // font size is 16
}


void SkiaFramework::DrawBefore(RSCanvas &canvas)
{
    TRACE_SCOPE("DrawBefore");
    std::lock_guard<std::mutex> lock(propsMutex_);
    Drawing::Pen paint;
    paint.SetAntiAlias(true);
    paint.SetColor(0x09000000);
    paint.SetWidth(1);
    Drawing::Pen paint2 = paint;
    paint2.SetColor(0x22000000);

    Drawing::Pen textPaint;
    textPaint.SetAntiAlias(true);
    textPaint.SetColor(0xff00007f);

    RSFont font;
    font.SetTypeface(RSTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.SetSize(16);   // font size is 16
    DrawPathAndString(canvas, font, paint, textPaint);
}

void SkiaFramework::DrawPathAndString(RSCanvas &canvas, RSFont &font, Drawing::Pen &paint1, Drawing::Pen &paint2)
{
    Drawing::Rect clipRect;
    invmat_.MapRect(clipRect, Drawing::Rect(0, 0, windowWidth_, windowHeight_));
    auto left = static_cast<int>(clipRect.GetLeft()) / 100 * 100; // Make it an integer multiple of 100
    auto right = static_cast<int>(clipRect.GetRight());
    auto top = static_cast<int>(clipRect.GetTop()) / 100 * 100; // Make it an integer multiple of 100
    auto bottom = static_cast<int>(clipRect.GetBottom());
    Drawing::Pen paint3 = paint1;
    paint3.SetColor(0x22000000);

    Drawing::Path path;
    // Draw grids, 20 * 20 grids and 100 * 100 grids
    for (int i = left; i <= right; i += 20) {
        path.MoveTo(i, -1e9);
        path.LineTo(i, 1e9);
    }

    for (int i = top; i <= bottom; i += 20) {   // 20 means draw 20 * 20 grids
        path.MoveTo(-1e9, i);
        path.LineTo(1e9, i);
    }
    canvas.AttachPen(paint3);
    canvas.DrawPath(path);
    canvas.DetachPen();
}

void SkiaFramework::DrawColorPicker(RSCanvas &canvas, Drawing::Bitmap &bitmap)
{
    if (left_ == false) {
        return;
    }

    TRACE_SCOPE("DrawColorPicker");
    std::lock_guard<std::mutex> lock(propsMutex_);
    RSFont font;
    font.SetTypeface(RSTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.SetSize(24);   // font size is 24

    Drawing::Pen paint;
    paint.SetAntiAlias(true);
    Drawing::Pen paint2;
    paint2.SetAntiAlias(true);
    paint2.SetColor(SK_ColorBLACK);

    auto color = bitmap.GetColor(x_, y_);
    paint.SetColor(color);
}

void SkiaFramework::DrawAfter(RSCanvas &canvas)
{
    if (left_ == false) {
        return;
    }

    TRACE_SCOPE("DrawAfter");
    std::lock_guard<std::mutex> lock(propsMutex_);
    Drawing::Pen paint;
    paint.SetAntiAlias(true);
    paint.SetColor(0x33000000);
    paint.SetWidth(1);

    Drawing::Pen textPaint;
    textPaint.SetAntiAlias(true);
    textPaint.SetColor(0xff0000ff);
    textPaint.SetWidth(1);

    RSFont font;
    font.SetTypeface(RSTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.SetSize(16);   // font size is 16

    Drawing::Path path;
    path.MoveTo(x_, 0);
    path.LineTo(x_, 1e9);
    path.MoveTo(0, y_);
    path.LineTo(1e9, y_);

    canvas.AttachPen(paint);
    canvas.DrawPath(path);
    canvas.DetachPen();
}
