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

#include "js_font.h"

#include "js_drawing_utils.h"
#include "js_typeface.h"
#include "native_value.h"
#include "path_napi/js_path.h"
#ifdef USE_M133_SKIA
#include "src/base/SkUTF.h"
#else
#include "src/utils/SkUTF.h"
#endif

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsFont::constructor_ = nullptr;
static std::mutex g_constructorInitMutex;
const std::string CLASS_NAME = "Font";

static napi_property_descriptor properties[] = {
    DECLARE_NAPI_FUNCTION("enableSubpixel", JsFont::EnableSubpixel),
    DECLARE_NAPI_FUNCTION("enableEmbolden", JsFont::EnableEmbolden),
    DECLARE_NAPI_FUNCTION("enableLinearMetrics", JsFont::EnableLinearMetrics),
    DECLARE_NAPI_FUNCTION("setBaselineSnap", JsFont::SetBaselineSnap),
    DECLARE_NAPI_FUNCTION("setEmbeddedBitmaps", JsFont::SetEmbeddedBitmaps),
    DECLARE_NAPI_FUNCTION("setForceAutoHinting", JsFont::SetForceAutoHinting),
    DECLARE_NAPI_FUNCTION("setSize", JsFont::SetSize),
    DECLARE_NAPI_FUNCTION("setTypeface", JsFont::SetTypeface),
    DECLARE_NAPI_FUNCTION("getTypeface", JsFont::GetTypeface),
    DECLARE_NAPI_FUNCTION("getSize", JsFont::GetSize),
    DECLARE_NAPI_FUNCTION("getMetrics", JsFont::GetMetrics),
    DECLARE_NAPI_FUNCTION("getWidths", JsFont::GetWidths),
    DECLARE_NAPI_FUNCTION("measureSingleCharacter", JsFont::MeasureSingleCharacter),
    DECLARE_NAPI_FUNCTION("measureSingleCharacterWithFeatures", JsFont::MeasureSingleCharacterWithFeatures),
    DECLARE_NAPI_FUNCTION("measureText", JsFont::MeasureText),
    DECLARE_NAPI_FUNCTION("setScaleX", JsFont::SetScaleX),
    DECLARE_NAPI_FUNCTION("setSkewX", JsFont::SetSkewX),
    DECLARE_NAPI_FUNCTION("setEdging", JsFont::SetEdging),
    DECLARE_NAPI_FUNCTION("setHinting", JsFont::SetHinting),
    DECLARE_NAPI_FUNCTION("countText", JsFont::CountText),
    DECLARE_NAPI_FUNCTION("isBaselineSnap", JsFont::IsBaselineSnap),
    DECLARE_NAPI_FUNCTION("isEmbeddedBitmaps", JsFont::IsEmbeddedBitmaps),
    DECLARE_NAPI_FUNCTION("isForceAutoHinting", JsFont::IsForceAutoHinting),
    DECLARE_NAPI_FUNCTION("isSubpixel", JsFont::IsSubpixel),
    DECLARE_NAPI_FUNCTION("isLinearMetrics", JsFont::IsLinearMetrics),
    DECLARE_NAPI_FUNCTION("getSkewX", JsFont::GetSkewX),
    DECLARE_NAPI_FUNCTION("isEmbolden", JsFont::IsEmbolden),
    DECLARE_NAPI_FUNCTION("getScaleX", JsFont::GetScaleX),
    DECLARE_NAPI_FUNCTION("getHinting", JsFont::GetHinting),
    DECLARE_NAPI_FUNCTION("getEdging", JsFont::GetEdging),
    DECLARE_NAPI_FUNCTION("textToGlyphs", JsFont::TextToGlyphs),
    DECLARE_NAPI_FUNCTION("createPathForGlyph", JsFont::CreatePathForGlyph),
    DECLARE_NAPI_FUNCTION("getBounds", JsFont::GetBounds),
    DECLARE_NAPI_FUNCTION("getTextPath", JsFont::CreatePathForText),
    DECLARE_NAPI_FUNCTION("setThemeFontFollowed", JsFont::SetThemeFontFollowed),
    DECLARE_NAPI_FUNCTION("isThemeFontFollowed", JsFont::IsThemeFontFollowed),
};

bool JsFont::CreateConstructor(napi_env env)
{
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Font class");
        return false;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of constructor");
        return false;
    }
    return true;
}

napi_value JsFont::Init(napi_env env, napi_value exportObj)
{
    {
        std::lock_guard<std::mutex> lock(g_constructorInitMutex);
        if (!constructor_) {
            if (!CreateConstructor(env)) {
                ROSEN_LOGE("Failed to CreateConstructor");
                return nullptr;
            }
        }
    }

    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to get the representation of constructor object");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set constructor");
        return nullptr;
    }

    napi_property_descriptor staticProperty[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createFont", JsFont::CreateFont),
    };
    status = napi_define_properties(env, exportObj, 1, staticProperty);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

napi_value JsFont::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    std::shared_ptr<Font> font = std::make_shared<Font>();
    font->SetTypeface(JsTypeface::GetZhCnTypeface());
    JsFont *jsFont = new JsFont(font);

    status = napi_wrap_async_finalizer(env, jsThis, jsFont, JsFont::Destructor, nullptr, nullptr, 0);
    if (status != napi_ok) {
        delete jsFont;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsFont::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsFont *napi = reinterpret_cast<JsFont *>(nativeObject);
        delete napi;
    }
}

napi_value JsFont::CreatePathForText(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnCreatePathForText(env, info) : nullptr;
}

napi_value JsFont::CreateFont(napi_env env, napi_callback_info info)
{
    {
        std::lock_guard<std::mutex> lock(g_constructorInitMutex);
        if (!constructor_) {
            if (!CreateConstructor(env)) {
                ROSEN_LOGE("Failed to CreateConstructor");
                return nullptr;
            }
        }
    }

    napi_value result = nullptr;
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to get the representation of constructor object");
        return nullptr;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to instantiate JavaScript font instance");
        return nullptr;
    }
    return result;
}

JsFont::~JsFont()
{
    m_font = nullptr;
}

napi_value JsFont::EnableSubpixel(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableSubpixel(env, info) : nullptr;
}

napi_value JsFont::EnableEmbolden(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableEmbolden(env, info) : nullptr;
}

napi_value JsFont::EnableLinearMetrics(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableLinearMetrics(env, info) : nullptr;
}

napi_value JsFont::SetBaselineSnap(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetBaselineSnap(env, info) : nullptr;
}

napi_value JsFont::SetEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetEmbeddedBitmaps(env, info) : nullptr;
}

napi_value JsFont::SetForceAutoHinting(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetForceAutoHinting(env, info) : nullptr;
}

napi_value JsFont::SetSize(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetSize(env, info) : nullptr;
}

napi_value JsFont::SetTypeface(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetTypeface(env, info) : nullptr;
}

napi_value JsFont::GetTypeface(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetTypeface(env, info) : nullptr;
}

napi_value JsFont::GetSize(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetSize(env, info) : nullptr;
}

napi_value JsFont::GetMetrics(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetMetrics(env, info) : nullptr;
}

napi_value JsFont::GetWidths(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetWidths(env, info) : nullptr;
}

napi_value JsFont::GetBounds(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetBounds(env, info) : nullptr;
}

napi_value JsFont::IsBaselineSnap(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsBaselineSnap(env, info) : nullptr;
}

napi_value JsFont::IsEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsEmbeddedBitmaps(env, info) : nullptr;
}

napi_value JsFont::IsForceAutoHinting(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsForceAutoHinting(env, info) : nullptr;
}

napi_value JsFont::MeasureSingleCharacter(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnMeasureSingleCharacter(env, info) : nullptr;
}

napi_value JsFont::MeasureSingleCharacterWithFeatures(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnMeasureSingleCharacterWithFeatures(env, info) : nullptr;
}

napi_value JsFont::MeasureText(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnMeasureText(env, info) : nullptr;
}

napi_value JsFont::SetScaleX(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetScaleX(env, info) : nullptr;
}

napi_value JsFont::SetSkewX(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetSkewX(env, info) : nullptr;
}

napi_value JsFont::SetEdging(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetEdging(env, info) : nullptr;
}

napi_value JsFont::SetHinting(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetHinting(env, info) : nullptr;
}

napi_value JsFont::CountText(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnCountText(env, info) : nullptr;
}

napi_value JsFont::IsSubpixel(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsSubpixel(env, info) : nullptr;
}

napi_value JsFont::IsLinearMetrics(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsLinearMetrics(env, info) : nullptr;
}

napi_value JsFont::GetSkewX(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetSkewX(env, info) : nullptr;
}

napi_value JsFont::IsEmbolden(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsEmbolden(env, info) : nullptr;
}

napi_value JsFont::GetScaleX(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetScaleX(env, info) : nullptr;
}

napi_value JsFont::GetHinting(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetHinting(env, info) : nullptr;
}

napi_value JsFont::GetEdging(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetEdging(env, info) : nullptr;
}

napi_value JsFont::TextToGlyphs(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnTextToGlyphs(env, info) : nullptr;
}

napi_value JsFont::CreatePathForGlyph(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnCreatePathForGlyph(env, info) : nullptr;
}

napi_value JsFont::SetThemeFontFollowed(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetThemeFontFollowed(env, info) : nullptr;
}

napi_value JsFont::IsThemeFontFollowed(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsThemeFontFollowed(env, info) : nullptr;
}

napi_value JsFont::OnEnableSubpixel(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableSubpixel font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isSubpixel = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isSubpixel);

    m_font->SetSubpixel(isSubpixel);
    return nullptr;
}

napi_value JsFont::OnEnableEmbolden(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableEmbolden font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isEmbolden = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isEmbolden);

    m_font->SetEmbolden(isEmbolden);
    return nullptr;
}

napi_value JsFont::OnEnableLinearMetrics(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableLinearMetrics font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isLinearMetrics = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isLinearMetrics);

    m_font->SetLinearMetrics(isLinearMetrics);
    return nullptr;
}

napi_value JsFont::OnSetBaselineSnap(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetBaselineSnap font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool baselineSnap = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, baselineSnap);

    m_font->SetBaselineSnap(baselineSnap);
    return nullptr;
}

napi_value JsFont::OnIsBaselineSnap(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsBaselineSnap font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool baselineSnap = m_font->IsBaselineSnap();
    return CreateJsValue(env, baselineSnap);
}

napi_value JsFont::OnSetEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetEmbeddedBitmaps font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool embeddedBitmaps = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, embeddedBitmaps);

    m_font->SetEmbeddedBitmaps(embeddedBitmaps);
    return nullptr;
}

napi_value JsFont::OnIsEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsEmbeddedBitmaps font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool embeddedBitmaps = m_font->IsEmbeddedBitmaps();
    return CreateJsValue(env, embeddedBitmaps);
}

napi_value JsFont::OnSetForceAutoHinting(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetForceAutoHinting font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isForceAutoHinting = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isForceAutoHinting);

    m_font->SetForceAutoHinting(isForceAutoHinting);
    return nullptr;
}

napi_value JsFont::OnIsForceAutoHinting(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsForceAutoHinting font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool isForceAutoHinting = m_font->IsForceAutoHinting();
    return CreateJsValue(env, isForceAutoHinting);
}

napi_value JsFont::OnGetWidths(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetWidths font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    uint32_t fontSize = 0;
    napi_get_array_length(env, argv[ARGC_ZERO], &fontSize);
    if (fontSize == 0) {
        return nullptr;
    }

    std::unique_ptr<uint16_t[]> glyphPtr = std::make_unique<uint16_t[]>(fontSize);
    for (uint32_t i = 0; i < fontSize; i++) {
        napi_value tempglyph = nullptr;
        napi_get_element(env, argv[ARGC_ZERO], i, &tempglyph);
        uint32_t glyph_t = 0;
        bool isColorOk = ConvertFromJsValue(env, tempglyph, glyph_t);
        if (!isColorOk) {
            ROSEN_LOGE("JsFont::OnGetWidths Argv[ARGC_ZERO] is invalid");
            return nullptr;
        }
        glyphPtr[i] = glyph_t;
    }

    std::unique_ptr<float[]> widthPtr = std::make_unique<float[]>(fontSize);
    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    realFont->GetWidths(glyphPtr.get(), fontSize, widthPtr.get());
    napi_value widthJsArray;
    napi_status status = napi_create_array(env, &widthJsArray);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_create_array");
        return nullptr;
    }
    for (size_t i = 0; i < fontSize; i++) {
        napi_value element = CreateJsValue(env, widthPtr[i]);
        napi_set_element(env, widthJsArray, i, element);
    }
    return widthJsArray;
}

