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

#include "screen_info.h"

#include <securec.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <ivi-layout-export.h>
#include <ivi-layout-private.h>
#include <libinput-seat-export.h>
#include "wmserver.h"

#define LOG_LABEL "screen-info"
#define MAX_LENGTH 1024

#define LOGD(fmt, ...) weston_log("%{public}s debug %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

#define LOGI(fmt, ...) weston_log("%{public}s info %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

#define LOGE(fmt, ...) weston_log("%{public}s error %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

void SetLibInputEventListener(const LibInputEventListener listener)
{
    set_libinput_event_listener(listener);
}

void FreeSeatsInfo(struct SeatInfo **seats)
{
    if (!seats) {
        LOGE("seats is null.");
        return;
    }
    for (int i = 0; seats[i]; i++) {
        if (seats[i]->seatName) {
            free(seats[i]->seatName);
        }
        free(seats[i]);
    }
    free(seats);
}

struct SeatInfo **GetSeatsInfo(void)
{
    struct WmsContext *pWmsCtx = GetWmsInstance();
    uint32_t seatNum = wl_list_length(&pWmsCtx->wlListSeat);
    struct SeatInfo **seats = (struct SeatInfo **)calloc(seatNum + 1, sizeof(intptr_t));
    if (!seats) {
        LOGE("calloc error!");
        return NULL;
    }

    struct WmsSeat *pWmsSeat = NULL;
    int index = 0;
    wl_list_for_each(pWmsSeat, &pWmsCtx->wlListSeat, wlListLink) {
        seats[index] = (struct SeatInfo *)calloc(1, sizeof(struct SeatInfo));
        if (!seats[index]) {
            LOGE("calloc error!");
            FreeSeatsInfo(seats);
            return NULL;
        }

        seats[index]->seatName = (char *)calloc(1, strlen(pWmsSeat->pWestonSeat->seat_name) + 1);
        if (!seats[index]->seatName) {
            LOGE("calloc error!");
            FreeSeatsInfo(seats);
            return NULL;
        }
        if (strcpy_s(seats[index]->seatName,
            strlen(pWmsSeat->pWestonSeat->seat_name) + 1, pWmsSeat->pWestonSeat->seat_name)) {
            LOGE("strcpy_s error!");
            FreeSeatsInfo(seats);
            return NULL;
        }
        seats[index]->deviceFlags = pWmsSeat->deviceFlags;
        seats[index]->focusWindowId = pWmsSeat->focusWindowId;
        index++;
    }

    return seats;
}

void FreeSurfaceInfo(struct SurfaceInfo *pSurface)
{
    if (pSurface) {
        free(pSurface);
    }
}

void FreeLayerInfo(struct LayerInfo *pLayer)
{
    if (pLayer) {
        if (pLayer->surfaces) {
            for (int i = 0; i < pLayer->nSurfaces; i++) {
                FreeSurfaceInfo(pLayer->surfaces[i]);
            }
            free(pLayer->surfaces);
        }
        free(pLayer);
    }
}

void FreeScreenInfo(struct ScreenInfo *pScreen)
{
    if (pScreen) {
        if (pScreen->layers) {
            for (int i = 0; i < pScreen->nLayers; i++) {
                FreeLayerInfo(pScreen->layers[i]);
            }
            free(pScreen->layers);
        }
        if (pScreen->connectorName) {
            free(pScreen->connectorName);
        }
        free(pScreen);
    }
}

void FreeScreensInfo(struct ScreenInfo **screens)
{
    if (!screens) {
        LOGE("screens is null.");
        return;
    }
    for (int i = 0; screens[i]; i++) {
        FreeScreenInfo(screens[i]);
    }
    free(screens);
}

struct SurfaceInfo *GetSurfaceInfo(struct ivi_layout_surface *surface)
{
    const struct ivi_layout_interface_for_wms *pLayoutInterface = GetWmsInstance()->pLayoutInterface;
    struct SurfaceInfo *pSurfaceInfo = (struct SurfaceInfo *)calloc(1, sizeof(struct SurfaceInfo));
    if (!pSurfaceInfo) {
        LOGE("calloc error!");
        return NULL;
    }

