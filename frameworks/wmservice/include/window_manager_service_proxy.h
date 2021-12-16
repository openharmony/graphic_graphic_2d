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

#ifndef FRAMEWORKS_WMSERVICE_INCLUDE_WINDOW_MANAGER_SERVICE_PROXY_H
#define FRAMEWORKS_WMSERVICE_INCLUDE_WINDOW_MANAGER_SERVICE_PROXY_H

#include <iwindow_manager_service.h>

#include <ianimation_service.h>
#include <map>
#include <mutex>
#include <queue>
#include <wms-client-protocol.h>

namespace OHOS {
class WindowManagerServiceProxy : public IWindowManagerService {
public:
    WindowManagerServiceProxy(struct wms *wms, struct wl_display *display, sptr<IAnimationService> &as);
    virtual ~WindowManagerServiceProxy() = default;

    virtual WMError GetDisplays(std::vector<struct WMDisplayInfo> &displays) override;
    virtual sptr<PromisePowerStatus> GetDisplayPower(int32_t did) override;
    virtual sptr<PromiseWMError> SetDisplayPower(int32_t did, DispPowerStatus status) override;
    virtual sptr<PromiseBacklight> GetDisplayBacklight(int32_t did) override;
    virtual sptr<PromiseWMError> SetDisplayBacklight(int32_t did, uint32_t level) override;
    virtual WMError GetDisplayModes(uint32_t &displayModes) override;
    virtual sptr<PromiseWMError> SetDisplayMode(WMSDisplayMode modes) override;
    virtual WMError AddDisplayChangeListener(IWindowManagerDisplayListenerClazz *listener) override;
    virtual sptr<PromiseWMError> OnWindowListChange(IWindowChangeListenerClazz *listener) override;

    virtual WMError SetDisplayDirection(WMSDisplayDirection direction) override;
    virtual WMError OnDisplayDirectionChange(DisplayDirectionChangeFunc func) override;

    virtual sptr<PromiseWMSImageInfo> ShotScreen(int32_t did) override;
    virtual sptr<PromiseWMSImageInfo> ShotWindow(int32_t wid) override;

    virtual sptr<PromiseWMError> SetStatusBarVisibility(bool visibility) override;
    virtual sptr<PromiseWMError> SetNavigationBarVisibility(bool visibility) override;

    virtual sptr<PromiseWMError> DestroyWindow(int32_t wid) override;
    virtual sptr<PromiseWMError> SwitchTop(int32_t wid) override;
    virtual sptr<PromiseWMError> Show(int32_t wid) override;
    virtual sptr<PromiseWMError> Hide(int32_t wid) override;
    virtual sptr<PromiseWMError> Move(int32_t wid, int32_t x, int32_t y) override;
    virtual sptr<PromiseWMError> Resize(int32_t wid, uint32_t width, uint32_t height) override;
    virtual sptr<PromiseWMError> ScaleTo(int32_t wid, uint32_t width, uint32_t height) override;
    virtual sptr<PromiseWMError> SetWindowType(int32_t wid, WindowType type) override;
    virtual sptr<PromiseWMError> SetWindowMode(int32_t wid, WindowMode mode) override;

    virtual sptr<PromiseWMError> CreateVirtualDisplay(int32_t x, int32_t y, int32_t width, int32_t height) override;
    virtual sptr<PromiseWMError> DestroyVirtualDisplay(uint32_t did) override;

    virtual GSError StartRotationAnimation(uint32_t did, int32_t degree) override;

    static void OnReply(wms_error);
    static void OnDisplayChange(uint32_t, const char *, wms_screen_status, int32_t, int32_t, wms_screen_type type);
    static void OnDisplayPower(uint32_t, int32_t);
    static void OnDisplayBacklight(uint32_t, uint32_t);
    static void OnDisplayModeChange(uint32_t);
    static void OnGlobalWindowStatus(uint32_t pid, uint32_t wid, uint32_t status);
    static void OnScreenShot(wms_error, uint32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    static void OnWindowShot(wms_error, uint32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);

private:
    struct wms *wms = nullptr;
    struct wl_display *display = nullptr;
    sptr<IAnimationService> as = nullptr;
    static inline IWindowManagerDisplayListenerClazz *displayListener = nullptr;
    static inline IWindowChangeListenerClazz *globalWindowChangeListener = nullptr;

    using SptrPromiseWMError = sptr<PromiseWMError>;
    using SptrPromiseWMSImageInfo = sptr<PromiseWMSImageInfo>;
    using SptrPromiseBacklight = sptr<PromiseBacklight>;
    using SptrPromisePowerStatus = sptr<PromisePowerStatus>;

    static inline std::vector<struct WMDisplayInfo> displays;
    static inline uint32_t displayModes = 0;

    static inline std::queue<SptrPromiseBacklight> backlightPromiseQueue;
    static inline std::mutex backlightPromiseQueueMutex;

    static inline std::queue<SptrPromisePowerStatus> powerStatusPromiseQueue;
    static inline std::mutex powerStatusPromiseQueueMutex;

    static inline std::queue<SptrPromiseWMError> promiseQueue;
    static inline std::mutex promiseQueueMutex;

    static inline std::map<int32_t, SptrPromiseWMSImageInfo> screenShotPromises;
    static inline std::mutex screenShotPromisesMutex;

    static inline std::map<int32_t, SptrPromiseWMSImageInfo> windowShotPromises;
    static inline std::mutex windowShotPromisesMutex;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMSERVICE_INCLUDE_WINDOW_MANAGER_SERVICE_PROXY_H
