/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "color_space_object_convertor.h"

#include "native_engine/native_reference.h"

#include "js_color_space.h"
#include "js_color_space_utils.h"

#define SENDABLE_PROPERTIES_NUM 3

namespace OHOS {
namespace ColorManager {
napi_value CreateJsColorSpaceObject(napi_env env, std::shared_ptr<ColorSpace>& colorSpace)
{
    if (colorSpace == nullptr) {
        CMLOGE("[NAPI]colorSpace is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "BusinessError 401: Parameter error, the value of colorSpace must not be nullptr"));
        return nullptr;
    }
    napi_value object = nullptr;
    napi_create_object(env, &object);
    if (object == nullptr) {
        CMLOGE("[NAPI]Fail to convert to js object");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "BusinessError 401: Parameter error, the value of env error. Can not create object."));
        napi_get_undefined(env, &object);
        return object;
    }

    std::unique_ptr<JsColorSpace> jsColorSpace = std::make_unique<JsColorSpace>(colorSpace);
    napi_wrap(env, object, jsColorSpace.release(), JsColorSpace::Finalizer, nullptr, nullptr);
    BindFunctions(env, object);

    std::shared_ptr<NativeReference> jsColorSpaceNativeRef;
    napi_ref jsColorSpaceRef = nullptr;
    napi_create_reference(env, object, 1, &jsColorSpaceRef);
    jsColorSpaceNativeRef.reset(reinterpret_cast<NativeReference*>(jsColorSpaceRef));
    return object;
}


napi_value CreateJsSendableColorSpaceObject(napi_env env, std::shared_ptr<ColorSpace>& colorSpace)
{
    if (colorSpace == nullptr) {
        CMLOGE("[NAPI]colorSpace is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "BusinessError 401: Parameter error, the value of colorSpace must not be nullptr"));
        return nullptr;
    }
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getColorSpaceName", JsColorSpace::GetSendableColorSpaceName),
        DECLARE_NAPI_FUNCTION("getWhitePoint", JsColorSpace::GetSendableWhitePoint),
        DECLARE_NAPI_FUNCTION("getGamma", JsColorSpace::GetSendableGamma),
    };
    napi_value object = nullptr;
    napi_create_sendable_object_with_properties(env, SENDABLE_PROPERTIES_NUM, properties, &object);
    if (object == nullptr) {
        CMLOGE("[NAPI]Fail to convert to js object");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "BusinessError 401: Parameter error, the value of env error. Can not create object."));
        napi_get_undefined(env, &object);
        return object;
    }

    std::unique_ptr<JsColorSpace> jsColorSpace = std::make_unique<JsColorSpace>(colorSpace);
    napi_wrap_sendable(env, object, jsColorSpace.release(), JsColorSpace::Finalizer, nullptr, nullptr);

    return object;
}

std::shared_ptr<ColorSpace> GetColorSpaceByJSObject(napi_env env, napi_value object)
{
    if (object == nullptr) {
        CMLOGE("[NAPI]GetColorSpaceByJSObject::jsObject is nullptr");
        return nullptr;
    }
    JsColorSpace* jsColorSpace = nullptr;
    napi_unwrap(env, object, (void **)&jsColorSpace);
    if (jsColorSpace == nullptr) {
        CMLOGE("[NAPI]GetColorSpaceByJSObject::jsColorSpace is nullptr");
        return nullptr;
    }
    return jsColorSpace->GetColorSpaceToken();
}
}  // namespace ColorManager
}  // namespace OHOS
