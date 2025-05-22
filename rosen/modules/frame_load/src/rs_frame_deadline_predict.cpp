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

#include "rs_frame_deadline_predict.h"

namespace OHOS::Rosen {
namespace {
constexpr int64_t FIXED_EXTRA_DRAWING_TIME = 3000000; // 3ms
constexpr int64_t SINGLE_SHIFT = 2700000; // 2.7ms
constexpr int64_t DOUBLE_SHIFT = 5400000; // 5.4ms
}

RsFrameDeadlinePredict& RsFrameDeadlinePredict::GetInstance()
{
    static RsFrameDeadlinePredict instance;
    return instance;
}

RsFrameDeadlinePredict::RsFrameDeadlinePredict() {}

void RsFrameDeadlinePredict::ReportRsFrameDeadline(OHOS::Rosen::HgmCore& hgmCore, bool forceRefreshFlag)
{
    int64_t extraReserve = 0;
    int64_t vsyncOffset = 0;
    uint32_t currentRate = hgmCore.GetFrameRateMgr()->GetCurrRefreshRate();
    int64_t idealPeriod = hgmCore.GetIdealPeriod(currentRate);
    int64_t drawingTime = idealPeriod;

    if (currentRate == OLED_120_HZ) {
        if (hgmCore.GetLtpoEnabled()) {
            vsyncOffset = CreateVSyncGenerator()->GetVSyncOffset();
            if (vsyncOffset > SINGLE_SHIFT && vsyncOffset <= DOUBLE_SHIFT) {
                extraReserve = SINGLE_SHIFT;
            } else if (vsyncOffset > DOUBLE_SHIFT && vsyncOffset < idealPeriod) {
                extraReserve = DOUBLE_SHIFT;
            }
        } else {
            extraReserve = FIXED_EXTRA_DRAWING_TIME;
        }
    }

    if (idealPeriod == preIdealPeriod_ && (extraReserve == preExtraReserve_ || currentRate != OLED_120_HZ)) {
        return;
    }
    drawingTime = (forceRefreshFlag) ? idealPeriod : idealPeriod + extraReserve;
    preIdealPeriod_ = idealPeriod;
    preExtraReserve_ = extraReserve;
    RS_TRACE_NAME_FMT("FrameDeadline: isForceRefresh: %d, currentRate: %u,"
        "vsyncOffset: %" PRId64 ", reservedDrawingTime: %" PRId64 "",
        forceRefreshFlag, currentRate, vsyncOffset, drawingTime);

    RsFrameReport::GetInstance().ReportFrameDeadline(drawingTime, currentRate);
}
} // namespace OHOS::Rosen
