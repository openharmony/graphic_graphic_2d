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

#ifndef FRAMEWORKS_WM_INCLUDE_SUBWINDOW_VIDEO_IMPL_H
#define FRAMEWORKS_WM_INCLUDE_SUBWINDOW_VIDEO_IMPL_H

#include <subwindow.h>
#include <videodisplaymanager.h>

#include "window_attribute.h"
#include "wl_subsurface.h"
#include "wl_surface.h"
#include "window_manager_impl.h"

namespace OHOS {
class SubwindowVideoImpl : public Subwindow {
public:
    ~SubwindowVideoImpl();

    GSError Init(const sptr<Window> &window,
                 const sptr<SubwindowOption> &option);

    virtual sptr<Surface> GetSurface() const override;

    virtual GSError Move(int32_t x, int32_t y) override;
    virtual GSError Resize(uint32_t width, uint32_t height) override;
    virtual GSError Destroy() override;

    virtual void OnPositionChange(WindowPositionChangeFunc func) override;
    virtual void OnSizeChange(WindowSizeChangeFunc func) override;

private:
    GSError CreateWlSurface(const sptr<WlSurface> &parentWlSurface);
    GSError CreateLayer();
    GSError CreateSHMBuffer();
    GSError CreateSurface();

    // base attribute
    WindowAttribute attr;
    wptr<Window> pw = nullptr;
    uint32_t layerId = 0;

    // functional member
    sptr<VideoDisplayManager> display = nullptr;
    sptr<Surface> csurf = nullptr;
    sptr<Surface> psurf = nullptr;
    sptr<WlSubsurface> wlSubsurf = nullptr;
    sptr<WlSurface> wlSurface = nullptr;
    sptr<WlBuffer> wlBuffer = nullptr;
    BeforeFrameSubmitFunc onBeforeFrameSubmitFunc = nullptr;
};
} // namespace OHOS
#endif // FRAMEWORKS_WM_INCLUDE_SUBWINDOW_VIDEO_IMPL_H