napi_value JsFont::OnSetSize(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetSize font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double textSize = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, textSize);

    m_font->SetSize((float)textSize);
    return nullptr;
}

napi_value JsFont::OnGetSize(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetSize font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double textSize = m_font->GetSize();
    return GetDoubleAndConvertToJsValue(env, textSize);
}

napi_value JsFont::OnGetMetrics(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetMetrics font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    FontMetrics metrics;
    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    realFont->GetMetrics(&metrics);
    return GetFontMetricsAndConvertToJsValue(env, &metrics);
}

napi_value JsFont::OnSetTypeface(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetTypeface font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsTypeface *jsTypeface = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsTypeface);

    m_font->SetTypeface(jsTypeface->GetTypeface());
    return nullptr;
}

napi_value JsFont::OnGetTypeface(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetTypeface font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::shared_ptr<Typeface> typeface = m_font->GetTypeface();
    return JsTypeface::CreateJsTypeface(env, typeface);
}

napi_value JsFont::OnGetBounds(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetBounds font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    uint32_t glyphscnt = UINT32_MAX;
    napi_status status = napi_get_array_length(env, argv[ARGC_ZERO], &glyphscnt);
    if (status != napi_ok || glyphscnt == 0) {
        ROSEN_LOGE("Failed to get size of glyph array %u", glyphscnt);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::unique_ptr<uint16_t[]> glyphPtr = std::make_unique<uint16_t[]>(glyphscnt);
    for (uint32_t i = 0; i < glyphscnt; i++) {
        napi_value tempglyph = nullptr;
        status = napi_get_element(env, argv[ARGC_ZERO], i, &tempglyph);
        if (status != napi_ok) {
            ROSEN_LOGE("Failed to get element of glyph array");
            return nullptr;
        }
        uint32_t glyph_t = 0;
        bool ret = ConvertFromJsValue(env, tempglyph, glyph_t);
        if (!ret) {
            ROSEN_LOGE("JsFont::OnGetBounds Argv[ARGC_ZERO] is invalid");
            return nullptr;
        }
        glyphPtr[i] = glyph_t;
    }
    std::unique_ptr<Rect[]> rectPtr = std::make_unique<Rect[]>(glyphscnt);
    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    realFont->GetWidths(glyphPtr.get(), glyphscnt, nullptr, rectPtr.get());
    napi_value rectJsArray;
    status = napi_create_array(env, &rectJsArray);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_create_array");
        return nullptr;
    }
    for (uint32_t i = 0; i < glyphscnt; i++) {
        napi_value element = GetRectAndConvertToJsValue(env, rectPtr[i]);
        status = napi_set_element(env, rectJsArray, i, element);
        if (status != napi_ok) {
            ROSEN_LOGE("failed to set array element");
            return nullptr;
        }
    }
    return rectJsArray;
}

napi_value JsFont::OnMeasureSingleCharacter(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnMeasureSingleCharacter font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    size_t len = 0;
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], nullptr, 0, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    if (len == 0 || len > 4) { // 4 is the maximum length of a character encoded in UTF8.
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. Input parameter0 should be single character.");
    }
    char str[len + 1];
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], str, len + 1, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }

    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    size_t byteLen = currentStr - str;
    if (byteLen != len) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. Input parameter0 should be single character.");
    }
    std::shared_ptr<Font> themeFont = MatchThemeFont(m_font, unicode);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    return GetDoubleAndConvertToJsValue(env, realFont->MeasureSingleCharacter(unicode));
}

