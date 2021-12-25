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

#include "cursor_module.h"

#include <display_type.h>
#include <gslogger.h>
#include <multimodal_event_handler.h>
#include <unistd.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("CursorModule")

inline int32_t max(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

inline int32_t min(int32_t a, int32_t b)
{
    return a < b ? a : b;
}
} // namespace

GSError CursorModule::Init()
{
    if (access("/data/cursor_enable", 0) == -1) {
        return GSERROR_NOT_SUPPORT;
    }

    handler = AppExecFwk::EventHandler::Current();
    auto wm = WindowManager::GetInstance();
    wm->GetDisplays(displays);
    if (displays.size() <= 0) {
        GSLOG2HI(ERROR) << "displays.size() == 0, no screen";
        return GSERROR_NO_SCREEN;
    }

    auto ret = resource.Parse(cursorFilepath);
    if (ret) {
        GSLOG2HI(ERROR) << "resource Parse failed with " << ret;
        return GSERROR_INTERNEL;
    }

    auto option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_CURSOR);
    option->SetX(displays[0].width / 0x2);
    option->SetY(displays[0].height / 0x2);
    option->SetWidth(resource.GetWidth());
    option->SetHeight(resource.GetHeight());
    auto gret = wm->CreateWindow(window, option);
    if (gret) {
        GSLOG2HI(ERROR) << "CreateWindow failed with " << gret;
        return gret;
    }

    window->SwitchTop();
    MMIEventHdl.RegisterStandardizedEventHandle(token_, window->GetID(), this);
    handler->PostTask(std::bind(&CursorModule::Update, this));
    return GSERROR_OK;
}

void CursorModule::Update()
{
    auto surface = window->GetSurface();
    sptr<SurfaceBuffer> buffer = nullptr;
    BufferRequestConfig rconfig = {
        .width = window->GetWidth(),
        .height = window->GetHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surface->GetDefaultUsage(),
    };
    auto ret = surface->RequestBufferNoFence(buffer, rconfig);
    if (ret != SURFACE_ERROR_OK) {
        GSLOG2HI(ERROR) << "RequestBufferNoFence failed with " << ret;
        return;
    }

    resource.GetNextData(reinterpret_cast<uint32_t *>(buffer->GetVirAddr()));
    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surface->FlushBuffer(buffer, -1, fconfig);
}

bool CursorModule::OnTouch(const TouchEvent &event)
{
    auto pmevent = reinterpret_cast<const MouseEvent *>(event.GetMultimodalEvent());
    if (pmevent != nullptr) {
        auto &mevent = *pmevent;
        OHOS::MmiPoint mmiPoint = mevent.GetCursor();
        if (mevent.GetAction() == 0) {
            int32_t x = window->GetX() + mmiPoint.GetX();
            int32_t y = window->GetY() + mmiPoint.GetY();
            x = min(max(x, 0), displays[0].width);
            y = min(max(y, 0), displays[0].height);
            window->Move(x, y);
        }
    } else {
        GSLOG2SO(ERROR) << "mouseEvent is nullptr";
    }
    return false;
}
} // namespace OHOS
