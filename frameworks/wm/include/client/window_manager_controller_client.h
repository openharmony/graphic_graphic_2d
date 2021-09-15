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

#include <list>
#include <pthread.h>
#include <vector>

#include <refbase.h>
#include <surface.h>
#include <window_manager_service_client.h>

#include "input_listener_manager.h"
#include "log_listener.h"
#include "multimodal_listener_manager.h"
#include "wayland_service.h"
#include "window_manager_server.h"
#include "wl_buffer_cache.h"
#include "wl_buffer.h"
#include "wl_display.h"
#include "wl_dma_buffer_factory.h"
#include "wl_shm_buffer_factory.h"
#include "wl_subsurface_factory.h"
#include "wl_surface_factory.h"
#include "wp_viewport_factory.h"

namespace OHOS {
struct InnerWindowInfo {
    sptr<WlSurface> wlSurface;
    sptr<WlSubsurface> wlSubsurface;
    sptr<WlBuffer> wlBuffer;
    sptr<WpViewport> wpViewport;
    WindowConfig windowconfig;
    uint32_t windowid;
    uint32_t layerid;
    uint32_t parentid;
    uint32_t voLayerId;
    sptr<Surface> surface;
    sptr<IBufferConsumerListener> listener;
    std::list<uint32_t> childIDList;
    bool subwidow;
    int32_t width;
    int32_t height;
    int32_t pos_x;
    int32_t pos_y;
    funcWindowInfoChange windowInfoChangeCb;
    void (* onWindowCreateCb)(uint32_t pid);
    sptr<InputListener> logListener;
    sptr<MultimodalListener> mmiListener;

    bool operator ==(const InnerWindowInfo &other) const
    {
        return windowid == other.windowid;
    }
};

class LayerControllerClient : public RefBase {
public:
    static sptr<LayerControllerClient> GetInstance();

    bool init(sptr<IWindowManagerService> &service);

    InnerWindowInfo *CreateWindow(int32_t id, WindowConfig &config);
    InnerWindowInfo *CreateSubWindow(int32_t subid, int32_t parentid, WindowConfig &config);
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
    void RegistOnTouchCb(int id, funcOnTouch cb);
    void RegistOnKeyCb(int id, funcOnKey cb);
    void RegistWindowInfoChangeCb(int id, funcWindowInfoChange cb);
    void RegistOnWindowCreateCb(int32_t id, void(* cb)(uint32_t pid));
    void SendWindowCreate(uint32_t pid);

    InnerWindowInfo *GetInnerWindowInfoFromId(uint32_t windowid);
    void RemoveInnerWindowInfo(uint32_t id);
    static void *thread_display_dispatch(void *param);

protected:
    bool CreateSurface(int32_t id);
    bool initScreen();

private:
    void SetSubSurfaceSize(int32_t id, int32_t width, int32_t height);

    static inline sptr<LayerControllerClient> instance = nullptr;
    LayerControllerClient();
    virtual ~LayerControllerClient();

    sptr<IWindowManagerService> wms = nullptr;
    bool isInit = false;
    std::mutex mutex;
    std::mutex windowListMutex;
    std::list<InnerWindowInfo> m_windowList;
    uint32_t m_screenId;
    int32_t m_screenWidth;
    int32_t m_screenHeight;
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