    struct ivi_layout_surface_properties *prop = &surface->prop;
    pSurfaceInfo->surfaceId = pLayoutInterface->get_id_of_surface(surface);
    pSurfaceInfo->dstX = prop->dest_x;
    pSurfaceInfo->dstY = prop->dest_y;
    pSurfaceInfo->dstW = prop->dest_width;
    pSurfaceInfo->dstH = prop->dest_height;
    pSurfaceInfo->srcX = prop->source_x;
    pSurfaceInfo->srcY = prop->source_y;
    pSurfaceInfo->srcW = prop->source_width;
    pSurfaceInfo->srcH = prop->source_height;
    pSurfaceInfo->opacity = wl_fixed_to_double(prop->opacity);
    pSurfaceInfo->visibility = prop->visibility;

    return pSurfaceInfo;
}

struct LayerInfo *GetLayerInfo(struct ivi_layout_layer *layer)
{
    const struct ivi_layout_interface_for_wms *pLayoutInterface = GetWmsInstance()->pLayoutInterface;
    struct LayerInfo *pLayerInfo = (struct LayerInfo *)calloc(1, sizeof(struct LayerInfo));
    if (!pLayerInfo) {
        LOGE("calloc error!");
        return NULL;
    }

    struct ivi_layout_layer_properties *prop = &layer->prop;
    pLayerInfo->layerId = pLayoutInterface->get_id_of_layer(layer);
    pLayerInfo->dstX = prop->dest_x;
    pLayerInfo->dstY = prop->dest_y;
    pLayerInfo->dstW = prop->dest_width;
    pLayerInfo->dstH = prop->dest_height;
    pLayerInfo->srcX = prop->source_x;
    pLayerInfo->srcY = prop->source_y;
    pLayerInfo->srcW = prop->source_width;
    pLayerInfo->srcH = prop->source_height;
    pLayerInfo->opacity = wl_fixed_to_double(prop->opacity);
    pLayerInfo->visibility = prop->visibility;

    int surfaceCnt;
    struct ivi_layout_surface **surfaceList = NULL;
    pLayoutInterface->get_surfaces_on_layer(layer, &surfaceCnt, &surfaceList);
    pLayerInfo->nSurfaces = surfaceCnt;

    if (surfaceCnt > 0) {
        pLayerInfo->surfaces = (struct SurfaceInfo **)calloc(surfaceCnt, sizeof(intptr_t));
        if (!pLayerInfo->surfaces) {
            LOGE("calloc error!");
            free(surfaceList);
            FreeLayerInfo(pLayerInfo);
            return NULL;
        }

        for (int j = 0; j < surfaceCnt; j++) {
            pLayerInfo->surfaces[j] = GetSurfaceInfo(surfaceList[j]);
            if (!pLayerInfo->surfaces[j]) {
                LOGE("GetSurfaceInfo error!");
                free(surfaceList);
                FreeLayerInfo(pLayerInfo);
                return NULL;
            }
            pLayerInfo->surfaces[j]->onLayerId = pLayerInfo->layerId;
        }
        free(surfaceList);
    }
    return pLayerInfo;
}

struct ScreenInfo *GetScreenInfo(const struct WmsScreen *pWmsScreen)
{
    const struct ivi_layout_interface_for_wms *pLayoutInterface = GetWmsInstance()->pLayoutInterface;
    struct ScreenInfo *pScreenInfo = (struct ScreenInfo *)calloc(1, sizeof(struct ScreenInfo));
    if (!pScreenInfo) {
        LOGE("calloc error!");
        return NULL;
    }

    pScreenInfo->screenId = pWmsScreen->screenId;
    pScreenInfo->connectorName = (char *)calloc(1, strlen(pWmsScreen->westonOutput->name) + 1);
    if (!pScreenInfo->connectorName) {
        LOGE("calloc error!");
        FreeScreenInfo(pScreenInfo);
        return NULL;
    }

    if (strcpy_s(pScreenInfo->connectorName,
        strlen(pWmsScreen->westonOutput->name) + 1, pWmsScreen->westonOutput->name)) {
        LOGE("strcpy_s error!");
        FreeScreenInfo(pScreenInfo);
        return NULL;
    }
    pScreenInfo->width = pWmsScreen->westonOutput->width;
    pScreenInfo->height = pWmsScreen->westonOutput->height;

