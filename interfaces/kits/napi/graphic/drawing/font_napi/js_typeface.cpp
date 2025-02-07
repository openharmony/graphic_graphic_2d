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

#include "js_typeface.h"

#include "native_value.h"

#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsTypeface::constructor_ = nullptr;
const std::string CLASS_NAME = "Typeface";
const std::string G_SYSTEM_FONT_DIR = "/system/fonts";
napi_value JsTypeface::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getFamilyName", JsTypeface::GetFamilyName),
        DECLARE_NAPI_STATIC_FUNCTION("makeFromFile", JsTypeface::MakeFromFile),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Typeface class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsTypeface::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    JsTypeface *jsTypeface = new JsTypeface(JsTypeface::GetZhCnTypeface());

    status = napi_wrap(env, jsThis, jsTypeface, JsTypeface::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTypeface;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsTypeface::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsTypeface *napi = reinterpret_cast<JsTypeface *>(nativeObject);
        delete napi;
    }
}

JsTypeface::~JsTypeface()
{
    m_typeface = nullptr;
}

napi_value JsTypeface::CreateJsTypeface(napi_env env, const std::shared_ptr<Typeface> typeface)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        auto jsTypeface = new JsTypeface(typeface);
        napi_create_object(env, &result);
        if (result == nullptr) {
            delete jsTypeface;
            ROSEN_LOGE("JsTypeface::MakeFromFile Create Typeface failed!");
            return nullptr;
        }
        status = napi_wrap(env, result, jsTypeface, JsTypeface::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            delete jsTypeface;
            ROSEN_LOGE("JsTypeface::MakeFromFile failed to wrap native instance");
            return nullptr;
        }
        napi_property_descriptor resultFuncs[] = {
            DECLARE_NAPI_FUNCTION("getFamilyName", JsTypeface::GetFamilyName),
        };
        napi_define_properties(env, result, sizeof(resultFuncs) / sizeof(resultFuncs[0]), resultFuncs);
        return result;
    }
    return result;
}

std::shared_ptr<Typeface> LoadZhCnTypeface()
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(JsTypeface::ZH_CN_TTF);
    if (typeface == nullptr) {
        typeface = Typeface::MakeDefault();
    }
    return typeface;
}

std::shared_ptr<Typeface> JsTypeface::zhCnTypeface_ = LoadZhCnTypeface();

std::shared_ptr<Typeface> JsTypeface::GetZhCnTypeface()
{
    return zhCnTypeface_;
}

std::shared_ptr<Typeface> JsTypeface::GetTypeface()
{
    return m_typeface;
}

napi_value JsTypeface::GetFamilyName(napi_env env, napi_callback_info info)
{
    JsTypeface* me = CheckParamsAndGetThis<JsTypeface>(env, info);
    return (me != nullptr) ? me->OnGetFamilyName(env, info) : nullptr;
}

napi_value JsTypeface::OnGetFamilyName(napi_env env, napi_callback_info info)
{
    if (m_typeface == nullptr) {
        ROSEN_LOGE("[NAPI]typeface is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    auto name = m_typeface->GetFamilyName();
    return GetStringAndConvertToJsValue(env, name);
}

napi_value JsTypeface::MakeFromFile(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_ONE);

    std::string text = "";
    GET_JSVALUE_PARAM(ARGC_ZERO, text);

    auto rawTypeface = Typeface::MakeFromFile(text.c_str());
    if (rawTypeface == nullptr) {
        ROSEN_LOGE("JsTypeface::MakeFromFile create rawTypeface failed!");
        return nullptr;
    }
    auto typeface = new JsTypeface(rawTypeface);
    std::string pathStr(text);
    if (pathStr.substr(0, G_SYSTEM_FONT_DIR.length()) != G_SYSTEM_FONT_DIR &&
        Drawing::Typeface::GetTypefaceRegisterCallBack() != nullptr) {
        bool ret = Drawing::Typeface::GetTypefaceRegisterCallBack()(rawTypeface);
        if (!ret) {
            delete typeface;
            ROSEN_LOGE("JsTypeface::MakeFromFile MakeRegister Typeface failed!");
            return nullptr;
        }
    }

    napi_value jsObj = nullptr;
    napi_create_object(env, &jsObj);
    if (jsObj == nullptr) {
        delete typeface;
        ROSEN_LOGE("JsTypeface::MakeFromFile Create Typeface failed!");
        return nullptr;
    }
    napi_status status = napi_wrap(env, jsObj, typeface, JsTypeface::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete typeface;
        ROSEN_LOGE("JsTypeface::MakeFromFile failed to wrap native instance");
        return nullptr;
    }
    napi_property_descriptor resultFuncs[] = {
        DECLARE_NAPI_FUNCTION("getFamilyName", JsTypeface::GetFamilyName),
    };
    napi_define_properties(env, jsObj, sizeof(resultFuncs) / sizeof(resultFuncs[0]), resultFuncs);
    return jsObj;
}

} // namespace Drawing
} // namespace OHOS::Rosen