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

#ifndef FRAMEWORKS_WM_INCLUDE_WINDOW_IMPL_H
#define FRAMEWORKS_WM_INCLUDE_WINDOW_IMPL_H

#include <window.h>

#include <mutex>

#include <window_manager_service_client.h>

#include "wl_surface.h"
#include "window_attribute.h"
#include "window_option_impl.h"

namespace OHOS {
class WindowImpl : public Window, public IBufferConsumerListenerClazz {
public:
    static GSError Create(sptr<Window> &window,
                          const sptr<WindowOption> &option,
                          const sptr<IWindowManagerService> &wms);

    sptr<WlSurface> GetWlSurface() const;

    virtual sptr<Surface> GetSurface() const override;
    virtual sptr<IBufferProducer> GetProducer() const override;
    virtual int32_t       GetID() const override;
    virtual int32_t       GetX() const override;
    virtual int32_t       GetY() const override;
    virtual uint32_t      GetWidth() const override;
    virtual uint32_t      GetHeight() const override;
    virtual uint32_t      GetDestWidth() const override;
    virtual uint32_t      GetDestHeight() const override;
    virtual bool          GetVisibility() const override;
    virtual WindowType    GetType() const override;
    virtual WindowMode    GetMode() const override;

    virtual sptr<Promise<GSError>> Show() override;
    virtual sptr<Promise<GSError>> Hide() override;
    virtual sptr<Promise<GSError>> Move(int32_t x, int32_t y) override;
    virtual sptr<Promise<GSError>> SwitchTop() override;
    virtual sptr<Promise<GSError>> SetWindowType(WindowType type) override;
    virtual sptr<Promise<GSError>> SetWindowMode(WindowMode mode) override;
    virtual sptr<Promise<GSError>> Resize(uint32_t width, uint32_t height) override;
    virtual sptr<Promise<GSError>> ScaleTo(uint32_t width, uint32_t height) override;
    virtual GSError Rotate(WindowRotateType type) override;
    virtual GSError Destroy() override;

    // prop listener
    virtual void OnPositionChange(WindowPositionChangeFunc func) override;
    virtual void OnSizeChange(WindowSizeChangeFunc func) override;
    virtual void OnVisibilityChange(WindowVisibilityChangeFunc func) override;
    virtual void OnTypeChange(WindowTypeChangeFunc func) override;
    virtual void OnModeChange(WindowModeChangeFunc func) override;
    virtual void OnSplitStatusChange(SplitStatusChangeFunc func) override;

    // pipMode
    virtual bool GetPIPMode() const override;
    virtual GSError EnterPIPMode(int32_t x, int32_t y,
                                 uint32_t width, uint32_t height) override;
    virtual GSError ExitPIPMode() override;
    virtual GSError OnPIPModeChange(WindowPIPModeChangeFunc func) override;

    // listener
    virtual GSError OnTouch(OnTouchFunc cb) override;
    virtual GSError OnKey(OnKeyFunc cb) override;

private:
    WindowImpl() = default;
    virtual ~WindowImpl() override;

    virtual void OnBufferAvailable() override;

    static GSError CheckAndNew(sptr<WindowImpl> &wi,
                               const sptr<WindowOption> &option,
                               const sptr<IWindowManagerService> &wms);

    static GSError CreateRemoteWindow(sptr<WindowImpl> &wi,
                                      const sptr<WindowOption> &option);

    static GSError CreateConsumerSurface(sptr<WindowImpl> &wi,
                                         const sptr<WindowOption> &option);

    // base attribute
    std::mutex mutex;
    WindowAttribute attr;
    bool isDestroyed = false;

    // pip attribute
    struct {
        int32_t x;
        int32_t y;
        uint32_t w;
        uint32_t h;
        WindowMode mode;
    } pipBackup;

    // functional member
    sptr<IWindowManagerService> wms = nullptr;
    sptr<WlSurface> wlSurface = nullptr;
    sptr<Surface> csurf = nullptr;
    sptr<Surface> psurf = nullptr;

    BeforeFrameSubmitFunc onBeforeFrameSubmitFunc = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_IMPL_H
