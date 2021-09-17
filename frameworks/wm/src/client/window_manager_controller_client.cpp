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

#include "window_manager_controller_client.h"

#include <map>
#include <queue>

#include "video_window.h"
#include "window_manager_define.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WindowManagerClient" };
} // namespace

#define LOCK(mutexName) \
    std::lock_guard<std::mutex> lock(mutexName)

#define GET_WINDOWINFO(info, id, ret) \
    InnerWindowInfo *info = LayerControllerClient::GetInstance()->GetInnerWindowInfoFromId((uint32_t)id); \
    if (info == nullptr) { \
        WMLOGFE("id: %{public}d, window info is nullptr", id); \
        return ret; \
    }

#define GET_WINDOWINFO_VOID(info, id) \
    InnerWindowInfo *info = LayerControllerClient::GetInstance()->GetInnerWindowInfoFromId((uint32_t)id); \
    if (info == nullptr) { \
        WMLOGFE("id: %{public}d, window info is nullptr", id); \
        return; \
    }

#define GET_WINDOWINFO_INNER_VOID(info, id) \
    InnerWindowInfo *info = GetInnerWindowInfoFromId((uint32_t)id); \
    if (info == nullptr) { \
        WMLOGFE("id: %{public}d, window info is nullptr", id); \
        return; \
    }

sptr<LayerControllerClient> LayerControllerClient::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new LayerControllerClient();
        }
    }
    return instance;
}

LayerControllerClient::LayerControllerClient() : m_screenWidth(0), m_screenHeight(0)
{
    WMLOG_I("DEBUG LayerControllerClient");
}

LayerControllerClient::~LayerControllerClient()
{
    WMLOG_I("DEBUG ~LayerControllerClient");
}

bool LayerControllerClient::init(sptr<IWindowManagerService> &service)
{
    if (isInit) {
        return true;
    }

    if (service == nullptr) {
        WMLOG_E("service is nullptr");
        return false;
    }
    wms = service;

    InputListenerManager::GetInstance()->Init();
    WlDMABufferFactory::GetInstance()->Init();
    WlSHMBufferFactory::GetInstance()->Init();
    WlSurfaceFactory::GetInstance()->Init();
    WlSubsurfaceFactory::GetInstance()->Init();
    WindowManagerServer::GetInstance()->Init();
    WpViewportFactory::GetInstance()->Init();

    WaylandService::GetInstance()->Start();
    WlDisplay::GetInstance()->Roundtrip();
    WlDisplay::GetInstance()->Roundtrip();

    pthread_t tid1;
    pthread_create(&tid1, NULL, LayerControllerClient::thread_display_dispatch, this);
    isInit = true;
    return true;
}

void LayerControllerClient::ChangeWindowType(int32_t id, WindowType type)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::ChangeWindowType id=%{public}d,type=%{public}d", id, type);

    GET_WINDOWINFO_INNER_VOID(wininfo, id);

    if (type == wininfo->windowconfig.type) {
        WMLOG_E("LayerControllerClient::ChangeWindowType window type is no need change");
        return;
    }

    wms->SetWindowType(id, type);
}

void BufferRelease(struct wl_buffer *wbuffer)
{
    sptr<Surface> surface = nullptr;
    sptr<SurfaceBuffer> sbuffer = nullptr;
    if (WlBufferCache::GetInstance()->GetSurfaceBuffer(wbuffer, surface, sbuffer)) {
        if (surface != nullptr && sbuffer != nullptr) {
            surface->ReleaseBuffer(sbuffer, -1);
        }
    }
}

