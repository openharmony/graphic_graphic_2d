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

#include "drawing_record_cmd.h"

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"
#include "pipeline/rs_record_cmd_utils.h"
#include "recording/record_cmd.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static RSRecordCmdUtils* CastToCmdUtils(OH_Drawing_RecordCmdUtils* cRecordCmdUtils)
{
    return reinterpret_cast<RSRecordCmdUtils*>(cRecordCmdUtils);
}

OH_Drawing_RecordCmdUtils* OH_Drawing_RecordCmdUtilsCreate()
{
    return (OH_Drawing_RecordCmdUtils*)new (std::nothrow) RSRecordCmdUtils;
}

OH_Drawing_ErrorCode OH_Drawing_RecordCmdUtilsDestroy(OH_Drawing_RecordCmdUtils* recordCmdUtils)
{
    if (recordCmdUtils == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    delete reinterpret_cast<RSRecordCmdUtils*>(recordCmdUtils);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RecordCmdUtilsBeginRecording(OH_Drawing_RecordCmdUtils* cRecordCmdUtils,
    int32_t width, int32_t height, OH_Drawing_Canvas** cCanvas)
{
    if (cRecordCmdUtils == nullptr || width <= 0 || height <= 0 || cCanvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    RSRecordCmdUtils* recordCmdUtils = CastToCmdUtils(cRecordCmdUtils);
    auto bounds = Drawing::Rect(0, 0, width, height);
    Drawing::Canvas* canvasPtr = recordCmdUtils->BeginRecording(bounds);
    if (canvasPtr == nullptr) {
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    *cCanvas = reinterpret_cast<OH_Drawing_Canvas*>(canvasPtr);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RecordCmdUtilsFinishingRecording(OH_Drawing_RecordCmdUtils* cRecordCmdUtils,
    OH_Drawing_RecordCmd** cRecordCmd)
{
    if (cRecordCmdUtils == nullptr || cRecordCmd == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    RSRecordCmdUtils* recordCmdUtils = CastToCmdUtils(cRecordCmdUtils);
    NativeHandle<RecordCmd>* recordCmdHandle = new(std::nothrow) NativeHandle<RecordCmd>;
    if (recordCmdHandle == nullptr) {
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    recordCmdHandle->value = recordCmdUtils->FinishRecording();
    if (recordCmdHandle->value == nullptr) {
        delete recordCmdHandle;
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    *cRecordCmd = Helper::CastTo<NativeHandle<RecordCmd>*, OH_Drawing_RecordCmd*>(recordCmdHandle);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RecordCmdDestroy(OH_Drawing_RecordCmd* recordCmd)
{
    if (recordCmd == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    delete reinterpret_cast<RecordCmd*>(recordCmd);
    return OH_DRAWING_SUCCESS;
}