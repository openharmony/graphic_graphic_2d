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

#include "js_shader_effect.h"
#include "matrix_napi/js_matrix.h"

#include <cstdint>
#include "image/image.h"
#include "sampling_options_napi/js_sampling_options.h"
#include "js_drawing_utils.h"
#include "utils/log.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsShaderEffect::constructor_ = nullptr;
const std::string CLASS_NAME = "ShaderEffect";
napi_value JsShaderEffect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createColorShader", JsShaderEffect::CreateColorShader),
        DECLARE_NAPI_STATIC_FUNCTION("createLinearGradient", JsShaderEffect::CreateLinearGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createRadialGradient", JsShaderEffect::CreateRadialGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createSweepGradient", JsShaderEffect::CreateSweepGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createComposeShader", JsShaderEffect::CreateComposeShader),
        DECLARE_NAPI_STATIC_FUNCTION("createConicalGradient", JsShaderEffect::CreateConicalGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createImageShader", JsShaderEffect::CreateImageShader),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init failed to define shadowLayer class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsShaderEffect::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    auto shaderEffect = std::make_shared<ShaderEffect>();
    JsShaderEffect *jsShaderEffect = new JsShaderEffect(shaderEffect);
    status = napi_wrap(env, jsThis, jsShaderEffect, JsShaderEffect::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsShaderEffect;
        ROSEN_LOGE("failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsShaderEffect::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsShaderEffect* napi = reinterpret_cast<JsShaderEffect*>(nativeObject);
        delete napi;
    }
}

JsShaderEffect::~JsShaderEffect()
{
    m_shaderEffect = nullptr;
}

napi_value JsShaderEffect::CreateColorShader(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    uint32_t color = 0;
    GET_UINT32_PARAM(ARGC_ZERO, color);

    std::shared_ptr<ShaderEffect> colorShader = ShaderEffect::CreateColorShader(color);
    return JsShaderEffect::Create(env, colorShader);
}

napi_value JsShaderEffect::CreateLinearGradient(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_SIX;
    napi_value argv[ARGC_SIX] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_FOUR, ARGC_SIX);
    Drawing::Point drawingStartPoint;
    double startPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_ZERO], startPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of x, y be number.");
    }
    drawingStartPoint = Drawing::Point(startPoint[ARGC_ZERO], startPoint[ARGC_ONE]);
    Drawing::Point drawingEndPoint;
    double endPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_ONE], endPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of x, y be number.");
    }
    drawingEndPoint = Drawing::Point(endPoint[ARGC_ZERO], endPoint[ARGC_ONE]);
    napi_value arrayColors = argv[ARGC_TWO];
    uint32_t size = 0;
    napi_get_array_length(env, arrayColors, &size);
    std::vector<ColorQuad> colors;
    colors.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayColors, i, &element);
        int32_t value = 0;
        napi_get_value_int32(env, element, &value);
        colors.emplace_back(value);
    }
    int32_t jsTileMode = 0;
    GET_ENUM_PARAM(ARGC_THREE, jsTileMode, 0, static_cast<int32_t>(TileMode::DECAL));

    std::vector<scalar> pos;
    Drawing::Matrix* drawingMatrixPtr = nullptr;

    if (argc == ARGC_FOUR) {
        std::shared_ptr<ShaderEffect> linearGradient = ShaderEffect::CreateLinearGradient(drawingStartPoint,
            drawingEndPoint, colors, pos, static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
        return JsShaderEffect::Create(env, linearGradient);
    }
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_FOUR], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateLinearGradient parameter4 type.");
    }
    if (valueType != napi_null) {
        napi_value arrayPos = argv[ARGC_FOUR];
        napi_get_array_length(env, arrayPos, &size);
        pos.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            napi_value element;
            napi_get_element(env, arrayPos, i, &element);
            double value = 0;
            napi_get_value_double(env, element, &value);
            pos.emplace_back(value);
        }
    }
    if (argc == ARGC_FIVE) {
        std::shared_ptr<ShaderEffect> linearGradient = ShaderEffect::CreateLinearGradient(drawingStartPoint,
            drawingEndPoint, colors, pos, static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
        return JsShaderEffect::Create(env, linearGradient);
    }

    if (napi_typeof(env, argv[ARGC_FIVE], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateLinearGradient parameter5 type.");
    }
    if (valueType == napi_object) {
        JsMatrix* jsMatrix = nullptr;
        GET_UNWRAP_PARAM(ARGC_FIVE, jsMatrix);
        if (jsMatrix->GetMatrix() != nullptr) {
            drawingMatrixPtr = jsMatrix->GetMatrix().get();
        }
    }

    std::shared_ptr<ShaderEffect> linearGradient = ShaderEffect::CreateLinearGradient(drawingStartPoint,
        drawingEndPoint, colors, pos, static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
    return JsShaderEffect::Create(env, linearGradient);
}

napi_value JsShaderEffect::CreateRadialGradient(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_SIX;
    napi_value argv[ARGC_SIX] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_FOUR, ARGC_SIX);
    Drawing::Point drawingCenterPoint;
    double centerPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_ZERO], centerPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of x, y be number.");
    }
    drawingCenterPoint = Drawing::Point(centerPoint[ARGC_ZERO], centerPoint[ARGC_ONE]);

    double radius = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, radius);

    napi_value arrayColors = argv[ARGC_TWO];
    uint32_t size = 0;
    napi_get_array_length(env, arrayColors, &size);
    std::vector<ColorQuad> colors;
    colors.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayColors, i, &element);
        int32_t value = 0;
        napi_get_value_int32(env, element, &value);
        colors.emplace_back(value);
    }

    int32_t jsTileMode = 0;
    GET_ENUM_PARAM(ARGC_THREE, jsTileMode, 0, static_cast<int32_t>(TileMode::DECAL));

    std::vector<scalar> pos;
    Drawing::Matrix* drawingMatrixPtr = nullptr;

    if (argc == ARGC_FOUR) {
        std::shared_ptr<ShaderEffect> radialGradient = ShaderEffect::CreateRadialGradient(drawingCenterPoint,
            radius, colors, pos, static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
        return JsShaderEffect::Create(env, radialGradient);
    }
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_FOUR], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateRadialGradient parameter4 type.");
    }
    if (valueType != napi_null) {
        napi_value arrayPos = argv[ARGC_FOUR];
        napi_get_array_length(env, arrayPos, &size);
        pos.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            napi_value element;
            napi_get_element(env, arrayPos, i, &element);
            double value = 0;
            napi_get_value_double(env, element, &value);
            pos.emplace_back(value);
        }
    }
    if (argc == ARGC_FIVE) {
        std::shared_ptr<ShaderEffect> radialGradient = ShaderEffect::CreateRadialGradient(drawingCenterPoint,
            radius, colors, pos, static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
        return JsShaderEffect::Create(env, radialGradient);
    }

    if (napi_typeof(env, argv[ARGC_FIVE], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateRadialGradient parameter5 type.");
    }
    if (valueType == napi_object) {
        JsMatrix* jsMatrix = nullptr;
        GET_UNWRAP_PARAM(ARGC_FIVE, jsMatrix);
        if (jsMatrix->GetMatrix() != nullptr) {
            drawingMatrixPtr = jsMatrix->GetMatrix().get();
        }
    }

    std::shared_ptr<ShaderEffect> radialGradient = ShaderEffect::CreateRadialGradient(drawingCenterPoint,
        radius, colors, pos, static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
    return JsShaderEffect::Create(env, radialGradient);
}

napi_value JsShaderEffect::CreateSweepGradient(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_SEVEN;
    napi_value argv[ARGC_SEVEN] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_FIVE, ARGC_SEVEN);

    Drawing::Point drawingCenterPoint;
    double centerPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_ZERO], centerPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of x, y be number.");
    }
    drawingCenterPoint = Drawing::Point(centerPoint[ARGC_ZERO], centerPoint[ARGC_ONE]);

    napi_value arrayColors = argv[ARGC_ONE];
    uint32_t size = 0;
    napi_get_array_length(env, arrayColors, &size);
    std::vector<ColorQuad> colors;
    colors.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayColors, i, &element);
        int32_t value = 0;
        napi_get_value_int32(env, element, &value);
        colors.emplace_back(value);
    }

    int32_t jsTileMode = 0;
    GET_ENUM_PARAM(ARGC_TWO, jsTileMode, 0, static_cast<int32_t>(TileMode::DECAL));
    double startAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, startAngle);
    double endAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, endAngle);
    std::vector<scalar> pos;
    Drawing::Matrix* drawingMatrixPtr = nullptr;

    if (argc == ARGC_FIVE) {
        std::shared_ptr<ShaderEffect> sweepGradient = ShaderEffect::CreateSweepGradient(drawingCenterPoint,
            colors, pos, static_cast<TileMode>(jsTileMode), startAngle, endAngle, drawingMatrixPtr);
        return JsShaderEffect::Create(env, sweepGradient);
    }
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_FIVE], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateRadialGradient parameter5 type.");
    }
    if (valueType != napi_null) {
        napi_value arrayPos = argv[ARGC_FIVE];
        napi_get_array_length(env, arrayPos, &size);
        pos.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            napi_value element;
            napi_get_element(env, arrayPos, i, &element);
            double value = 0;
            napi_get_value_double(env, element, &value);
            pos.emplace_back(value);
        }
    }
    if (argc == ARGC_SIX) {
        std::shared_ptr<ShaderEffect> sweepGradient = ShaderEffect::CreateSweepGradient(drawingCenterPoint,
            colors, pos, static_cast<TileMode>(jsTileMode), startAngle, endAngle, drawingMatrixPtr);
        return JsShaderEffect::Create(env, sweepGradient);
    }

    if (napi_typeof(env, argv[ARGC_SIX], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateRadialGradient parameter6 type.");
    }
    if (valueType == napi_object) {
        JsMatrix* jsMatrix = nullptr;
        GET_UNWRAP_PARAM(ARGC_SIX, jsMatrix);
        if (jsMatrix->GetMatrix() != nullptr) {
            drawingMatrixPtr = jsMatrix->GetMatrix().get();
        }
    }
    std::shared_ptr<ShaderEffect> sweepGradient = ShaderEffect::CreateSweepGradient(drawingCenterPoint,
        colors, pos, static_cast<TileMode>(jsTileMode), startAngle, endAngle, drawingMatrixPtr);
    return JsShaderEffect::Create(env, sweepGradient);
}

napi_value JsShaderEffect::CreateComposeShader(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_ZERO], &valueType) != napi_ok || valueType != napi_object) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateComposeShader parameter0 type.");
    }
    JsShaderEffect* dstJsShaderEffect = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, dstJsShaderEffect);
    if (dstJsShaderEffect->GetShaderEffect() == nullptr) {
        ROSEN_LOGE("JsShaderEffect::CreateBlendShader destination ShaderEffect is nullptr");
        return nullptr;
    }

    if (napi_typeof(env, argv[ARGC_ONE], &valueType) != napi_ok || valueType != napi_object) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateComposeShader parameter1 type.");
    }
    JsShaderEffect* srcJsShaderEffect = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, srcJsShaderEffect);
    if (srcJsShaderEffect->GetShaderEffect() == nullptr) {
        ROSEN_LOGE("JsShaderEffect::CreateBlendShader source ShaderEffect is nullptr");
        return nullptr;
    }

    int32_t jsBlendMode = 0;
    GET_ENUM_PARAM_RANGE(ARGC_TWO, jsBlendMode, static_cast<int32_t>(BlendMode::CLEAR),
        static_cast<int32_t>(BlendMode::LUMINOSITY));

    std::shared_ptr<ShaderEffect> effectShader = ShaderEffect::CreateBlendShader(*dstJsShaderEffect->GetShaderEffect(),
        *srcJsShaderEffect->GetShaderEffect(), static_cast<BlendMode>(jsBlendMode));
    return JsShaderEffect::Create(env, effectShader);
}

