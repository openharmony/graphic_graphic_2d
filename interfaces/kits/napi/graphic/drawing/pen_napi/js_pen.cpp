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

#include "js_pen.h"

#include <cstdint>

#include "color_filter_napi/js_color_filter.h"
#include "image_filter_napi/js_image_filter.h"
#include "js_color_space.h"
#include "js_drawing_utils.h"
#include "mask_filter_napi/js_mask_filter.h"
#include "matrix_napi/js_matrix.h"
#include "path_effect_napi/js_path_effect.h"
#include "path_napi/js_path.h"
#include "shader_effect_napi/js_shader_effect.h"
#include "shadow_layer_napi/js_shadow_layer.h"

#include "utils/colorspace_convertor.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPen::constructor_ = nullptr;
const std::string CLASS_NAME = "Pen";
napi_value JsPen::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setAntiAlias", SetAntiAlias),
        DECLARE_NAPI_FUNCTION("setAlpha", SetAlpha),
        DECLARE_NAPI_FUNCTION("getAlpha", GetAlpha),
        DECLARE_NAPI_FUNCTION("setBlendMode", SetBlendMode),
        DECLARE_NAPI_FUNCTION("setCapStyle", SetCapStyle),
        DECLARE_NAPI_FUNCTION("getCapStyle", GetCapStyle),
        DECLARE_NAPI_FUNCTION("setColor", SetColor),
        DECLARE_NAPI_FUNCTION("getColor", GetColor),
        DECLARE_NAPI_FUNCTION("setColor4f", SetColor4f),
        DECLARE_NAPI_FUNCTION("getColor4f", GetColor4f),
        DECLARE_NAPI_FUNCTION("getHexColor", GetHexColor),
        DECLARE_NAPI_FUNCTION("setColorFilter", SetColorFilter),
        DECLARE_NAPI_FUNCTION("getColorFilter", GetColorFilter),
        DECLARE_NAPI_FUNCTION("setImageFilter", SetImageFilter),
        DECLARE_NAPI_FUNCTION("setDither", SetDither),
        DECLARE_NAPI_FUNCTION("setJoinStyle", SetJoinStyle),
        DECLARE_NAPI_FUNCTION("getJoinStyle", GetJoinStyle),
        DECLARE_NAPI_FUNCTION("setMaskFilter", SetMaskFilter),
        DECLARE_NAPI_FUNCTION("setPathEffect", SetPathEffect),
        DECLARE_NAPI_FUNCTION("setStrokeWidth", SetStrokeWidth),
        DECLARE_NAPI_FUNCTION("setShadowLayer", SetShadowLayer),
        DECLARE_NAPI_FUNCTION("setShaderEffect", SetShaderEffect),
        DECLARE_NAPI_FUNCTION("getFillPath", GetFillPath),
        DECLARE_NAPI_FUNCTION("isAntiAlias", IsAntiAlias),
        DECLARE_NAPI_FUNCTION("getWidth", GetWidth),
        DECLARE_NAPI_FUNCTION("setMiterLimit", SetMiterLimit),
        DECLARE_NAPI_FUNCTION("getMiterLimit", GetMiterLimit),
        DECLARE_NAPI_FUNCTION("reset", Reset),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Init Failed to define Pen class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsPen::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsPen* jsPen = nullptr;
    if (argCount == 0) {
        jsPen = new JsPen();
    } else {
        JsPen* otherPen = nullptr;
        GET_UNWRAP_PARAM(ARGC_ZERO, otherPen);
        Pen* pen = otherPen->GetPen();
        if (pen != nullptr) {
            jsPen = new JsPen(*pen);
        }
    }
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::Constructor jsPen is nullptr");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsPen, JsPen::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPen;
        ROSEN_LOGE("JsPen::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsPen::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPen* napi = reinterpret_cast<JsPen*>(nativeObject);
        delete napi;
    }
}

JsPen::JsPen()
{
    pen_ = new Pen();
}

JsPen::JsPen(const Pen& pen)
{
    pen_ = new Pen(pen);
}

JsPen::~JsPen()
{
    delete pen_;
    pen_ = nullptr;
}

napi_value JsPen::SetColor(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetColor pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_FOUR);

    Drawing::Color drawingColor;
    if (argc == ARGC_ONE) {
        ColorQuad color;
        if (!ConvertFromAdaptHexJsColor(env, argv[ARGC_ZERO], color)) {
            ROSEN_LOGE("JsPen::SetColor Argv[0] is invalid");
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Parameter verification failed. The range of color channels must be [0, 255].");
        }
        drawingColor.SetColorQuad(color);
    } else if (argc == ARGC_FOUR) {
        int32_t alpha = 0;
        GET_COLOR_PARAM(ARGC_ZERO, alpha);
        int32_t red = 0;
        GET_COLOR_PARAM(ARGC_ONE, red);
        int32_t green = 0;
        GET_COLOR_PARAM(ARGC_TWO, green);
        int32_t blue = 0;
        GET_COLOR_PARAM(ARGC_THREE, blue);
        drawingColor = Color::ColorQuadSetARGB(alpha, red, green, blue);
    } else {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect number of parameters.");
    }
    pen->SetColor(drawingColor);
    return nullptr;
}

napi_value JsPen::SetStrokeWidth(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetStrokeWidth pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double width = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, width);

    pen->SetWidth(static_cast<float>(width));
    return nullptr;
}


napi_value JsPen::SetAntiAlias(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAntiAlias pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool aa = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, aa);

    pen->SetAntiAlias(aa);
    return nullptr;
}

napi_value JsPen::SetAlpha(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t alpha = 0;
    if (!ConvertFromJsNumber(env, argv[ARGC_ZERO], alpha, 0, Color::RGB_MAX)) {
        ROSEN_LOGE("JsPen::SetAlpha Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The alpha range must be [0, 255].");
    }

    pen->SetAlpha(alpha);
    return nullptr;
}

napi_value JsPen::SetBlendMode(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetBlendMode pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t mode = 0;
    GET_ENUM_PARAM(ARGC_ZERO, mode, 0, static_cast<int32_t>(BlendMode::LUMINOSITY));

    pen->SetBlendMode(static_cast<BlendMode>(mode));
    return nullptr;
}

napi_value JsPen::SetColorFilter(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetColorFilter pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsColorFilter* jsColorFilter = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ZERO, jsColorFilter);

    Filter filter = pen->GetFilter();
    filter.SetColorFilter(jsColorFilter ? jsColorFilter->GetColorFilter() : nullptr);
    pen->SetFilter(filter);
    return nullptr;
}

napi_value JsPen::GetColorFilter(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetColorFilter jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetColorFilter pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (pen->HasFilter()) {
        return JsColorFilter::Create(env, pen->GetFilter().GetColorFilter());
    }
    return nullptr;
}

napi_value JsPen::SetImageFilter(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetImageFilter jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetImageFilter pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsImageFilter* jsImageFilter = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ZERO, jsImageFilter);

    Filter filter = pen->GetFilter();
    filter.SetImageFilter(jsImageFilter != nullptr ? jsImageFilter->GetImageFilter() : nullptr);
    pen->SetFilter(filter);
    return nullptr;
}

napi_value JsPen::SetMaskFilter(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetMaskFilter jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetMaskFilter pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMaskFilter* jsMaskFilter = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ZERO, jsMaskFilter);

    Filter filter = pen->GetFilter();
    filter.SetMaskFilter(jsMaskFilter ? jsMaskFilter->GetMaskFilter() : nullptr);
    pen->SetFilter(filter);
    return nullptr;
}

napi_value JsPen::SetDither(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool dither = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, dither);
    return nullptr;
}

napi_value JsPen::SetJoinStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetJoinStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetJoinStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t joinStyle = 0;
    GET_ENUM_PARAM(ARGC_ZERO, joinStyle, 0, static_cast<int32_t>(Pen::JoinStyle::BEVEL_JOIN));

    pen->SetJoinStyle(static_cast<Pen::JoinStyle>(joinStyle));
    return nullptr;
}

napi_value JsPen::GetJoinStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetJoinStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetJoinStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return CreateJsNumber(env, static_cast<int32_t>(pen->GetJoinStyle()));
}

static int32_t CapCastToTsCap(Pen::CapStyle cap)
{
    int32_t tsCap = 0; // 0: CapStyle::FLAT_CAP
    switch (cap) {
        case Pen::CapStyle::FLAT_CAP:
            tsCap = 0; // 0: CapStyle::FLAT_CAP
            break;
        case Pen::CapStyle::SQUARE_CAP:
            tsCap = 1; // 1: CapStyle::SQUARE_CAP
            break;
        case Pen::CapStyle::ROUND_CAP:
            tsCap = 2; // 2: CapStyle::ROUND_CAP
            break;
        default:
            break;
    }
    return tsCap;
}

static Pen::CapStyle TsCapCastToCap(int32_t tsCap)
{
    Pen::CapStyle cap = Pen::CapStyle::FLAT_CAP;
    switch (tsCap) {
        case 0: // 0: CapStyle::FLAT_CAP
            cap = Pen::CapStyle::FLAT_CAP;
            break;
        case 1: // 1: CapStyle::SQUARE_CAP
            cap = Pen::CapStyle::SQUARE_CAP;
            break;
        case 2: // 2: CapStyle::ROUND_CAP
            cap = Pen::CapStyle::ROUND_CAP;
            break;
        default:
            break;
    }
    return cap;
}

