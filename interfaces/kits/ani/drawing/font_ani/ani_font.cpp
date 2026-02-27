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

#include "ani_font.h"

#include "path_ani/ani_path.h"
#include "typeface_ani/ani_typeface.h"
#ifdef USE_M133_SKIA
#include "src/base/SkUTF.h"
#else
#include "src/utils/SkUTF.h"
#endif
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/font_napi/js_font.h"

namespace OHOS::Rosen {
namespace Drawing {

static std::string GetFontHintingItemName(FontHinting hinting)
{
    switch (hinting) {
        case FontHinting::NONE:
            return "NONE";
        case FontHinting::SLIGHT:
            return "SLIGHT";
        case FontHinting::NORMAL:
            return "NORMAL";
        case FontHinting::FULL:
            return "FULL";
        default:
            return "NONE";
    }
}

static std::string GetFontEdgingItemName(FontEdging edging)
{
    switch (edging) {
        case FontEdging::ALIAS:
            return "ALIAS";
        case FontEdging::ANTI_ALIAS:
            return "ANTI_ALIAS";
        case FontEdging::SUBPIXEL_ANTI_ALIAS:
            return "SUBPIXEL_ANTI_ALIAS";
        default:
            return "ALIAS";
    }
}

ani_object CreateAniFontMetrics(ani_env* env, const FontMetrics& fontMetrics)
{
    ani_object aniFontMetrics = CreateAniObject(env, AniGlobalClass::GetInstance().fontMetrics,
        AniGlobalMethod::GetInstance().fontMetricsCtor,
        ani_int(static_cast<int>(fontMetrics.fFlags)),
        ani_double(fontMetrics.fTop),
        ani_double(fontMetrics.fAscent),
        ani_double(fontMetrics.fDescent),
        ani_double(fontMetrics.fBottom),
        ani_double(fontMetrics.fLeading),
        ani_double(fontMetrics.fAvgCharWidth),
        ani_double(fontMetrics.fMaxCharWidth),
        ani_double(fontMetrics.fXMin),
        ani_double(fontMetrics.fXMax),
        ani_double(fontMetrics.fXHeight),
        ani_double(fontMetrics.fCapHeight),
        ani_double(fontMetrics.fUnderlineThickness),
        ani_double(fontMetrics.fUnderlinePosition),
        ani_double(fontMetrics.fStrikeoutThickness),
        ani_double(fontMetrics.fStrikeoutPosition)
    );
    if (IsUndefined(env, aniFontMetrics)) {
        ROSEN_LOGE("[ANI] create aniFontMetrics failed.");
        return aniFontMetrics;
    }
    return aniFontMetrics;
}

static const std::array g_methods = {
    ani_native_function { "constructorNative", ":",
        reinterpret_cast<void*>(AniFont::Constructor) },
    ani_native_function { "getMetrics", ":C{@ohos.graphics.drawing.drawing.FontMetrics}",
        reinterpret_cast<void*>(AniFont::GetMetrics) },
    ani_native_function { "getSize", ":d",
        reinterpret_cast<void*>(AniFont::GetSize) },
    ani_native_function { "getTypeface", nullptr, reinterpret_cast<void*>(AniFont::GetTypeface) },
    ani_native_function { "enableSubpixel", nullptr, reinterpret_cast<void*>(AniFont::EnableSubpixel) },
    ani_native_function { "enableEmbolden", nullptr, reinterpret_cast<void*>(AniFont::EnableEmbolden) },
    ani_native_function { "enableLinearMetrics", nullptr, reinterpret_cast<void*>(AniFont::EnableLinearMetrics) },
    ani_native_function { "setSize", "d:",
        reinterpret_cast<void*>(AniFont::SetSize) },
    ani_native_function { "setTypeface", "C{@ohos.graphics.drawing.drawing.Typeface}:",
        reinterpret_cast<void*>(AniFont::SetTypeface) },
    ani_native_function { "measureSingleCharacter", nullptr,
        reinterpret_cast<void*>(AniFont::MeasureSingleCharacter) },
    ani_native_function { "getWidths", nullptr, reinterpret_cast<void*>(AniFont::GetWidths) },
    ani_native_function { "textToGlyphs", nullptr, reinterpret_cast<void*>(AniFont::TextToGlyphs) },
    ani_native_function { "setBaselineSnap", nullptr, reinterpret_cast<void*>(AniFont::SetBaselineSnap) },
    ani_native_function { "isBaselineSnap", nullptr, reinterpret_cast<void*>(AniFont::IsBaselineSnap) },
    ani_native_function { "setEmbeddedBitmaps", nullptr, reinterpret_cast<void*>(AniFont::SetEmbeddedBitmaps) },
    ani_native_function { "getSkewX", nullptr, reinterpret_cast<void*>(AniFont::GetSkewX) },
    ani_native_function { "isEmbeddedBitmaps", nullptr, reinterpret_cast<void*>(AniFont::IsEmbeddedBitmaps) },
    ani_native_function { "setForceAutoHinting", nullptr, reinterpret_cast<void*>(AniFont::SetForceAutoHinting) },
    ani_native_function { "measureText", nullptr, reinterpret_cast<void*>(AniFont::MeasureText) },
    ani_native_function { "setScaleX", nullptr, reinterpret_cast<void*>(AniFont::SetScaleX) },
    ani_native_function { "isSubpixel", nullptr, reinterpret_cast<void*>(AniFont::IsSubpixel) },
    ani_native_function { "isLinearMetrics", nullptr, reinterpret_cast<void*>(AniFont::IsLinearMetrics) },
    ani_native_function { "isEmbolden", nullptr, reinterpret_cast<void*>(AniFont::IsEmbolden) },
    ani_native_function { "setHinting", nullptr, reinterpret_cast<void*>(AniFont::SetHinting) },
    ani_native_function { "countText", nullptr, reinterpret_cast<void*>(AniFont::CountText) },
    ani_native_function { "setSkewX", nullptr, reinterpret_cast<void*>(AniFont::SetSkewX) },
    ani_native_function { "setEdging", nullptr, reinterpret_cast<void*>(AniFont::SetEdging) },
    ani_native_function { "isForceAutoHinting", nullptr, reinterpret_cast<void*>(AniFont::IsForceAutoHinting) },
    ani_native_function { "getScaleX", nullptr, reinterpret_cast<void*>(AniFont::GetScaleX) },
    ani_native_function { "getHinting", nullptr, reinterpret_cast<void*>(AniFont::GetHinting) },
    ani_native_function { "getEdging", nullptr, reinterpret_cast<void*>(AniFont::GetEdging) },
    ani_native_function { "createPathForGlyph", nullptr, reinterpret_cast<void*>(AniFont::CreatePathForGlyph) },
    ani_native_function { "getBounds", nullptr, reinterpret_cast<void*>(AniFont::GetBounds) },
    ani_native_function { "getTextPath", nullptr, reinterpret_cast<void*>(AniFont::GetTextPath) },
    ani_native_function { "setThemeFontFollowed", nullptr, reinterpret_cast<void*>(AniFont::SetThemeFontFollowed) },
    ani_native_function { "isThemeFontFollowed", nullptr, reinterpret_cast<void*>(AniFont::IsThemeFontFollowed) },
    ani_native_function { "measureSingleCharacterWithFeatures", nullptr,
        reinterpret_cast<void*>(AniFont::MeasureSingleCharacterWithFeatures) },
};

ani_status AniFont::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().font;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_FONT_NAME);
        return ANI_NOT_FOUND;
    }
    ani_status ret = env->Class_BindNativeMethods(cls, g_methods.data(), g_methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_FONT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "fontTransferStaticNative", nullptr, reinterpret_cast<void*>(FontTransferStatic) },
        ani_native_function { "getFontAddr", nullptr, reinterpret_cast<void*>(GetFontAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_FONT_NAME);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniFont::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Font> font = std::make_shared<Font>();
    font->SetTypeface(AniTypeface::GetZhCnTypeface());
    AniFont* aniFont = new AniFont(font);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().fontNativeObj, reinterpret_cast<ani_long>(aniFont))) {
        ROSEN_LOGE("AniFont::Constructor failed create aniFont");
        delete aniFont;
        return;
    }
}

