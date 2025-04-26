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

#include "ani_paragraph.h"

#include <codecvt>
#include <cstdint>

#include "ani.h"
#include "ani_common.h"
#include "ani_text_utils.h"
#include "draw/canvas.h"
#include "font_collection.h"
#include "typography_create.h"
#include "text/font_metrics.h"
#include "utils/text_log.h"

namespace OHOS::Text::NAI {

AniParagraph::AniParagraph()
{
}

void AniParagraph::setTypography(std::unique_ptr<OHOS::Rosen::Typography>& typography)
{
    paragraph_ = std::move(typography);
}

void AniParagraph::Constructor(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph = new AniParagraph();
    if (ANI_OK != env->Object_SetFieldByName_Long(object, "nativeObj", reinterpret_cast<ani_long>(aniParagraph))) {
        TEXT_LOGE("Failed to create ani Paragraph obj");
        return;
    }
}

ani_status AniParagraph::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    ani_status ret;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraph null env:%{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraph can't find class:%{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::string paintSignature = std::string(ANI_CLASS_CANVAS) + "DD:V";
    std::string paintOnPathSignature = std::string(ANI_CLASS_CANVAS) + std::string(ANI_CLASS_PATH) + "DD:V";
    std::array methods = {
        ani_native_function{"constructorNative", ":V", reinterpret_cast<void*>(Constructor)},
        ani_native_function{"layoutSync", "D:V", reinterpret_cast<void*>(LayoutSync)},
        ani_native_function{"paint", paintSignature.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{"paintOnPath", paintOnPathSignature.c_str(), reinterpret_cast<void*>(PaintOnPath)},
        ani_native_function{"getLongestLine", ":D", reinterpret_cast<void*>(GetLongestLine)},
        ani_native_function{"getLineMetrics", ":Lescompat/Array;", reinterpret_cast<void*>(GetLineMetrics)},
        ani_native_function{"nativeGetLineMetricsAt", "D:L@ohos/graphics/text/text/LineMetrics;",
                            reinterpret_cast<void*>(GetLineMetricsAt)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraph bind methods fail::%{public}d", ret);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniParagraph::LayoutSync(ani_env* env, ani_object object, ani_double width)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        return;
    }
    aniParagraph->paragraph_->Layout(width);
}

void AniParagraph::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        return;
    }
    Drawing::Canvas* canvasI = nullptr;
    aniParagraph->paragraph_->Paint(canvasI, x, y);
}

void AniParagraph::PaintOnPath(ani_env* env, ani_object object, ani_object canvas, ani_object path, ani_double hOffset,
                               ani_double vOffset)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        return;
    }
    Drawing::Canvas* canvasI = nullptr;
    Drawing::Path* pathI = nullptr;
    aniParagraph->paragraph_->Paint(canvasI, pathI, hOffset, vOffset);
}

ani_double AniParagraph::GetLongestLine(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        return 0;
    }
    return aniParagraph->paragraph_->GetActualWidth();
}

ani_object AniConvertTextStyle(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXT_STYLE_I, ":V");
    //env->Object_SetPropertyByName_Ref(aniObj, "decoration", ani_double(textStyle));
    env->Object_SetPropertyByName_Double(aniObj, "color", ani_int(textStyle.color.CastToColorQuad()));
    env->Object_SetPropertyByName_Int(aniObj, "fontWeight", ani_size(textStyle.fontWeight));
    env->Object_SetPropertyByName_Double(aniObj, "fontStyle", ani_double(textStyle.fontStyle));
    env->Object_SetPropertyByName_Double(aniObj, "baseline", ani_double(textStyle.baseline));
    //env->Object_SetPropertyByName_Ref(aniObj, "fontFamilies", ani_double(textStyle.fontFamilies));
    env->Object_SetPropertyByName_Double(aniObj, "fontSize", ani_double(textStyle.fontSize));
    env->Object_SetPropertyByName_Double(aniObj, "letterSpacing", ani_double(textStyle.letterSpacing));
    env->Object_SetPropertyByName_Double(aniObj, "wordSpacing", ani_double(textStyle.wordSpacing));
    env->Object_SetPropertyByName_Double(aniObj, "heightScale", ani_double(textStyle.heightScale));
    env->Object_SetPropertyByName_Double(aniObj, "halfLeading", ani_double(textStyle.halfLeading));
    env->Object_SetPropertyByName_Double(aniObj, "heightOnly", ani_double(textStyle.heightOnly));
    //env->Object_SetPropertyByName_Ref(aniObj, "ellipsis", ani_double(textStyle.ellipsis));
    env->Object_SetPropertyByName_Double(aniObj, "ellipsisMode", (static_cast<ani_int>(textStyle.ellipsisModal)));
    //env->Object_SetPropertyByName_Ref(aniObj, "locale", ani_double(textStyle.locale));
    env->Object_SetPropertyByName_Double(aniObj, "baselineShift", ani_double(textStyle.baseLineShift));
    //env->Object_SetPropertyByName_Ref(aniObj, "backgroundRect", ani_double(textStyle.backgroundRect));
    //env->Object_SetPropertyByName_Ref(aniObj, "textShadows", ani_double(textStyle.shadows));
    //env->Object_SetPropertyByName_Ref(aniObj, "fontFeatures", ani_double(textStyle.fontFeatures));
    return aniObj;
}

ani_object AniConvertFontMetrics(ani_env* env, const Drawing::FontMetrics& fontMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_METRICS, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "flags", ani_int(fontMetrics.fFlags));
    env->Object_SetPropertyByName_Double(aniObj, "top", ani_double(fontMetrics.fTop));
    env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_double(fontMetrics.fAscent));
    env->Object_SetPropertyByName_Double(aniObj, "descent", ani_double(fontMetrics.fDescent));
    env->Object_SetPropertyByName_Double(aniObj, "bottom", ani_double(fontMetrics.fBottom));
    env->Object_SetPropertyByName_Double(aniObj, "leading", ani_double(fontMetrics.fLeading));
    env->Object_SetPropertyByName_Double(aniObj, "avgCharWidth", ani_double(fontMetrics.fAvgCharWidth));
    env->Object_SetPropertyByName_Double(aniObj, "maxCharWidth", ani_double(fontMetrics.fMaxCharWidth));
    env->Object_SetPropertyByName_Double(aniObj, "xMin", ani_double(fontMetrics.fXMin));
    env->Object_SetPropertyByName_Double(aniObj, "xMax", ani_double(fontMetrics.fXMax));
    env->Object_SetPropertyByName_Double(aniObj, "xHeight", ani_double(fontMetrics.fXHeight));
    env->Object_SetPropertyByName_Double(aniObj, "capHeight", ani_double(fontMetrics.fCapHeight));
    env->Object_SetPropertyByName_Double(aniObj, "underlineThickness", ani_double(fontMetrics.fUnderlineThickness));
    env->Object_SetPropertyByName_Double(aniObj, "underlinePosition", ani_double(fontMetrics.fUnderlinePosition));
    env->Object_SetPropertyByName_Double(aniObj, "strikethroughThickness", ani_double(fontMetrics.fStrikeoutThickness));
    env->Object_SetPropertyByName_Double(aniObj, "strikethroughPosition", ani_double(fontMetrics.fStrikeoutPosition));
    return aniObj;
}

ani_object AniConvertRunMetrics(ani_env* env, const std::map<size_t, RunMetrics>& map)
{
    ani_object mapAniObj = AniTextUtils::CreateAniMap(env);
    ani_ref mapRef = nullptr;
    for (const auto& [key, runMetrics] : map) {
        ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RUNMETRICS_I, ":V");
        if (runMetrics.textStyle != nullptr) {
            env->Object_SetPropertyByName_Ref(aniObj, "textStyle", AniConvertTextStyle(env, *runMetrics.textStyle));
        }
        //env->Object_SetPropertyByName_Ref(aniObj, "fontMetrics", AniConvertFontMetrics(env, runMetrics.fontMetrics));
        if (ANI_OK
            != env->Object_CallMethodByName_Ref(
                mapAniObj, "set", "Lstd/core/Object;Lstd/core/Object;:Lescompat/Map;", &mapRef,
                AniTextUtils::CreateAniDoubleObj(env, static_cast<ani_double>(key)), aniObj)) {
            TEXT_LOGE("Object_CallMethodByName_Ref set failed");
            break;
        };
    }
    return mapAniObj;
}

ani_object AniConvertLineMetrics(ani_env* env, const LineMetrics& lineMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_LINEMETRICS_I, ":V");
    env->Object_SetPropertyByName_Int(aniObj, "startIndex", ani_int(lineMetrics.startIndex));
    env->Object_SetPropertyByName_Int(aniObj, "endIndex", ani_int(lineMetrics.endIndex));
    env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_double(lineMetrics.ascender));
    env->Object_SetPropertyByName_Double(aniObj, "descent", ani_double(lineMetrics.descender));
    env->Object_SetPropertyByName_Double(aniObj, "height", ani_double(lineMetrics.height));
    env->Object_SetPropertyByName_Double(aniObj, "width", ani_double(lineMetrics.width));
    env->Object_SetPropertyByName_Double(aniObj, "left", ani_double(lineMetrics.x));
    env->Object_SetPropertyByName_Double(aniObj, "baseline", ani_double(lineMetrics.baseline));
    env->Object_SetPropertyByName_Int(aniObj, "lineNumber", ani_int(lineMetrics.lineNumber));
    env->Object_SetPropertyByName_Double(aniObj, "topHeight", ani_double(lineMetrics.y));
    env->Object_SetPropertyByName_Ref(aniObj, "runMetrics", AniConvertRunMetrics(env, lineMetrics.runMetrics));
    return aniObj;
}

ani_ref AniParagraph::GetLineMetrics(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return arrayObj;
    }
    std::vector<LineMetrics> vectorLineMetrics = aniParagraph->paragraph_->GetLineMetrics();
    arrayObj = AniTextUtils::CreateAniArray(env, vectorLineMetrics.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Create arrayObject failed");
        return arrayObj;
    }
    ani_size index = 0;
    for (auto lineMetrics : vectorLineMetrics) {
        ani_object aniObj = AniConvertLineMetrics(env, lineMetrics);
        if (ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Object_CallMethodByName_Void $_set failed");
            break;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetLineMetricsAt(ani_env* env, ani_object object, ani_double lineNumber)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return AniTextUtils::CreateAniUndefined(env);
    }
    LineMetrics lineMetrics;
    if (!aniParagraph->paragraph_->GetLineMetricsAt(lineNumber, &lineMetrics)) {
        TEXT_LOGE("Failed to get line metrics");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return AniConvertLineMetrics(env, lineMetrics);
}
} // namespace OHOS::Text::NAI