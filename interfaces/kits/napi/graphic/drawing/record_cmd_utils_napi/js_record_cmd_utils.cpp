/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "js_record_cmd_utils.h"

#include "canvas_napi/js_canvas.h"
#include "draw/canvas.h"
#include "js_drawing_utils.h"
#include "js_drawing_type_tags.h"
#include "record_cmd_napi/js_record_cmd.h"
#include "recording/record_cmd.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {

thread_local napi_ref JsRecordCmdUtils::constructor_ = nullptr;
const std::string RECORD_CMD_UTILS_CLASS_NAME = "RecordCmdUtils";

static const napi_property_descriptor g_properties[] = {
    DECLARE_NAPI_FUNCTION("beginRecording", JsRecordCmdUtils::BeginRecording),
    DECLARE_NAPI_FUNCTION("finishRecording", JsRecordCmdUtils::FinishRecording),
    DECLARE_NAPI_FUNCTION("getHeight", JsRecordCmdUtils::GetHeight),
    DECLARE_NAPI_FUNCTION("getWidth", JsRecordCmdUtils::GetWidth),
};

JsRecordCmdUtils::~JsRecordCmdUtils()
{
    rsRecordCmdUtils_ = nullptr;
}

napi_value JsRecordCmdUtils::Init(napi_env env, napi_value exportObj)
{
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, RECORD_CMD_UTILS_CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor,
        nullptr, sizeof(g_properties) / sizeof(g_properties[0]), g_properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmdUtils::Init napi_define_class failed");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmdUtils::Init napi_create_reference failed");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, RECORD_CMD_UTILS_CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmdUtils::Init napi_set_named_property failed");
        return nullptr;
    }

    return exportObj;
}

napi_value JsRecordCmdUtils::Constructor(napi_env env, napi_callback_info info)
{
    napi_value jsThis = nullptr;
    size_t argCount = 0;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmdUtils::Constructor napi_get_cb_info failed");
        return nullptr;
    }

    if (argCount != 0) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsRecordCmdUtils::Constructor Invalid params.");
    }

    std::shared_ptr<RSRecordCmdUtils> rsRecordCmdUtils = std::make_shared<RSRecordCmdUtils>();
    JsRecordCmdUtils *jsRecordCmdUtils = new JsRecordCmdUtils(rsRecordCmdUtils);
    if (jsRecordCmdUtils == nullptr) {
        ROSEN_LOGE("JsRecordCmdUtils::Constructor new JsRecordCmdUtils failed");
        return nullptr;
    }

    status = napi_wrap_s(env, jsThis, jsRecordCmdUtils, JsRecordCmdUtils::Destructor, nullptr,
        &RECORD_CMD_UTILS_TYPE_TAG, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmdUtils::Constructor napi_wrap_s failed");
        delete jsRecordCmdUtils;
        return nullptr;
    }

    return jsThis;
}

void JsRecordCmdUtils::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject == nullptr) {
        return;
    }
    JsRecordCmdUtils* jsRecordCmdUtils = reinterpret_cast<JsRecordCmdUtils*>(nativeObject);
    delete jsRecordCmdUtils;
}

napi_value JsRecordCmdUtils::BeginRecording(napi_env env, napi_callback_info info)
{
    JsRecordCmdUtils* me = CheckParamsAndGetThisWithTag<JsRecordCmdUtils>(env, info, &RECORD_CMD_UTILS_TYPE_TAG);
    return (me != nullptr) ? me->OnBeginRecording(env, info) : nullptr;
}

napi_value JsRecordCmdUtils::OnBeginRecording(napi_env env, napi_callback_info info)
{
    if (rsRecordCmdUtils_ == nullptr) {
        ROSEN_LOGE("JsRecordCmdUtils::BeginRecording rsRecordCmdUtils is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    int32_t width = 0;
    GET_INT32_PARAM(ARGC_ZERO, width);

    int32_t height = 0;
    GET_INT32_PARAM(ARGC_ONE, height);

    if (width <= 0 || height <= 0) {
        ROSEN_LOGE("JsRecordCmdUtils::BeginRecording invalid width or height");
        return NapiThrowError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid params.");
    }

    auto bounds = Drawing::Rect(0, 0, width, height);
    Drawing::Canvas *canvas = rsRecordCmdUtils_->BeginRecording(bounds);
    if (canvas == nullptr) {
        ROSEN_LOGE("JsRecordCmdUtils::BeginRecording BeginRecording failed");
        return NapiThrowError(
            env, DrawingErrorCode::ERROR_INVALID_PARAM, "Insufficient memory, failed to create the canvas");
    }

    return JsCanvas::CreateJsCanvas(env, canvas);
}

napi_value JsRecordCmdUtils::FinishRecording(napi_env env, napi_callback_info info)
{
    JsRecordCmdUtils* me = CheckParamsAndGetThisWithTag<JsRecordCmdUtils>(env, info, &RECORD_CMD_UTILS_TYPE_TAG);
    return (me != nullptr) ? me->OnFinishRecording(env, info) : nullptr;
}

napi_value JsRecordCmdUtils::OnFinishRecording(napi_env env, napi_callback_info info)
{
    if (rsRecordCmdUtils_ == nullptr) {
        ROSEN_LOGE("JsRecordCmdUtils::FinishRecording rsRecordCmdUtils is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::shared_ptr<Drawing::RecordCmd> recordCmd = rsRecordCmdUtils_->FinishRecording();
    if (recordCmd == nullptr) {
        ROSEN_LOGE("JsRecordCmdUtils::FinishRecording FinishRecording failed");
        return nullptr;
    }

    return JsRecordCmd::CreateJsRecordCmd(env, recordCmd);
}

napi_value JsRecordCmdUtils::GetHeight(napi_env env, napi_callback_info info)
{
    JsRecordCmdUtils* me = CheckParamsAndGetThisWithTag<JsRecordCmdUtils>(env, info, &RECORD_CMD_UTILS_TYPE_TAG);
    return (me != nullptr) ? me->OnGetHeight(env, info) : nullptr;
}

napi_value JsRecordCmdUtils::OnGetHeight(napi_env env, napi_callback_info info)
{
    if (rsRecordCmdUtils_ == nullptr) {
        ROSEN_LOGE("JsRecordCmdUtils::GetHeight rsRecordCmdUtils is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    int32_t height = rsRecordCmdUtils_->GetHeight();
    return CreateJsNumber(env, height);
}

napi_value JsRecordCmdUtils::GetWidth(napi_env env, napi_callback_info info)
{
    JsRecordCmdUtils* me = CheckParamsAndGetThisWithTag<JsRecordCmdUtils>(env, info, &RECORD_CMD_UTILS_TYPE_TAG);
    return (me != nullptr) ? me->OnGetWidth(env, info) : nullptr;
}

napi_value JsRecordCmdUtils::OnGetWidth(napi_env env, napi_callback_info info)
{
    if (rsRecordCmdUtils_ == nullptr) {
        ROSEN_LOGE("JsRecordCmdUtils::GetWidth rsRecordCmdUtils is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    int32_t width = rsRecordCmdUtils_->GetWidth();
    return CreateJsNumber(env, width);
}

} // namespace Drawing
} // namespace OHOS::Rosen