napi_value JsFont::OnMeasureSingleCharacterWithFeatures(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnMeasureSingleCharacterWithFeatures font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    size_t len = 0;
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], nullptr, 0, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    if (len == 0 || len > 4) { // 4 is the maximum length of a character encoded in UTF8.
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. Input parameter0 should be single character.");
    }
    std::vector<char> strBuffer(len + 1);
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], strBuffer.data(), len + 1, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }

    strBuffer[len] = 0;
    const char* currentPtr = strBuffer.data();
    const char* endPtr = strBuffer.data() + len;
    int32_t unicode = SkUTF::NextUTF8(&currentPtr, endPtr);
    size_t byteLen = currentPtr - strBuffer.data();
    if (byteLen != len) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. Input parameter0 should be single character.");
    }

    std::shared_ptr<Font> themeFont = MatchThemeFont(m_font, unicode);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;

    napi_value array = argv[ARGC_ONE];
    uint32_t size = 0;
    if (napi_get_array_length(env, array, &size) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid src array");
    }
    std::shared_ptr<DrawingFontFeatures> featuresPtr = std::make_shared<DrawingFontFeatures>();
    MakeFontFeaturesFromJsArray(env, featuresPtr, size, array);
    return GetDoubleAndConvertToJsValue(env, realFont->MeasureSingleCharacterWithFeatures(strBuffer.data(),
        unicode, featuresPtr));
}

napi_value JsFont::OnMeasureText(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnMeasureText font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    std::string text = "";
    if (!ConvertFromJsValue(env, argv[ARGC_ZERO], text)) {
        ROSEN_LOGE("JsFont::OnMeasureText Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The text input must be string.");
    }

    TextEncoding TextEncoding = TextEncoding::UTF8;
    if (!ConvertFromJsTextEncoding(env, TextEncoding, argv[1])) {
        ROSEN_LOGE("JsFont::OnMeasureText ConvertFromJsTextEncoding failed");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The TextEncoding input must be valid.");
    }

    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    double textSize = realFont->MeasureText(text.c_str(), text.length(), TextEncoding);
    return GetDoubleAndConvertToJsValue(env, textSize);
}

napi_value JsFont::OnSetScaleX(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetScaleX font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double scaleX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, scaleX);

    JS_CALL_DRAWING_FUNC(m_font->SetScaleX(scaleX));
    return nullptr;
}

napi_value JsFont::OnSetSkewX(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetSkewX font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double skewX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, skewX);

    JS_CALL_DRAWING_FUNC(m_font->SetSkewX(skewX));
    return nullptr;
}

napi_value JsFont::OnSetEdging(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetEdging font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t fontEdging = 0;
    GET_ENUM_PARAM(ARGC_ZERO, fontEdging, 0, static_cast<int32_t>(FontEdging::SUBPIXEL_ANTI_ALIAS));

    JS_CALL_DRAWING_FUNC(m_font->SetEdging(static_cast<FontEdging>(fontEdging)));
    return nullptr;
}

napi_value JsFont::OnSetHinting(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetHinting font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t fontHinting = 0;
    GET_ENUM_PARAM(ARGC_ZERO, fontHinting, 0, static_cast<int32_t>(FontHinting::FULL));

    JS_CALL_DRAWING_FUNC(m_font->SetHinting(static_cast<FontHinting>(fontHinting)));
    return nullptr;
}

napi_value JsFont::OnCountText(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnCountText font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    std::string text = "";
    if (!ConvertFromJsValue(env, argv[ARGC_ZERO], text)) {
        ROSEN_LOGE("JsFont::OnCountText Argv[ARGC_ZERO] is invalid");
        return nullptr;
    }

    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    int32_t textSize = realFont->CountText(text.c_str(), text.length(), TextEncoding::UTF8);
    return CreateJsNumber(env, textSize);
}

napi_value JsFont::OnIsSubpixel(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsSubpixel font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool subpixel = m_font->IsSubpixel();
    return CreateJsValue(env, subpixel);
}

napi_value JsFont::OnIsLinearMetrics(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsLinearMetrics font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool linear = m_font->IsLinearMetrics();
    return CreateJsValue(env, linear);
}

napi_value JsFont::OnGetSkewX(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetSkewX font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double skewX = m_font->GetSkewX();
    return GetDoubleAndConvertToJsValue(env, skewX);
}