    int layerCount;
    struct ivi_layout_layer **layerList = NULL;
    pLayoutInterface->get_layers_on_screen(pWmsScreen->westonOutput, &layerCount, &layerList);
    pScreenInfo->nLayers = layerCount;

    if (layerCount > 0) {
        pScreenInfo->layers = (struct LayerInfo **)calloc(layerCount, sizeof(intptr_t));
        if (!pScreenInfo->layers) {
            LOGE("calloc error!");
            free(layerList);
            FreeScreenInfo(pScreenInfo);
            return NULL;
        }
        for (int i = 0; i < layerCount; i++) {
            pScreenInfo->layers[i] = GetLayerInfo(layerList[i]);
            if (!pScreenInfo->layers[i]) {
                LOGE("calloc error!");
                free(layerList);
                FreeScreenInfo(pScreenInfo);
                return NULL;
            }
            pScreenInfo->layers[i]->onScreenId = pScreenInfo->screenId;
        }
        free(layerList);
    }
    return pScreenInfo;
}

struct ScreenInfo **GetScreensInfo(void)
{
    struct WmsContext *pWmsCtx = GetWmsInstance();
    uint32_t screenNum = wl_list_length(&pWmsCtx->wlListScreen);
    if (screenNum + 1 <= 0) {
        return NULL;
    }

    struct ScreenInfo **screens = (struct ScreenInfo **)calloc(screenNum + 1, sizeof(intptr_t));
    if (!screens) {
        LOGE("calloc error!");
        return NULL;
    }

    int index = 0;
    struct WmsScreen *pWmsScreen = NULL;
    wl_list_for_each(pWmsScreen, &pWmsCtx->wlListScreen, wlListLink) {
        screens[index] = GetScreenInfo(pWmsScreen);
        if (!screens[index]) {
            LOGE("GetScreenInfo error!");
            FreeScreensInfo(screens);
            return NULL;
        }
        index++;
    }
    return screens;
}

void LayerInfoDebugPrint(const struct LayerInfo *pLayer)
{
    LOGI("    layer %{public}d (0x%{public}x)", pLayer->layerId, pLayer->layerId);
    LOGI("    ---------------------------------------");
    LOGI("    - destination region:   x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d",
            pLayer->dstX, pLayer->dstY, pLayer->dstW, pLayer->dstH);
    LOGI("    - source region:        x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d",
            pLayer->srcX, pLayer->srcY, pLayer->srcW, pLayer->srcH);
    LOGI("    - opacity:              %{public}lf", pLayer->opacity);
    LOGI("    - visibility:           %{public}d", pLayer->visibility);

    char buffer[MAX_LENGTH] = {0};
    for (int k = 0; k < pLayer->nSurfaces; k++) {
        struct SurfaceInfo *pSurface = pLayer->surfaces[k];
        if (sprintf_s(buffer + strlen(buffer), MAX_LENGTH - strlen(buffer), "%d(0x%x)%s",
            pSurface->surfaceId, pSurface->surfaceId, (k != pLayer->nSurfaces - 1) ? "," : "") < 0) {
            LOGE("sprintf_s error.");
        }
    }
    LOGI("    - surface render order: %{public}s", buffer);
    LOGI("    - on screen:            %{public}d(0x%{public}x)", pLayer->onScreenId, pLayer->onScreenId);
    LOGI("");

    for (int k = 0; k < pLayer->nSurfaces; k++) {
        struct SurfaceInfo *pSurface = pLayer->surfaces[k];

        LOGI("        surface %{public}d (0x%{public}x)", pSurface->surfaceId, pSurface->surfaceId);
        LOGI("        ---------------------------------------");
        LOGI("        - destination region: x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d",
            pSurface->dstX, pSurface->dstY, pSurface->dstW, pSurface->dstH);
        LOGI("        - source region:      x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d",
            pSurface->srcX, pSurface->srcY, pSurface->srcW, pSurface->srcH);
        LOGI("        - opacity:            %{public}lf", pSurface->opacity);
        LOGI("        - visibility:         %{public}d", pSurface->visibility);
        LOGI("        - on layer:           %{public}d(0x%{public}x)",
            pSurface->onLayerId, pSurface->onLayerId);
        LOGI("");
    }
}

