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

#define USE_IVI_INPUT_FOCUS
#ifdef USE_IVI_INPUT_FOCUS
#include <ivi-input-export.h>
#endif
#include <ivi-layout-export.h>
#include <wms-server-protocol.h>

#ifndef USE_IVI_INPUT_FOCUS
#define INPUT_DEVICE_KEYBOARD   ((unsigned int) 1 << 0)
#define INPUT_DEVICE_POINTER    ((unsigned int) 1 << 1)
#define INPUT_DEVICE_TOUCH      ((unsigned int) 1 << 2)
#define INPUT_DEVICE_ALL        ((unsigned int) ~0)
#endif

struct WmsContext {
    struct weston_compositor *pCompositor;
    const struct ivi_layout_interface_for_wms *pLayoutInterface;
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
#ifdef USE_IVI_INPUT_FOCUS
    const struct ivi_input_interface_for_wms *pInputInterface;
#endif
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
    struct weston_output *westonOutput;
};

struct WmsContext *GetWmsInstance(void);

#endif // FRAMEWORKS_WMSERVER_SRC_WMSERVER_H
