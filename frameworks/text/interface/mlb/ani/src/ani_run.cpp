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

#include <memory>
#include <stdint.h>

#include "ani.h"
#include "ani_common.h"
#include "ani_drawing_converter.h"
#include "ani_run.h"
#include "ani_text_rect_converter.h"
#include "ani_text_utils.h"
#include "ani_typographic_bounds_converter.h"
#include "canvas_ani/ani_canvas.h"
#include "font_ani/ani_font.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

constexpr const char* NATIVE_PARAGRAPH_OBJ = "nativeParagraphObj";

ani_status AniRun::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env{nullptr};
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_RUN, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }
    std::string paintSignature = std::string(ANI_CLASS_CANVAS) + "DD:V";
    std::string nativeGetGlyphsSignature = std::string(ANI_INTERFACE_RANGE) + ":" + std::string(ANI_ARRAY);
    std::string nativeGetPositionsSignature = std::string(ANI_INTERFACE_RANGE) + ":" + std::string(ANI_ARRAY);
    std::string getOffsetsSignature = ":" + std::string(ANI_ARRAY);
    std::string getFontSignature = ":" + std::string(ANI_CLASS_FONT);
    std::string getStringIndicesSignature = std::string(ANI_INTERFACE_RANGE) + ":" + std::string(ANI_ARRAY);
    std::string getStringRangeSignature = ":" + std::string(ANI_INTERFACE_RANGE);
    std::string getTypographicBoundsSignature = ":" + std::string(ANI_INTERFACE_TYPOGRAPHIC_BOUNDS);
    std::string getImageBoundsSignature = ":" + std::string(ANI_INTERFACE_RECT);
    std::array methods = {
        ani_native_function{"getGlyphCount", ":D", reinterpret_cast<void*>(GetGlyphCount)},
        ani_native_function{"getGlyphs", (":" + std::string(ANI_ARRAY)).c_str(), reinterpret_cast<void*>(GetGlyphs)},
        ani_native_function{
            "nativeGetGlyphs", nativeGetGlyphsSignature.c_str(), reinterpret_cast<void*>(GetGlyphsByRange)},
        ani_native_function{
            "getPositions", (":" + std::string(ANI_ARRAY)).c_str(), reinterpret_cast<void*>(GetPositions)},
        ani_native_function{
            "nativeGetPositions", nativeGetPositionsSignature.c_str(), reinterpret_cast<void*>(GetPositionsByRange)},
        ani_native_function{"getOffsets", getOffsetsSignature.c_str(), reinterpret_cast<void*>(GetOffsets)},
        ani_native_function{"getFont", getFontSignature.c_str(), reinterpret_cast<void*>(GetFont)},
        ani_native_function{"paint", paintSignature.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{
            "getStringIndices", getStringIndicesSignature.c_str(), reinterpret_cast<void*>(GetStringIndices)},
        ani_native_function{"getStringRange", getStringRangeSignature.c_str(), reinterpret_cast<void*>(GetStringRange)},
        ani_native_function{"getTypographicBounds", getTypographicBoundsSignature.c_str(),
            reinterpret_cast<void*>(GetTypographicBounds)},
        ani_native_function{"getImageBounds", getImageBoundsSignature.c_str(), reinterpret_cast<void*>(GetImageBounds)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for Run, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

ani_object AniRun::CreateRun(ani_env* env, std::unique_ptr<Rosen::Run>& run)
{
    ani_object runObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RUN, ":V");
    Run* runPtr = run.release();
    ani_status ret = env->Object_SetFieldByName_Long(runObj, NATIVE_OBJ, reinterpret_cast<ani_long>(runPtr));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to set type set run");
        delete runPtr;
        runPtr = nullptr;
    }
    return runObj;
}

void AniRun::SetParagraph(ani_env* env, ani_object textLine, std::unique_ptr<Rosen::Typography>& paragraph)
{
    Typography* typographyPtr = paragraph.release();
    ani_status ret =
        env->Object_SetFieldByName_Long(textLine, NATIVE_PARAGRAPH_OBJ, reinterpret_cast<ani_long>(typographyPtr));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to set type set paragraph");
        delete typographyPtr;
        typographyPtr = nullptr;
    }
}

ani_double AniRun::GetGlyphCount(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return run->GetGlyphCount();
}

ani_object AniRun::GetGlyphs(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    std::vector<uint16_t> glyphs = run->GetGlyphs();

    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, glyphs.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to Create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& glpyh : glyphs) {
        ani_status ret = env->Object_CallMethodByName_Void(
            arrayObj, "$_set", "ILstd/core/Object;:V", index, AniTextUtils::CreateAniDoubleObj(env, glpyh));
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set glyphs item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetGlyphsByRange(ani_env* env, ani_object object, ani_object range)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    OHOS::Text::ANI::RectRange rectRange;
    ani_status ret =AniTextRectConverter::ParseRangeToNative(env, range, rectRange);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse range, ani_status: %{public}d", ret);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<uint16_t> glyphs = run->GetGlyphs(rectRange.start, rectRange.end);
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, glyphs.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to Create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& glpyh : glyphs) {
        ret = env->Object_CallMethodByName_Void(
            arrayObj, "$_set", "ILstd/core/Object;:V", index, AniTextUtils::CreateAniDoubleObj(env, glpyh));
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set glyphs item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetPositions(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    std::vector<Drawing::Point> points = run->GetPositions();

    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, points.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to Create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& point : points) {
        ani_object aniObj{nullptr};
        ani_status status = AniDrawingConverter::ParsePointToAni(env, point, aniObj);
        if (ANI_OK != status
            || ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Failed to set points item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetPositionsByRange(ani_env* env, ani_object object, ani_object range)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    OHOS::Text::ANI::RectRange rectRange;
    if (ANI_OK != AniTextRectConverter::ParseRangeToNative(env, range, rectRange)) {
        TEXT_LOGE("Failed to parse range");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<Drawing::Point> points = run->GetPositions(rectRange.start, rectRange.end);

    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, points.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to Create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& point : points) {
        ani_object aniObj{nullptr};
        ani_status status = AniDrawingConverter::ParsePointToAni(env, point, aniObj);
        if (ANI_OK != status
            || ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Failed to set points item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetOffsets(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<Drawing::Point> points = run->GetOffsets();

    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, points.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to Create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& point : points) {
        ani_object aniObj{nullptr};
        ani_status status = AniDrawingConverter::ParsePointToAni(env, point, aniObj);
        if (ANI_OK != status
            || ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Failed to set points item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetFont(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    Drawing::AniFont* aniFont = new Drawing::AniFont(run->GetFont());
    ani_object fontObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT, ":V");
    ani_status ret = env->Object_SetFieldByName_Long(fontObj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniFont));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to set type set textLine");
        delete aniFont;
        aniFont = nullptr;
    }
    return fontObj;
}

void AniRun::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =  AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Failed to get canvas");
        return;
    }

    run->Paint(aniCanvas->GetCanvas(), x, y);
}

ani_object AniRun::GetStringIndices(ani_env* env, ani_object object, ani_object range)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    OHOS::Text::ANI::RectRange rectRange;
    if (ANI_OK != AniTextRectConverter::ParseRangeToNative(env, range, rectRange)) {
        TEXT_LOGE("Failed to parse range");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<uint64_t> stringIndices = run->GetStringIndices(rectRange.start, rectRange.end);
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, stringIndices.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to Create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& stringIndex : stringIndices) {
        ani_status ret = env->Object_CallMethodByName_Void(
            arrayObj, "$_set", "ILstd/core/Object;:V", index, AniTextUtils::CreateAniDoubleObj(env, stringIndex));
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set stringIndices item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetStringRange(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    uint64_t location = 0;
    uint64_t length = 0;
    run->GetStringRange(&location, &length);
    Boundary boundary{location, length};
    ani_object boundaryObj{nullptr};
    ani_status ret = AniTextRectConverter::ParseBoundaryToAni(env, boundary, boundaryObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return boundaryObj;
}

ani_object AniRun::GetTypographicBounds(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    float ascent = 0.0;
    float descent = 0.0;
    float leading = 0.0;
    double width = run->GetTypographicBounds(&ascent, &descent, &leading);
    ani_object typographicBoundsObj{nullptr};
    ani_status result = AniTypographicBoundsConverter::ParseTypographicBoundsToAni(
        env, typographicBoundsObj, ascent, descent, leading, width);
    if (result != ANI_OK) {
        TEXT_LOGE("Failed to parse typographic bounds to ani");
        return AniTextUtils::CreateAniUndefined(env);
    }

    return typographicBoundsObj;
}

ani_object AniRun::GetImageBounds(ani_env* env, ani_object object)
{
    Run* run = AniTextUtils::GetNativeFromObj<Run>(env, object);
    if (run == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    Drawing::Rect rect = run->GetImageBounds();
    ani_object rectObj{nullptr};
    if (ANI_OK != AniDrawingConverter::ParseRectToAni(env, rect, rectObj)) {
        TEXT_LOGE("Failed to create rect");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return rectObj;
}
} // namespace OHOS::Text::ANI