void LayerControllerClient::CreateWlBuffer(sptr<Surface>& surface, uint32_t windowId)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::CreateWlBuffer id=%{public}d", windowId);

    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = surface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        WMLOG_I("LayerControllerClient::CreateWlBuffer AcquireBuffer failed");
        return;
    }

    GET_WINDOWINFO_INNER_VOID(windowInfo, windowId);

    auto bc = WlBufferCache::GetInstance();
    auto wbuffer = bc->GetWlBuffer(surface, buffer);
    if (wbuffer == nullptr) {
        auto dmaWlBuffer = WlDMABufferFactory::GetInstance()->Create(buffer->GetBufferHandle());
        if (dmaWlBuffer == nullptr) {
            WMLOG_E("Create DMA Buffer Failed");
            ret = surface->ReleaseBuffer(buffer, -1);
            if (ret != SURFACE_ERROR_OK) {
                WMLOG_W("ReleaseBuffer failed");
            }
            return;
        }
        dmaWlBuffer->OnRelease(BufferRelease);

        wbuffer = dmaWlBuffer;
        bc->AddWlBuffer(wbuffer, surface, buffer);
    }

    if (wbuffer) {
        windowInfo->wlSurface->Attach(wbuffer, 0, 0);
        windowInfo->wlSurface->Damage(damage.x, damage.y, damage.w, damage.h);
        windowInfo->wlSurface->Commit();
        WlDisplay::GetInstance()->Flush();
    }
    WMLOG_I("LayerControllerClient::CreateWlBuffer end");
}

void *LayerControllerClient::thread_display_dispatch(void *param)
{
    WMLOG_I("LayerControllerClient::thread_display_dispatch start");

    LayerControllerClient *ptr = static_cast<LayerControllerClient*>(param);
    while (true) {
        int32_t ret = WlDisplay::GetInstance()->Dispatch();
        if (ret == -1) {
            WMLOG_I("LayerControllerClient::thread_display_dispatch error, errno: %{public}d", errno);
            break;
        }
    }

    return nullptr;
}

bool LayerControllerClient::CreateSurface(int32_t id)
{
    WMLOG_I("CreateSurface (windowid = %{public}d) start", id);

    GET_WINDOWINFO(windowInfo, id, false);

    windowInfo->surface = Surface::CreateSurfaceAsConsumer();
    if (windowInfo->surface) {
        windowInfo->listener = new SurfaceListener(windowInfo->surface, id);
        if (windowInfo->listener == nullptr) {
            WMLOG_E("windowInfo->listener new failed");
        }
        windowInfo->surface->RegisterConsumerListener(windowInfo->listener);

        int width = LayerControllerClient::GetInstance()->GetMaxWidth();
        int height = LayerControllerClient::GetInstance()->GetMaxHeight();
        windowInfo->surface->SetDefaultWidthAndHeight(width, height);
    }
    WMLOG_I("CreateSurface (windowid = %{public}d) end", id);
    return true;
}

InnerWindowInfo *LayerControllerClient::CreateWindow(int32_t id, WindowConfig &config)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::CreateWindow start");

    InnerWindowInfo newInfo = {
        .windowconfig = config,
        .windowid = id,
        .layerid = config.type * LAYER_ID_TYPE_OFSSET + LAYER_ID_APP_TYPE_BASE,
        .width = config.width,
        .height = config.height,
        .pos_x = config.pos_x,
        .pos_y = config.pos_y,
    };

    {
        LOCK(windowListMutex);
        m_windowList.push_back(newInfo);
    }
    CreateSurface(id);

    auto info = GetInnerWindowInfoFromId(id);
    info->logListener = LogListener::GetInstance()->AddListener(&info->windowid);
    info->mmiListener = MultimodalListenerManager::GetInstance()->AddListener(&info->windowid);
    return info;
}