napi_value JsShaderEffect::CreateConicalGradient(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_EIGHT;
    napi_value argv[ARGC_EIGHT] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_SIX, ARGC_EIGHT);

    Drawing::Point drawingStartPoint;
    double startPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_ZERO], startPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of x, y be number.");
    }
    drawingStartPoint = Drawing::Point(startPoint[ARGC_ZERO], startPoint[ARGC_ONE]);

    double startRadius = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, startRadius);

    Drawing::Point drawingEndPoint;
    double endPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_TWO], endPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter2 type. The type of x, y be number.");
    }
    drawingEndPoint = Drawing::Point(endPoint[ARGC_ZERO], endPoint[ARGC_ONE]);
    double endRadius = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, endRadius);

    napi_value arrayColors = argv[ARGC_FOUR];
    uint32_t size = 0;
    napi_get_array_length(env, arrayColors, &size);
    std::vector<ColorQuad> colors;
    colors.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayColors, i, &element);
        int32_t value = 0;
        napi_get_value_int32(env, element, &value);
        colors.emplace_back(value);
    }

    int32_t jsTileMode = 0;
    GET_ENUM_PARAM(ARGC_FIVE, jsTileMode, 0, static_cast<int32_t>(TileMode::DECAL));
    std::vector<scalar> pos;
    Drawing::Matrix* drawingMatrixPtr = nullptr;

    if (argc == ARGC_SIX) {
        std::shared_ptr<ShaderEffect> twoPointConicalGradient = ShaderEffect::CreateTwoPointConical(drawingStartPoint,
            startRadius, drawingEndPoint, endRadius, colors, pos, static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
        return JsShaderEffect::Create(env, twoPointConicalGradient);
    }
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_SIX], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateConicalGradient parameter6 type.");
    }
    if (valueType != napi_null) {
        napi_value arrayPos = argv[ARGC_SIX];
        napi_get_array_length(env, arrayPos, &size);
        pos.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            napi_value element;
            napi_get_element(env, arrayPos, i, &element);
            double value = 0;
            napi_get_value_double(env, element, &value);
            pos.emplace_back(value);
        }
    }
    if (argc == ARGC_SEVEN) {
        std::shared_ptr<ShaderEffect> twoPointConicalGradient = ShaderEffect::CreateTwoPointConical(
            drawingStartPoint, startRadius, drawingEndPoint, endRadius, colors, pos,
            static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
        return JsShaderEffect::Create(env, twoPointConicalGradient);
    }

    if (napi_typeof(env, argv[ARGC_SEVEN], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect CreateConicalGradient parameter7 type.");
    }
    if (valueType == napi_object) {
        JsMatrix* jsMatrix = nullptr;
        GET_UNWRAP_PARAM(ARGC_SEVEN, jsMatrix);
        if (jsMatrix->GetMatrix() != nullptr) {
            drawingMatrixPtr = jsMatrix->GetMatrix().get();
        }
    }

    std::shared_ptr<ShaderEffect> twoPointConicalGradient = ShaderEffect::CreateTwoPointConical(
        drawingStartPoint, startRadius, drawingEndPoint, endRadius, colors, pos,
        static_cast<TileMode>(jsTileMode), drawingMatrixPtr);
    return JsShaderEffect::Create(env, twoPointConicalGradient);
}

napi_value JsShaderEffect::CreateImageShader(napi_env env, napi_callback_info info)
{
    #ifdef ROSEN_OHOS
    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = { nullptr };
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_FOUR, ARGC_FIVE);

    Media::PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi);
    auto pixel = pixelMapNapi->GetPixelNapiInner();
    if (pixel == nullptr) {
        ROSEN_LOGE("JsShaderEffect::CreateImageShader pixelmap GetPixelNapiInner is nullptr");
        return nullptr;
    }
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
    if (image == nullptr) {
        ROSEN_LOGE("JsShaderEffect::CreateImageShader image is nullptr");
        return nullptr;
    }
    int32_t jsTileModeX = 0;
    int32_t jsTileModeY = 0;
    GET_ENUM_PARAM_RANGE(ARGC_ONE, jsTileModeX, 0, static_cast<int32_t>(TileMode::DECAL));
    GET_ENUM_PARAM_RANGE(ARGC_TWO, jsTileModeY, 0, static_cast<int32_t>(TileMode::DECAL));
    JsSamplingOptions* jsSamplingOptions = nullptr;
    GET_UNWRAP_PARAM(ARGC_THREE, jsSamplingOptions);
    std::shared_ptr<SamplingOptions> samplingOptions = jsSamplingOptions->GetSamplingOptions();
    if (samplingOptions == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect samplingOptions parameter.");
    }

    Drawing::Matrix matrix;
    if (argc == ARGC_FIVE) {
        JsMatrix* jsMatrix = nullptr;
        GET_UNWRAP_PARAM_OR_NULL(ARGC_FOUR, jsMatrix);

        if (jsMatrix != nullptr && jsMatrix->GetMatrix() != nullptr) {
            matrix = *jsMatrix->GetMatrix();
        }
    }
    std::shared_ptr<ShaderEffect> imageShader = ShaderEffect::CreateImageShader(*image,
        static_cast<TileMode>(jsTileModeX), static_cast<TileMode>(jsTileModeY), *samplingOptions, matrix);
    return JsShaderEffect::Create(env, imageShader);
#endif
    return nullptr;
}

napi_value JsShaderEffect::Create(napi_env env, const std::shared_ptr<ShaderEffect> gradient)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || gradient == nullptr) {
        ROSEN_LOGE("[NAPI]Object or JsShaderEffect is null!");
        return nullptr;
    }

    std::unique_ptr<JsShaderEffect> jsShaderEffect = std::make_unique<JsShaderEffect>(gradient);
    napi_wrap(env, objValue, jsShaderEffect.release(), JsShaderEffect::Finalizer, nullptr, nullptr);

    if (objValue == nullptr) {
        ROSEN_LOGE("[NAPI]objValue is null!");
        return nullptr;
    }
    return objValue;
}

void JsShaderEffect::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsShaderEffect>(static_cast<JsShaderEffect*>(data));
}

std::shared_ptr<ShaderEffect> JsShaderEffect::GetShaderEffect()
{
    return m_shaderEffect;
}

} // namespace Drawing
} // namespace OHOS::Rosen