napi_value JsFont::OnIsEmbolden(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsEmbolden font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool fakeBoldText = m_font->IsEmbolden();
    return CreateJsValue(env, fakeBoldText);
}

napi_value JsFont::OnGetScaleX(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetScaleX font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double scaleX = m_font->GetScaleX();
    return GetDoubleAndConvertToJsValue(env, scaleX);
}

napi_value JsFont::OnGetHinting(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetHinting font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    FontHinting hinting = m_font->GetHinting();
    return CreateJsNumber(env, static_cast<uint32_t>(hinting));
}

napi_value JsFont::OnGetEdging(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetEdging font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    FontEdging edging = m_font->GetEdging();
    return CreateJsNumber(env, static_cast<uint32_t>(edging));
}

napi_value JsFont::OnTextToGlyphs(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnTextToGlyphs font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);

    std::string text = "";
    if (!ConvertFromJsValue(env, argv[ARGC_ZERO], text)) {
        ROSEN_LOGE("JsFont::OnTextToGlyphs Argv[ARGC_ZERO] is invalid");
        return nullptr;
    }

    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    uint32_t glyphCount = static_cast<uint32_t>(realFont->CountText(text.c_str(), text.length(), TextEncoding::UTF8));
    if (argc == ARGC_TWO) {
        uint32_t inputCount = 0;
        GET_UINT32_PARAM(ARGC_ONE, inputCount);
        if (glyphCount != inputCount) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        }
    }

    std::unique_ptr<uint16_t[]> glyphPtr = std::make_unique<uint16_t[]>(glyphCount);
    realFont->TextToGlyphs(text.c_str(), text.length(), TextEncoding::UTF8, glyphPtr.get(), glyphCount);

    napi_value glyphJsArray;
    napi_status status = napi_create_array(env, &glyphJsArray);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_create_array");
        return nullptr;
    }
    for (size_t i = 0; i < glyphCount; i++) {
        napi_value element = CreateJsValue(env, glyphPtr[i]);
        napi_set_element(env, glyphJsArray, i, element);
    }
    return glyphJsArray;
}

napi_value JsFont::OnCreatePathForGlyph(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnCreatePathForGlyph font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    uint32_t id = 0;
    GET_UINT32_PARAM(ARGC_ZERO, id);

    Path* path = new Path();
    if (path == nullptr) {
        ROSEN_LOGE("JsFont::OnCreatePathForGlyph Failed to create Path");
        return nullptr;
    }
    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    if  (!realFont->GetPathForGlyph(static_cast<uint16_t>(id), path)) {
        delete path;
        return nullptr;
    }
    return JsPath::CreateJsPath(env, path);
}

std::shared_ptr<Font> JsFont::GetFont()
{
    return m_font;
}

void JsFont::SetFont(std::shared_ptr<Font> font)
{
    m_font = font;
}

napi_value JsFont::OnCreatePathForText(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    std::string text = "";
    if (!ConvertFromJsValue(env, argv[ARGC_ZERO], text)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Failed to convert the first parameter");
    }

    uint32_t byteLength = 0;
    if (!ConvertFromJsNumber(env, argv[ARGC_ONE], byteLength)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Failed to convert the second parameter");
    }

    double x = 0.0;
    if (!ConvertFromJsNumber(env, argv[ARGC_TWO], x)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Failed to convert the third parameter");
    }

    double y = 0.0;
    if (!ConvertFromJsNumber(env, argv[ARGC_THREE], y)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Failed to convert the fourth parameter");
    }

    Path* path = new Path();
    std::shared_ptr<Font> themeFont = GetThemeFont(m_font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? m_font : themeFont;
    realFont->GetTextPath(text.c_str(), byteLength, TextEncoding::UTF8, x, y, path);
    return JsPath::CreateJsPath(env, path);
}

napi_value JsFont::OnSetThemeFontFollowed(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetThemeFontFollowed font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool followed = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, followed);

    m_font->SetThemeFontFollowed(followed);
    return nullptr;
}

napi_value JsFont::OnIsThemeFontFollowed(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsThemeFontFollowed font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool followed = m_font->IsThemeFontFollowed();
    return CreateJsValue(env, followed);
}
} // namespace Drawing
} // namespace OHOS::Rosen