napi_value JsPen::SetCapStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetCapStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetCapStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t capStyle = 0;
    GET_ENUM_PARAM(ARGC_ZERO, capStyle, 0, static_cast<int32_t>(Pen::CapStyle::SQUARE_CAP));

    pen->SetCapStyle(TsCapCastToCap(capStyle));
    return nullptr;
}

napi_value JsPen::GetCapStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetCapStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetCapStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Pen::CapStyle capStyle = pen->GetCapStyle();
    return CreateJsNumber(env, CapCastToTsCap(capStyle));
}

napi_value JsPen::SetPathEffect(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetPathEffect jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetPathEffect pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsPathEffect* jsPathEffect = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ZERO, jsPathEffect);

    pen->SetPathEffect(jsPathEffect ? jsPathEffect->GetPathEffect() : nullptr);
    return nullptr;
}

napi_value JsPen::SetShadowLayer(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetShadowLayer jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetShadowLayer pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsShadowLayer* jsShadowLayer = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ZERO, jsShadowLayer);

    pen->SetLooper(jsShadowLayer ? jsShadowLayer->GetBlurDrawLooper() : nullptr);
    return nullptr;
}

napi_value JsPen::SetShaderEffect(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetShaderEffect jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetShaderEffect pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsShaderEffect* jsShaderEffect = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ZERO, jsShaderEffect);

    pen->SetShaderEffect(jsShaderEffect ? jsShaderEffect->GetShaderEffect() : nullptr);
    return nullptr;
}

napi_value JsPen::GetFillPath(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetFillPath jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetFillPath pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);
    
    JsPath* src = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, src);
    if (src->GetPath() == nullptr) {
        ROSEN_LOGE("JsPen::GetFillPath src jsPath is nullptr");
        return nullptr;
    }

    JsPath* dst = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, dst);
    if (dst->GetPath() == nullptr) {
        ROSEN_LOGE("JsPen::GetFillPath dst jsPath is nullptr");
        return nullptr;
    }

    return CreateJsValue(env, pen->GetFillPath(*src->GetPath(), *dst->GetPath(), nullptr, Matrix()));
}

napi_value JsPen::IsAntiAlias(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::IsAntiAlias jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::IsAntiAlias pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, pen->IsAntiAlias());
}

napi_value JsPen::GetAlpha(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetAlpha jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    uint32_t alpha = pen->GetAlpha();
    return CreateJsNumber(env, alpha);
}

napi_value JsPen::GetWidth(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetWidth jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetWidth pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    scalar width = pen->GetWidth();
    return CreateJsNumber(env, width);
}

napi_value JsPen::SetMiterLimit(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetMiterLimit jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetMiterLimit pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double miterLimit = 0;
    GET_DOUBLE_PARAM(ARGC_ZERO, miterLimit);

    pen->SetMiterLimit(miterLimit);
    return nullptr;
}

napi_value JsPen::GetMiterLimit(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetMiterLimit jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetMiterLimit pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    scalar miterLimit = pen->GetMiterLimit();
    return CreateJsNumber(env, miterLimit);
}

napi_value JsPen::Reset(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::Reset jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::Reset pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    pen->Reset();
    return nullptr;
}

napi_value JsPen::GetColor(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetColor jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetColor pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Color color = pen->GetColor();
    return GetColorAndConvertToJsValue(env, color);
}

napi_value JsPen::SetColor4f(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetColor4f pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    Drawing::Color4f drawingColor4f;
    if (!ConvertFromAdaptJsColor4F(env, argv[ARGC_ZERO], drawingColor4f)) {
        ROSEN_LOGE("JsPen::SetColor4f Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Parameter verification failed.");
    }
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
    ColorManager::JsColorSpace* jsColorSpace = nullptr;
    GET_UNWRAP_PARAM_OR_NULL(ARGC_ONE, jsColorSpace);
    if (jsColorSpace != nullptr) {
        auto colorManagerColorSpace = jsColorSpace->GetColorSpaceToken();
        if (colorManagerColorSpace != nullptr) {
            drawingColorSpace = Drawing::ColorSpaceConvertor::
                ColorSpaceConvertToDrawingColorSpace(colorManagerColorSpace);
        }
    }
    pen->SetColor(drawingColor4f, drawingColorSpace);
    return nullptr;
}

napi_value JsPen::GetColor4f(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetColor4f jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetColor4f pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    const Color4f& color4f = pen->GetColor4f();
    return GetColor4FAndConvertToJsValue(env, color4f);
}

napi_value JsPen::GetHexColor(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetHexColor jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetHexColor pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Color color = pen->GetColor();
    return CreateJsNumber(env, color.CastToColorQuad());
}

Pen* JsPen::GetPen()
{
    return pen_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
