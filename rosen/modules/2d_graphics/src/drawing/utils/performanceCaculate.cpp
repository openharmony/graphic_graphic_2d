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
#ifdef ROSEN_OHOS
#include <parameter.h>
#include <parameters.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#endif
#include "utils/performanceCaculate.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

#ifdef ROSEN_OHOS
const int SECOND_TO_NANO = 1000000000;
#endif
const int DRAWING_FLUSH_NUM = 4;

bool PerformanceCaculate::performanceCaculateSwitch_ = false;

#ifdef ROSEN_OHOS
PerformanceCaculate::TestFlag PerformanceCaculate::drawingTestFlag_ = static_cast<PerformanceCaculate::TestFlag> (
    std::atoi(OHOS::system::GetParameter("persist.sys.graphic.drawing.test", "0").c_str()));
#else
PerformanceCaculate::TestFlag PerformanceCaculate::drawingTestFlag_ = PerformanceCaculate::TestFlag::TSET_ALL;
#endif

int PerformanceCaculate::caculateTimeCount_ = 0;

bool PerformanceCaculate::GetDrawingTestJsEnabled()
{
    return performanceCaculateSwitch_ && drawingTestFlag_ == PerformanceCaculate::TestFlag::TEST_JS;
}

bool PerformanceCaculate::GetDrawingTestNapiEnabled()
{
    return performanceCaculateSwitch_ && drawingTestFlag_ == PerformanceCaculate::TestFlag::TEST_NAPI;
}

bool PerformanceCaculate::GetDrawingTestSkiaEnabled()
{
    return performanceCaculateSwitch_ && drawingTestFlag_ == PerformanceCaculate::TestFlag::TEST_SKIA;
}

bool PerformanceCaculate::GetDrawingTestRecordingEnabled()
{
    return (drawingTestFlag_ == PerformanceCaculate::TestFlag::TEST_RECORDING ||
        drawingTestFlag_ == PerformanceCaculate::TestFlag::TEST_SKIA);
}

long long PerformanceCaculate::GetUpTime(bool addCount)
{
    long long time = 0;
#ifdef ROSEN_OHOS
    struct timespec tv {};
    if (clock_gettime(CLOCK_BOOTTIME, &tv) >= 0) {
        time = tv.tv_sec * SECOND_TO_NANO + tv.tv_nsec;
    }
    if (addCount) {
        caculateTimeCount_++;
    }
#endif
    return time;
}

bool PerformanceCaculate::GetDrawingFlushPrint()
{
    return caculateTimeCount_ == DRAWING_FLUSH_NUM;
}

void PerformanceCaculate::ResetCaculateTimeCount()
{
    caculateTimeCount_ = 0;
}

void PerformanceCaculate::SetCaculateSwitch(bool start)
{
    performanceCaculateSwitch_ = start;
#ifdef ROSEN_OHOS
    if (performanceCaculateSwitch_) {
        drawingTestFlag_ = static_cast<PerformanceCaculate::TestFlag> (
            std::atoi(OHOS::system::GetParameter("persist.sys.graphic.drawing.test", "0").c_str()));
    }
#endif
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS