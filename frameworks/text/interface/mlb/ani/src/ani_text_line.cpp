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

#include "ani_text_line.h"

#include <memory>
#include <vector>

#include "ani_common.h"
#include "ani_drawing_utils.h"
#include "ani_run.h"
#include "ani_text_rect_converter.h"
#include "ani_text_utils.h"
#include "ani_typographic_bounds_converter.h"
#include "canvas_ani/ani_canvas.h"
#include "SkPoint.h"
#include "text_line_base.h"
#include "typography.h"
#include "typography_types.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
constexpr size_t ARGC_TWO = 2;
const std::string GET_TEXT_RANGE_SIGN = ":C{" + std::string(ANI_INTERFACE_RANGE) + "}";
const std::string GET_LYPH_RUNS_SIGN = ":C{" + std::string(ANI_ARRAY) + "}";
const std::string PAINT_SIGN = "C{" + std::string(ANI_CLASS_CANVAS) + "}dd:";
const std::string CREATE_TRUNCATED_LINE_SIGN =
    "dE{" + std::string(ANI_ENUM_ELLIPSIS_MODE) + "}C{std.core.String}:C{" + std::string(ANI_CLASS_TEXT_LINE) + "}";
const std::string GET_TYPOGRAPHIC_BOUNDS_SIGN = ":C{" + std::string(ANI_INTERFACE_TYPOGRAPHIC_BOUNDS) + "}";
const std::string GET_IMAGE_BOUNDS_SIGN = ":C{" + std::string(ANI_INTERFACE_RECT) + "}";
const std::string GET_STRING_INDEX_FOR_POSITION_SIGN = "C{" + std::string(ANI_INTERFACE_POINT) + "}:i";
}

ani_status AniTextLine::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }

    ani_class cls = nullptr;
    ret = AniTextUtils::FindClassWithCache(env, ANI_CLASS_TEXT_LINE, cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ret;
    }

    std::array methods = {
        ani_native_function{"getGlyphCount", ":i", reinterpret_cast<void*>(GetGlyphCount)},
        ani_native_function{"getTextRange", GET_TEXT_RANGE_SIGN.c_str(), reinterpret_cast<void*>(GetTextRange)},
        ani_native_function{"getGlyphRuns", GET_LYPH_RUNS_SIGN.c_str(), reinterpret_cast<void*>(GetGlyphRuns)},
        ani_native_function{"paint", PAINT_SIGN.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{
            "createTruncatedLine", CREATE_TRUNCATED_LINE_SIGN.c_str(), reinterpret_cast<void*>(CreateTruncatedLine)},
        ani_native_function{"getTypographicBounds", GET_TYPOGRAPHIC_BOUNDS_SIGN.c_str(),
            reinterpret_cast<void*>(GetTypographicBounds)},
        ani_native_function{"getImageBounds", GET_IMAGE_BOUNDS_SIGN.c_str(), reinterpret_cast<void*>(GetImageBounds)},
        ani_native_function{"getTrailingSpaceWidth", ":d", reinterpret_cast<void*>(GetTrailingSpaceWidth)},
        ani_native_function{"getStringIndexForPosition", GET_STRING_INDEX_FOR_POSITION_SIGN.c_str(),
            reinterpret_cast<void*>(GetStringIndexForPosition)},
        ani_native_function{"getOffsetForStringIndex", "i:d", reinterpret_cast<void*>(GetOffsetForStringIndex)},
        // Lstd/core/Function<number>: <number> is an int from 0 to N, means the number of parameters in the function
        ani_native_function{
            "enumerateCaretOffsets", "C{std.core.Function3}:", reinterpret_cast<void*>(EnumerateCaretOffsets)},
        ani_native_function{"getAlignmentOffset", "dd:d", reinterpret_cast<void*>(GetAlignmentOffset)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for TextLine, ret %{public}d", ret);
        return ret;
    }
    return ANI_OK;
}

ani_object AniTextLine::CreateTextLine(ani_env* env, Rosen::TextLineBase* textLine)
{
    if (textLine == nullptr) {
        TEXT_LOGE("Failed to create text line, emtpy ptr");
        return AniTextUtils::CreateAniUndefined(env);
    }
    AniTextLine* aniTextLine = new AniTextLine();
    aniTextLine->textLine_ = std::shared_ptr<Rosen::TextLineBase>(textLine);
    ani_object textLineObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXT_LINE, ":");
    ani_status ret = env->Object_CallMethodByName_Void(
        textLineObj, BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniTextLine));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to set type set textLine, ani_status %{public}d", ret);
        delete aniTextLine;
        aniTextLine = nullptr;
        return AniTextUtils::CreateAniUndefined(env);
    }
    return textLineObj;
}

ani_int AniTextLine::GetGlyphCount(ani_env* env, ani_object object)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniTextline->textLine_->GetGlyphCount();
}

ani_object AniTextLine::GetTextRange(ani_env* env, ani_object object)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    Boundary boundary = aniTextline->textLine_->GetTextRange();
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

    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }

    std::vector<std::unique_ptr<Run>> runs = aniTextline->textLine_->GetGlyphRuns();
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
        ani_status ret = env->Object_CallMethodByName_Void(arrayObj, "$_set", "iC{std.core.Object}:", index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set runs item %{public}zu", index);
            delete runPtr;
            runPtr = nullptr;
            continue;
        }
        index++;
    }
    return arrayObj;
}

void AniTextLine::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas = AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Failed to get canvas");
        return;
    }

    aniTextline->textLine_->Paint(aniCanvas->GetCanvas(), x, y);
}

ani_object AniTextLine::CreateTruncatedLine(
    ani_env* env, ani_object object, ani_double width, ani_object ellipsisMode, ani_object ellipsis)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
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

    std::unique_ptr<TextLineBase> textLine =
        aniTextline->textLine_->CreateTruncatedLine(width, ellipsisModal, ellipsisStr);
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
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    double width = aniTextline->textLine_->GetTypographicBounds(&ascent, &descent, &leading);
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
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    Drawing::Rect rect = aniTextline->textLine_->GetImageBounds();
    ani_object rectObj = nullptr;
    if (ANI_OK != OHOS::Rosen::Drawing::CreateRectObj(env, rect, rectObj)) {
        TEXT_LOGE("Failed to create rect");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return rectObj;
}

ani_double AniTextLine::GetTrailingSpaceWidth(ani_env* env, ani_object object)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return aniTextline->textLine_->GetTrailingSpaceWidth();
}

ani_int AniTextLine::GetStringIndexForPosition(ani_env* env, ani_object object, ani_object point)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    OHOS::Rosen::Drawing::Point drawingPoint;
    OHOS::Rosen::Drawing::GetPointFromPointObj(env, point, drawingPoint);

    SkPoint SkPoint = {drawingPoint.GetX(), drawingPoint.GetY()};
    return aniTextline->textLine_->GetStringIndexForPosition(SkPoint);
}

ani_double AniTextLine::GetOffsetForStringIndex(ani_env* env, ani_object object, ani_int index)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return aniTextline->textLine_->GetOffsetForStringIndex(index);
}

static bool CaretOffsetsCallBack(
    ani_env* env, ani_fn_object& callback, int32_t index, double leftOffset, double rightOffset)
{
    ani_object jsIndex = AniTextUtils::CreateAniIntObj(env, index);
    for (size_t i = 0; i < ARGC_TWO; i++) {
        ani_object jsOffset = (i == 0) ? AniTextUtils::CreateAniDoubleObj(env, leftOffset) :
            AniTextUtils::CreateAniDoubleObj(env, rightOffset);
        ani_object jsLeadingEdge =
            (i == 0) ? AniTextUtils::CreateAniBooleanObj(env, true) : AniTextUtils::CreateAniBooleanObj(env, false);
        std::vector<ani_ref> vec = {jsOffset, jsIndex, jsLeadingEdge};
        ani_ref fnReturnVal = nullptr;
        ani_status ret = env->FunctionalObject_Call(callback, vec.size(), vec.data(), &fnReturnVal);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to call callback function, ani_status %{public}d", ret);
            return false;
        }
        ani_boolean result = false;
        ret = env->Object_CallMethodByName_Boolean(static_cast<ani_object>(fnReturnVal), "unboxed", ":z", &result);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to get result, ani_status %{public}d", ret);
            return false;
        }
        if (result) {
            TEXT_LOGI("Callback function call stop");
            return false;
        }
    }
    return true;
}

void AniTextLine::EnumerateCaretOffsets(ani_env* env, [[maybe_unused]] ani_object object, ani_fn_object callback)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    bool isHardBreak = false;
    std::map<int32_t, double> offsetMap = aniTextline->textLine_->GetIndexAndOffsets(isHardBreak);
    double leftOffset = 0.0;
    for (auto it = offsetMap.begin(); it != offsetMap.end(); ++it) {
        if (!CaretOffsetsCallBack(env, callback, it->first, leftOffset, it->second)) {
            return;
        }
        leftOffset = it->second;
    }
    if (isHardBreak && offsetMap.size() > 0) {
        if (!CaretOffsetsCallBack(env, callback, offsetMap.rbegin()->first + 1, leftOffset, leftOffset)) {
            return;
        }
    }
}

ani_double AniTextLine::GetAlignmentOffset(
    ani_env* env, ani_object object, ani_double alignmentFactor, ani_double alignmentWidth)
{
    AniTextLine* aniTextline = AniTextUtils::GetNativeFromObj<AniTextLine>(env, object);
    if (aniTextline == nullptr || aniTextline->textLine_ == nullptr) {
        TEXT_LOGE("Text line is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return aniTextline->textLine_->GetAlignmentOffset(alignmentFactor, alignmentWidth);
}
} // namespace OHOS::Text::ANI