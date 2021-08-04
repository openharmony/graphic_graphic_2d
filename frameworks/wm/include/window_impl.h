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

#include "log_listener.h"
#include "multimodal_listener_manager.h"
#include "wl_surface.h"
#include "window_attribute.h"
#include "window_option_impl.h"

namespace OHOS {
class WindowImpl : public Window, public IBufferConsumerListenerClazz {
public:
    static WMError Create(sptr<Window> &window,
                          const sptr<WindowOption> &option,
                          const sptr<IWindowManagerService> &wms);

    sptr<WlSurface> GetWlSurface() const;

    virtual sptr<Surface> GetSurface() const override;
    virtual int32_t GetID() const override;

    virtual sptr<Promise<WMError>> Show() override;
    virtual sptr<Promise<WMError>> Hide() override;
    virtual sptr<Promise<WMError>> Move(int32_t x, int32_t y) override;
    virtual sptr<Promise<WMError>> SwitchTop() override;
    virtual sptr<Promise<WMError>> SetWindowType(WindowType type) override;
    virtual sptr<Promise<WMError>> SetWindowMode(WindowMode mode) override;
    virtual sptr<Promise<WMError>> Resize(uint32_t width, uint32_t height) override;
    virtual sptr<Promise<WMError>> ScaleTo(uint32_t width, uint32_t height) override;
    virtual WMError Rotate(WindowRotateType type) override;
    virtual WMError Destroy() override;

    // prop listener
    virtual void OnPositionChange(WindowPositionChangeFunc func) override;
    virtual void OnSizeChange(WindowSizeChangeFunc func) override;
    virtual void OnVisibilityChange(WindowVisibilityChangeFunc func) override;
    virtual void OnTypeChange(WindowTypeChangeFunc func) override;

    // listener
    virtual WMError OnTouch(OnTouchFunc cb) override;
    virtual WMError OnKey(OnKeyFunc cb) override;

    // pointer listener
    virtual WMError OnPointerEnter(PointerEnterFunc func) override;
    virtual WMError OnPointerLeave(PointerLeaveFunc func) override;
    virtual WMError OnPointerMotion(PointerMotionFunc func) override;
    virtual WMError OnPointerButton(PointerButtonFunc func) override;
    virtual WMError OnPointerFrame(PointerFrameFunc func) override;
    virtual WMError OnPointerAxis(PointerAxisFunc func) override;
    virtual WMError OnPointerAxisSource(PointerAxisSourceFunc func) override;
    virtual WMError OnPointerAxisStop(PointerAxisStopFunc func) override;
    virtual WMError OnPointerAxisDiscrete(PointerAxisDiscreteFunc func) override;

    // keyboard listener
    virtual WMError OnKeyboardKeymap(KeyboardKeymapFunc func) override;
    virtual WMError OnKeyboardEnter(KeyboardEnterFunc func) override;
    virtual WMError OnKeyboardLeave(KeyboardLeaveFunc func) override;
    virtual WMError OnKeyboardKey(KeyboardKeyFunc func) override;
    virtual WMError OnKeyboardModifiers(KeyboardModifiersFunc func) override;
    virtual WMError OnKeyboardRepeatInfo(KeyboardRepeatInfoFunc func) override;

    // touch listener
    virtual WMError OnTouchDown(TouchDownFunc func) override;
    virtual WMError OnTouchUp(TouchUpFunc func) override;
    virtual WMError OnTouchMotion(TouchMotionFunc func) override;
    virtual WMError OnTouchFrame(TouchFrameFunc func) override;
    virtual WMError OnTouchCancel(TouchCancelFunc func) override;
    virtual WMError OnTouchShape(TouchShapeFunc func) override;
    virtual WMError OnTouchOrientation(TouchOrientationFunc func) override;

private:
    WindowImpl() = default;
    virtual ~WindowImpl() override;

    virtual void OnBufferAvailable() override;

    static WMError CheckAndNew(sptr<WindowImpl> &wi,
                               const sptr<WindowOption> &option,
                               const sptr<IWindowManagerService> &wms);

    static WMError CreateRemoteWindow(sptr<WindowImpl> &wi,
                                      const sptr<WindowOption> &option);

    static WMError CreateConsumerSurface(sptr<WindowImpl> &wi,
                                         const sptr<WindowOption> &option);

    // base attribute
    std::mutex mutex;
    WindowAttribute attr;
    bool isDestroyed = false;

    // functional member
    sptr<IWindowManagerService> wms = nullptr;
    sptr<WlSurface> wlSurface = nullptr;
    sptr<Surface> csurface = nullptr;
    sptr<Surface> psurface = nullptr;
    sptr<WindowOption> testParam = nullptr;

    sptr<InputListener> logListener = nullptr;
    sptr<MultimodalListener> mmiListener = nullptr;
    sptr<InputListener> exportListener = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_IMPL_H
