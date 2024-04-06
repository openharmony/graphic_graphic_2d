/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SCREEN_INFO_H
#define RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SCREEN_INFO_H

#pragma once

#include <string>
#include <surface_type.h>
#include <unordered_set>

#include "screen_types.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
enum class ScreenState : uint8_t {
    HDI_OUTPUT_ENABLE,
    PRODUCER_SURFACE_ENABLE,
    DISABLED,
    NOT_EXISTED,
    UNKNOWN,
};

struct ScreenInfo {
    ScreenId id = INVALID_SCREEN_ID;
    uint32_t width = 0; // render resolution
    uint32_t height = 0;
    uint32_t phyWidth = 0; // physical screen resolution
    uint32_t phyHeight = 0;
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ScreenState state = ScreenState::UNKNOWN;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    std::unordered_set<uint64_t> filteredAppSet = {};

    uint32_t skipFrameInterval = DEFAULT_SKIP_FRAME_INTERVAL; // skip frame interval for change screen refresh rate

    GraphicPixelFormat pixelFormat = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    ScreenHDRFormat hdrFormat = ScreenHDRFormat::NOT_SUPPORT_HDR;

    uint32_t GetRotatedWidth() const
    {
        return (rotation == ScreenRotation::ROTATION_0 || rotation == ScreenRotation::ROTATION_180) ? width : height;
    }

    uint32_t GetRotatedHeight() const
    {
        return (rotation == ScreenRotation::ROTATION_0 || rotation == ScreenRotation::ROTATION_180) ? height : width;
    }
    uint32_t GetRotatedPhyWidth() const
    {
        return (rotation == ScreenRotation::ROTATION_0 || rotation == ScreenRotation::ROTATION_180) ? phyWidth
                                                                                                    : phyHeight;
    }

    uint32_t GetRotatedPhyHeight() const
    {
        return (rotation == ScreenRotation::ROTATION_0 || rotation == ScreenRotation::ROTATION_180) ? phyHeight
                                                                                                    : phyWidth;
    }

    float GetRogWidthRatio() const
    {
        return (width == 0) ? 1.f : static_cast<float>(phyWidth) / width;
    }

    float GetRogHeightRatio() const
    {
        return (height == 0) ? 1.f : static_cast<float>(phyHeight) / height;
    }

    // dfx
    std::string ToString() const
    {
        std::string ret = "ScreenInfo:{\n";
        ret += "  id:" + std::to_string(id);
        ret += "  width:" + std::to_string(width);
        ret += "  height:" + std::to_string(height);
        ret += "\n}\n";
        return ret;
    }
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SCREEN_INFO_H