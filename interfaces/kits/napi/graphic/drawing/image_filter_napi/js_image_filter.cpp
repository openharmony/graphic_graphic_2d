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
#include "image/image.h"
#include "utils/sampling_options.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "ImageFilter";
thread_local napi_ref JsImageFilter::constructor_ = nullptr;
napi_value JsImageFilter::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createBlendImageFilter", JsImageFilter::CreateBlendImageFilter),
        DECLARE_NAPI_STATIC_FUNCTION("createComposeImageFilter", JsImageFilter::CreateComposeImageFilter),
        DECLARE_NAPI_STATIC_FUNCTION("createBlurImageFilter", JsImageFilter::CreateBlurImageFilter),
        DECLARE_NAPI_STATIC_FUNCTION("createFromColorFilter", JsImageFilter::CreateFromColorFilter),
        DECLARE_NAPI_STATIC_FUNCTION("createFromImage", JsImageFilter::CreateFromImage),
        DECLARE_NAPI_STATIC_FUNCTION("createFromShaderEffect", JsImageFilter::CreateFromShaderEffect),
        DECLARE_NAPI_STATIC_FUNCTION("createOffsetImageFilter", JsImageFilter::CreateOffsetImageFilter),
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

napi_value JsImageFilter::CreateBlendImageFilter(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_THREE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    int32_t blendMode = 0;
    GET_ENUM_PARAM_RANGE(ARGC_ZERO, blendMode, 0, static_cast<int32_t>(BlendMode::LUMINOSITY));

    JsImageFilter* jsBackground = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsBackground);

    JsImageFilter* jsForeground = nullptr;
    GET_UNWRAP_PARAM(ARGC_TWO, jsForeground);

    std::shared_ptr<ImageFilter> background = jsBackground->GetImageFilter();
    std::shared_ptr<ImageFilter> foreground = jsForeground->GetImageFilter();

    std::shared_ptr<ImageFilter> imgFilter = ImageFilter::CreateBlendImageFilter(static_cast<BlendMode>(blendMode),
        background, foreground);
    return JsImageFilter::Create(env, imgFilter);
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

napi_value JsImageFilter::CreateComposeImageFilter(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    JsImageFilter* jsOuter = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ZERO, jsOuter);

    JsImageFilter* jsInner = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ONE, jsInner);

    if (jsInner == nullptr && jsOuter == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Inner and outer are nullptr");
    }

    std::shared_ptr<ImageFilter> outer = jsOuter ? jsOuter->GetImageFilter() : nullptr;
    std::shared_ptr<ImageFilter> inner = jsInner ? jsInner->GetImageFilter() : nullptr;
    std::shared_ptr<ImageFilter> imageFilter = ImageFilter::CreateComposeImageFilter(outer, inner);
    if (imageFilter == nullptr) {
        ROSEN_LOGE("JsImageFilter::CreateComposeImageFilter: imageFilter is nullptr.");
        return nullptr;
    }
    return JsImageFilter::Create(env, imageFilter);
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

napi_value JsImageFilter::CreateFromImage(napi_env env, napi_callback_info info)
{
#ifdef ROSEN_OHOS
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_THREE);

    Media::PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi);

    std::shared_ptr<Media::PixelMap> pixelMap = pixelMapNapi->GetPixelNapiInner();
    if (pixelMap == nullptr) {
        ROSEN_LOGE("JsImageFilter::CreateFromImage GetPixelNapiInner failed!");
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
    if (image == nullptr) {
        ROSEN_LOGE("JsImageFilter::CreateFromImage image is nullptr!");
        return nullptr;
    }
    Drawing::Rect srcRect = Drawing::Rect(0.0f, 0.0f, image->GetWidth(), image->GetHeight());
    Drawing::Rect dstRect = srcRect;
    if (argc >= ARGC_TWO) {
        napi_valuetype valueType = napi_undefined;
        if (napi_typeof(env, argv[ARGC_ONE], &valueType) != napi_ok) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect CreateFromImage parameter1 type.");
        }
        if (valueType != napi_null && valueType != napi_object) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect valueType CreateFromImage parameter1 type.");
        }
        if (valueType == napi_object) {
            double srcLtrb[ARGC_FOUR] = {0};
            if (!ConvertFromJsRect(env, argv[ARGC_ONE], srcLtrb, ARGC_FOUR)) {
                return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                    "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
            }
            srcRect = Drawing::Rect(srcLtrb[ARGC_ZERO], srcLtrb[ARGC_ONE], srcLtrb[ARGC_TWO], srcLtrb[ARGC_THREE]);
        }
    }
    if (argc == ARGC_THREE) {
        napi_valuetype valueType = napi_undefined;
        if (napi_typeof(env, argv[ARGC_TWO], &valueType) != napi_ok) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect CreateFromImage parameter2 type.");
        }
        if (valueType != napi_null && valueType != napi_object) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect valueType CreateFromImage parameter2 type.");
        }
        if (valueType == napi_object) {
            double dstLtrb[ARGC_FOUR] = {0};
            if (!ConvertFromJsRect(env, argv[ARGC_TWO], dstLtrb, ARGC_FOUR)) {
                return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                    "Incorrect parameter2 type. The type of left, top, right and bottom must be number.");
            }
            dstRect = Drawing::Rect(dstLtrb[ARGC_ZERO], dstLtrb[ARGC_ONE], dstLtrb[ARGC_TWO], dstLtrb[ARGC_THREE]);
        }
    }

    std::shared_ptr<ImageFilter> imgFilter = ImageFilter::CreateImageImageFilter(image, srcRect, dstRect,
        SamplingOptions(FilterMode::LINEAR));
    return JsImageFilter::Create(env, imgFilter);
#else
    return nullptr;
#endif
}

napi_value JsImageFilter::CreateFromShaderEffect(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    JsShaderEffect* jsShaderEffect = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsShaderEffect);
    std::shared_ptr<ShaderEffect> shaderEffect = jsShaderEffect->GetShaderEffect();
    std::shared_ptr<ImageFilter> imgFilter = ImageFilter::CreateShaderImageFilter(shaderEffect);
    return JsImageFilter::Create(env, imgFilter);
}

napi_value JsImageFilter::CreateOffsetImageFilter(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_TWO, ARGC_THREE);

    double dx = 0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    std::shared_ptr<ImageFilter> input = nullptr;
    if (argc == ARGC_THREE) {
        JsImageFilter* jsInput = nullptr;
        GET_UNWRAP_PARAM_OR_NULL(ARGC_TWO, jsInput);
        if (jsInput != nullptr) {
            input = jsInput->GetImageFilter();
        }
    }
    auto imageFilter = ImageFilter::CreateOffsetImageFilter(dx, dy, input);
    return JsImageFilter::Create(env, imageFilter);
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
