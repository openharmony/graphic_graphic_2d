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

#include "record_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_canvas.h"
#include "drawing_record_cmd.h"
#include "drawing_types.h"

constexpr size_t DATA_MIN_SIZE = 2;
namespace OHOS {
namespace Rosen {
namespace Drawing {

void RecordFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    OH_Drawing_Canvas* canvas = GetObject<OH_Drawing_Canvas*>();
    OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
}

void RecordFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    OH_Drawing_RecordCmd* recordCmd = GetObject<OH_Drawing_RecordCmd*>();
    OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils, &recordCmd);
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::RecordFuzzTest001(data, size);
    OHOS::Rosen::Drawing::RecordFuzzTest002(data, size);
    return 0;
}