ani_object AniFont::GetMetrics(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetTypeface font is nullptr.");
        return ani_object{};
    }

    FontMetrics metrics;
    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    realFont->GetMetrics(&metrics);
    return CreateAniFontMetrics(env, metrics);
}

ani_double AniFont::GetSize(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetSize font is nullptr.");
        return -1;
    }

    return aniFont->GetFont()->GetSize();
}

ani_object AniFont::GetTypeface(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetMetrics font is nullptr.");
        return ani_object{};
    }

    std::shared_ptr<Typeface> typeface = aniFont->GetFont()->GetTypeface();
    AniTypeface* aniTypeface = new AniTypeface(typeface);
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().typeface,
        AniGlobalMethod::GetInstance().typefaceCtor);
    if (ANI_OK != env->Object_SetField_Long(aniObj,
        AniGlobalField::GetInstance().typefaceNativeObj, reinterpret_cast<ani_long>(aniTypeface))) {
        ROSEN_LOGE("AniFont::GetTypeface failed cause by Object_SetField_Long");
        delete aniTypeface;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniFont::EnableSubpixel(ani_env* env, ani_object obj, ani_boolean isSubpixel)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::EnableSubpixel font is nullptr.");
        return;
    }

    bool subpixel = static_cast<bool>(isSubpixel);
    aniFont->GetFont()->SetSubpixel(subpixel);
}

void AniFont::EnableEmbolden(ani_env* env, ani_object obj, ani_boolean isEmbolden)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::EnableEmbolden font is nullptr.");
        return;
    }

    bool embolden = static_cast<bool>(isEmbolden);
    aniFont->GetFont()->SetEmbolden(embolden);
}

void AniFont::EnableLinearMetrics(ani_env* env, ani_object obj, ani_boolean isLinearMetrics)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::EnableLinearMetrics font is nullptr.");
        return;
    }

    bool linearMetrics = static_cast<bool>(isLinearMetrics);
    aniFont->GetFont()->SetLinearMetrics(linearMetrics);
}

void AniFont::SetSize(ani_env* env, ani_object obj, ani_double size)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetSize font is nullptr.");
        return;
    }

    aniFont->GetFont()->SetSize(size);
}

void AniFont::SetTypeface(ani_env* env, ani_object obj, ani_object typeface)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetTypeface font is nullptr.");
        return;
    }

    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, typeface, AniGlobalField::GetInstance().typefaceNativeObj);
    if (aniTypeface == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetTypeface typeface is nullptr.");
        return;
    }

    aniFont->GetFont()->SetTypeface(aniTypeface->GetTypeface());
}

ani_double AniFont::MeasureSingleCharacterWithFeatures(ani_env* env, ani_object obj, ani_string text,
    ani_array featuresobj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureSingleCharacterWithFeatures font is nullptr.");
        return -1;
    }

    ani_size len = 0;
    ani_status status = env->String_GetUTF8Size(text, &len);
    if (status != ANI_OK || len == 0 || len > ARGC_FOUR) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureSingleCharacterWithFeatures text should be single character.");
        return -1;
    }

    char str[len + 1];
    ani_size realLen = 0;
    env->String_GetUTF8(text, str, len + 1, &realLen);
    str[realLen] = '\0';
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    size_t byteLen = currentStr - str;
    if (byteLen != len) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureSingleCharacterWithFeatures text should be single character.");
        return -1;
    }
    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = MatchThemeFont(font, unicode);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;

    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(featuresobj, &aniLength)) {
        ROSEN_LOGE("AniFont::MeasureSingleCharacterWithFeatures features are invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawPoints incorrect type points.");
        return -1;
    }
    uint32_t size = static_cast<uint32_t>(aniLength);

    std::shared_ptr<Drawing::DrawingFontFeatures> drawingFontFeatures =
        std::make_shared<Drawing::DrawingFontFeatures>();
    if (!MakeFontFeaturesFromAniObjArray(env, drawingFontFeatures, size, featuresobj)) {
        ROSEN_LOGE("AniFont::MeasureSingleCharacterWithFeatures MakeFontFeaturesFromAniObjArray is fail");
        return -1;
    }
    return realFont->MeasureSingleCharacterWithFeatures(str, unicode, drawingFontFeatures);
}

ani_double AniFont::MeasureSingleCharacter(ani_env* env, ani_object obj, ani_string text)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureSingleCharacter font is nullptr.");
        return -1;
    }

    ani_size len = 0;
    env->String_GetUTF8Size(text, &len);
    if (len == 0 || len > ARGC_FOUR) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureSingleCharacter text should be single character.");
        return -1;
    }

    char str[len + 1];
    ani_size realLen = 0;
    env->String_GetUTF8(text, str, len + 1, &realLen);
    str[realLen] = '\0';
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    size_t byteLen = currentStr - str;
    if (byteLen != len) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureSingleCharacter text should be single character.");
        return -1;
    }
    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = MatchThemeFont(font, unicode);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    return realFont->MeasureSingleCharacter(unicode);
}

