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

#include "performance_caculate_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/performanceCaculate.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void PerformanceCaculateFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool addCount = GetObject<bool>();
    PerformanceCaculate::GetUpTime(addCount);
    PerformanceCaculate::SetCaculateSwitch(addCount);
    PerformanceCaculate::GetDrawingTestJsEnabled();
    PerformanceCaculate::GetDrawingTestNapiEnabled();
    PerformanceCaculate::GetDrawingTestSkiaEnabled();
    PerformanceCaculate::GetDrawingTestRecordingEnabled();
    PerformanceCaculate::GetDrawingFlushPrint();
    PerformanceCaculate::ResetCaculateTimeCount();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PerformanceCaculateFuzzTest000(data, size);
    return 0;
}