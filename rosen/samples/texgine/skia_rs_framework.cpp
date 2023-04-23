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

struct RSData {
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    std::shared_ptr<RSSurfaceNode> snode_ = nullptr;
    std::shared_ptr<RSDisplayNode> dnode_ = nullptr;
    std::function<void(void)> onVsync_ = nullptr;
    int64_t lastTime_ = 0;

    void RequestVsync()
    {
        TRACE_COUNT("RequestVsync", 1);
        auto func = [this](int64_t ts, void *data) {
            TRACE_COUNT("RequestVsync", 0);
            (void) ts;
            (void) data;
            if (this->onVsync_ != nullptr) {
                this->onVsync_();
            }
        };
        receiver_->RequestNextVSync({.callback_ = func});
    }
};

SkiaFramework::SkiaFramework()
{
    UpdateTraceLabel();
    data_ = new struct RSData();
}

SkiaFramework::~SkiaFramework()
{
    auto &rsdata = *reinterpret_cast<struct RSData *>(data_);
    rsdata.dnode_->RemoveChild(rsdata.snode_);
    rsdata.snode_ = nullptr;
    rsdata.dnode_ = nullptr;

    if (auto tp = RSTransactionProxy::GetInstance(); tp != nullptr) {
        tp->FlushImplicitTransaction();
    }
    delete reinterpret_cast<struct RSData *>(data_);
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
    auto &rsdata = *reinterpret_cast<struct RSData *>(data_);
    // vsync
    rsdata.runner_ = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(rsdata.runner_);
    rsdata.receiver_ = RSInterfaces::GetInstance().CreateVSyncReceiver("vsync", handler);
    rsdata.receiver_->Init();

    // input
    auto mmi = MMI::InputManager::GetInstance();
    if (mmi == nullptr) {
        return;
    }

    struct PointerFilter : public OHOS::MMI::IInputEventFilter {
        PointerFilter(SkiaFramework* skiaFramework) : sf(skiaFramework) {}

        bool OnInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) const override {return false;}
        bool OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const override
        {
            TRACE_SCOPE("HandleInput");
            std::lock_guard<std::mutex> lock(sf->propsMutex_);

            TRACE_BEGIN("HandleInputLocked");
            auto &rsdata = *reinterpret_cast<struct RSData *>(sf->data_);
            const auto &ids = pointerEvent->GetPointerIds();
            const auto &time = pointerEvent->GetActionTime();
            const auto &action = pointerEvent->GetPointerAction();
            MMI::PointerEvent::PointerItem firstPointer = {};
            MMI::PointerEvent::PointerItem secondPointer = {};

            if (ids.size() == 1) {
                pointerEvent->GetPointerItem(ids[0], firstPointer);
                const auto &x = firstPointer.GetDisplayX();
                const auto &y = firstPointer.GetDisplayY();

                if (action == MMIPE::POINTER_ACTION_DOWN) {
                    // 400 * 1000 is the discriminant time for consecutive clicks
                    if (time - rsdata.lastTime_ <= 400 * 1000) {
                        sf->right_ = false;
                        sf->left_ = true;
                    }

                    if (!sf->right_ && !sf->left_) {
                        sf->right_ = true;
                    }
                    rsdata.lastTime_ = pointerEvent->GetActionTime();
                }

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

                    sf->mat_ = SkMatrix::MakeTrans(-sf->diffx_, -sf->diffy_).preConcat(sf->mat_);
                    sf->diffx_ = sf->x_ - sf->downRX_;
                    sf->diffy_ = sf->y_ - sf->downRY_;
                    sf->mat_ = SkMatrix::MakeTrans(sf->diffx_, sf->diffy_).preConcat(sf->mat_);
                    sf->UpdateInvertMatrix();
                }
                if (sf->right_ && action == MMIPE::POINTER_ACTION_UP) {
                    sf->dirty_ = true;
                    sf->right_ = false;
                    sf->mat_ = SkMatrix::MakeTrans(-sf->diffx_, -sf->diffy_).preConcat(sf->mat_);
                    sf->mat_ = SkMatrix::MakeTrans(x - sf->downRX_, y - sf->downRY_).preConcat(sf->mat_);
                    sf->UpdateInvertMatrix();
                    sf->diffx_ = sf->diffy_ = 0;
                }
            }

            if (ids.size() >= 2) {
                sf->right_ = false;
                sf->left_ = false;

                pointerEvent->GetPointerItem(ids[0], firstPointer);
                pointerEvent->GetPointerItem(ids[1], secondPointer);
                const auto &x1 = firstPointer.GetDisplayX();
                const auto &y1 = firstPointer.GetDisplayY();
                const auto &x2 = secondPointer.GetDisplayX();
                const auto &y2 = secondPointer.GetDisplayY();

                if (action == MMIPE::POINTER_ACTION_DOWN) {
                    sf->scalex_ = (x1 + x2) / 2;
                    sf->scaley_ = (y1 + y2) / 2;
                    sf->scalediff_ = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
                    sf->scaleMat_ = sf->mat_;
                }
                if (action == MMIPE::POINTER_ACTION_MOVE || action == MMIPE::POINTER_ACTION_UP) {
                    sf->dirty_ = true;
                    auto scalediff = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
                    auto point = sf->invmat_.mapXY(sf->scalex_, sf->scaley_);
                    sf->mat_ = sf->scaleMat_;
                    sf->mat_ = sf->mat_.preConcat(SkMatrix::MakeTrans(+point.x(), +point.y()));
                    sf->mat_ = sf->mat_.preConcat(SkMatrix::MakeScale(scalediff /
                                (sf->scalediff_.load() != 0 ? sf->scalediff_.load() : 1)));
                    sf->mat_ = sf->mat_.preConcat(SkMatrix::MakeTrans(-point.x(), -point.y()));
                    sf->UpdateInvertMatrix();
                }
            }

            rsdata.RequestVsync();
            pointerEvent->MarkProcessed();
            TRACE_END();
            return true;
        }

        SkiaFramework* sf = nullptr;
    };
}
