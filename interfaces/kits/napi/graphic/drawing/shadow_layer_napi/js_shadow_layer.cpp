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

#include "js_shadow_layer.h"

#include <cstdint>
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsShadowLayer::constructor_ = nullptr;
const std::string CLASS_NAME = "ShadowLayer";
napi_value JsShadowLayer::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("create", JsShadowLayer::Create),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShadowLayer::Init failed to define shadowLayer class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShadowLayer::Init failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShadowLayer::Init failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsShadowLayer::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShadowLayer::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsShadowLayer *jsShadowLayer = new(std::nothrow) JsShadowLayer();
    if (jsShadowLayer == nullptr) {
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsShadowLayer, JsShadowLayer::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsShadowLayer;
        ROSEN_LOGE("JsShadowLayer::Constructor failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsShadowLayer::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsShadowLayer* napi = reinterpret_cast<JsShadowLayer*>(nativeObject);
        delete napi;
    }
}

JsShadowLayer::~JsShadowLayer()
{
    m_blurDrawLooper = nullptr;
}

napi_value JsShadowLayer::Create(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_number);
    CHECK_EACH_PARAM(ARGC_ONE, napi_number);
    CHECK_EACH_PARAM(ARGC_TWO, napi_number);
    CHECK_EACH_PARAM(ARGC_THREE, napi_object);

    double blurRadius = 0.0;
    if (!ConvertFromJsValue(env, argv[ARGC_ZERO], blurRadius)) {
        ROSEN_LOGE("JsShadowLayer::Create argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    double dx = 0.0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], dx)) {
        ROSEN_LOGE("JsShadowLayer::Create argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    double dy = 0.0;
    if (!ConvertFromJsValue(env, argv[ARGC_TWO], dy)) {
        ROSEN_LOGE("JsShadowLayer::Create argv[2] is invalid");
        return NapiGetUndefined(env);
    }

    int32_t argb[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor(env, argv[ARGC_ZERO], argb, ARGC_FOUR)) {
        ROSEN_LOGE("JsPen::SetColor Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }

    ColorQuad color = Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
    std::shared_ptr<BlurDrawLooper> looper = BlurDrawLooper::CreateBlurDrawLooper(blurRadius, dx, dy, color);
    return JsShadowLayer::CreateLooper(env, looper);
}

napi_value JsShadowLayer::CreateLooper(napi_env env, const std::shared_ptr<BlurDrawLooper> blurDrawLooper)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || blurDrawLooper == nullptr) {
        ROSEN_LOGE("JsShadowLayer::CreateLooper object is null!");
        return NapiGetUndefined(env);
    }

    std::unique_ptr<JsShadowLayer> jsShadowLayer = std::make_unique<JsShadowLayer>(blurDrawLooper);
    napi_wrap(env, objValue, jsShadowLayer.release(), JsShadowLayer::Finalizer, nullptr, nullptr);

    if (objValue == nullptr) {
        ROSEN_LOGE("JsShadowLayer::CreateLooper objValue is null!");
        return NapiGetUndefined(env);
    }

    return objValue;
}

void JsShadowLayer::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsShadowLayer>(static_cast<JsShadowLayer*>(data));
}

std::shared_ptr<BlurDrawLooper> JsShadowLayer::GetBlurDrawLooper()
{
    return m_blurDrawLooper;
}

} // namespace Drawing
} // namespace OHOS::Rosen
