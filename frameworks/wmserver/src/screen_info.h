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

#ifndef FRAMEWORKS_WMSERVER_SRC_SCREEN_INFO_H
#define FRAMEWORKS_WMSERVER_SRC_SCREEN_INFO_H

struct SurfaceInfo {
    int surfaceId;
    int dstX;
    int dstY;
    int dstW;
    int dstH;
    int srcX;
    int srcY;
    int srcW;
    int srcH;
    double opacity;
    int visibility; // 0 or 1
    int onLayerId;
};

struct LayerInfo {
    int layerId;
    int dstX;
    int dstY;
    int dstW;
    int dstH;
    int srcX;
    int srcY;
    int srcW;
    int srcH;
    double opacity;
    int visibility; // 0 or 1
    int onScreenId;
    int nSurfaces;
    struct SurfaceInfo **surfaces;
};

struct ScreenInfo {
    int screenId;
    char *connectorName;
    int width;
    int height;
    int nLayers;
    struct LayerInfo **layers;
};

struct SeatInfo {
    char *seatName;
    int deviceFlags;
    int focusWindowId;
};

struct SeatInfo **GetSeatsInfo(void);
void FreeSeatsInfo(struct SeatInfo **seats);

struct ScreenInfo **GetScreensInfo(void);
void FreeScreensInfo(struct ScreenInfo **screens);

typedef void (*ScreenInfoChangeListener)();
void SetScreenListener(const ScreenInfoChangeListener listener);

typedef void (*SeatInfoChangeListener)();
void SetSeatListener(const SeatInfoChangeListener listener);

struct multimodal_libinput_event;
typedef void (*LibInputEventListener)(struct multimodal_libinput_event *event);
void SetLibInputEventListener(const LibInputEventListener listener);

#endif // FRAMEWORKS_WMSERVER_SRC_SCREEN_INFO_H
