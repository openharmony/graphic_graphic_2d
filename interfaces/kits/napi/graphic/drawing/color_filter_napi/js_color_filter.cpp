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

#include "js_color_filter.h"

#include "native_value.h"
#include "draw/color.h"

#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "ColorFilter";
thread_local napi_ref JsColorFilter::constructor_ = nullptr;
napi_value JsColorFilter::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createBlendModeColorFilter", JsColorFilter::CreateBlendModeColorFilter),
        DECLARE_NAPI_STATIC_FUNCTION("createComposeColorFilter", JsColorFilter::CreateComposeColorFilter),
        DECLARE_NAPI_STATIC_FUNCTION("createLinearToSRGBGamma", JsColorFilter::CreateLinearToSRGBGamma),
        DECLARE_NAPI_STATIC_FUNCTION("createSRGBGammaToLinear", JsColorFilter::CreateSRGBGammaToLinear),
        DECLARE_NAPI_STATIC_FUNCTION("createLumaColorFilter", JsColorFilter::CreateLumaColorFilter),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define ColorFilter class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

void JsColorFilter::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsColorFilter>(static_cast<JsColorFilter*>(data));
}

JsColorFilter::~JsColorFilter()
{
    m_ColorFilter = nullptr;
}

napi_value JsColorFilter::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    JsColorFilter *jsColorFilter = new(std::nothrow) JsColorFilter();
    status = napi_wrap(env, jsThis, jsColorFilter, JsColorFilter::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsColorFilter;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsColorFilter::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsColorFilter *napi = reinterpret_cast<JsColorFilter *>(nativeObject);
        delete napi;
    }
}

napi_value JsColorFilter::CreateBlendModeColorFilter(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    int32_t argb[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor(env, argv[ARGC_ZERO], argb, ARGC_FOUR)) {
        ROSEN_LOGE("JsPen::SetColor Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }

    int32_t jsMode = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ONE, jsMode);

    auto color = Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateBlendModeColorFilter(color, BlendMode(jsMode));
    return JsColorFilter::Create(env, colorFilter);
}

napi_value JsColorFilter::CreateComposeColorFilter(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);
    CHECK_EACH_PARAM(ARGC_ONE, napi_object);

    JsColorFilter *jsColorFilter1 = nullptr;
    JsColorFilter *jsColorFilter2 = nullptr;
    napi_unwrap(env, argv[0], (void **)&jsColorFilter1);
    napi_unwrap(env, argv[1], (void **)&jsColorFilter2);
    if (jsColorFilter1 == nullptr || jsColorFilter2 == nullptr) {
        ROSEN_LOGE("JsColorFilter::CreateComposeColorFilter argv is invalid");
        return nullptr;
    }

    std::shared_ptr<ColorFilter> colorFilter1 = jsColorFilter1->GetColorFilter();
    std::shared_ptr<ColorFilter> colorFilter2 = jsColorFilter2->GetColorFilter();
    if (colorFilter1 == nullptr || colorFilter2 == nullptr) {
        ROSEN_LOGE("JsColorFilter::CreateComposeColorFilter colorFilter is nullptr");
        return nullptr;
    }

    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    return JsColorFilter::Create(env, colorFilter);
}

napi_value JsColorFilter::CreateLinearToSRGBGamma(napi_env env, napi_callback_info info)
{
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    return JsColorFilter::Create(env, colorFilter);
}

napi_value JsColorFilter::CreateSRGBGammaToLinear(napi_env env, napi_callback_info info)
{
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    return JsColorFilter::Create(env, colorFilter);
}

napi_value JsColorFilter::CreateLumaColorFilter(napi_env env, napi_callback_info info)
{
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLumaColorFilter();
    return JsColorFilter::Create(env, colorFilter);
}

napi_value JsColorFilter::Create(napi_env env, const std::shared_ptr<ColorFilter> colorFilter)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || colorFilter == nullptr) {
        ROSEN_LOGE("[NAPI]Object or JsColorFilter is null!");
        return nullptr;
    }

    std::unique_ptr<JsColorFilter> jsColorFilter = std::make_unique<JsColorFilter>(colorFilter);
    napi_wrap(env, objValue, jsColorFilter.release(), JsColorFilter::Finalizer, nullptr, nullptr);

    if (objValue == nullptr) {
        ROSEN_LOGE("[NAPI]objValue is null!");
        return nullptr;
    }
    return objValue;
}

std::shared_ptr<ColorFilter> JsColorFilter::GetColorFilter()
{
    return m_ColorFilter;
}
} // namespace Drawing
} // namespace OHOS::Rosen
