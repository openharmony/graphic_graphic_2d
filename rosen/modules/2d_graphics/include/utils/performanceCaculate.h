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

#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#ifdef DRAWING_PERFORMANCE_TEST
#define DRAWING_PERFORMANCE_TEST_JS_RETURN(data)                            \
    if (Drawing::PerformanceCaculate::GetDrawingTestJsEnabled()) {          \
        return (data);                                                      \
    }
#define DRAWING_PERFORMANCE_TEST_NAP_RETURN(data)                           \
    if (Drawing::PerformanceCaculate::GetDrawingTestNapiEnabled()) {        \
        return (data);                                                      \
    }
#define DRAWING_PERFORMANCE_TEST_SKIA_RETURN(data)                          \
    if (Drawing::PerformanceCaculate::GetDrawingTestSkiaEnabled()) {        \
        return (data);                                                      \
    }
#define DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN                       \
    if (Drawing::PerformanceCaculate::GetDrawingTestSkiaEnabled()) {        \
        return;                                                             \
    }
#define DRAWING_PERFORMANCE_START_CACULATE Drawing::PerformanceCaculate::SetCaculateSwitch(true)
#define DRAWING_PERFORMANCE_STOP_CACULATE Drawing::PerformanceCaculate::SetCaculateSwitch(false)
#else
#define DRAWING_PERFORMANCE_TEST_JS_RETURN(data)
#define DRAWING_PERFORMANCE_TEST_NAP_RETURN(data)
#define DRAWING_PERFORMANCE_TEST_SKIA_RETURN(data)
#define DRAWING_PERFORMANCE_TEST_SKIA_NO_PARAM_RETURN
#define DRAWING_PERFORMANCE_START_CACULATE
#define DRAWING_PERFORMANCE_STOP_CACULATE
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PerformanceCaculate {
public:
    enum TestFlag {
        TSET_ALL,
        TEST_JS,
        TEST_NAPI,
        TEST_SKIA,
        TEST_RECORDING,
    };

    static bool GetDrawingTestJsEnabled();
    static bool GetDrawingTestNapiEnabled();
    static bool GetDrawingTestSkiaEnabled();
    static bool GetDrawingTestRecordingEnabled();

    static long long GetUpTime(bool addCount = true);

    static void SetCaculateSwitch(bool start = true);

    static bool GetDrawingFlushPrint();
    static void ResetCaculateTimeCount();

private:
    
    static bool performanceCaculateSwitch_;
    static TestFlag drawingTestFlag_;
    static int caculateTimeCount_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif