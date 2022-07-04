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
constexpr size_t FrameEventTypeInterval = 2;
constexpr int32_t Frame60Ms = 1000 / 60;
constexpr int32_t Frame30Ms = 1000 / 30;
constexpr auto loopstart = static_cast<size_t>(FrameEventType::LoopStart);
constexpr auto loopend = static_cast<size_t>(FrameEventType::LoopEnd);
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
        uint8_t alpha = SumHeight(*rit) >= 16.0 ? 0x7f : 0x3f;
        auto y = height;
        for (size_t i = loopstart; i < loopend; i += FrameEventTypeInterval) {
            const auto &cm = frameEventColorMap;
            if (auto it = cm.find(static_cast<FrameEventType>(i)); it != cm.end()) {
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

    auto frameOffset = 0.5 * heightPerMs;
    auto frameLength = 1 * heightPerMs;
    auto rect60 = SkRect::MakeXYWH(0, (frameTotalMs - Frame60Ms) * heightPerMs + frameOffset,
                                   width, frameLength);
    auto rect30 = SkRect::MakeXYWH(0, (frameTotalMs - Frame30Ms) * heightPerMs + frameOffset,
                                   width, frameLength);
    paint.setColor(0xbf00ff00);
    canvas.drawRect(rect60, paint);
    paint.setColor(0xbfff0000);
    canvas.drawRect(rect30, paint);
}

double FramePainter::SumHeight(const struct FrameInfo &info)
{
    constexpr auto loopstart = static_cast<size_t>(FrameEventType::LoopStart);
    constexpr auto loopend = static_cast<size_t>(FrameEventType::LoopEnd);

    auto sum = 0.0;
    for (size_t i = loopstart; i < loopend; i += FrameEventTypeInterval) {
        const auto &cm = frameEventColorMap;
        if (cm.find(static_cast<FrameEventType>(i)) == cm.end()) {
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
