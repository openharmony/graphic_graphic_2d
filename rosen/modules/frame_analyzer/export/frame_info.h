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

#include "ring_queue.h"

namespace OHOS {
namespace Rosen {
enum class FrameEventType : int32_t {
    HandleInputStart = 0,
    HandleInputEnd = 1,
    UploadStart = 2,
    UploadEnd = 3,
    AnimateStart = 4,
    AnimateEnd = 5,
    LayoutStart = 6,
    LayoutEnd = 7,
    DrawStart = 8,
    DrawEnd = 9,
    WaitVsyncStart = 10,
    WaitVsyncEnd = 11,
    ReleaseStart = 12,
    ReleaseEnd = 13,
    FlushStart = 14,
    FlushEnd = 15,
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