ani_double AniFont::MeasureText(ani_env* env, ani_object obj, ani_string aniText, ani_enum_item encoding)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::MeasureText font is nullptr.");
        return -1;
    }

    ani_int textEncoding;
    if (ANI_OK != env->EnumItem_GetValue_Int(encoding, &textEncoding)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureText get textEncoding failed.");
        return -1;
    }
    if (CheckInt32OutOfRange(textEncoding, 0, static_cast<int32_t>(TextEncoding::GLYPH_ID))) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::MeasureText textEncoding is out of range.");
        return -1;
    }

    std::string text = CreateStdString(env, aniText);
    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    double textSize = realFont->MeasureText(text.c_str(), text.length(), static_cast<TextEncoding>(textEncoding));
    return textSize;
}

void AniFont::SetScaleX(ani_env* env, ani_object obj, ani_double scaleX)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetScaleX font is nullptr.");
        return;
    }

    aniFont->GetFont()->SetScaleX(scaleX);
}

void AniFont::SetBaselineSnap(ani_env* env, ani_object obj, ani_boolean isBaselineSnap)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetBaselineSnap font is nullptr.");
        return;
    }

    bool baselineSnap = static_cast<bool>(isBaselineSnap);
    aniFont->GetFont()->SetBaselineSnap(baselineSnap);
}

ani_boolean AniFont::IsBaselineSnap(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::IsBaselineSnap font is nullptr.");
        return ANI_FALSE;
    }

    bool baselineSnap = aniFont->GetFont()->IsBaselineSnap();
    return static_cast<ani_boolean>(baselineSnap);
}

void AniFont::SetEmbeddedBitmaps(ani_env* env, ani_object obj, ani_boolean isEmbeddedBitmaps)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetEmbeddedBitmaps font is nullptr.");
        return;
    }

    bool embeddedBitmaps = static_cast<bool>(isEmbeddedBitmaps);
    aniFont->GetFont()->SetEmbeddedBitmaps(embeddedBitmaps);
}

ani_boolean AniFont::IsEmbeddedBitmaps(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::IsEmbeddedBitmaps font is nullptr.");
        return ANI_FALSE;
    }

    bool isEmbeddedBitmaps = aniFont->GetFont()->IsEmbeddedBitmaps();
    return static_cast<ani_boolean>(isEmbeddedBitmaps);
}

void AniFont::SetForceAutoHinting(ani_env* env, ani_object obj, ani_boolean isForceAutoHinting)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::SetForceAutoHinting font is nullptr.");
        return;
    }

    bool forceAutoHinting = static_cast<bool>(isForceAutoHinting);
    aniFont->GetFont()->SetForceAutoHinting(forceAutoHinting);
}

ani_object AniFont::GetWidths(ani_env* env, ani_object obj, ani_array glyphs)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetWidths font is nullptr.");
        return CreateAniUndefined(env);
    }

    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(glyphs, &aniLength) || aniLength == 0) {
        ROSEN_LOGE("AniFont::GetWidths Failed to get size of glyph array");
        return CreateAniUndefined(env);
    }
    uint32_t fontSize = static_cast<uint32_t>(aniLength);
    std::unique_ptr<uint16_t[]> glyphPtr = std::make_unique<uint16_t[]>(fontSize);
    for (uint32_t i = 0; i < fontSize; i++) {
        ani_int glyph;
        ani_ref glyphRef;
        if (ANI_OK != env->Array_Get(glyphs, static_cast<ani_size>(i), &glyphRef) ||
            ANI_OK != env->Object_CallMethod_Int(
                static_cast<ani_object>(glyphRef), AniGlobalMethod::GetInstance().intGet, &glyph)) {
            ROSEN_LOGE("AniFont::GetWidths Incorrect parameter glyph type.");
            return CreateAniUndefined(env);
        }
        glyphPtr[i] = glyph;
    }

    std::shared_ptr<Font> font = aniFont->GetFont();
    std::unique_ptr<float[]> widthPtr = std::make_unique<float[]>(fontSize);
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    realFont->GetWidths(glyphPtr.get(), fontSize, widthPtr.get());
    ani_array arrayObj = CreateAniArrayWithSize(env, fontSize);
    if (arrayObj == nullptr) {
        return CreateAniUndefined(env);
    }

    for (uint32_t i = 0; i < fontSize; i++) {
        ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().doubleCls,
            AniGlobalMethod::GetInstance().doubleCtor, widthPtr[i]);
        if (IsUndefined(env, aniObj)) {
            ROSEN_LOGE("AniFont::GetWidths Failed to create width item");
            return aniObj;
        }
        ani_status ret = env->Array_Set(arrayObj, static_cast<ani_size>(i), aniObj);
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniFont::GetWidths Failed to set width item");
            return CreateAniUndefined(env);
        }
    }
    return arrayObj;
}

