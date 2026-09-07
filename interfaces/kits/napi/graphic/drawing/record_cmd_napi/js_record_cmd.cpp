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

#include "js_record_cmd.h"

#include "js_drawing_utils.h"
#include "js_drawing_type_tags.h"
#include "recording/record_cmd.h"

namespace OHOS::Rosen {
namespace Drawing {

thread_local napi_ref JsRecordCmd::constructor_ = nullptr;
const std::string CLASS_NAME = "RecordCmd";

static const napi_property_descriptor g_properties[] = {};

napi_value JsRecordCmd::Init(napi_env env, napi_value exportObj)
{
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(g_properties) / sizeof(g_properties[0]), g_properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmd::Init napi_define_class failed");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmd::Init napi_create_reference failed");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmd::Init napi_set_named_property failed");
        return nullptr;
    }

    return exportObj;
}

napi_value JsRecordCmd::Constructor(napi_env env, napi_callback_info info)
{
    napi_value jsThis = nullptr;
    size_t argc = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmd::Constructor napi_get_cb_info failed");
        return nullptr;
    }

    if (argc != 0) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::shared_ptr<RecordCmd> recordCmd = std::make_shared<RecordCmd>(nullptr, Drawing::Rect());
    JsRecordCmd *jsRecordCmd = new JsRecordCmd(recordCmd);
    status = napi_wrap_s(env, jsThis, jsRecordCmd, JsRecordCmd::Destructor, nullptr, &RECORD_CMD_TYPE_TAG, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRecordCmd::Constructor napi_wrap_s failed");
        delete jsRecordCmd;
        return nullptr;
    }

    return jsThis;
}

void JsRecordCmd::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject == nullptr) {
        return;
    }
    JsRecordCmd* jsRecordCmd = reinterpret_cast<JsRecordCmd*>(nativeObject);
    delete jsRecordCmd;
}

JsRecordCmd::~JsRecordCmd()
{
    recordCmd_ = nullptr;
}

napi_value JsRecordCmd::CreateJsRecordCmd(napi_env env, const std::shared_ptr<RecordCmd> recordCmd)
{
    if (recordCmd == nullptr) {
        ROSEN_LOGE("JsRecordCmd::CreateJsRecordCmd recordCmd is nullptr");
        return nullptr;
    }

    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok || constructor == nullptr) {
        ROSEN_LOGE("JsRecordCmd::CreateJsRecordCmd napi_get_reference_value failed");
        return nullptr;
    }

    napi_value result = nullptr;
    napi_create_object(env, &result);
    if (result == nullptr) {
        ROSEN_LOGE("JsRecordCmd::CreateJsRecordCmd create object failed!");
        return nullptr;
    }

    JsRecordCmd* jsRecordCmd = new JsRecordCmd(recordCmd);
    status = napi_wrap_s(env, result, jsRecordCmd, JsRecordCmd::Destructor, nullptr, &RECORD_CMD_TYPE_TAG, nullptr);
    if (status != napi_ok) {
        delete jsRecordCmd;
        ROSEN_LOGE("JsRecordCmd::CreateJsRecordCmd failed to wrap native instance");
        return nullptr;
    }
    napi_define_properties(env, result, sizeof(g_properties) / sizeof(g_properties[0]), g_properties);
    return result;
}

} // namespace Drawing
} // namespace OHOS::Rosen
