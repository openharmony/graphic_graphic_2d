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

#ifndef ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_INFO_H
#define ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_INFO_H

#include <array>
#include <cstdint>
#include <map>

#include "ring_queue.h"

namespace OHOS {
namespace Rosen {
/*
 * Event ids must be continuous, and special
 * enumerations can be defined after Max.
 * The ID of the start event must be odd,
 * and the ID of the end event must be even.
 */
enum class FrameEventType : int32_t {
    // UI Event
    HandleInputStart = 0,
    HandleInputEnd,
    AnimateStart,
    AnimateEnd,
    BuildStart,
    BuildEnd,
    UploadStart,
    UploadEnd,
    LayoutStart,
    LayoutEnd,
    DrawStart,
    DrawEnd,

    // RS Event
    WaitVsyncStart,
    WaitVsyncEnd,
    ReleaseStart,
    ReleaseEnd,
    FlushStart,
    FlushEnd,

    // static define
    Max,

    // ui marks range
    UIMarksStart = HandleInputStart,
    UIMarksEnd = DrawEnd + 1,
    UIMarksLen = UIMarksEnd - UIMarksStart,

    // total range
    LoopStart = HandleInputStart,
    LoopEnd = Max,
    LoopLen = LoopEnd - LoopStart,
};

/*
 * Alpha will be determinated by runtime code.
 * If event doesn't exist, it won't draw.
 */
static const std::map<FrameEventType, uint32_t> frameEventColorMap = {
    // FrameEventType::HandleInputStart
    {FrameEventType::AnimateStart,     0x0000cc00}, // mid green
    {FrameEventType::BuildStart,       0x0000ffff}, // cyan
    // FrameEventType::UploadStart
    {FrameEventType::LayoutStart,      0x0000ff00}, // green
    {FrameEventType::DrawStart,        0x000000ff}, // blue
    {FrameEventType::WaitVsyncStart,   0x00006600}, // old green
    {FrameEventType::ReleaseStart,     0x00ffff00}, // yellow
    {FrameEventType::FlushStart,       0x00ff0000}, // red
};

static const std::map<FrameEventType, std::string> frameEventTypeStringMap = {
    {FrameEventType::HandleInputStart, "HandleInput"},
    {FrameEventType::AnimateStart,     "Animate"},
    {FrameEventType::BuildStart,       "Build"},
    {FrameEventType::UploadStart,      "Upload"},
    {FrameEventType::LayoutStart,      "Layout"},
    {FrameEventType::DrawStart,        "Draw"},
    {FrameEventType::WaitVsyncStart,   "WaitVsync"},
    {FrameEventType::ReleaseStart,     "Release"},
    {FrameEventType::FlushStart,       "Flush"},
};

static inline bool IsStartFrameEventType(int index)
{
    // even is start event, 0x2 for test even
    return (index % 0x2) == 0;
}

static inline std::string GetAsyncNameByFrameEventType(int index)
{
    // 0x2 for get event id
    return std::string("Frame.") + std::to_string(index / 0x2) + "." +
        frameEventTypeStringMap.at(static_cast<FrameEventType>(index &~ 1));
}

static inline std::string GetNameByFrameEventType(FrameEventType type)
{
    if (IsStartFrameEventType(static_cast<int>(type))) {
        return frameEventTypeStringMap.at(type) + "Start";
    } else {
        // end type - 1 => start type
        auto index = static_cast<FrameEventType>(static_cast<int>(type) - 1);
        return frameEventTypeStringMap.at(index) + "End";
    }
}

struct UIMarks {
    int32_t frameNumber = 0;
    std::array<int64_t, static_cast<size_t>(FrameEventType::UIMarksLen)> times = {};
};

struct FrameInfo {
    int32_t frameNumber = 0;
    bool skiped = false;
    std::array<int64_t, static_cast<size_t>(FrameEventType::LoopLen)> times = {};
};

static constexpr int32_t frameQueueMaxSize = 60;
static constexpr double frameTotalMs = 160;
static constexpr const char *switchRenderingText = "debug.graphic.frame";
static constexpr const char *switchRenderingPaintText = "paint";
static constexpr const char *switchRenderingSaverText = "saver";
static constexpr const char *switchRenderingDisableText = "disable";
static constexpr const char *saveDirectory = "/data/frame_render";

using FrameInfoQueue = RingQueue<struct FrameInfo, frameQueueMaxSize>;
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_INFO_H
