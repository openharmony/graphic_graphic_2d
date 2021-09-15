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

#include "wmserver.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "backend.h"
#include "ivi-layout-private.h"
#include "libweston-internal.h"
#include "screen_info.h"
#include "weston.h"

#define LOG_LABEL "wms-controller"

#define LOGD(fmt, ...) weston_log("%{public}s debug %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

#define LOGI(fmt, ...) weston_log("%{public}s info %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

#define LOGE(fmt, ...) weston_log("%{public}s error %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

#define WINDOW_ID_BIT 5
#define WINDOW_ID_LIMIT  (1 << WINDOW_ID_BIT)
#define WINDOW_ID_FLAGS_FILL_ALL ((uint32_t) ~0)
#define WINDOW_ID_NUM_MAX 1024
#define WINDOW_ID_INVALID 0

#define LAYER_ID_TYPE_BASE 5000
#define LAYER_ID_TYPE_OFFSET 1000

#define BAR_WIDTH_PERCENT 0.07
#define ALARM_WINDOW_WIDTH 400
#define ALARM_WINDOW_HEIGHT 300
#define ALARM_WINDOW_WIDTH_HALF 200
#define ALARM_WINDOW_HEIGHT_HALF 150
#define ALARM_WINDOW_HALF 2
#define SCREEN_SHOT_FILE_PATH "/data/screenshot-XXXXXX"
#define TIMER_INTERVAL_MS 300
#define HEIRHT_AVERAGE 2
#define PIXMAN_FORMAT_AVERAGE 8
#define BYTE_SPP_SIZE 4
#define ASSERT assert

struct WindowSurface {
    struct WmsController *controller;
    struct ivi_layout_surface *layoutSurface;
    struct weston_surface *surface;
    struct wl_listener surfaceDestroyListener;
    struct wl_listener propertyChangedListener;

    uint32_t surfaceId;
    uint32_t screenId;
    uint32_t type;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    int32_t lastSurfaceWidth;
    int32_t lastSurfaceHeight;

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

static struct WmsContext g_wmsCtx = {0};

static ScreenInfoChangeListener g_screenInfoChangeListener = NULL;
static SeatInfoChangeListener g_seatInfoChangeListener = NULL;

static void SendGlobalWindowStatus(const struct WmsController *pController, uint32_t windowId, uint32_t status)
{
    LOGD("start.");
    struct WmsContext *pWmsCtx = pController->pWmsCtx;
    struct WmsController *pControllerTemp;
    pid_t pid;

    wl_client_get_credentials(pController->pWlClient, &pid, NULL, NULL);

    wl_list_for_each(pControllerTemp, &pWmsCtx->wlListGlobalEventResource, wlListLinkRes) {
        wms_send_global_window_status(pControllerTemp->pWlResource, pid, windowId, status);
    }
    LOGD("end.");
}

void SetSeatListener(const SeatInfoChangeListener listener)
{
    g_seatInfoChangeListener = listener;
}

static void SeatInfoChangerNotify(void)
{
    if (g_seatInfoChangeListener) {
        LOGD("call seatInfoChangeListener.");
        g_seatInfoChangeListener();
    } else {
        LOGD("seatInfoChangeListener is not set.");
    }
}

void SetScreenListener(const ScreenInfoChangeListener listener)
{
    g_screenInfoChangeListener = listener;
}

static void ScreenInfoChangerNotify(void)
{
    if (g_screenInfoChangeListener) {
        LOGD("call screenInfoChangeListener.");
        g_screenInfoChangeListener();
    } else {
        LOGD("screenInfoChangeListener is not set.");
    }
}

struct WmsContext *GetWmsInstance(void)
{
    return &g_wmsCtx;
}

static inline uint32_t GetBit(uint32_t flags, int32_t n)
{
    return ((flags) & (1 << (n)));
}

static inline void SetBit(uint32_t *flags, int32_t n)
{
    (*flags) |= (1 << (n));
}

static inline void ClearBit(uint32_t *flags, int32_t n)
{
    (*flags) &= ~(1 << (n));
}

static inline int GetLayerId(uint32_t type, uint32_t screenId)
{
    return ((LAYER_ID_TYPE_BASE) + (LAYER_ID_TYPE_OFFSET * type) + (screenId));
}

static void WindowSurfaceCommitted(struct weston_surface *surface, int32_t sx, int32_t sy);

static struct WindowSurface *GetWindowSurface(const struct weston_surface *surface)
{
    struct WindowSurface *windowSurface = NULL;

    if (surface->committed != WindowSurfaceCommitted) {
        return NULL;
    }

    windowSurface = surface->committed_private;
    ASSERT(windowSurface);
    ASSERT(windowSurface->surface == surface);