namespace {
void GetSubInnerWindowInfo(InnerWindowInfo &info,
    const int subid, const int parentid, const WindowConfig &config)
{
    info.windowconfig = config;
    info.windowid = subid;
    info.parentid = parentid;
    info.subwidow = true;
    info.width = config.width;
    info.height = config.height;
    info.pos_x = config.pos_x;
    info.pos_y = config.pos_y;
    info.voLayerId = -1U;
    info.windowInfoChangeCb = nullptr;
}

bool CreateVideoSubWindow(InnerWindowInfo &newSubInfo)
{
    uint32_t voLayerId = -1U;
    sptr<Surface> surface = nullptr;
    int32_t ret = VideoWindow::CreateLayer(newSubInfo, voLayerId, surface);
    if (ret == 0) {
        newSubInfo.voLayerId = voLayerId;
        newSubInfo.surface = surface;

        newSubInfo.wlBuffer = WlSHMBufferFactory::GetInstance()->Create(
            newSubInfo.width, newSubInfo.height, WL_SHM_FORMAT_XRGB8888);
        if (newSubInfo.wlBuffer == nullptr) {
            WMLOG_I("CreateVideoSubWindow CreateShmBuffer failed");
            return false;
        }

        if (newSubInfo.windowconfig.type == WINDOW_TYPE_VIDEO) {
            newSubInfo.wlSurface->SetSurfaceType(WL_SURFACE_TYPE_VIDEO);
        }

        newSubInfo.wlSurface->Attach(newSubInfo.wlBuffer, 0, 0);
        newSubInfo.wlSurface->Damage(0, 0, newSubInfo.width, newSubInfo.height);
        newSubInfo.wlSurface->Commit();
        return true;
    }

    WMLOG_I("CreateVideoSubWindow VideoWindow::CreateLayer failed");
    return false;
}
}

InnerWindowInfo *LayerControllerClient::CreateSubWindow(int32_t subid, int32_t parentid, WindowConfig &config)
{
    LOCK(mutex);
    WMLOG_I("%{public}s start parentID is %{public}d, subid is %{public}d",
        "LayerControllerClient::CreateSubWindow", parentid, subid);

    GET_WINDOWINFO(parentInnerWindowInfo, parentid, nullptr);
    InnerWindowInfo newSubInfo = {};
    GetSubInnerWindowInfo(newSubInfo, subid, parentid, config);

    newSubInfo.wlSurface = WlSurfaceFactory::GetInstance()->Create();
    if (newSubInfo.wlSurface == nullptr) {
        WMLOG_I("CreateSubWindow createwlSubSuface (subid = %{public}d) failed", subid);
        return nullptr;
    }

    newSubInfo.wlSubsurface = WlSubsurfaceFactory::GetInstance()->Create(
        newSubInfo.wlSurface, parentInnerWindowInfo->wlSurface);
    if (newSubInfo.wlSubsurface) {
        newSubInfo.wlSubsurface->SetPosition(newSubInfo.pos_x, newSubInfo.pos_y);
        newSubInfo.wlSubsurface->SetDesync();
    }
    WMLOG_I("CreateSubWindow createwlSubSuface (subid = %{public}d) success", subid);

    if (newSubInfo.windowconfig.type != WINDOW_TYPE_VIDEO) {
        newSubInfo.surface = Surface::CreateSurfaceAsConsumer();
        if (newSubInfo.surface) {
            newSubInfo.listener = new SurfaceListener(newSubInfo.surface, subid);
            if (newSubInfo.listener == nullptr) {
                WMLOG_E("newSubInfo.listener new failed");
            }
            newSubInfo.surface->RegisterConsumerListener(newSubInfo.listener);
        }
    } else {
        if (!CreateVideoSubWindow(newSubInfo)) {
            return nullptr;
        }
    }

    {
        LOCK(windowListMutex);
        m_windowList.push_back(newSubInfo);
    }

    parentInnerWindowInfo->childIDList.push_back(subid);
    WMLOG_E("LayerControllerClient::CreateSubWindow END");
    return GetInnerWindowInfoFromId(subid);
}

void LayerControllerClient::RegistOnTouchCb(int id, funcOnTouch cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s", __func__);
    if (cb) {
        WMLOG_I("LayerControllerClient::RegistOnTouchCb OK");
        GET_WINDOWINFO_VOID(windowInfo, id);
        windowInfo->mmiListener->onTouchCb = cb;
    }
}