ani_object AniFont::TextToGlyphs(ani_env* env, ani_object obj, ani_string aniText, ani_object glyphCount)
{
    std::string text = CreateStdString(env, aniText);
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::TextToGlyphs font is nullptr.");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    uint32_t count = static_cast<uint32_t>(realFont->CountText(text.c_str(), text.length(), TextEncoding::UTF8));

    if (!IsUndefined(env, glyphCount)) {
        ani_int aniGlyphCount;
        if (ANI_OK != env->Object_CallMethod_Int(glyphCount, AniGlobalMethod::GetInstance().intGet, &aniGlyphCount)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniFont::TextToGlyphs incorrect type glyph.");
            return CreateAniUndefined(env);
        }
        if (count != static_cast<uint32_t>(aniGlyphCount)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::TextToGlyphs Invalid params.");
            return CreateAniUndefined(env);
        }
    }
    std::unique_ptr<uint16_t[]> glyphPtr = std::make_unique<uint16_t[]>(count);
    realFont->TextToGlyphs(text.c_str(), text.length(), TextEncoding::UTF8, glyphPtr.get(), count);

    ani_array arrayObj = CreateAniArrayWithSize(env, count);
    if (arrayObj == nullptr) {
        ROSEN_LOGE("AniFont::TextToGlyphs Failed to Create arrayObject");
        return CreateAniUndefined(env);
    }

    for (uint32_t i = 0; i < count; i++) {
        ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().intCls,
            AniGlobalMethod::GetInstance().intCtor, glyphPtr[i]);
        if (IsUndefined(env, aniObj)) {
            ROSEN_LOGE("AniFont::TextToGlyphs Failed to create Int object");
            return aniObj;
        }
        ani_status ret = env->Array_Set(arrayObj, static_cast<ani_size>(i), aniObj);
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniFont::TextToGlyphs Failed to set width item");
            return CreateAniUndefined(env);
        }
    }
    return arrayObj;
}

ani_boolean AniFont::IsSubpixel(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::IsSubpixel font is nullptr.");
        return ANI_FALSE;
    }

    bool isSubpixel = aniFont->GetFont()->IsSubpixel();
    return static_cast<ani_boolean>(isSubpixel);
}

ani_boolean AniFont::IsLinearMetrics(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::IsLinearMetrics font is nullptr.");
        return ANI_FALSE;
    }

    bool isLinearMetrics = aniFont->GetFont()->IsLinearMetrics();
    return static_cast<ani_boolean>(isLinearMetrics);
}

ani_double AniFont::GetSkewX(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetSkewX font is nullptr.");
        return -1;
    }

    double skewX = aniFont->GetFont()->GetSkewX();
    return skewX;
}

ani_boolean AniFont::IsEmbolden(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::IsEmbolden font is nullptr.");
        return ANI_FALSE;
    }

    bool isEmbolden = aniFont->GetFont()->IsEmbolden();
    return static_cast<ani_boolean>(isEmbolden);
}

void AniFont::SetHinting(ani_env* env, ani_object obj, ani_enum_item hinting)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetHinting font is nullptr.");
        return;
    }

    ani_int fontHinting;
    if (ANI_OK != env->EnumItem_GetValue_Int(hinting, &fontHinting)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::SetHinting incorrect type fontHinting.");
        return;
    }
    if (CheckInt32OutOfRange(fontHinting, 0, static_cast<int32_t>(FontHinting::FULL))) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::SetHinting fontHinting is out of range.");
        return;
    }
    aniFont->GetFont()->SetHinting(static_cast<FontHinting>(fontHinting));
}

