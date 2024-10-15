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

#include "js_strut_style.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
thread_local napi_ref JsStrutStyleManager::constructor_ = nullptr;
const std::string CLASS_NAME = "StrutStyle";

void JsStrutStyleManager::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsStrutStyleManager *napi = reinterpret_cast<JsStrutStyleManager *>(nativeObject);
        delete napi;
    }
}

napi_value JsStrutStyleManager::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_STATIC_FUNCTION("isStrutStyleEqual", IsStrutStyleEqual),
    };

    napi_status status = napi_define_properties(env, exportObj, sizeof(descriptors) / sizeof(napi_property_descriptor),
        descriptors);
    if (status != napi_ok) {
        return nullptr;
    }
    return exportObj;
}

napi_value JsStrutStyleManager::IsStrutStyleEqual(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        TEXT_LOGE("JsStrutStyleManager::IsStrutStyleEqual Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    TypographyStyle styleFrom;
    TypographyStyle styleTo;
    SetStrutStyleFromJS(env, argv[0], styleFrom);
    SetStrutStyleFromJS(env, argv[1], styleTo);

    bool equal = (styleFrom == styleTo);
    return CreateJsValue(env, equal);
}

} // namespace OHOS::Rosen