void LayerControllerClient::RegistOnKeyCb(int id, funcOnKey cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::RegistOnKeyCb start");
    if (cb) {
        WMLOG_I("LayerControllerClient::RegistOnKeyCb OK");
        GET_WINDOWINFO_VOID(windowInfo, id);
        windowInfo->mmiListener->keyboardKeyCb = cb;
    }
}

void LayerControllerClient::RegistWindowInfoChangeCb(int id, funcWindowInfoChange cb)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::%{public}s begin",  __func__);
    if (cb) {
        WMLOG_I("LayerControllerClient::RegistWindowInfoChangeCb OK");
        GET_WINDOWINFO_VOID(windowInfo, id);
        windowInfo->windowInfoChangeCb = cb;
    }
}

void LayerControllerClient::RegistOnWindowCreateCb(int32_t id, void(* cb)(uint32_t pid))
{
    LOCK(mutex);
    GET_WINDOWINFO_VOID(info, id);
    info->onWindowCreateCb = cb;
}

void LayerControllerClient::SendWindowCreate(uint32_t pid)
{
    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        if (it->onWindowCreateCb) {
            it->onWindowCreateCb(pid);
        }
    }
}

void LayerControllerClient::SetSubSurfaceSize(int32_t id, int32_t width, int32_t height)
{
    WMLOG_E("%{public}s start id(%{public}d) width(%{public}d) height(%{public}d)",
        "LayerControllerClient::SetSubSurfaceSize", id, width, height);

    GET_WINDOWINFO_VOID(windowInfo, id);
    if (windowInfo->subwidow == true) {
        windowInfo->wpViewport = WpViewportFactory::GetInstance()->Create(windowInfo->wlSurface);
        if (windowInfo->wpViewport) {
            windowInfo->wpViewport->SetSource(windowInfo->pos_x, windowInfo->pos_y, width, height);
        }
        windowInfo->wlSurface->Commit();
    }
}

void LayerControllerClient::DestroyWindow(int32_t id)
{
    LOCK(mutex);

    std::queue<uint32_t> q;
    q.push(id);
    while (!q.empty()) {
        uint32_t id = q.front();
        q.pop();

        InnerWindowInfo *info = GetInnerWindowInfoFromId(id);
        if (info) {
            for (auto jt = info->childIDList.begin(); jt != info->childIDList.end(); jt++) {
                q.push(*jt);
            }
        }

        RemoveInnerWindowInfo(id);
    }
    return;
}

void LayerControllerClient::Move(int32_t id, int32_t x, int32_t y)
{
    LOCK(mutex);

    GET_WINDOWINFO_VOID(wininfo, id);
    wininfo->pos_x = x;
    wininfo->pos_y = y;
    wininfo->windowconfig.pos_x = x;
    wininfo->windowconfig.pos_y = y;

    if (wininfo->subwidow) {
        wininfo->wlSubsurface->SetPosition(x, y);
    } else {
        WMLOG_I("LayerControllerClient::Move id(%{public}d) x(%{public}d) y(%{public}d)", id, x, y);
        wms->Move(id, x, y);
    }

    struct WindowInfo info = {
        .width = wininfo->width,
        .height = wininfo->height,
        .pos_x = wininfo->pos_x,
        .pos_y = wininfo->pos_y
    };

    if (wininfo->windowInfoChangeCb) {
        wininfo->windowInfoChangeCb(info);
    }
}

void LayerControllerClient::ReSize(int32_t id, int32_t width, int32_t height)
{
    WMLOG_I("LayerControllerClient::ReSize id(%{public}d) width(%{public}d) width(%{public}d)", id, width, height);
    LOCK(mutex);

    GET_WINDOWINFO_VOID(wininfo, id);
    wininfo->windowconfig.width = width;
    wininfo->windowconfig.height = height;
    wininfo->width = width;
    wininfo->height = height;

    if (wininfo->subwidow) {
        SetSubSurfaceSize(id, width, height);
    } else {
        wms->Resize(id, width, height);
    }

    struct WindowInfo info = {
        .width = wininfo->width,
        .height = wininfo->height,
        .pos_x = wininfo->pos_x,
        .pos_y = wininfo->pos_y
    };

    if (wininfo->windowInfoChangeCb) {
        wininfo->windowInfoChangeCb(info);
    }
}

