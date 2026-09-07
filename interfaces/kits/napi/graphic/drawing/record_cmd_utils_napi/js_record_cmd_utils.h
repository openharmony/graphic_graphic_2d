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

#ifndef OHOS_ROSEN_JS_RECORD_CMD_UTILS_H
#define OHOS_ROSEN_JS_RECORD_CMD_UTILS_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "pipeline/rs_record_cmd_utils.h"

namespace OHOS::Rosen {
namespace Drawing {

class Canvas;

class JsRecordCmdUtils final {
public:
    explicit JsRecordCmdUtils(std::shared_ptr<RSRecordCmdUtils> rsRecordCmdUtils = nullptr)
        : rsRecordCmdUtils_(rsRecordCmdUtils) {};
    ~JsRecordCmdUtils();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);

    static napi_value BeginRecording(napi_env env, napi_callback_info info);
    static napi_value FinishRecording(napi_env env, napi_callback_info info);
    static napi_value GetHeight(napi_env env, napi_callback_info info);
    static napi_value GetWidth(napi_env env, napi_callback_info info);

    std::shared_ptr<RSRecordCmdUtils> GetRSRecordCmdUtils()
    {
        return rsRecordCmdUtils_;
    }

private:
    napi_value OnBeginRecording(napi_env env, napi_callback_info info);
    napi_value OnFinishRecording(napi_env env, napi_callback_info info);
    napi_value OnGetHeight(napi_env env, napi_callback_info info);
    napi_value OnGetWidth(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    std::shared_ptr<RSRecordCmdUtils> rsRecordCmdUtils_ = nullptr;
};

} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_RECORD_CMD_UTILS_H
