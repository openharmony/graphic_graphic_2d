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

#include <map>
#include <mutex>
#include <queue>
#include <wms-client-protocol.h>

namespace OHOS {
class WindowManagerServiceProxy : public IWindowManagerService {
public:
    WindowManagerServiceProxy(struct wms *wms, struct wl_display *display);
    virtual ~WindowManagerServiceProxy() = default;

    virtual WMError GetDisplays(std::vector<struct WMDisplayInfo> &displays) override;
    virtual WMError GetDisplayModes(uint32_t &displayModes) override;
    virtual sptr<Promise<WMError>> SetDisplayMode(WMSDisplayMode modes) override;
    virtual WMError AddDisplayChangeListener(IWindowManagerDisplayListenerClazz *listener) override;
    virtual sptr<Promise<WMError>> OnWindowListChange(IWindowChangeListenerClazz *listener) override;

    virtual WMError SetDisplayDirection(WMSDisplayDirection direction) override;
    virtual WMError OnDisplayDirectionChange(DisplayDirectionChangeFunc func) override;

    virtual sptr<Promise<WMSImageInfo>> ShotScreen(int32_t did) override;
    virtual sptr<Promise<WMSImageInfo>> ShotWindow(int32_t wid) override;

    virtual sptr<Promise<WMError>> DestroyWindow(int32_t wid) override;
    virtual sptr<Promise<WMError>> SwitchTop(int32_t wid) override;
    virtual sptr<Promise<WMError>> Show(int32_t wid) override;
    virtual sptr<Promise<WMError>> Hide(int32_t wid) override;
    virtual sptr<Promise<WMError>> Move(int32_t wid, int32_t x, int32_t y) override;
    virtual sptr<Promise<WMError>> Resize(int32_t wid, uint32_t width, uint32_t height) override;
    virtual sptr<Promise<WMError>> ScaleTo(int32_t wid, uint32_t width, uint32_t height) override;
    virtual sptr<Promise<WMError>> SetWindowType(int32_t wid, WindowType type) override;

    static void OnDisplayChange(uint32_t, const char *, wms_screen_status, int32_t, int32_t);
    static void OnDisplayModeChange(uint32_t);
    static void OnReply(wms_error);
    static void OnScreenShot(wms_error, uint32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    static void OnWindowShot(wms_error, uint32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    static void OnGlobalWindowStatus(uint32_t pid, uint32_t wid, uint32_t status);

private:
    struct wms *wms = nullptr;
    struct wl_display *display = nullptr;
    static inline IWindowManagerDisplayListenerClazz *displayListener = nullptr;
    static inline IWindowChangeListenerClazz *globalWindowChangeListener = nullptr;

    static inline std::vector<struct WMDisplayInfo> displays;
    static inline uint32_t displayModes = 0;
    static inline std::queue<sptr<Promise<WMError>>> promiseQueue;

    static inline std::map<int32_t, sptr<Promise<WMSImageInfo>>> screenShotPromises;
    static inline std::map<int32_t, sptr<Promise<WMSImageInfo>>> windowShotPromises;
    static inline std::mutex screenShotPromisesMutex;
    static inline std::mutex windowShotPromisesMutex;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMSERVICE_INCLUDE_WINDOW_MANAGER_SERVICE_PROXY_H