void LayerControllerClient::Show(int32_t id)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::Show id=%{public}d", id);
    wms->Show(id);
}

void LayerControllerClient::Hide(int32_t id)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::Hide id=%{public}d", id);
    wms->Hide(id);
}

void LayerControllerClient::Rotate(int32_t id, int32_t type)
{
    LOCK(mutex);
    WMLOG_I("LayerControllerClient::Rotate id=%{public}d type=%{public}d start", id, type);
    GET_WINDOWINFO_VOID(wininfo, id);
    wininfo->wlSurface->SetBufferTransform(static_cast<wl_output_transform>(type));
    wininfo->wlSurface->Commit();
}

int32_t LayerControllerClient::GetMaxWidth()
{
    if (m_screenWidth == 0) {
        std::vector<WMDisplayInfo> displays;
        wms->GetDisplays(displays);
        if (displays.size() != 0) {
            m_screenWidth = displays[0].width;
            m_screenHeight = displays[0].height;
        }
    }
    WMLOG_I("LayerControllerClient::GetMaxWidth m_screenWidth=%{public}d", m_screenWidth);
    return m_screenWidth;
}

int32_t LayerControllerClient::GetMaxHeight()
{
    if (m_screenHeight == 0) {
        std::vector<WMDisplayInfo> displays;
        wms->GetDisplays(displays);
        if (displays.size() != 0) {
            m_screenWidth = displays[0].width;
            m_screenHeight = displays[0].height;
        }
    }
    WMLOG_I("LayerControllerClient::GetMaxHeight m_screenHeight=%{public}d", m_screenHeight);
    return m_screenHeight;
}

void ProcessWindowInfo(InnerWindowInfo &info, sptr<IWindowManagerService> &wms)
{
    int layerid = WINDOW_LAYER_DEFINE_NORMAL_ID;
    layerid += info.windowconfig.type * LAYER_ID_TYPE_OFSSET;
    if (info.subwidow) {
        if (info.windowconfig.type == WINDOW_TYPE_VIDEO) {
            VideoWindow::DestroyLayer(info.voLayerId);
        }
    } else {
        wms->DestroyWindow(info.windowid);
    }

    if (info.surface) {
        info.surface->CleanCache();
    }

    LogListener::GetInstance()->RemoveListener(info.logListener);
    MultimodalListenerManager::GetInstance()->RemoveListener(info.mmiListener);
}

void LayerControllerClient::RemoveInnerWindowInfo(uint32_t id)
{
    LOCK(windowListMutex);

    for (auto it = m_windowList.begin(); it != m_windowList.end(); it++) {
        if (it->windowid == id) {
            ProcessWindowInfo(*it, wms);
            m_windowList.erase(it);
            it--;
            break;
        }
    }
}

InnerWindowInfo *LayerControllerClient::GetInnerWindowInfoFromId(uint32_t windowid)
{
    LOCK(windowListMutex);
    for (auto &info : m_windowList) {
        if (info.windowid == windowid) {
            return &info;
        }
    }

    return nullptr;
}

SurfaceListener::SurfaceListener(sptr<Surface>& surface, uint32_t windowid)
{
    WMLOG_I("DEBUG SurfaceListener");
    surface_ = surface;
    windowid_ = windowid;
}

SurfaceListener::~SurfaceListener()
{
    WMLOG_I("DEBUG ~SurfaceListener");
    surface_ = nullptr;
}

void SurfaceListener::OnBufferAvailable()
{
    WMLOG_I("OnBufferAvailable");
    auto surface = surface_.promote();
    if (surface) {
        LayerControllerClient::GetInstance()->CreateWlBuffer(surface, windowid_);
    } else {
        WMLOG_E("surface.promote failed");
    }
}
}
