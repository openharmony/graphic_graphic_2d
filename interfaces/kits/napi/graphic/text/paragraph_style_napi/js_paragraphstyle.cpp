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
#include "js_drawing_utils.h"
#include "js_paragraphstyle.h"

namespace OHOS::Rosen {
thread_local napi_ref JsParagraphStyle::constructor_ = nullptr;
const std::string CLASS_NAME = "JsParagraphStyle";
napi_value JsParagraphStyle::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to define ParagraphStyle class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to set constructor");
        return nullptr;
    }

    napi_property_descriptor staticProperty[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createJsParagraphStyle", JsParagraphStyle::CreateJsParagraphStyle),
    };
    status = napi_define_properties(env, exportObj, 1, staticProperty);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to define static function");
        return nullptr;
    }
    return exportObj;
}

napi_value JsParagraphStyle::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    std::shared_ptr<TypographyStyle> paragraphStyle = std::make_shared<TypographyStyle>();
    JsParagraphStyle *jsParagraphStyle = new(std::nothrow) JsParagraphStyle(paragraphStyle);

    status = napi_wrap(env, jsThis, jsParagraphStyle,
        JsParagraphStyle::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsParagraphStyle;
        ROSEN_LOGE("failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsParagraphStyle::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsParagraphStyle *napi = reinterpret_cast<JsParagraphStyle *>(nativeObject);
        delete napi;
    }
}

napi_value JsParagraphStyle::CreateJsParagraphStyle(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to get the representation of constructor object");
        return nullptr;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to instantiate JavaScript JsParagraphStyle instance");
        return nullptr;
    }
    return result;
}

JsParagraphStyle::~JsParagraphStyle()
{
    m_paragraphStyle = nullptr;
}

std::shared_ptr<TypographyStyle> JsParagraphStyle::GetParagraphStyle()
{
    return m_paragraphStyle;
}
} // namespace OHOS::Rosen