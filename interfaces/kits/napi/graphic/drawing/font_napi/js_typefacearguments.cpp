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

#include "js_typefacearguments.h"

#include "native_value.h"

#include "js_drawing_utils.h"


namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsTypeFaceArguments::constructor_ = nullptr;
const std::string CLASS_NAME = "TypefaceArguments";
constexpr uint32_t VARIATION_AXIS_LENGTH = 4;
constexpr uint32_t AXIS_OFFSET_ZERO = 24;
constexpr uint32_t AXIS_OFFSET_ONE = 16;
constexpr uint32_t AXIS_OFFSET_TWO = 8;
napi_value JsTypeFaceArguments::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] =  {
        DECLARE_NAPI_FUNCTION("addVariation", JsTypeFaceArguments::AddVariation),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) /sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsTypeFaceArguments::Init failed to define Tool class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsTypeFaceArguments::Init failed to create a refenence of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsTypeFaceArguments::Init failed to set constructor");
        return nullptr;
    }
    return exportObj;
}

napi_value JsTypeFaceArguments::Constructor(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ZERO;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    if (argc != ARGC_ZERO) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    auto typeFaceArguments = std::make_shared<TypeFaceArgumentsHelper>();
    JsTypeFaceArguments* jsTypeFaceArguments = new JsTypeFaceArguments(typeFaceArguments);

    status = napi_wrap(env, jsThis, jsTypeFaceArguments, JsTypeFaceArguments::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTypeFaceArguments;
        ROSEN_LOGE("faile to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsTypeFaceArguments::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void) finalize;
    if (nativeObject != nullptr) {
        JsTypeFaceArguments *napi = reinterpret_cast<JsTypeFaceArguments*>(nativeObject);
        delete napi;
    }
}

JsTypeFaceArguments::~JsTypeFaceArguments()
{
    m_typeFaceArguments = nullptr;
}

std::shared_ptr<TypeFaceArgumentsHelper> JsTypeFaceArguments::GetTypeFaceArgumentsHelper()
{
    return m_typeFaceArguments;
}

void JsTypeFaceArguments::ConvertToFontArguments(std::shared_ptr<TypeFaceArgumentsHelper> typeFaceArgumentsHelper,
    FontArguments& fontArguments)
{
    fontArguments.SetCollectionIndex(typeFaceArgumentsHelper->fontCollectionIndex);
    fontArguments.SetVariationDesignPosition({reinterpret_cast<const FontArguments::VariationPosition::Coordinate*>(
        typeFaceArgumentsHelper->coordinate.data()), typeFaceArgumentsHelper->coordinate.size()});
}

uint32_t JsTypeFaceArguments::ConvertAxisToNumber(const std::string& axis)
{
    return (((uint32_t)axis[ARGC_ZERO] << AXIS_OFFSET_ZERO) | ((uint32_t)axis[ARGC_ONE] << AXIS_OFFSET_ONE) |
            ((uint32_t)axis[ARGC_TWO] << AXIS_OFFSET_TWO) | ((uint32_t)axis[ARGC_THREE]));
}

napi_value JsTypeFaceArguments::AddVariation(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    std::string axis = "";
    GET_JSVALUE_PARAM(ARGC_ZERO, axis);
    if (axis.length() != VARIATION_AXIS_LENGTH) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED,
            std::string("Incorrect parameter0 length, the length must be four"));
    }

    double value = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, value);

    JsTypeFaceArguments* jsTypeFaceArguments = CheckParamsAndGetThis<JsTypeFaceArguments>(env, info);
    if (jsTypeFaceArguments == nullptr) {
        ROSEN_LOGE("JsTypeFaceArguments::AddVariation this is nullptr");
        return nullptr;
    }
    auto typeFaceArgumentsHelper = jsTypeFaceArguments->GetTypeFaceArgumentsHelper();
    if (typeFaceArgumentsHelper == nullptr) {
        ROSEN_LOGE("JsTypeFaceArguments::AddVariation failed to GetTypeFaceArgumentsHelper");
        return nullptr;
    }
    typeFaceArgumentsHelper->coordinate.push_back({ConvertAxisToNumber(axis), (float)value});
    return nullptr;
}

} // namespace Drawing
} // namespace OHOS::Rosen