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

#include "frame_painter.h"

#include <map>

#include <hilog/log.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>

#include "frame_collector.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "FramePainter" };
std::map<FrameEventType, SkColor> colorMap = {
    {FrameEventType::UploadStart,    0x0000ffff}, // cyan
    {FrameEventType::AnimateStart,   0x0000cc00}, // mid green
    {FrameEventType::LayoutStart,    0x0000ff00}, // green
    {FrameEventType::DrawStart,      0x000000ff}, // blue
    {FrameEventType::WaitVsyncStart, 0x00006600}, // old green
    {FrameEventType::ReleaseStart,   0x00ffff00}, // yellow
    {FrameEventType::FlushStart,     0x00ff0000}, // red
};
} // namespace

FramePainter::FramePainter(FrameCollector &collector) : collector_(collector)
{
}

void FramePainter::Draw(SkCanvas &canvas)
{
    if (collector_.IsEnabled() == false) {
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(paint.kFill_Style);
    const auto &width = canvas.imageInfo().width();
    const auto &height = canvas.imageInfo().height();
    ::OHOS::HiviewDFX::HiLog::Debug(LABEL, "FramePainter::Draw %{public}dx%{public}d", width, height);

    auto heightPerMs = height / frameTotalMs;
    auto barWidth = width / (frameQueueMaxSize + 2.0);
    auto x = barWidth * (frameQueueMaxSize - 1);
    auto fq = collector_.LockGetFrameQueue();
    for (auto rit = fq.rbegin(); rit != fq.rend(); rit++) {
        constexpr auto loopstart = static_cast<size_t>(FrameEventType::LoopStart);
        constexpr auto loopend = static_cast<size_t>(FrameEventType::LoopEnd);

        uint8_t alpha = SumHeight(*rit) >= 16.0 ? 0x7f : 0x3f;
        auto y = height;
        for (size_t i = loopstart; i < loopend; i += 2) {
            if (auto it = colorMap.find(static_cast<FrameEventType>(i)); it != colorMap.end()) {
                constexpr auto alphaOffset = 24;
                paint.setColor(it->second | (alpha << alphaOffset));
            } else {
                continue;
            }

            auto diff = (rit->times[i + 1] - rit->times[i]) / 1e6;
            if (diff < 0) {
                ::OHOS::HiviewDFX::HiLog::Warn(LABEL, "FramePainter::Draw %{public}zu range is negative", i);
                continue;
            } else if (diff == 0) {
                continue;
            }

            auto diffHeight = diff * heightPerMs;
            y -= diffHeight;
            canvas.drawRect(SkRect::MakeXYWH(x, y, barWidth, diffHeight), paint);
        }
        x -= barWidth;
    }
    collector_.UnlockFrameQueue();

    paint.setColor(0xbf00ff00);
    canvas.drawRect(SkRect::MakeXYWH(0, (frameTotalMs - 16 + 0.5) * heightPerMs, width, heightPerMs), paint);
    paint.setColor(0xbfff0000);
    canvas.drawRect(SkRect::MakeXYWH(0, (frameTotalMs - 32 + 0.5) * heightPerMs, width, heightPerMs), paint);
}

double FramePainter::SumHeight(const struct FrameInfo &info)
{
    constexpr auto loopstart = static_cast<size_t>(FrameEventType::LoopStart);
    constexpr auto loopend = static_cast<size_t>(FrameEventType::LoopEnd);

    auto sum = 0.0;
    for (size_t i = loopstart; i < loopend; i += 2) {
        if (colorMap.find(static_cast<FrameEventType>(i)) == colorMap.end()) {
            continue;
        }

        auto diff = (info.times[i + 1] - info.times[i]) / 1e6;
        if (diff <= 0) {
            continue;
        }

        sum += diff;
    }

    return sum;
}
} // namespace Rosen
} // namespace OHOS