ani_int AniFont::CountText(ani_env* env, ani_object obj, ani_string aniText)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::CountText font is nullptr.");
        return -1;
    }

    std::string text = CreateStdString(env, aniText);
    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    int32_t textSize = realFont->CountText(text.c_str(), text.length(), TextEncoding::UTF8);
    return textSize;
}

void AniFont::SetSkewX(ani_env* env, ani_object obj, ani_double skewX)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetSkewX font is nullptr.");
        return;
    }

    aniFont->GetFont()->SetSkewX(skewX);
}

void AniFont::SetEdging(ani_env* env, ani_object obj, ani_enum_item edging)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::SetEdging font is nullptr.");
        return;
    }

    ani_int fontEdging;
    if (ANI_OK != env->EnumItem_GetValue_Int(edging, &fontEdging)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::SetEdging incorrect type fontEdging.");
        return;
    }
    if (CheckInt32OutOfRange(fontEdging, 0, static_cast<int32_t>(FontEdging::SUBPIXEL_ANTI_ALIAS))) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::SetEdging fontEdging is out of range.");
        return;
    }
    aniFont->GetFont()->SetEdging(static_cast<FontEdging>(fontEdging));
}

ani_boolean AniFont::IsForceAutoHinting(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::IsForceAutoHinting font is nullptr.");
        return ANI_FALSE;
    }

    bool isForceAutoHinting = aniFont->GetFont()->IsForceAutoHinting();
    return static_cast<ani_boolean>(isForceAutoHinting);
}

ani_double AniFont::GetScaleX(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetScaleX font is nullptr.");
        return -1;
    }

    double scaleX = aniFont->GetFont()->GetScaleX();
    return scaleX;
}

ani_enum_item AniFont::GetHinting(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetHinting font is nullptr.");
        return {};
    }
    FontHinting hinting = aniFont->GetFont()->GetHinting();
    std::string itemName = GetFontHintingItemName(hinting);
    ani_enum_item enumItem;
    if (!CreateAniEnumByEnumName(env, AniGlobalEnum::GetInstance().fontHinting, itemName, enumItem)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Failed to obtain the FontHinting enumeration.");
        return {};
    }
    return enumItem;
}

ani_enum_item AniFont::GetEdging(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetEdging font is nullptr.");
        return {};
    }
    FontEdging edging = aniFont->GetFont()->GetEdging();
    std::string itemName = GetFontEdgingItemName(edging);
    ani_enum_item enumItem;
    if (!CreateAniEnumByEnumName(env, AniGlobalEnum::GetInstance().fontEdging, itemName, enumItem)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Failed to obtain the FontEdging enumeration.");
        return {};
    }

    return enumItem;
}

ani_object AniFont::CreatePathForGlyph(ani_env* env, ani_object obj, ani_int index)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::CreatePathForGlyph font is nullptr.");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    std::shared_ptr<Path> path = std::make_shared<Path>();
    if (!realFont->GetPathForGlyph(static_cast<uint16_t>(index), path.get())) {
        ROSEN_LOGE("AniFont::CreatePathForGlyph Failed GetPathForGlyph");
        return CreateAniUndefined(env);
    }
    AniPath* aniPath = new AniPath(path);
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().path,
        AniGlobalMethod::GetInstance().pathCtor);
    if (ANI_OK != env->Object_SetField_Long(aniObj,
        AniGlobalField::GetInstance().pathNativeObj, reinterpret_cast<ani_long>(aniPath))) {
        ROSEN_LOGE("AniFont::CreatePathForGlyph failed cause by Object_SetField_Long");
        delete aniPath;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

ani_object AniFont::GetBounds(ani_env* env, ani_object obj, ani_array glyphs)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetBounds font is nullptr.");
        return CreateAniUndefined(env);
    }

    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(glyphs, &aniLength) || aniLength == 0) {
        ROSEN_LOGE("AniFont::GetBounds Failed to get size of glyph array %{public}zu", aniLength);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetBounds Invalid params.");
        return CreateAniUndefined(env);
    }
    
    uint32_t glyphscnt = static_cast<uint32_t>(aniLength);
    std::unique_ptr<uint16_t[]> glyphPtr = std::make_unique<uint16_t[]>(glyphscnt);
    for (uint32_t i = 0; i < glyphscnt; i++) {
        ani_int glyph;
        ani_ref glyphRef;
        if (ANI_OK != env->Array_Get(glyphs, static_cast<ani_size>(i), &glyphRef) ||
            ANI_OK != env->Object_CallMethod_Int(
                static_cast<ani_object>(glyphRef), AniGlobalMethod::GetInstance().intGet, &glyph)) {
            ROSEN_LOGE("AniFont::GetBounds Incorrect parameter glyph type.");
            return CreateAniUndefined(env);
        }
        glyphPtr[i] = glyph;
    }

    std::shared_ptr<Font> font = aniFont->GetFont();
    std::unique_ptr<Rect[]> rectPtr = std::make_unique<Rect[]>(glyphscnt);
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    realFont->GetWidths(glyphPtr.get(), glyphscnt, nullptr, rectPtr.get());
    ani_array arrayObj = CreateAniArrayWithSize(env, glyphscnt);
    if (arrayObj == nullptr) {
        ROSEN_LOGE("AniFont::GetBounds Failed to Create arrayObject");
        return CreateAniUndefined(env);
    }
    for (uint32_t i = 0; i < glyphscnt; i++) {
        ani_object aniObj;
        CreateRectObj(env, rectPtr[i], aniObj);
        ani_status ret = env->Array_Set(arrayObj, static_cast<ani_size>(i), aniObj);
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniFont::GetBounds Failed to set rect item");
            return CreateAniUndefined(env);
        }
    }
    return arrayObj;
}