void ScreensInfoDebugPrint(struct ScreenInfo **screens)
{
    for (int i = 0; screens[i]; i++) {
        struct ScreenInfo *pScreen = screens[i];

        LOGI("screen %{public}d (0x%{public}x)", pScreen->screenId, pScreen->screenId);
        LOGI("---------------------------------------");
        LOGI("- connector name:       %{public}s", pScreen->connectorName);
        LOGI("- resolution:           w=%{public}d, h=%{public}d", pScreen->width, pScreen->height);
        char buffer[MAX_LENGTH] = {0};
        for (int j = 0; j < pScreen->nLayers; j++) {
            struct LayerInfo *pLayer = pScreen->layers[j];
            if (sprintf_s(buffer + strlen(buffer), MAX_LENGTH - strlen(buffer), "%d(0x%x)%s",
                pLayer->layerId, pLayer->layerId, (j != pScreen->nLayers - 1) ? "," : "") < 0) {
                LOGE("sprintf_s error.");
            }
        }
        LOGI("- layer render order:   %{public}s", buffer);
        LOGI("");

        for (int j = 0; j < pScreen->nLayers; j++) {
            LayerInfoDebugPrint(pScreen->layers[j]);
        }
    }
}

void SeatsInfoDebugPrint(struct SeatInfo **seats)
{
    for (int i = 0; seats[i]; i++) {
        LOGI("-----------------------------");
        LOGI("seatName: %{public}s", seats[i]->seatName);
        LOGI("deviceFlags: %{public}d", seats[i]->deviceFlags);
        LOGI("focusWindowId: %{public}d", seats[i]->focusWindowId);
    }
}

void GetScreenInformation(struct wl_client *client, struct wl_resource *resource, uint32_t type)
{
    if (type == SCREEN_INFO_TYPE_SCREEN) {
        LOGI("SCREEN_INFO_TYPE_SCREEN");
        struct ScreenInfo **screens = GetScreensInfo();
        if (screens) {
            ScreensInfoDebugPrint(screens);
            FreeScreensInfo(screens);
        }
        screen_info_send_reply(resource, 0);
    } else if (type == SCREEN_INFO_TYPE_SEAT) {
        LOGI("SCREEN_INFO_TYPE_SEAT");
        struct SeatInfo **seats = GetSeatsInfo();
        if (seats) {
            SeatsInfoDebugPrint(seats);
            FreeSeatsInfo(seats);
        }
        screen_info_send_reply(resource, 0);
    } else {
        LOGE("unknown type[%{public}d].", type);
        screen_info_send_reply(resource, 1);
    }
}

static void OnScreenInfoChange(void)
{
    LOGD("OnScreenInfoChange is called.");
}

static void OnSeatInfoChange(void)
{
    LOGD("OnSeatInfoChange is called.");
}

static void OnLibInputEvent(struct multimodal_libinput_event *event)
{
    LOGD("OnLibInputEvent is called.");
}

void SetListener(struct wl_client *client, struct wl_resource *resource, uint32_t type)
{
    if (type == SCREEN_INFO_LISTENER_SET_ENABLE) {
        LOGI("Set Listener.");
        SetScreenListener(OnScreenInfoChange);
        SetSeatListener(OnSeatInfoChange);
        SetLibInputEventListener(OnLibInputEvent);
        screen_info_send_reply(resource, 0);
    } else {
        LOGI("Unset Listener.");
        SetScreenListener(NULL);
        SetSeatListener(NULL);
        SetLibInputEventListener(NULL);
        screen_info_send_reply(resource, 0);
    }
}

static const struct screen_info_interface g_screenInfoInterface = {
    GetScreenInformation,
    SetListener
};

static void BindScreenInfo(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &screen_info_interface, version, id);
    if (resource == NULL) {
        LOGE("wl_resource_create failed");
        return;
    }
    wl_resource_set_implementation(resource, &g_screenInfoInterface, NULL, NULL);
}

int ScreenInfoInit(const struct weston_compositor *pCompositor)
{
    LOGD("start. ");
    if (!wl_global_create(pCompositor->wl_display, &screen_info_interface, 1, NULL, BindScreenInfo)) {
        LOGE("wl_global_create failed");
        return 1;
    }
    LOGD("end. ");
    return 0;
}
