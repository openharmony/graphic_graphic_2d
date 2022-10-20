/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "js_color_space.h"

#include <array>

#include "color_space.h"
#include "js_color_space_utils.h"

namespace OHOS {
namespace ColorManager {
void JsColorSpace::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    auto jsColorSpace = std::unique_ptr<JsColorSpace>(static_cast<JsColorSpace*>(data));
    if (jsColorSpace == nullptr) {
        CMLOGE("[NAPI]Finalizer jsColorSpace is nullptr");
        return;
    }
    auto csToken = jsColorSpace->colorSpaceToken_;
    if (csToken == nullptr) {
        CMLOGE("[NAPI]Finalizer colorSpaceToken_ is nullptr");
        return;
    }
}

NativeValue* JsColorSpace::GetColorSpaceName(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(engine, info);
    if (me == nullptr) {
        engine->Throw(CreateJsError(*engine,
            static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetColorSpaceName(*engine, *info);
}

NativeValue* JsColorSpace::GetWhitePoint(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(engine, info);
    if (me == nullptr) {
        engine->Throw(CreateJsError(*engine,
            static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetWhitePoint(*engine, *info);
}

NativeValue* JsColorSpace::GetGamma(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(engine, info);
    if (me == nullptr) {
        engine->Throw(CreateJsError(*engine,
            static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetGamma(*engine, *info);
}

NativeValue* JsColorSpace::OnGetColorSpaceName(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        return engine.CreateUndefined();
    }
    ColorSpaceName csName = colorSpaceToken_->GetColorSpaceName();
    if (NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.count(csName) != 0) {
        CMLOGI("[NAPI]get color space name %{public}u, api type %{public}u",
            csName, NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.at(csName));
        return CreateJsValue(engine, NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.at(csName));
    }
    CMLOGE("[NAPI]get color space name %{public}u, but not in api type", csName);
    engine.Throw(CreateJsError(engine,
        static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
        "Parameter check fails. Color space type " + std::to_string(static_cast<int32_t>(csName)) +
        "does not in supported type list."));
    return engine.CreateUndefined();
}

NativeValue* JsColorSpace::OnGetWhitePoint(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        return engine.CreateUndefined();
    }
    std::array<float, DIMES_2> wp = colorSpaceToken_->GetWhitePoint();
    NativeValue* arrayValue = engine.CreateArray(DIMES_2);
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    for (uint32_t i = 0; i < DIMES_2; i++) {
        array->SetElement(i, CreateJsValue(engine, wp[i]));
    }
    return arrayValue;
}

NativeValue* JsColorSpace::OnGetGamma(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        return engine.CreateUndefined();
    }
    float gamma = colorSpaceToken_->GetGamma();
    return CreateJsValue(engine, gamma);
}

void BindFunctions(NativeEngine& engine, NativeObject* object)
{
    const char *moduleName = "JsColorSpace";
    BindNativeFunction(engine, *object, "getColorSpaceName", moduleName, JsColorSpace::GetColorSpaceName);
    BindNativeFunction(engine, *object, "getWhitePoint", moduleName, JsColorSpace::GetWhitePoint);
    BindNativeFunction(engine, *object, "getGamma", moduleName, JsColorSpace::GetGamma);
}
} // namespace ColorManager
} // namespace OHOS
