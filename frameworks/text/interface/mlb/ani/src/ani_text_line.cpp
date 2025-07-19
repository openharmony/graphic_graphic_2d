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
#include <vector>

#include "SkPoint.h"
#include "ani_common.h"
#include "ani_drawing_converter.h"
#include "ani_run.h"
#include "ani_text_line.h"
#include "ani_text_rect_converter.h"
#include "ani_text_utils.h"
#include "ani_typographic_bounds_converter.h"
#include "canvas_ani/ani_canvas.h"
#include "text_line_base.h"
#include "typography.h"
#include "typography_types.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

ani_status AniTextLine::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_ClASS_TEXT_LINE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::string getTextRangeSignature = ":" + std::string(ANI_INTERFACE_RANGE);
    std::string getGlyphRunsSignature = ":" + std::string(ANI_ARRAY);
    std::string paintSignature = std::string(ANI_CLASS_CANVAS) + "DD:V";
    std::string createTruncatedLineSignature =
        "D" + std::string(ANI_ENUM_ELLIPSIS_MODE) + "Lstd/core/String;:" + std::string(ANI_ClASS_TEXT_LINE);
    std::string getTypographicBoundsSignature = ":" + std::string(ANI_INTERFACE_TYPOGRAPHIC_BOUNDS);
    std::string getImageBoundsSignature = ":" + std::string(ANI_INTERFACE_RECT);
    std::string getStringIndexForPositionSignature = std::string(ANI_INTERFACE_POINT) + ":D";

    std::array methods = {
        ani_native_function{"getGlyphCount", ":D", reinterpret_cast<void*>(GetGlyphCount)},
        ani_native_function{"getTextRange", getTextRangeSignature.c_str(), reinterpret_cast<void*>(GetTextRange)},
        ani_native_function{"getGlyphRuns", getGlyphRunsSignature.c_str(), reinterpret_cast<void*>(GetGlyphRuns)},
        ani_native_function{"paint", paintSignature.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{
            "createTruncatedLine", createTruncatedLineSignature.c_str(), reinterpret_cast<void*>(CreateTruncatedLine)},
        ani_native_function{"getTypographicBounds", getTypographicBoundsSignature.c_str(),
            reinterpret_cast<void*>(GetTypographicBounds)},
        ani_native_function{"getImageBounds", getImageBoundsSignature.c_str(), reinterpret_cast<void*>(GetImageBounds)},
        ani_native_function{"getTrailingSpaceWidth", ":D", reinterpret_cast<void*>(GetTrailingSpaceWidth)},
        ani_native_function{"getStringIndexForPosition", getStringIndexForPositionSignature.c_str(),
            reinterpret_cast<void*>(GetStringIndexForPosition)},
        ani_native_function{"getOffsetForStringIndex", "D:D", reinterpret_cast<void*>(GetOffsetForStringIndex)},
        // Lstd/core/Function<number>: <number> is an int from 0 to N, means the number of parameters in the function
        ani_native_function{
            "enumerateCaretOffsets", "Lstd/core/Function3;:V", reinterpret_cast<void*>(EnumerateCaretOffsets)},
        ani_native_function{"getAlignmentOffset", "DD:D", reinterpret_cast<void*>(GetAlignmentOffset)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for TextLine, ret %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}

ani_object AniTextLine::CreateTextLine(ani_env* env, Rosen::TextLineBase* textLine)
{
    if (textLine == nullptr) {
        TEXT_LOGE("Failed to create text line, emtpy ptr");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_object textLineObj = AniTextUtils::CreateAniObject(env, ANI_ClASS_TEXT_LINE, ":V");
    ani_status ret = env->Object_SetFieldByName_Long(textLineObj, NATIVE_OBJ, reinterpret_cast<ani_long>(textLine));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to set type set textLine, ani_status: %{public}d", ret);
        return AniTextUtils::CreateAniUndefined(env);
    }
    return textLineObj;
}

ani_double AniTextLine::GetGlyphCount(ani_env* env, ani_object object)
{
    TextLineBase* textline = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textline == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return textline->GetGlyphCount();
}

ani_object AniTextLine::GetTextRange(ani_env* env, ani_object object)
{
    TextLineBase* textline = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textline == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    Boundary boundary = textline->GetTextRange();
    ani_object boundaryObj = nullptr;
    ani_status ret = AniTextRectConverter::ParseBoundaryToAni(env, boundary, boundaryObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return boundaryObj;
}

ani_object AniTextLine::GetGlyphRuns(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);

    TextLineBase* textline = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textline == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }

    std::vector<std::unique_ptr<Run>> runs = textline->GetGlyphRuns();
    if (runs.empty()) {
        TEXT_LOGE("Run is empty");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }

    arrayObj = AniTextUtils::CreateAniArray(env, runs.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return arrayObj;
    }
    
    ani_size index = 0;
    for (auto& run : runs) {
        if (run == nullptr) {
            continue;
        }
        Run* runPtr = run.release();
        ani_object aniObj = AniRun::CreateRun(env, runPtr);
        if (AniTextUtils::IsUndefined(env, aniObj)) {
            TEXT_LOGE("Failed to create run");
            delete runPtr;
            runPtr = nullptr;
            continue;
        }
        ani_status ret = env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set runs item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

void AniTextLine::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    TextLineBase* textline = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textline == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =  AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Failed to get canvas");
        return;
    }

    textline->Paint(aniCanvas->GetCanvas(), x, y);
}

ani_object AniTextLine::CreateTruncatedLine(
    ani_env* env, ani_object object, ani_double width, ani_object ellipsisMode, ani_object ellipsis)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::string ellipsisStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ellipsis), ellipsisStr);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to convert ellipsisStr");
        return AniTextUtils::CreateAniUndefined(env);
    }

    ani_size index = 0;
    EllipsisModal ellipsisModal = EllipsisModal::HEAD;
    ret = env->EnumItem_GetIndex(reinterpret_cast<ani_enum_item>(ellipsisMode), &index);
    if (ret == ANI_OK) {
        ellipsisModal = static_cast<EllipsisModal>(index);
    }

    std::unique_ptr<TextLineBase> textLine = textLineBase->CreateTruncatedLine(width, ellipsisModal, ellipsisStr);
    if (textLine == nullptr) {
        TEXT_LOGE("Failed to create truncated textLine");
        return AniTextUtils::CreateAniUndefined(env);
    }
    TextLineBase* textLineBasePtr = textLine.release();
    ani_object textLineObj = CreateTextLine(env, textLineBasePtr);
    if (AniTextUtils::IsUndefined(env, textLineObj)) {
        TEXT_LOGE("Failed to create text line");
        delete textLineBasePtr;
        textLineBasePtr = nullptr;
    }
    return textLineObj;
}

ani_object AniTextLine::GetTypographicBounds(ani_env* env, ani_object object)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    double width = textLineBase->GetTypographicBounds(&ascent, &descent, &leading);
    ani_object typographicBoundsObj = nullptr;
    ani_status result = AniTypographicBoundsConverter::ParseTypographicBoundsToAni(
        env, typographicBoundsObj, ascent, descent, leading, width);
    if (result != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }

    return typographicBoundsObj;
}

ani_object AniTextLine::GetImageBounds(ani_env* env, ani_object object)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    Drawing::Rect rect = textLineBase->GetImageBounds();
    ani_object rectObj = nullptr;
    if (ANI_OK != AniDrawingConverter::ParseRectToAni(env, rect, rectObj)) {
        TEXT_LOGE("Failed to create rect");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return rectObj;
}

ani_double AniTextLine::GetTrailingSpaceWidth(ani_env* env, ani_object object)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return textLineBase->GetTrailingSpaceWidth();
}

ani_double AniTextLine::GetStringIndexForPosition(ani_env* env, ani_object object, ani_object point)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    OHOS::Rosen::Drawing::Point drawingPoint;
    AniDrawingConverter::ParseDrawingPointToNative(env, point, drawingPoint);

    SkPoint SkPoint = {drawingPoint.GetX(), drawingPoint.GetY()};
    return textLineBase->GetStringIndexForPosition(SkPoint);
}

ani_double AniTextLine::GetOffsetForStringIndex(ani_env* env, ani_object object, ani_double index)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return textLineBase->GetOffsetForStringIndex(index);
}

static bool CaretOffsetsCallBack(ani_env* env, ani_fn_object& callback, std::vector<ani_ref>& vec)
{
    ani_status ret;
    ani_ref fnReturnVal;
    ret = env->FunctionalObject_Call(callback, vec.size(), vec.data(), &fnReturnVal);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to call callback function, ani_status: %{public}d", ret);
        return false;
    }
    ani_boolean result = false;
    env->Object_CallMethodByName_Boolean(static_cast<ani_object>(fnReturnVal), "unboxed", ":Z", &result);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get result, ani_status: %{public}d", ret);
        return false;
    }
    return result;
}

void AniTextLine::EnumerateCaretOffsets(ani_env* env, [[maybe_unused]] ani_object object, ani_fn_object callback)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    bool isHardBreak = false;
    std::map<int32_t, double> offsetMap = textLineBase->GetIndexAndOffsets(isHardBreak);
    double leftOffset = 0.0;
    for (auto it = offsetMap.begin(); it != offsetMap.end(); ++it) {
        std::vector<ani_ref> vec;
        vec.push_back(AniTextUtils::CreateAniDoubleObj(env, it->first));
        vec.push_back(AniTextUtils::CreateAniDoubleObj(env, leftOffset));
        vec.push_back(AniTextUtils::CreateAniBooleanObj(env, it->second));
        if (!CaretOffsetsCallBack(env, callback, vec)) {
            return;
        }
        leftOffset = it->second;
    }
    if (isHardBreak && offsetMap.size() > 0) {
        std::vector<ani_ref> vec;
        vec.push_back(AniTextUtils::CreateAniDoubleObj(env, offsetMap.rbegin()->first + 1));
        vec.push_back(AniTextUtils::CreateAniDoubleObj(env, leftOffset));
        vec.push_back(AniTextUtils::CreateAniBooleanObj(env, leftOffset));
        if (!CaretOffsetsCallBack(env, callback, vec)) {
            return;
        }
    }
}

ani_double AniTextLine::GetAlignmentOffset(
    ani_env* env, ani_object object, ani_double alignmentFactor, ani_double alignmentWidth)
{
    TextLineBase* textLineBase = AniTextUtils::GetNativeFromObj<TextLineBase>(env, object);
    if (textLineBase == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return textLineBase->GetAlignmentOffset(alignmentFactor, alignmentWidth);
}
} // namespace OHOS::Text::ANI