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

#ifndef FRAMEWORKS_WM_INCLUDE_CLIENT_WINDOW_MANAGER_CONTROLLER_CLIENT_H
#define FRAMEWORKS_WM_INCLUDE_CLIENT_WINDOW_MANAGER_CONTROLLER_CLIENT_H

#include <condition_variable>
#include <iostream>
#include <list>
#include <pthread.h>
#include <securec.h>
#include <string>
#include <vector>

#include <ilm_types.h>
#include <ivi-application-client-protocol.h>
#include <linux-dmabuf-unstable-v1-client-protocol.h>
#include <refbase.h>
#include <surface.h>
#include <touch_event.h>
#include <viewporter-client-protocol.h>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>

#include "iwindow_manager_service.h"

namespace OHOS {
typedef struct t_wlContextStruct {
    struct wl_display* wlDisplay;
    struct wl_registry* wlRegistry;
    struct wl_compositor* wlCompositor;
    struct wl_egl_window* wlNativeWindow;
    struct wl_surface* wlSurface;
    struct wl_pointer *wl_pointer;
    struct wl_keyboard *wlKeyboard;
    struct wl_touch *wlTouch;
    struct wl_shell_surface* wlShellSurface;
    struct ivi_application* iviApp;
    struct ivi_surface* iviSurface;
    struct zwp_linux_dmabuf_v1 *dmabuf;
    struct wl_seat *wl_seat;
    struct wl_cursor_theme *cursor_theme;
    struct wl_surface *wl_pointer_surface;
    struct wl_cursor *cursor;
    struct wl_shm *wl_shm;
    struct wl_subcompositor *wlSubCompositor;
    struct wp_viewporter *viewporter;
    uint32_t formats;
    uint8_t enable_cursor;
    uint8_t button_press;
    int32_t width;
    int32_t height;
    int32_t point_x;
    int32_t point_y;
    uint32_t mask;
} WLContextStruct;

struct TouchEventInfo {
    bool isRefreshed;
    int32_t serial;
    uint32_t startTime;
    uint32_t currentTime;
    int32_t x;
    int32_t y;
    float touchPressure;
};

struct ActionEventInfo {
    int32_t touchCount;
    bool isDown;
    bool isUp;
    bool isMotion;
    TouchEventInfo touchEventInfos[MAX_TOUCH_NUM];
};

static ActionEventInfo actionEventInfo;

struct InnerWindowInfo {
    struct wl_display* wlDisplay;
    struct wl_surface* wlSurface;
    struct wl_subsurface* wlSubSurface;
    struct ivi_surface* iviSurface;
    struct wl_buffer* wl_buffer;
    struct wp_viewport *viewport;
    struct wl_callback *p_cb;
    WindowConfig windowconfig;
    std::string unitName;
    uint32_t windowid;
    uint32_t layerid;
    uint32_t parentid;
    uint32_t voLayerId;
    sptr<Surface> surface;
    sptr<IBufferConsumerListener> listener;
    std::list<uint32_t> childIDList;
    bool threadrunning;
    bool subwidow;
    int32_t width;
    int32_t height;
    int32_t pos_x;
    int32_t pos_y;
    void* shm_data;
    funcPointerEnter pointerEnterCb;
    funcPointerLeave pointerLeaveCb;
    funcPointerMotion pointerMotionCb;
    funcPointerButton pointerButtonCb;
    funcPointerFrame pointerFrameCb;
    funcPointerAxis pointerAxisCb;
    funcPointerAxisSource pointerAxisSourceCb;
    funcPointerAxisStop pointerAxisStopCb;
    funcPointerAxisDiscrete pointerAxisDiscreteCb;
    funcTouchDown touchDownCb;
    funcTouchUp touchUpCb;
    funcTouchEmotion touchEmotionCb;
    funcTouchFrame touchFrameCb;
    funcTouchCancel touchCancelCb;
    funcTouchShape touchShapeCb;
    funcTouchOrientation touchOrientationCb;
    funcOnKey keyboardKeyCb;
    funcOnTouch onTouchCb;
    funcWindowInfoChange windowInfoChangeCb;
    void (* onWindowCreateCb)(uint32_t pid);

    bool operator == (const InnerWindowInfo& other) const
    {
        return windowid == other.windowid;
    }
};

class BufferInfo {
public:
    BufferInfo();
    ~BufferInfo();

    void AsyncInit();
    void Await();
    void Async(std::function<void(BufferInfo*)> mutexFunc);

