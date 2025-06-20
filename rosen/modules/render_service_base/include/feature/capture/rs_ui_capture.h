/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_FEATURE_RS_UI_CAPTURE_H
#define RENDER_SERVICE_BASE_FEATURE_RS_UI_CAPTURE_H

#include "pixel_map.h"

namespace OHOS {
namespace Rosen {

struct Location {
    int32_t x;                 // the left of selfDrawingNode rect
    int32_t y;                 // the top of selfDrawingNode rect
    int32_t width;             // the width of selfDrawingNode rect
    int32_t height;            // the height of selfDrawingNode rect
    int32_t z;                 // the z-order of all selfDrawingNode in the process

    bool operator==(const Location& location) const
    {
        return x == location.x && y == location.y && width == location.width && height == location.height &&
               z == location.z;
    }
};

struct PixelMapInfo {
    std::shared_ptr<Media::PixelMap> pixelMap;
    Location location;
    std::string nodeName;

    bool operator==(const PixelMapInfo& pixelMapInfo) const
    {
        return pixelMap == pixelMapInfo.pixelMap && location == pixelMapInfo.location &&
               nodeName == pixelMapInfo.nodeName;
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_RS_UI_CAPTURE_H