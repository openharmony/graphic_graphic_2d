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
        CMLOGE("[NAPI]jsColorSpace is nullptr");
        return;
    }
}

NativeValue* JsColorSpace::GetColorSpaceName(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(engine, info);
    return (me != nullptr) ? me->OnGetColorSpaceName(*engine, *info) : nullptr;
}

NativeValue* JsColorSpace::GetWhitePoint(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(engine, info);
    return (me != nullptr) ? me->OnGetWhitePoint(*engine, *info) : nullptr;
}

NativeValue* JsColorSpace::GetGamma(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(engine, info);
    return (me != nullptr) ? me->OnGetGamma(*engine, *info) : nullptr;
}

NativeValue* JsColorSpace::OnGetColorSpaceName(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc > 0) {
        CMLOGE("[NAPI]Argc is invalid: %{public}zu", info.argc);
    }
    if (colorSpaceToken_ == nullptr) {
        return engine.CreateUndefined();
    }
    ColorSpaceName csName = colorSpaceToken_->GetColorSpaceName();
    NativeValue* objValue = engine.CreateUndefined();
    if (NATIVE_JS_TO_COLOR_SPACE_TYPE_MAP.count(csName) != 0) {
        CMLOGI("[NAPI]get color space name %{public}u, api type %{public}u",
            csName, NATIVE_JS_TO_COLOR_SPACE_TYPE_MAP.at(csName));
        objValue = CreateJsValue(engine, NATIVE_JS_TO_COLOR_SPACE_TYPE_MAP.at(csName));
    } else {
        CMLOGE("[NAPI]get color space name %{public}u, but not in api type", csName);
        objValue = CreateJsValue(engine, csName);
    }
    return objValue;
}

NativeValue* JsColorSpace::OnGetWhitePoint(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc > 0) {
        CMLOGE("[NAPI]Argc is invalid: %{public}zu", info.argc);
    }
    if (colorSpaceToken_ == nullptr) {
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
    if (info.argc > 0) {
        CMLOGE("[NAPI]Argc is invalid: %{public}zu", info.argc);
    }
    if (colorSpaceToken_ == nullptr) {
        return engine.CreateUndefined();
    }
    float gamma = colorSpaceToken_->GetGamma();
    NativeValue* objValue = CreateJsValue(engine, gamma);
    return objValue;
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