    wptr<Surface> surface_;
    wptr<SurfaceBuffer> buffer_;
    struct wl_buffer* wlBuffer_;

private:
    bool created_;
    std::mutex createdMutex_;
    std::condition_variable createdVariable_;
};

class LayerControllerClient : public RefBase {
public:
    static sptr<LayerControllerClient> GetInstance();

    InnerWindowInfo* CreateWindow(int32_t id, WindowConfig& config);
    InnerWindowInfo* CreateSubWindow(int32_t subid, int32_t parentid, WindowConfig& config);
    void CreateWlBuffer(sptr<Surface>& surface, uint32_t id);
    void DestroyWindow(int32_t id);
    void Move(int32_t id, int32_t x, int32_t y);
    void Show(int32_t id);
    void Hide(int32_t id);
    void ReSize(int32_t id, int32_t width, int32_t height);
    void Rotate(int32_t id, int32_t type);
    int32_t GetMaxWidth();
    int32_t GetMaxHeight();
    void ChangeWindowType(int32_t id, WindowType type);
    void StartShotScreen(FuncShotDone done_cb);
    void StartShotWindow(int32_t winID, FuncShotDone done_cb);
    void RegistPointerButtonCb(int id, funcPointerButton cb);
    void RegistPointerEnterCb(int id, funcPointerEnter cb);
    void RegistPointerLeaveCb(int id, funcPointerLeave cb);
    void RegistPointerMotionCb(int id, funcPointerMotion cb);
    void RegistPointerAxisDiscreteCb(int id, funcPointerAxisDiscrete cb);
    void RegistPointerAxisSourceCb(int id, funcPointerAxisSource cb);
    void RegistPointerAxisStopCb(int id, funcPointerAxisStop cb);
    void RegistPointerAxisCb(int id, funcPointerAxis cb);
    void RegistTouchUpCb(int id, funcTouchUp cb);
    void RegistTouchDownCb(int id, funcTouchDown cb);
    void RegistTouchEmotionCb(int id, funcTouchEmotion cb);
    void RegistTouchFrameCb(int id, funcTouchFrame cb);
    void RegistTouchCancelCb(int id, funcTouchCancel cb);
    void RegistTouchShapeCb(int id, funcTouchShape cb);
    void RegistTouchOrientationCb(int id, funcTouchOrientation cb);
    void RegistOnTouchCb(int id, funcOnTouch cb);
    void RegistOnKeyCb(int id, funcOnKey cb);
    void RegistWindowInfoChangeCb(int id, funcWindowInfoChange cb);
    void RegistOnWindowCreateCb(int32_t id, void(* cb)(uint32_t pid));
    void SendWindowCreate(uint32_t pid);

    static void registry_handle_global(
                    void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
    static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name);
    InnerWindowInfo* GetInnerWindowInfoFromId(uint32_t windowid);
    void RemoveInnerWindowInfo(uint32_t id);
    static void* thread_display_dispatch(void* param);

protected:
    bool CreateSurface(int32_t id);
    bool initIlm();
    bool cleanupIlm();
    bool initScreen();

private:
    BufferInfo* GetBufferInfo(sptr<SurfaceBuffer>& buffer);
    void CleanBufferInfo();
    static void BufferRelease(void *data, struct wl_buffer *wlBuffer);
    void SetSubSurfaceSize(int32_t id, int32_t width, int32_t height);

    static sptr<LayerControllerClient> instance;
    LayerControllerClient();
    virtual ~LayerControllerClient();

    std::mutex mutex;
    std::mutex windowListMutex;
    std::list<InnerWindowInfo> m_windowList;
    std::list<BufferInfo> bufferInfos_;
    void wm_register_notification(WindowConfig& config);
    uint32_t m_screenId;
    int32_t m_screenWidth;
    int32_t m_screenHeight;
    std::string m_currentUnit;
    WLContextStruct wlContextStruct_;
    static bool m_bIsThreadRunning;
};

class SurfaceListener : public IBufferConsumerListener {
public:
    SurfaceListener(sptr<Surface>& surface, uint32_t windowid);
    virtual ~SurfaceListener();

    virtual void OnBufferAvailable() override;

private:
    wptr<Surface> surface_;
    uint32_t windowid_;
};
}

#endif // FRAMEWORKS_WM_INCLUDE_CLIENT_WINDOW_MANAGER_CONTROLLER_CLIENT_H
