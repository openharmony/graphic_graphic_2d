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

#ifndef FRAMEWORKS_WMSERVER_SRC_WMSERVER_H
#define FRAMEWORKS_WMSERVER_SRC_WMSERVER_H

#include <display_device.h>

#ifdef USE_IVI_INPUT_FOCUS
#include <ivi-input-export.h>
#endif
#include <ivi-layout-export.h>
#include <wms-server-protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USE_IVI_INPUT_FOCUS
#define INPUT_DEVICE_KEYBOARD   ((unsigned int) 1 << 0)
#define INPUT_DEVICE_POINTER    ((unsigned int) 1 << 1)
#define INPUT_DEVICE_TOUCH      ((unsigned int) 1 << 2)
#define INPUT_DEVICE_ALL        ((unsigned int) ~0)
#endif

struct WmsContext {
    struct weston_compositor *pCompositor;
    struct ivi_layout_interface_for_wms *pLayoutInterface;
    struct wl_list wlListController;
    struct wl_list wlListWindow;
    struct wl_list wlListScreen;
    struct wl_list wlListSeat;
    struct wl_list wlListGlobalEventResource;
    struct wl_listener wlListenerDestroy;
    struct wl_listener wlListenerOutputCreated;
    struct wl_listener wlListenerOutputDestroyed;
    struct wl_listener wlListenerSeatCreated;
    uint32_t displayMode;
    struct WmsScreen *pMainScreen;
#ifdef USE_IVI_INPUT_FOCUS
    const struct ivi_input_interface_for_wms *pInputInterface;
#endif
    DeviceFuncs *deviceFuncs;
    uint32_t splitMode;
};

struct WmsSeat {
    struct wl_list wlListLink;
    struct WmsContext *pWmsCtx;
    uint32_t deviceFlags;
    uint32_t focusWindowId;
    struct weston_seat *pWestonSeat;
    struct wl_listener wlListenerDestroyed;
};

struct WmsScreen {
    struct wl_list wlListLink;
    struct WmsContext *pWmsCtx;
    uint32_t screenId;
    uint32_t screenType;
    struct weston_output *westonOutput;
};

struct WmsContext *GetWmsInstance(void);

struct WindowSurface {
    struct WmsController *controller;
    struct ivi_layout_surface *layoutSurface;
    struct weston_surface *surface;
    struct wl_listener surfaceDestroyListener;
    struct wl_listener propertyChangedListener;

    uint32_t surfaceId;
    uint32_t screenId;
    uint32_t type;
    uint32_t mode;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    int32_t lastSurfaceWidth;
    int32_t lastSurfaceHeight;
    int32_t firstCommit;
    bool isSplited;

    struct wl_list link;
};

struct ScreenshotFrameListener {
    struct wl_listener frameListener;
    struct wl_listener outputDestroyed;
    uint32_t idScreen;
    struct weston_output *output;
};

struct WmsController {
    struct wl_resource *pWlResource;
    uint32_t id;
    uint32_t windowIdFlags;
    struct wl_client *pWlClient;
    struct wl_list wlListLink;
    struct wl_list wlListLinkRes;
    struct WmsContext *pWmsCtx;
    struct ScreenshotFrameListener stListener;
};

void SetDestinationRectangle(struct WindowSurface *windowSurface,
    int32_t x, int32_t y, int32_t width, int32_t height);
void SetSourceRectangle(const struct WindowSurface *windowSurface,
    int32_t x, int32_t y, int32_t width, int32_t height);
void SetWindowPosition(struct WindowSurface *ws, int32_t x, int32_t y);
void SetWindowSize(struct WindowSurface *ws, uint32_t width, uint32_t height);
struct ivi_layout_layer *GetLayer(struct weston_output *westonOutput, uint32_t layerId, bool *isNewLayer);
void AddSetWindowTopListener(void(*fn)(struct WindowSurface *ws));
void AddSurfaceDestroyListener(void(*fn)(struct WindowSurface *ws));

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORKS_WMSERVER_SRC_WMSERVER_H
