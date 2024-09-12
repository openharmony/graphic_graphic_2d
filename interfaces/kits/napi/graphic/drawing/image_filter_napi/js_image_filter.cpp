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

#include "js_image_filter.h"

#include "native_value.h"

#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "ImageFilter";
thread_local napi_ref JsImageFilter::constructor_ = nullptr;
napi_value JsImageFilter::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createBlurImageFilter", JsImageFilter::CreateBlurImageFilter),
        DECLARE_NAPI_STATIC_FUNCTION("createFromColorFilter", JsImageFilter::CreateFromColorFilter),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsImageFilter::Init Failed to define jsImageFilter class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsImageFilter::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsImageFilter::Init Failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("JsImageFilter::Init Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

void JsImageFilter::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsImageFilter>(static_cast<JsImageFilter*>(data));
}

JsImageFilter::~JsImageFilter()
{
    m_ImageFilter = nullptr;
}

napi_value JsImageFilter::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsImageFilter::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsImageFilter *jsImageFilter = new JsImageFilter();

    status = napi_wrap(env, jsThis, jsImageFilter, JsImageFilter::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsImageFilter;
        ROSEN_LOGE("JsImageFilter::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsImageFilter::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsImageFilter *napi = reinterpret_cast<JsImageFilter *>(nativeObject);
        delete napi;
    }
}

napi_value JsImageFilter::CreateBlurImageFilter(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_THREE, ARGC_FOUR);

    double sigmaX = 0;
    GET_DOUBLE_CHECK_GT_ZERO_PARAM(ARGC_ZERO, sigmaX);

    double sigmaY = 0;
    GET_DOUBLE_CHECK_GT_ZERO_PARAM(ARGC_ONE, sigmaY);

    int32_t tMode = 0;
    GET_ENUM_PARAM(ARGC_TWO, tMode, 0, static_cast<int32_t>(TileMode::DECAL));

    JsImageFilter *jsImageFilter = nullptr;
    if (argc > ARGC_THREE) {
        GET_UNWRAP_PARAM_OR_NULL(ARGC_THREE, jsImageFilter);
    }

    std::shared_ptr<ImageFilter> imageFilter = (jsImageFilter == nullptr) ?
        nullptr : jsImageFilter->GetImageFilter();

    std::shared_ptr<ImageFilter> imgFilter = ImageFilter::CreateBlurImageFilter(sigmaX, sigmaY,
        static_cast<TileMode>(tMode), imageFilter, ImageBlurType::GAUSS);
    return JsImageFilter::Create(env, imgFilter);
}

napi_value JsImageFilter::CreateFromColorFilter(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);

    JsColorFilter *jsColorFilter = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsColorFilter);

    JsImageFilter *jsImageFilter = nullptr;
    if (argc > ARGC_ONE) {
        GET_UNWRAP_PARAM_OR_NULL(ARGC_ONE, jsImageFilter);
    }

    std::shared_ptr<ColorFilter> colorFilter = jsColorFilter->GetColorFilter();
    std::shared_ptr<ImageFilter> imageFilter = (jsImageFilter == nullptr) ?
        nullptr : jsImageFilter->GetImageFilter();

    std::shared_ptr<ImageFilter> imgFilter = ImageFilter::CreateColorFilterImageFilter(*colorFilter, imageFilter);
    return JsImageFilter::Create(env, imgFilter);
}

napi_value JsImageFilter::Create(napi_env env, const std::shared_ptr<ImageFilter> imageFilter)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        ROSEN_LOGE("JsImageFilter::Create objValue is null!");
        return nullptr;
    }

    std::unique_ptr<JsImageFilter> jsImageFilter = std::make_unique<JsImageFilter>(imageFilter);
    napi_wrap(env, objValue, jsImageFilter.release(), JsImageFilter::Finalizer, nullptr, nullptr);

    return objValue;
}

std::shared_ptr<ImageFilter> JsImageFilter::GetImageFilter()
{
    return m_ImageFilter;
}
} // namespace Drawing
} // namespace OHOS::Rosen