    return windowSurface;
}

static void SetSourceRectangle(const struct WindowSurface *windowSurface,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    struct ivi_layout_interface_for_wms *layoutInterface = windowSurface->controller->pWmsCtx->pLayoutInterface;
    struct ivi_layout_surface *layoutSurface = windowSurface->layoutSurface;

    const struct ivi_layout_surface_properties *prop = layoutInterface->get_properties_of_surface(layoutSurface);

    if (x < 0) {
        x = prop->source_x;
    }
    if (y < 0) {
        y = prop->source_y;
    }

    if (width < 0) {
        width = prop->source_width;
    }

    if (height < 0) {
        height = prop->source_height;
    }

    layoutInterface->surface_set_source_rectangle(layoutSurface,
        (uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height);
}

static void SetDestinationRectangle(
    const struct WindowSurface *windowSurface,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    struct ivi_layout_interface_for_wms *layoutInterface = windowSurface->controller->pWmsCtx->pLayoutInterface;
    struct ivi_layout_surface *layoutSurface = windowSurface->layoutSurface;

    const struct ivi_layout_surface_properties *prop = layoutInterface->get_properties_of_surface(layoutSurface);
    layoutInterface->surface_set_transition(layoutSurface,
        IVI_LAYOUT_TRANSITION_NONE, TIMER_INTERVAL_MS); // ms

    if (x < 0) {
        x = prop->dest_x;
    }
    if (y < 0) {
        y = prop->dest_y;
    }
    if (width < 0) {
        width = prop->dest_width;
    }
    if (height < 0) {
        height = prop->dest_height;
    }

    layoutInterface->surface_set_destination_rectangle(layoutSurface,
        (uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height);
}

static void WindowSurfaceCommitted(struct weston_surface *surface, int32_t sx, int32_t sy)
{
    struct WindowSurface *windowSurface = GetWindowSurface(surface);
    struct ivi_layout_interface_for_wms *layoutInterface = NULL;

    ASSERT(windowSurface);

    if (!windowSurface) {
        return;
    }

    if (surface->width == 0 || surface->height == 0) {
        return;
    }

    if (windowSurface->lastSurfaceWidth != surface->width || windowSurface->lastSurfaceHeight != surface->height) {
        LOGI(" width = %{public}d, height = %{public}d", surface->width, surface->height);

        layoutInterface = windowSurface->controller->pWmsCtx->pLayoutInterface;

        SetSourceRectangle(windowSurface, 0, 0, surface->width, surface->height);
        SetDestinationRectangle(windowSurface,
            windowSurface->x, windowSurface->y, windowSurface->width, windowSurface->height);

        layoutInterface->surface_set_force_refresh(windowSurface->layoutSurface);
        layoutInterface->commit_changes();

        windowSurface->lastSurfaceWidth = surface->width;
        windowSurface->lastSurfaceHeight = surface->height;
    }
}

static uint32_t GetDisplayModeFlag(struct WmsContext *ctx)
{
    uint32_t screen_num = wl_list_length(&ctx->wlListScreen);
    uint32_t flag = WMS_DISPLAY_MODE_SINGLE;

    if (screen_num > 1) {
        flag = WMS_DISPLAY_MODE_SINGLE | WMS_DISPLAY_MODE_CLONE |
             WMS_DISPLAY_MODE_EXTEND | WMS_DISPLAY_MODE_EXPAND;
    }

    return flag;
}

static void DisplayModeUpdate(const struct WmsContext *pCtx)
{
    struct WmsController *pController = NULL;
    uint32_t flag = GetDisplayModeFlag(pCtx);

    wl_list_for_each(pController, &pCtx->wlListController, wlListLink) {
        wms_send_display_mode(pController->pWlResource, flag);
    }
}

static bool CheckWindowId(struct wl_client *client,
                          uint32_t windowId)
{
    pid_t pid;

    wl_client_get_credentials(client, &pid, NULL, NULL);
    if ((windowId >> WINDOW_ID_BIT) == pid) {
        return true;
    }

    return false;
}

static struct WindowSurface *GetSurface(
    const struct wl_list *surfaceList, uint32_t surfaceId)
{
    struct WindowSurface *windowSurface = NULL;

    wl_list_for_each(windowSurface, surfaceList, link) {
        if (windowSurface->surfaceId == surfaceId) {
            return windowSurface;
        }
    }

    return NULL;
}

static void ClearWindowId(struct WmsController *pController, uint32_t windowId)
{
    LOGD("windowId %{public}d.", windowId);
    if (GetBit(pController->windowIdFlags, windowId % WINDOW_ID_LIMIT) != 0) {
        ClearBit(&pController->windowIdFlags, windowId % WINDOW_ID_LIMIT);
        return;
    }
    LOGE("windowIdFlags %{public}d is not set.", windowId % WINDOW_ID_LIMIT);
}

static uint32_t GetWindowId(struct WmsController *pController)
{
    pid_t pid;
    uint32_t windowId = WINDOW_ID_INVALID;
    uint32_t windowCount = wl_list_length(&pController->pWmsCtx->wlListWindow);
    if (windowCount >= WINDOW_ID_NUM_MAX) {
        LOGE("failed, window count = %{public}d", WINDOW_ID_NUM_MAX);
        return windowId;
    }

    if (pController->windowIdFlags == WINDOW_ID_FLAGS_FILL_ALL) {
        LOGE("failed, number of window per process = %{public}d", WINDOW_ID_LIMIT);
        return windowId;
    }

    wl_client_get_credentials(pController->pWlClient, &pid, NULL, NULL);

    for (int i = 0; i < WINDOW_ID_LIMIT; i++) {
        if (GetBit(pController->windowIdFlags, i) == 0) {
            SetBit(&pController->windowIdFlags, i);
            windowId = pid * WINDOW_ID_LIMIT + i;
            break;
        }
    }

    LOGD("success, windowId = %{public}d", windowId);

    return windowId;
}

static void SurfaceDestroy(const struct WindowSurface *surface)
{
    LOGD("surfaceId:%{public}d start.", surface->surfaceId);
    ASSERT(surface != NULL);

    wl_list_remove(&surface->surfaceDestroyListener.link);
    wl_list_remove(&surface->propertyChangedListener.link);

    if (surface->layoutSurface != NULL) {
        surface->controller->pWmsCtx->pLayoutInterface->surface_destroy(
            surface->layoutSurface);
    }

    ClearWindowId(surface->controller, surface->surfaceId);
    wl_list_remove(&surface->link);

    if (surface->surface) {
        surface->surface->committed = NULL;
        surface->surface->committed_private = NULL;
    }

    wms_send_window_status(surface->controller->pWlResource,
        WMS_WINDOW_STATUS_DESTROYED, surface->surfaceId, 0, 0, 0, 0);

    SendGlobalWindowStatus(surface->controller, surface->surfaceId, WMS_WINDOW_STATUS_DESTROYED);

    free(surface);

    LOGD(" end.");
}

static void WindowSurfaceDestroy(const struct wl_listener *listener,
    const struct weston_compositor *data)
{
    LOGD("start.");

    struct WindowSurface *windowSurface = wl_container_of(listener, windowSurface, surfaceDestroyListener);
    SurfaceDestroy(windowSurface);

    LOGD("end.");
}

static struct ivi_layout_layer *GetLayer(
    const struct weston_output *westonOutput,
    const struct ivi_layout_interface_for_wms *pLayoutInterface,
    uint32_t layerId)
{
    LOGD("start.");
    struct ivi_layout_layer *layoutLayer = pLayoutInterface->get_layer_from_id(layerId);
    if (!layoutLayer) {
        layoutLayer = pLayoutInterface->layer_create_with_dimension(
            layerId, westonOutput->width, westonOutput->height);
        if (!layoutLayer) {
            LOGE("ivi_layout_layer_create_with_dimension failed.");
            return NULL;
        }

        pLayoutInterface->screen_add_layer(westonOutput, layoutLayer);
        pLayoutInterface->layer_set_visibility(layoutLayer, true);
    }

    LOGD("end.");
    return layoutLayer;
}

static struct WmsScreen *GetScreenFromId(const struct WmsContext *ctx,
                                         uint32_t screenId)
{
    struct WmsScreen *screen = NULL;
    wl_list_for_each(screen, &ctx->wlListScreen, wlListLink) {
        if (screen->screenId == screenId) {
            return screen;
        }
    }
    return NULL;
}

static struct WmsScreen *GetScreen(const struct WindowSurface *windowSurface)
{
    struct WmsScreen *screen = NULL;
    struct WmsContext *ctx = windowSurface->controller->pWmsCtx;
    wl_list_for_each(screen, &ctx->wlListScreen, wlListLink) {
        if (screen->screenId == windowSurface->screenId) {
            return screen;
        }
    }
    return NULL;
}

static void CalcWindowInfo(struct WindowSurface *surface)
{
    int32_t maxWitdh, maxHeight, barHeight, allBarsHeight;
#ifdef USE_DUMMY_SCREEN
    maxWitdh = DUMMY_SCREEN_WIDTH;
    maxHeight = DUMMY_SCREEN_HEIGHT;
    barHeight = (BAR_WIDTH_PERCENT * maxHeight);
    allBarsHeight = barHeight + barHeight;
#else
    struct WmsScreen *screen = GetScreen(surface);
    if (!screen) {
        LOGE("GetScreen error.");
        return;
    }
    maxWitdh = screen->westonOutput->width;
    maxHeight = screen->westonOutput->height;
    barHeight = (BAR_WIDTH_PERCENT * maxHeight);
    allBarsHeight = barHeight + barHeight;
#endif /* USE_DUMMY_SCREEN */

    surface->width = maxWitdh;
    surface->x = 0;

    switch (surface->type) {
        case WMS_WINDOW_TYPE_NORMAL:
            surface->height = maxHeight - allBarsHeight;
            surface->y = barHeight;
            break;
        case WMS_WINDOW_TYPE_STATUS_BAR:
            surface->height = barHeight;
            surface->y = 0;
            break;
        case WMS_WINDOW_TYPE_NAVI_BAR:
            surface->height = barHeight;
            surface->y = maxHeight - surface->height;
            break;
        case WMS_WINDOW_TYPE_ALARM:
            surface->width = ALARM_WINDOW_WIDTH;
            surface->height = ALARM_WINDOW_HEIGHT;
            surface->x = maxWitdh / ALARM_WINDOW_HALF - ALARM_WINDOW_WIDTH_HALF;
            surface->y = maxHeight / ALARM_WINDOW_HALF - ALARM_WINDOW_HEIGHT_HALF;
            break;
        default:
            LOGI("default branch.");
            surface->height = maxHeight - allBarsHeight;
            surface->y = barHeight;
            break;
    }

    return;
}

static bool AddWindow(struct WindowSurface *windowSurface)
{
    struct ivi_layout_layer *layoutLayer = NULL;
    struct WmsContext *ctx = windowSurface->controller->pWmsCtx;
    struct WmsScreen *screen = NULL;
    uint32_t layerId;

    LOGD("start.");

    wl_list_for_each(screen, &ctx->wlListScreen, wlListLink) {
        if (screen->screenId == windowSurface->screenId
            ||  ctx->displayMode == WMS_DISPLAY_MODE_CLONE) {
            layerId = GetLayerId(windowSurface->type, screen->screenId);
            layoutLayer = GetLayer(screen->westonOutput, ctx->pLayoutInterface, layerId);
            if (!layoutLayer) {
                LOGE("GetLayer failed.");
                return false;
            }
            ctx->pLayoutInterface->layer_add_surface(layoutLayer,
                windowSurface->layoutSurface);
            ctx->pLayoutInterface->surface_set_visibility(
                windowSurface->layoutSurface, true);
        }
    }

    // window position,size calc.
    CalcWindowInfo(windowSurface);

    LOGD("end.");
    return true;
}

static void ControllerCommitChanges(const struct wl_client *client,
                                    const struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;

    ctx->pLayoutInterface->commit_changes();

    LOGD("end.");
}

static void ControllerSetDisplayMode(const struct wl_client *client,
                                     const struct wl_resource *resource,
                                     uint32_t displayMode)
{
    LOGD("start. displayMode %{public}d", displayMode);
    int32_t ret;
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;

    if (displayMode != WMS_DISPLAY_MODE_SINGLE &&
        displayMode != WMS_DISPLAY_MODE_CLONE &&
        displayMode != WMS_DISPLAY_MODE_EXTEND &&
        displayMode != WMS_DISPLAY_MODE_EXPAND) {
        LOGE("displayMode %{public}d erorr.", displayMode);
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    if (ctx->displayMode == displayMode) {
        LOGE("current displayMode is the same.");
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    if (displayMode == WMS_DISPLAY_MODE_CLONE) {
        ctx->pLayoutInterface->commit_changes();
        ret = ctx->pLayoutInterface->screen_clone(0, 1);
        LOGE("screen_clone ret = %{public}d", ret);
    } else {
        ctx->pLayoutInterface->commit_changes();
        ret = ctx->pLayoutInterface->screen_clear(1);
        LOGE("screen_clear ret = %{public}d", ret);
    }

    if (ret == IVI_SUCCEEDED) {
        ctx->displayMode = displayMode;
        wms_send_reply_error(resource, WMS_ERROR_OK);
        ScreenInfoChangerNotify();
    } else {
        wms_send_reply_error(resource, WMS_ERROR_INNER_ERROR);
    }

    LOGD("end. displayMode %{public}d", ctx->displayMode);
}

static void SetWindowType(const struct WmsContext *pCtx,
                          const struct WindowSurface *pWindowSurface,
                          const struct wl_resource *pWlResource,
                          uint32_t windowType)
{
    struct WmsScreen *pScreen = NULL;
    uint32_t layerIdOld;
    uint32_t layerIdNew;
    struct ivi_layout_layer *pLayoutLayerOld = NULL;
    struct ivi_layout_layer *pLayoutLayerNew = NULL;
    struct ivi_layout_interface_for_wms *pLayoutInterface = pCtx->pLayoutInterface;

    wl_list_for_each(pScreen, &pCtx->wlListScreen, wlListLink) {
        if (pScreen->screenId == pWindowSurface->screenId
            || pCtx->displayMode == WMS_DISPLAY_MODE_CLONE) {
            layerIdOld = GetLayerId(pWindowSurface->type, pScreen->screenId);
            pLayoutLayerOld = pLayoutInterface->get_layer_from_id(layerIdOld);
            if (!pLayoutLayerOld) {
                if (pScreen->screenId == pWindowSurface->screenId) {
                    LOGE("get_layer_from_id failed. layerId=%{public}d", layerIdOld);
                    wms_send_reply_error(pWlResource, WMS_ERROR_INNER_ERROR);
                    return;
                } else {
                    continue;
                }
            }

            layerIdNew = GetLayerId(windowType, pScreen->screenId);

            pLayoutLayerNew = GetLayer(pScreen->westonOutput, pLayoutInterface, layerIdNew);
            if (!pLayoutLayerNew) {
                LOGE("GetLayer failed.");
                wms_send_reply_error(pWlResource, WMS_ERROR_INNER_ERROR);
                return;
            }

            pLayoutInterface->layer_remove_surface(pLayoutLayerOld, pWindowSurface->layoutSurface);
            pLayoutInterface->layer_add_surface(pLayoutLayerNew, pWindowSurface->layoutSurface);
        }
    }
}

static void ControllerSetWindowType(const struct wl_client *pWlClient,
                                    const struct wl_resource *pWlResource,
                                    uint32_t windowId, uint32_t windowType)
{
    LOGD("start. windowId=%{public}d, windowType=%{public}d", windowId, windowType);
    struct WmsController *pWmsController = wl_resource_get_user_data(pWlResource);
    struct WmsContext *pWmsCtx = pWmsController->pWmsCtx;
    struct WindowSurface *pWindowSurface = NULL;

    if (!CheckWindowId(pWlClient, windowId)) {
        LOGE("CheckWindowId failed [%{public}d].", windowId);
        wms_send_reply_error(pWlResource, WMS_ERROR_PID_CHECK);
        return;
    }

    if (windowType >= WMS_WINDOW_TYPE_MAX_COUNT) {
        LOGE("windowType %{public}d error.", windowType);
        wms_send_reply_error(pWlResource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    pWindowSurface = GetSurface(&pWmsCtx->wlListWindow, windowId);
    if (!pWindowSurface) {
        LOGE("pWindowSurface is not found[%{public}d].", windowId);
        wms_send_reply_error(pWlResource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    if (pWindowSurface->type == windowType) {
        LOGE("window type is not need change.");
        wms_send_reply_error(pWlResource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    SetWindowType(pWmsCtx, pWindowSurface, pWlResource, windowType);

    pWindowSurface->type = windowType;

    wms_send_reply_error(pWlResource, WMS_ERROR_OK);
    LOGD("end.");
}

static void ControllerSetWindowVisibility(
    const struct wl_client *client, const struct wl_resource *resource,
    uint32_t windowId, uint32_t visibility)
{
    LOGD("start. windowId=%{public}d, visibility=%{public}d", windowId, visibility);
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;
    struct WindowSurface *windowSurface = NULL;

    if (!CheckWindowId(client, windowId)) {
        LOGE("CheckWindowId failed [%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_PID_CHECK);
        return;
    }

    windowSurface = GetSurface(&ctx->wlListWindow, windowId);
    if (!windowSurface) {
        LOGE("windowSurface is not found[%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    ctx->pLayoutInterface->surface_set_visibility(windowSurface->layoutSurface, visibility);

    wms_send_reply_error(resource, WMS_ERROR_OK);
    LOGD("end.");
}

static void ControllerSetWindowSize(const struct wl_client *client,
    const struct wl_resource *resource, uint32_t windowId,
    int32_t width, int32_t height)
{
    LOGD("start. windowId=%{public}d, width=%{public}d, height=%{public}d", windowId, width, height);
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;
    struct WindowSurface *windowSurface = NULL;

    if (!CheckWindowId(client, windowId)) {
        LOGE("CheckWindowId failed [%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_PID_CHECK);
        return;
    }

    windowSurface = GetSurface(&ctx->wlListWindow, windowId);
    if (!windowSurface) {
        LOGE("windowSurface is not found[%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    SetSourceRectangle(windowSurface, 0, 0, width, height);
    SetDestinationRectangle(windowSurface, windowSurface->x, windowSurface->y, width, height);

    windowSurface->width = width;
    windowSurface->height = height;
    wms_send_reply_error(resource, WMS_ERROR_OK);

    LOGD("end.");
}

static void ControllerSetWindowScale(const struct wl_client *client,
    const struct wl_resource *resource, uint32_t windowId,
    int32_t width, int32_t height)
{
    LOGD("start. windowId=%{public}d, width=%{public}d, height=%{public}d", windowId, width, height);
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;
    struct WindowSurface *windowSurface = NULL;

    if (!CheckWindowId(client, windowId)) {
        LOGE("CheckWindowId failed [%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_PID_CHECK);
        return;
    }

    windowSurface = GetSurface(&ctx->wlListWindow, windowId);
    if (!windowSurface) {
        LOGE("windowSurface is not found[%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    SetDestinationRectangle(windowSurface, windowSurface->x, windowSurface->y, width, height);
    windowSurface->width = width;
    windowSurface->height = height;

    wms_send_reply_error(resource, WMS_ERROR_OK);

    LOGD("end.");
}

static void ControllerSetWindowPosition(const struct wl_client *client,
    const struct wl_resource *resource, uint32_t windowId, int32_t x, int32_t y)
{
    LOGD("start. windowId=%{public}d, x=%{public}d, y=%{public}d", windowId, x, y);
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;

    if (!CheckWindowId(client, windowId)) {
        LOGE("CheckWindowId failed [%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_PID_CHECK);
        return;
    }

    struct WindowSurface *windowSurface = GetSurface(&ctx->wlListWindow, windowId);
    if (!windowSurface) {
        LOGE("windowSurface is not found[%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    SetDestinationRectangle(windowSurface, x, y, windowSurface->width, windowSurface->height);
    windowSurface->x = x;
    windowSurface->y = y;

    wms_send_reply_error(resource, WMS_ERROR_OK);
    LOGD("end.");
}

#ifndef USE_IVI_INPUT_FOCUS
static void PointerSetFocus(const struct WmsSeat *seat)
{
    LOGD("start.");
    LOGD("seat->pWestonSeat->seat_name: [%{public}s]\n", seat->pWestonSeat->seat_name);
    LOGD("seat->deviceFlags: %{public}d", seat->deviceFlags);
    LOGD("seat->focusWindowId: %{public}d", seat->focusWindowId);

    struct WindowSurface *forcedWindow = GetSurface(&seat->pWmsCtx->wlListWindow, seat->focusWindowId);
    if (!forcedWindow) {
        LOGE("forcedWindow is not found[%{public}d].", seat->focusWindowId);
        return;
    }

    struct weston_pointer *pointer = weston_seat_get_pointer(seat->pWestonSeat);
    if (!pointer) {
        LOGE("weston_seat_get_pointer is NULL.");
        return;
    }

    if (pointer->button_count > 0) {
        LOGE("pointer->button_count > 0");
        return;
    }

    if (pointer->focus != NULL) {
        LOGE("weston_pointer_clear_focus.");
        weston_pointer_clear_focus(pointer);
    }

    struct weston_surface *forcedSurface = forcedWindow->surface;
    LOGI("weston_pointer_set_focus0.");
    if (forcedSurface != NULL && !wl_list_empty(&forcedSurface->views)) {
        LOGI("weston_pointer_set_focus1.");
        struct weston_view *view = wl_container_of(forcedSurface->views.next, view, surface_link);
        wl_fixed_t sx, sy;
        weston_view_from_global_fixed(view, pointer->x, pointer->y, &sx, &sy);
        LOGI("weston_pointer_set_focus x[%{public}d], y[%{public}d], sx[%{public}d], sy[%{public}d].",
            pointer->x, pointer->y, sx, sy);
        if (pointer->focus != view) {
            LOGI("weston_pointer_set_focus2.");
            weston_pointer_set_focus(pointer, view, sx, sy);
        }
        LOGI("weston_pointer_set_focus3.");
    }

    LOGD("end.");
}
#endif

static bool FocusUpdate(const struct WindowSurface *surface)
{
    LOGD("start.");
    int flag = INPUT_DEVICE_ALL;

    if ((surface->type == WMS_WINDOW_TYPE_STATUS_BAR) || (surface->type == WMS_WINDOW_TYPE_NAVI_BAR)) {
        flag = INPUT_DEVICE_POINTER | INPUT_DEVICE_TOUCH;
    }

#ifdef USE_IVI_INPUT_FOCUS
    int surfaceCount = 0;
    struct ivi_layout_surface **surfaceList = NULL;
    struct ivi_layout_interface_for_wms *layoutInterface = surface->controller->pWmsCtx->pLayoutInterface;
    struct ivi_input_interface_for_wms *pInputInterface = surface->controller->pWmsCtx->pInputInterface;
    int32_t layerId = GetLayerId(surface->type, surface->screenId);
    struct ivi_layout_layer *layoutLayer = layoutInterface->get_layer_from_id(layerId);
    if (!layoutLayer) {
        LOGE("get_layer_from_id failed.");
        return false;
    }

    if (layoutInterface->get_surfaces_on_layer(layoutLayer, &surfaceCount, &surfaceList) == IVI_FAILED) {
        LOGE("get_surfaces_on_layer failed.");
        return false;
    }

    for (int j = 0; j < surfaceCount; j++) {
        pInputInterface->set_focus(surfaceList[j]->id_surface, flag, false);
    }
    pInputInterface->set_focus(surface->surfaceId, flag, true);

    free(surfaceList);
#else
    struct WmsContext *pWmsCtx = surface->controller->pWmsCtx;
    struct WmsScreen *pScreen = GetScreen(surface);
    if (!pScreen) {
        LOGE("GetScreen error.");
        return false;
    }

    struct WmsSeat *pSeat = NULL;
    wl_list_for_each(pSeat, &pWmsCtx->wlListSeat, wlListLink) {
        if (!strcmp(pSeat->pWestonSeat->seat_name, "default")) {
            pSeat->deviceFlags = flag;
            pSeat->focusWindowId = surface->surfaceId;
            PointerSetFocus(pSeat);
        }
    }
    SeatInfoChangerNotify();
#endif

    LOGD("end.");
    return true;
}

static void ControllerSetWindowTop(const struct wl_client *client,
    const struct wl_resource *resource, uint32_t windowId)
{
    LOGD("start. windowId=%{public}d", windowId);
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;
    struct WindowSurface *windowSurface = NULL;

    if (!CheckWindowId(client, windowId)) {
        LOGE("CheckWindowId failed [%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_PID_CHECK);
        return;
    }

    windowSurface = GetSurface(&ctx->wlListWindow, windowId);
    if (!windowSurface) {
        LOGE("windowSurface is not found[%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    ctx->pLayoutInterface->surface_change_top(windowSurface->layoutSurface);
    if (!FocusUpdate(windowSurface)) {
        LOGE("FocusUpdate failed.");
        wms_send_reply_error(resource, WMS_ERROR_INNER_ERROR);
        return;
    }

    wms_send_reply_error(resource, WMS_ERROR_OK);
    LOGD("end.");
}

static void ControllerDestroyWindow(const struct wl_client *client,
    const struct wl_resource *resource, uint32_t windowId)
{
    LOGD("start. windowId=%{public}d", windowId);
    struct WmsController *controller = wl_resource_get_user_data(resource);
    struct WmsContext *ctx = controller->pWmsCtx;
    struct WindowSurface *windowSurface = NULL;

    if (!CheckWindowId(client, windowId)) {
        LOGE("CheckWindowId failed [%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_PID_CHECK);
        return;
    }

    windowSurface = GetSurface(&ctx->wlListWindow, windowId);
    if (!windowSurface) {
        LOGE("windowSurface is not found[%{public}d].", windowId);
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    SurfaceDestroy(windowSurface);
    wms_send_reply_error(resource, WMS_ERROR_OK);
    LOGD("end.");
}

static void WindowPropertyChanged(const struct wl_listener *listener, const struct ivi_layout_surface *data)
{
    LOGD("start.");
    ScreenInfoChangerNotify();
    LOGD("end.");
}

static void CreateWindow(struct WmsController *pWmsController,
    struct weston_surface *pWestonSurface,
    uint32_t windowId, uint32_t screenId, uint32_t windowType)
{
    struct WindowSurface *pWindow = NULL;
    struct WmsContext *pWmsCtx = pWmsController->pWmsCtx;
    struct wl_resource *pWlResource = pWmsController->pWlResource;

    pWindow = calloc(1, sizeof(*pWindow));
    if (!pWindow) {
        LOGE("calloc failed.");
        wl_client_post_no_memory(pWmsController->pWlClient);
        wms_send_window_status(pWlResource,
            WMS_WINDOW_STATUS_FAILED, WINDOW_ID_INVALID, 0, 0, 0, 0);

        ClearWindowId(pWmsController, windowId);
        return;
    }

    pWindow->layoutSurface = pWmsCtx->pLayoutInterface->surface_create(pWestonSurface, windowId);
    /* check if windowId is already used for wl_surface */
    if (pWindow->layoutSurface == NULL) {
        LOGE("layoutInterface->surface_create failed.");
        wms_send_window_status(pWlResource,
            WMS_WINDOW_STATUS_FAILED, WINDOW_ID_INVALID, 0, 0, 0, 0);

        ClearWindowId(pWmsController, windowId);
        free(pWindow);
        return;
    }

    pWindow->controller = pWmsController;
    pWindow->surface = pWestonSurface;
    pWindow->surfaceId = windowId;
    pWindow->type = windowType;
    pWindow->screenId = screenId;

    if (!AddWindow(pWindow)) {
        LOGE("AddWindow failed.");
        wms_send_window_status(pWlResource, WMS_WINDOW_STATUS_FAILED, WINDOW_ID_INVALID, 0, 0, 0, 0);

        pWmsCtx->pLayoutInterface->surface_destroy(pWindow->layoutSurface);
        ClearWindowId(pWmsController, windowId);
        free(pWindow);
        return;
    }

    pWestonSurface->committed = WindowSurfaceCommitted;
    pWestonSurface->committed_private = pWindow;

    wl_list_init(&pWindow->link);
    wl_list_insert(&pWmsCtx->wlListWindow, &pWindow->link);

    pWindow->surfaceDestroyListener.notify = WindowSurfaceDestroy;
    wl_signal_add(&pWestonSurface->destroy_signal, &pWindow->surfaceDestroyListener);

    pWindow->propertyChangedListener.notify = WindowPropertyChanged;
    wl_signal_add(&pWindow->layoutSurface->property_changed, &pWindow->propertyChangedListener);

    wms_send_window_status(pWlResource, WMS_WINDOW_STATUS_CREATED, windowId,
                           pWindow->x, pWindow->y, pWindow->width, pWindow->height);
    SendGlobalWindowStatus(pWmsController, windowId, WMS_WINDOW_STATUS_CREATED);
}

static void ControllerCreateWindow(const struct wl_client *pWlClient,
    const struct wl_resource *pWlResource,
    const struct wl_resource *pWlSurfaceResource,
    uint32_t screenId, uint32_t windowType)
{
    LOGD("start. screenId=%{public}d, windowType=%{public}d", screenId, windowType);
    uint32_t windowId = WINDOW_ID_INVALID;
    struct WmsController *pWmsController = wl_resource_get_user_data(pWlResource);
    struct WmsContext *pWmsCtx = pWmsController->pWmsCtx;
    struct weston_surface *westonSurface = wl_resource_get_user_data(pWlSurfaceResource);

    if (windowType >= WMS_WINDOW_TYPE_MAX_COUNT) {
        LOGE("windowType %{public}d error.", windowType);
        wms_send_window_status(pWlResource, WMS_WINDOW_STATUS_FAILED, WINDOW_ID_INVALID, 0, 0, 0, 0);
        return;
    }

    if (screenId > 0 && pWmsCtx->displayMode != WMS_DISPLAY_MODE_EXPAND) {
        LOGE("screenId %{public}d error.", screenId);
        wms_send_window_status(pWlResource, WMS_WINDOW_STATUS_FAILED, WINDOW_ID_INVALID, 0, 0, 0, 0);
        return;
    }

    if (westonSurface->committed == WindowSurfaceCommitted &&
        westonSurface->committed_private != NULL) {
        LOGE("the westonSurface is using by other window.");
        wms_send_window_status(pWlResource, WMS_WINDOW_STATUS_FAILED, WINDOW_ID_INVALID, 0, 0, 0, 0);
        return;
    }

    windowId = GetWindowId(pWmsController);
    if (windowId != WINDOW_ID_INVALID) {
        CreateWindow(pWmsController, westonSurface, windowId, screenId, windowType);
    } else {
        LOGE("create window restriction..");
        wms_send_window_status(pWlResource, WMS_WINDOW_STATUS_FAILED, WINDOW_ID_INVALID, 0, 0, 0, 0);
    }

    LOGD("end.");
}

static int CreateScreenshotFile(off_t size)
{
    char template[] = SCREEN_SHOT_FILE_PATH;
    int fd = mkstemp(template);
    if (fd < 0) {
        return -1;
    }

    unlink(template);
    if (ftruncate(fd, size) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static void ControllerWindowshot(const struct wl_client *pWlClient,
    const struct wl_resource *pWlResource, uint32_t windowId)
{
    LOGD("start. windowId = %{public}d.", windowId);
    struct WmsController *pWmsController = wl_resource_get_user_data(pWlResource);

    struct WindowSurface *pWindowSurface = GetSurface(&pWmsController->pWmsCtx->wlListWindow, windowId);
    if (!pWindowSurface) {
        LOGE("pWindowSurface is not found[%{public}d].", windowId);
        wms_send_windowshot_error(pWlResource, WMS_ERROR_INVALID_PARAM, windowId);
        return;
    }

    if (!pWindowSurface->surface) {
        LOGE("pWindowSurface->surface is NULL.");
        wms_send_windowshot_error(pWlResource, WMS_ERROR_INNER_ERROR, windowId);
        return;
    }

    int32_t width = 0;
    int32_t height = 0;
    weston_surface_get_content_size(pWindowSurface->surface, &width, &height);
    int32_t stride = width * BYTE_SPP_SIZE;

    if (!width || !height || !stride) {
        LOGE("weston_surface_get_content_size error.");
        wms_send_windowshot_error(pWlResource, WMS_ERROR_INNER_ERROR, windowId);
        return;
    }

    int32_t size = stride * height;
    int fd = CreateScreenshotFile(size);
    if (fd < 0) {
        LOGE("CreateScreenshotFile error.");
        wms_send_windowshot_error(pWlResource, WMS_ERROR_INNER_ERROR, windowId);
        return;
    }

    char *pBuffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pBuffer == MAP_FAILED) {
        LOGE("mmap error.");
        wms_send_windowshot_error(pWlResource, WMS_ERROR_INNER_ERROR, windowId);
        close(fd);
        return;
    }

    if (weston_surface_copy_content(pWindowSurface->surface, pBuffer, size, 0, 0, width, height) != 0) {
        LOGE("weston_surface_copy_content error.");
        wms_send_windowshot_error(pWlResource, WMS_ERROR_INNER_ERROR, windowId);
        munmap(pBuffer, size);
        close(fd);
        return;
    }

    struct timespec timeStamp;
    weston_compositor_read_presentation_clock(pWmsController->pWmsCtx->pCompositor, &timeStamp);

    wms_send_windowshot_done(pWlResource, windowId, fd, width, height,
        stride, WL_SHM_FORMAT_ABGR8888, timeStamp.tv_sec, timeStamp.tv_nsec);

    munmap(pBuffer, size);
    close(fd);
}

static void FlipY(int32_t stride, int32_t height, uint32_t *data)
{
    int i, y, p, q;
    // assuming stride aligned to 4 bytes
    int pitch = stride / sizeof(*data);
    for (y = 0; y < height / HEIRHT_AVERAGE; ++y) {
        p = y * pitch;
        q = (height - y - 1) * pitch;
        for (i = 0; i < pitch; ++i) {
            uint32_t tmp = data[p + i];
            data[p + i] = data[q + i];
            data[q + i] = tmp;
        }
    }
}

static void ClearFrameListener(const struct ScreenshotFrameListener *pListener)
{
    wl_list_remove(&pListener->frameListener.link);
    wl_list_init(&pListener->frameListener.link);
    wl_list_remove(&pListener->outputDestroyed.link);
    wl_list_init(&pListener->outputDestroyed.link);
}

static void Screenshot(const struct ScreenshotFrameListener *pFrameListener, uint32_t shmFormat)
{
    struct WmsController *pWmsController = wl_container_of(pFrameListener, pWmsController, stListener);
    struct weston_output *westonOutput = pFrameListener->output;
    int32_t width = westonOutput->current_mode->width;
    int32_t height = westonOutput->current_mode->height;
    pixman_format_code_t format = westonOutput->compositor->read_format;
    int32_t stride = width * (PIXMAN_FORMAT_BPP(format) / PIXMAN_FORMAT_AVERAGE);
    size_t size = stride * height;

    int fd = CreateScreenshotFile(size);
    if (fd < 0) {
        LOGE("CreateScreenshotFile error.");
        wms_send_screenshot_error(pWmsController->pWlResource, WMS_ERROR_INNER_ERROR, pFrameListener->idScreen);
        return;
    }

    uint32_t *readPixs = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (readPixs == MAP_FAILED) {
        LOGE("mmap error.");
        wms_send_screenshot_error(pWmsController->pWlResource, WMS_ERROR_INNER_ERROR, pFrameListener->idScreen);
        close(fd);
        return;
    }

    if (westonOutput->compositor->renderer->read_pixels(westonOutput, format, readPixs, 0, 0, width, height) < 0) {
        LOGE("read_pixels error.");
        wms_send_screenshot_error(pWmsController->pWlResource, WMS_ERROR_INNER_ERROR, pFrameListener->idScreen);
        munmap(readPixs, size);
        close(fd);
        return;
    }

    if (westonOutput->compositor->capabilities & WESTON_CAP_CAPTURE_YFLIP) {
        FlipY(stride, height, readPixs);
    }

    wms_send_screenshot_done(pWmsController->pWlResource, pFrameListener->idScreen, fd, width, height, stride,
        shmFormat, westonOutput->frame_time.tv_sec, westonOutput->frame_time.tv_nsec);

    munmap(readPixs, size);
    close(fd);
}

static void ScreenshotNotify(const struct wl_listener *pWlListener, const struct weston_compositor *pCompositor)
{
    LOGD("start.");
    struct ScreenshotFrameListener *pFrameListener = wl_container_of(pWlListener, pFrameListener, frameListener);
    struct WmsController *pWmsController = wl_container_of(pFrameListener, pWmsController, stListener);
    pixman_format_code_t format = pFrameListener->output->compositor->read_format;
    uint32_t shmFormat;

    --pFrameListener->output->disable_planes;
    ClearFrameListener(pFrameListener);

    switch (format) {
        case PIXMAN_a8r8g8b8:
            shmFormat = WL_SHM_FORMAT_ARGB8888;
            break;
        case PIXMAN_x8r8g8b8:
            shmFormat = WL_SHM_FORMAT_XRGB8888;
            break;
        case PIXMAN_a8b8g8r8:
            shmFormat = WL_SHM_FORMAT_ABGR8888;
            break;
        case PIXMAN_x8b8g8r8:
            shmFormat = WL_SHM_FORMAT_XBGR8888;
            break;
        default:
            LOGE("unsupported pixel format = %{public}d", format);
            wms_send_screenshot_error(pWmsController->pWlResource, WMS_ERROR_INNER_ERROR, pFrameListener->idScreen);
            return;
    }

    Screenshot(pFrameListener, shmFormat);
    LOGD("end.");
}

static void ScreenshotOutputDestroyed(const struct wl_listener *pListener,
                                      const struct weston_compositor *pData)
{
    LOGD("start.");
    struct ScreenshotFrameListener *pFrameListener =
        wl_container_of(pListener, pFrameListener, outputDestroyed);
    struct WmsController *pController =
        wl_container_of(pFrameListener, pController, stListener);

    LOGE("screen[%{public}d] output destroyed.", pFrameListener->idScreen);
    wms_send_screenshot_error(pController->pWlResource, WMS_ERROR_INNER_ERROR, pFrameListener->idScreen);
    ClearFrameListener(pFrameListener);

    LOGD("end.");
}

static void ControllerScreenshot(const struct wl_client *pClient,
    const struct wl_resource *pResource, uint32_t screenId)
{
    LOGD("start. screenId = %{public}d.", screenId);
    struct WmsController *pWmsController = wl_resource_get_user_data(pResource);

    struct WmsScreen *pWmsScreen = GetScreenFromId(pWmsController->pWmsCtx, screenId);
    if (!pWmsScreen) {
        LOGE("screen is not found[%{public}d].", screenId);
        wms_send_screenshot_error(pResource, WMS_ERROR_INVALID_PARAM, screenId);
        return;
    }

    pWmsController->stListener.output = pWmsScreen->westonOutput;

    pWmsController->stListener.outputDestroyed.notify = ScreenshotOutputDestroyed;
    wl_signal_add(&pWmsScreen->westonOutput->destroy_signal, &pWmsController->stListener.outputDestroyed);

    pWmsController->stListener.frameListener.notify = ScreenshotNotify;
    wl_signal_add(&pWmsScreen->westonOutput->frame_signal, &pWmsController->stListener.frameListener);

    pWmsScreen->westonOutput->disable_planes++;
    weston_output_damage(pWmsScreen->westonOutput);

    LOGD("end.");
}

static void AddGlobalWindowStatus(const struct WmsController *pController)
{
    LOGD("start.");
    struct WmsContext *pWmsCtx = pController->pWmsCtx;
    struct WmsController *pControllerTemp;
    bool found = false;

    wl_list_for_each(pControllerTemp, &pWmsCtx->wlListGlobalEventResource, wlListLinkRes) {
        if (pControllerTemp == pController) {
            LOGE("GlobalWindowStatus is already set.");
            found = true;
        }
    }

    if (!found) {
        wl_list_insert(&pWmsCtx->wlListGlobalEventResource, &pController->wlListLinkRes);
    }

    LOGD("end.");
}

static void ControllerSetGlobalWindowStatus(const struct wl_client *pClient,
    const struct wl_resource *pResource, int32_t status)
{
    LOGD("start. status = %{public}d.", status);
    struct WmsController *pWmsController = wl_resource_get_user_data(pResource);
    struct WmsContext *pWmsCtx = pWmsController->pWmsCtx;

    if (status == 0) {
        wl_list_remove(&pWmsController->wlListLinkRes);
    } else {
        AddGlobalWindowStatus(pWmsController);
    }

    wms_send_reply_error(pResource, WMS_ERROR_OK);
    LOGD("end.");
}

// wms controller interface implementation
static const struct wms_interface g_controllerImplementation = {
    ControllerCreateWindow,
    ControllerDestroyWindow,
    ControllerSetWindowTop,
    ControllerSetWindowSize,
    ControllerSetWindowScale,
    ControllerSetWindowPosition,
    ControllerSetWindowVisibility,
    ControllerSetWindowType,
    ControllerSetDisplayMode,
    ControllerCommitChanges,
    ControllerSetGlobalWindowStatus,
    ControllerScreenshot,
    ControllerWindowshot,
};

static void UnbindWmsController(const struct wl_resource *pResource)
{
    LOGD("start.");
    struct WmsController *pController = wl_resource_get_user_data(pResource);
    struct WmsContext *pWmsCtx = pController->pWmsCtx;
    struct WindowSurface *pWindowSurface = NULL;
    struct WindowSurface *pNext = NULL;

    struct WmsController *pControllerTemp = NULL;
    struct WmsController *pControllerNext = NULL;

    wl_list_remove(&pController->wlListLinkRes);

    wl_list_for_each_safe(pWindowSurface, pNext, &pWmsCtx->wlListWindow, link) {
        if (pWindowSurface->controller == pController) {
            SurfaceDestroy(pWindowSurface);
        }
    }

    wl_list_remove(&pController->wlListLink);
    wl_list_remove(&pController->stListener.frameListener.link);
    wl_list_remove(&pController->stListener.outputDestroyed.link);

    free(pController);
    pController = NULL;
    LOGD("end.");
}

static void BindWmsController(struct wl_client *pClient,
    struct WmsContext *pData, uint32_t version, uint32_t id)
{
    LOGD("start.");
    struct WmsContext *pCtx = pData;
    (void)version;

    struct WmsController *pController = calloc(1, sizeof(*pController));
    if (pController == NULL) {
        LOGE("calloc failed");
        wl_client_post_no_memory(pClient);
        return;
    }

    pController->pWlResource = wl_resource_create(pClient, &wms_interface, version, id);
    if (pController->pWlResource == NULL) {
        LOGE("wl_resource_create failed");
        wl_client_post_no_memory(pClient);
        return;
    }

    wl_resource_set_implementation(pController->pWlResource,
        &g_controllerImplementation, pController, UnbindWmsController);
    pController->pWmsCtx = pCtx;
    pController->pWlClient = pClient;
    pController->id = id;

    wl_list_init(&pController->wlListLinkRes);
    wl_list_insert(&pCtx->wlListController, &pController->wlListLink);
    wl_list_init(&pController->stListener.frameListener.link);
    wl_list_init(&pController->stListener.outputDestroyed.link);

    struct weston_output *pOutput = NULL;
    wl_list_for_each(pOutput, &pCtx->pCompositor->output_list, link) {
        wms_send_screen_status(pController->pWlResource, pOutput->id, pOutput->name, WMS_SCREEN_STATUS_ADD,
#ifdef USE_DUMMY_SCREEN
            DUMMY_SCREEN_WIDTH, DUMMY_SCREEN_HEIGHT);
#else
            pOutput->width, pOutput->height);
#endif /* USE_DUMMY_SCREEN */
    }

#ifdef USE_DUMMY_SCREEN
    pOutput = pCtx->pLayoutInterface->get_dummy_output();
    wms_send_screen_status(pController->pWlResource, pOutput->id, pOutput->name, WMS_SCREEN_STATUS_ADD,
                           pOutput->width, pOutput->height);
#endif /* USE_DUMMY_SCREEN */

    uint32_t flag = GetDisplayModeFlag(pController->pWmsCtx);
    wms_send_display_mode(pController->pWlResource, flag);

    LOGD("end.");
}

static void DestroyScreen(const struct WmsScreen *pScreen)
{
    wl_list_remove(&pScreen->wlListLink);
    free(pScreen);
}

static void WmsScreenDestroy(const struct WmsContext *pCtx)
{
    LOGD("start.");
    struct WmsScreen *pScreen = NULL;
    struct WmsScreen *pNext = NULL;

    wl_list_for_each_safe(pScreen, pNext, &pCtx->wlListScreen, wlListLink) {
        DestroyScreen(pScreen);
    }
    LOGD("end.");
}

static void DestroySeat(const struct WmsSeat *pSeat)
{
    LOGD("start.");
    wl_list_remove(&pSeat->wlListenerDestroyed.link);
    wl_list_remove(&pSeat->wlListLink);
    free(pSeat);
    LOGD("end.");
}

static void SeatDestroyedEvent(const struct wl_listener *listener, const struct weston_seat *seat)
{
    LOGD("start.");
    struct WmsSeat *pSeat = wl_container_of(listener, pSeat, wlListenerDestroyed);
    wl_list_remove(&pSeat->wlListenerDestroyed.link);
    wl_list_remove(&pSeat->wlListLink);
    free(pSeat);
    SeatInfoChangerNotify();
    LOGD("end.");
}

static void WmsControllerDestroy(const struct wl_listener *listener,
    const struct weston_compositor *data)
{
    LOGD("start.");
    struct WmsContext *ctx =
        wl_container_of(listener, ctx, wlListenerDestroy);
    struct WmsController *pController = NULL;
    struct WmsController *pCtlNext = NULL;
    struct WmsSeat *pSeat = NULL;
    struct WmsSeat *pNext = NULL;

    wl_list_for_each_safe(pController, pCtlNext, &ctx->wlListController, wlListLink) {
        wl_resource_destroy(pController->pWlResource);
    }

    wl_list_remove(&ctx->wlListenerDestroy.link);
    wl_list_remove(&ctx->wlListenerOutputCreated.link);
    wl_list_remove(&ctx->wlListenerOutputDestroyed.link);
    wl_list_remove(&ctx->wlListenerSeatCreated.link);

    WmsScreenDestroy(ctx);
    wl_list_for_each_safe(pSeat, pNext, &ctx->wlListSeat, wlListLink) {
        DestroySeat(pSeat);
    }

    free(ctx);
    LOGD("end.");
}

static int32_t CreateScreen(struct WmsContext *pCtx,
                            struct weston_output *pOutput)
{
    struct WmsScreen *pScreen;

    pScreen = calloc(1, sizeof(*pScreen));
    if (pScreen == NULL) {
        LOGE("no memory to allocate client screen\n");
        return -1;
    }
    pScreen->pWmsCtx = pCtx;
    pScreen->westonOutput = pOutput;
    pScreen->screenId = pOutput->id;

    wl_list_insert(&pCtx->wlListScreen, &pScreen->wlListLink);

    return 0;
}

static void OutputDestroyedEvent(const struct wl_listener *listener,
                                 struct weston_output *data)
{
    LOGD("start.");
    struct WmsContext *pCtx = wl_container_of(listener, pCtx, wlListenerOutputDestroyed);
    struct WmsScreen *pScreen = NULL;
    struct WmsScreen *pNext = NULL;
    struct weston_output *destroyedOutput = (struct weston_output*)data;

    wl_list_for_each_safe(pScreen, pNext, &pCtx->wlListScreen, wlListLink) {
        if (pScreen->westonOutput == destroyedOutput) {
            DestroyScreen(pScreen);
        }
    }

    DisplayModeUpdate(pCtx);

    ScreenInfoChangerNotify();

    LOGD("end.");
}

static void OutputCreatedEvent(const struct wl_listener *listener, struct weston_output *data)
{
    LOGD("start.");
    struct WmsContext *ctx = wl_container_of(listener, ctx, wlListenerOutputCreated);
    struct weston_output *createdOutput = (struct weston_output*)data;

    CreateScreen(ctx, createdOutput);

    DisplayModeUpdate(ctx);

    ScreenInfoChangerNotify();

    LOGD("end.");
}

static void SeatCreatedEvent(const struct wl_listener *listener, struct weston_seat *seat)
{
    LOGD("start.");
    struct WmsContext *pCtx = wl_container_of(listener, pCtx, wlListenerSeatCreated);

    struct WmsSeat *pSeat = NULL;
    pSeat = calloc(1, sizeof(*pSeat));
    if (pSeat == NULL) {
        LOGE("no memory to allocate wms seat.");
        return;
    }
    pSeat->pWmsCtx = pCtx;
    pSeat->pWestonSeat = seat;
    wl_list_insert(&pCtx->wlListSeat, &pSeat->wlListLink);

    pSeat->wlListenerDestroyed.notify = &SeatDestroyedEvent;
    wl_signal_add(&seat->destroy_signal, &pSeat->wlListenerDestroyed);
    SeatInfoChangerNotify();
    LOGD("end.");
}

int ScreenInfoInit(const struct weston_compositor *pCompositor);

static int WmsContextInit(struct WmsContext *ctx, struct weston_compositor *compositor)
{
    wl_list_init(&ctx->wlListController);
    wl_list_init(&ctx->wlListWindow);
    wl_list_init(&ctx->wlListScreen);
    wl_list_init(&ctx->wlListSeat);
    wl_list_init(&ctx->wlListGlobalEventResource);

    ctx->pCompositor = compositor;
    ctx->pLayoutInterface = ivi_layout_get_api_for_wms(compositor);
    if (!ctx->pLayoutInterface) {
        free(ctx);
        LOGE("ivi_xxx_get_api_for_wms failed.");
        return -1;
    }

#ifdef USE_IVI_INPUT_FOCUS
    ctx->pInputInterface = ivi_input_get_api_for_wms(compositor);
    if (!ctx->pInputInterface) {
        free(ctx);
        LOGE("ivi_xxx_get_api_for_wms failed.");
        return -1;
    }
#endif

    ctx->wlListenerOutputCreated.notify = OutputCreatedEvent;
    ctx->wlListenerOutputDestroyed.notify = OutputDestroyedEvent;

    wl_signal_add(&compositor->output_created_signal, &ctx->wlListenerOutputCreated);
    wl_signal_add(&compositor->output_destroyed_signal, &ctx->wlListenerOutputDestroyed);

    ctx->wlListenerSeatCreated.notify = &SeatCreatedEvent;
    wl_signal_add(&compositor->seat_created_signal, &ctx->wlListenerSeatCreated);

    struct weston_seat *seat = NULL;
    wl_list_for_each(seat, &compositor->seat_list, link) {
        SeatCreatedEvent(&ctx->wlListenerSeatCreated, seat);
    }

    if (!wl_global_create(compositor->wl_display,
        &wms_interface, 1, ctx, BindWmsController)) {
        LOGE("wl_global_create failed.");
        return -1;
    }

    ctx->wlListenerDestroy.notify = WmsControllerDestroy;
#ifdef USE_DUMMY_SCREEN
    ctx->displayMode = WMS_DISPLAY_MODE_CLONE;
#else
    ctx->displayMode = WMS_DISPLAY_MODE_SINGLE;
#endif
    wl_signal_add(&compositor->destroy_signal, &ctx->wlListenerDestroy);
    return 0;
}

WL_EXPORT int wet_module_init(struct weston_compositor *compositor,
    int *argc, char *argv[])
{
    LOGD("start.");
    struct weston_output *output = NULL;
    struct WmsContext *ctx = GetWmsInstance();

    if (WmsContextInit(ctx, compositor) < 0) {
        LOGE("WmsContextInit failed.");
        return -1;
    }

    wl_list_for_each(output, &compositor->output_list, link) {
        if (CreateScreen(ctx, output) < 0) {
            WmsScreenDestroy(ctx);
            free(ctx);
            return -1;
        }
    }

#ifdef USE_DUMMY_SCREEN
    output = ctx->pLayoutInterface->get_dummy_output();
    if (CreateScreen(ctx, output) < 0) {
        WmsScreenDestroy(ctx);
        free(ctx);
        return -1;
    }
#endif /* USE_DUMMY_SCREEN */

    ScreenInfoInit(compositor);

    LOGD("end.");
    return 0;
}