ani_object AniFont::GetTextPath(ani_env* env, ani_object obj, ani_string aniText,
    ani_int byteLength, ani_double x, ani_double y)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniFont::GetTextPath font is nullptr.");
        return CreateAniUndefined(env);
    }

    std::string text = CreateStdString(env, aniText);
    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    std::shared_ptr<Font> realFont = themeFont == nullptr ? font : themeFont;
    std::shared_ptr<Path> path = std::make_shared<Path>();
    realFont->GetTextPath(text.c_str(), byteLength, TextEncoding::UTF8, x, y, path.get());
    AniPath* aniPath = new AniPath(path);
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().path,
        AniGlobalMethod::GetInstance().pathCtor);
    if (ANI_OK != env->Object_SetField_Long(aniObj,
        AniGlobalField::GetInstance().pathNativeObj, reinterpret_cast<ani_long>(aniPath))) {
        ROSEN_LOGE("AniFont::GetTextPath failed cause by Object_SetField_Long");
        delete aniPath;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniFont::SetThemeFontFollowed(ani_env* env, ani_object obj, ani_boolean followed)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::SetThemeFontFollowed font is nullptr.");
        return;
    }

    bool isFollowed = static_cast<bool>(followed);
    aniFont->GetFont()->SetThemeFontFollowed(isFollowed);
}

ani_boolean AniFont::IsThemeFontFollowed(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniFont::IsThemeFontFollowed font is nullptr.");
        return ANI_FALSE;
    }

    bool isFollowed = aniFont->GetFont()->IsThemeFontFollowed();
    return static_cast<ani_boolean>(isFollowed);
}


ani_object AniFont::FontTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniFont::FontTransferStatic failed to unwrap");
        return CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniFont::FontTransferStatic unwrapResult is null");
        return CreateAniUndefined(env);
    }
    auto jsFont = reinterpret_cast<JsFont*>(unwrapResult);
    if (jsFont->GetFont() == nullptr) {
        ROSEN_LOGE("AniFont::FontTransferStatic jsFont is null");
        return CreateAniUndefined(env);
    }

    auto aniFont = new AniFont(jsFont->GetFont());
    if (ANI_OK != env->Object_SetField_Long(
        output, AniGlobalField::GetInstance().fontNativeObj, reinterpret_cast<ani_long>(aniFont))) {
        ROSEN_LOGE("AniFont::FontTransferStatic failed create aniFont");
        delete aniFont;
        return CreateAniUndefined(env);
    }
    return output;
}

ani_long AniFont::GetFontAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, input, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ROSEN_LOGE("AniFont::GetFontAddr aniFont is null");
        return 0;
    }

    return reinterpret_cast<ani_long>(aniFont->GetFontPtrAddr());
}

std::shared_ptr<Font> AniFont::GetFont()
{
    return font_;
}

std::shared_ptr<Font>* AniFont::GetFontPtrAddr()
{
    return &font_;
}

AniFont::~AniFont()
{
    font_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen
