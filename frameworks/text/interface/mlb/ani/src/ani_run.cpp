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

#include "ani_run.h"

#include <cstdint>
#include <memory>

#include "ani_common.h"
#include "ani_drawing_utils.h"
#include "ani_text_rect_converter.h"
#include "ani_text_utils.h"
#include "ani_typographic_bounds_converter.h"
#include "canvas_ani/ani_canvas.h"
#include "font_ani/ani_font.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
const std::string PAINT_SIGNATURE = "C{" + std::string(ANI_CLASS_CANVAS) + "}dd:";
const std::string NATIVE_GET_GLYPHS_SIGNATURE =
    "C{" + std::string(ANI_INTERFACE_RANGE) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string NATIVE_GET_POSITIONS_SIGNATURE =
    "C{" + std::string(ANI_INTERFACE_RANGE) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_OFFSETS_SIGNATURE = ":C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_FONT_SIGNATURE = ":C{" + std::string(ANI_CLASS_FONT) + "}";
const std::string GET_STRING_INDICES_SIGNATURE =
    "C{" + std::string(ANI_INTERFACE_RANGE) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_STRING_RANGE_SIGNATURE = ":C{" + std::string(ANI_INTERFACE_RANGE) + "}";
const std::string GET_TYPOGRAPHIC_BOUNDS_SIGNATURE = ":C{" + std::string(ANI_INTERFACE_TYPOGRAPHIC_BOUNDS) + "}";
const std::string GET_IMAGE_BOUNDS_SIGNATURE = ":C{" + std::string(ANI_INTERFACE_RECT) + "}";
const std::string RETURN_ARRAY_SIGN = ":C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_TEXT_DIRECTION_SIGNATURE = ":E{" + std::string(ANI_ENUM_TEXT_DIRECTION) + "}";
const std::string GET_ADVANCES_SIGNATURE = "C{" + std::string(ANI_INTERFACE_RANGE) + "}:C{" + std::string(ANI_ARRAY) + "}";
} // namespace

std::vector<ani_native_function> AniRun::InitMethods(ani_env* env)
{
    std::vector<ani_native_function> methods = {
        ani_native_function{"getGlyphCount", ":i", reinterpret_cast<void*>(GetGlyphCount)},
        ani_native_function{"getGlyphs", RETURN_ARRAY_SIGN.c_str(), reinterpret_cast<void*>(GetGlyphs)},
        ani_native_function{
            "nativeGetGlyphs", NATIVE_GET_GLYPHS_SIGNATURE.c_str(), reinterpret_cast<void*>(GetGlyphsByRange)},
        ani_native_function{
            "getPositions", RETURN_ARRAY_SIGN.c_str(), reinterpret_cast<void*>(GetPositions)},
        ani_native_function{
            "nativeGetPositions", NATIVE_GET_POSITIONS_SIGNATURE.c_str(), reinterpret_cast<void*>(GetPositionsByRange)},
        ani_native_function{"getOffsets", GET_OFFSETS_SIGNATURE.c_str(), reinterpret_cast<void*>(GetOffsets)},
        ani_native_function{"getFont", GET_FONT_SIGNATURE.c_str(), reinterpret_cast<void*>(GetFont)},
        ani_native_function{"paint", PAINT_SIGNATURE.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{
            "getStringIndices", GET_STRING_INDICES_SIGNATURE.c_str(), reinterpret_cast<void*>(GetStringIndices)},
        ani_native_function{
            "getStringRange", GET_STRING_RANGE_SIGNATURE.c_str(), reinterpret_cast<void*>(GetStringRange)},
        ani_native_function{"getTypographicBounds", GET_TYPOGRAPHIC_BOUNDS_SIGNATURE.c_str(),
            reinterpret_cast<void*>(GetTypographicBounds)},
        ani_native_function{
            "getImageBounds", GET_IMAGE_BOUNDS_SIGNATURE.c_str(), reinterpret_cast<void*>(GetImageBounds)},
        ani_native_function{
            "getTextDirection", GET_TEXT_DIRECTION_SIGNATURE.c_str(), reinterpret_cast<void*>(GetTextDirection)},
        ani_native_function{
            "getAdvances", GET_ADVANCES_SIGNATURE.c_str(), reinterpret_cast<void*>(GetAdvances)},
    };
    return methods;
}

ani_status AniRun::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }

    ani_class cls = nullptr;
    ret = AniTextUtils::FindClassWithCache(env, ANI_CLASS_RUN, cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ret;
    }

    std::vector<ani_native_function> methods = InitMethods(env);
    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for Run, ret %{public}d", ret);
        return ret;
    }
    return ANI_OK;
}

ani_object AniRun::CreateRun(ani_env* env, Rosen::Run* run)
{
    if (run == nullptr) {
        TEXT_LOGE("Failed to create run, emtpy ptr");
        return AniTextUtils::CreateAniUndefined(env);
    }
    AniRun* aniRun = new AniRun();
    aniRun->run_ = std::shared_ptr<Rosen::Run>(run);
    ani_object runObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RUN, ":");
    ani_status ret = env->Object_CallMethodByName_Void(
        runObj, BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniRun));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to set type set run");
        delete aniRun;
        aniRun = nullptr;
        return AniTextUtils::CreateAniUndefined(env);
    }
    return runObj;
}

ani_int AniRun::GetGlyphCount(ani_env* env, ani_object object)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniRun->run_->GetGlyphCount();
}

ani_object AniRun::GetGlyphs(ani_env* env, ani_object object)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    std::vector<uint16_t> glyphs = aniRun->run_->GetGlyphs();

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, glyphs.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& glpyh : glyphs) {
        ani_status ret = env->Object_CallMethodByName_Void(
            arrayObj, "$_set", "iC{std.core.Object}:", index, AniTextUtils::CreateAniIntObj(env, glpyh));
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
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    OHOS::Text::ANI::RectRange rectRange;
    ani_status ret = AniTextRectConverter::ParseRangeToNative(env, range, rectRange);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse range, ani_status %{public}d", ret);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    if (rectRange.start < 0 || rectRange.end < 0) {
        TEXT_LOGE("Invalid range, start %{public}zu, end %{public}zu", rectRange.start, rectRange.end);
        return AniTextUtils::CreateAniUndefined(env);
    }
    
    std::vector<uint16_t> glyphs = aniRun->run_->GetGlyphs(rectRange.start, rectRange.end);
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, glyphs.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& glpyh : glyphs) {
        ret = env->Object_CallMethodByName_Void(
            arrayObj, "$_set", "iC{std.core.Object}:", index, AniTextUtils::CreateAniIntObj(env, glpyh));
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
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    std::vector<Drawing::Point> points = aniRun->run_->GetPositions();

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, points.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& point : points) {
        ani_object aniObj = nullptr;
        ani_status status = OHOS::Rosen::Drawing::CreatePointObj(env, point, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to create point ani obj, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethodByName_Void(arrayObj, "$_set", "iC{std.core.Object}:", index, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to set points item, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetPositionsByRange(ani_env* env, ani_object object, ani_object range)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
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
    if (rectRange.start < 0 || rectRange.end < 0) {
        TEXT_LOGE("Invalid range, start %{public}zu, end %{public}zu", rectRange.start, rectRange.end);
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<Drawing::Point> points = aniRun->run_->GetPositions(rectRange.start, rectRange.end);

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, points.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& point : points) {
        ani_object aniObj = nullptr;
        ani_status status = OHOS::Rosen::Drawing::CreatePointObj(env, point, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to create point ani obj, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethodByName_Void(arrayObj, "$_set", "iC{std.core.Object}:", index, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to set points item, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetOffsets(ani_env* env, ani_object object)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<Drawing::Point> points = aniRun->run_->GetOffsets();

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, points.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& point : points) {
        ani_object aniObj = nullptr;
        ani_status status = OHOS::Rosen::Drawing::CreatePointObj(env, point, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to create point ani obj, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethodByName_Void(arrayObj, "$_set", "iC{std.core.Object}:", index, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to set points item, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniRun::GetFont(ani_env* env, ani_object object)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    Drawing::AniFont* aniFont = new Drawing::AniFont(aniRun->run_->GetFont());
    ani_object fontObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT, ":");
    ani_status ret = env->Object_CallMethodByName_Void(
        fontObj, BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniFont));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to set type set textLine");
        delete aniFont;
        aniFont = nullptr;
    }
    return fontObj;
}

void AniRun::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas = AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Failed to get canvas");
        return;
    }

    aniRun->run_->Paint(aniCanvas->GetCanvas(), x, y);
}

ani_object AniRun::GetStringIndices(ani_env* env, ani_object object, ani_object range)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
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
    if (rectRange.start < 0 || rectRange.end < 0) {
        TEXT_LOGE("Invalid range, start %{public}zu, end %{public}zu", rectRange.start, rectRange.end);
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<uint64_t> stringIndices = aniRun->run_->GetStringIndices(rectRange.start, rectRange.end);
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, stringIndices.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& stringIndex : stringIndices) {
        ani_status ret = env->Object_CallMethodByName_Void(
            arrayObj, "$_set", "iC{std.core.Object}:", index, AniTextUtils::CreateAniIntObj(env, stringIndex));
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
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    uint64_t location = 0;
    uint64_t length = 0;
    aniRun->run_->GetStringRange(&location, &length);
    Boundary boundary{location, length};
    ani_object boundaryObj = nullptr;
    ani_status ret = AniTextRectConverter::ParseBoundaryToAni(env, boundary, boundaryObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return boundaryObj;
}

ani_object AniRun::GetTypographicBounds(ani_env* env, ani_object object)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    float ascent = 0.0;
    float descent = 0.0;
    float leading = 0.0;
    double width = aniRun->run_->GetTypographicBounds(&ascent, &descent, &leading);
    ani_object typographicBoundsObj = nullptr;
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
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    Drawing::Rect rect = aniRun->run_->GetImageBounds();
    ani_object rectObj = nullptr;
    if (ANI_OK != OHOS::Rosen::Drawing::CreateRectObj(env, rect, rectObj)) {
        TEXT_LOGE("Failed to create rect");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return rectObj;
}

ani_object AniRun::GetTextDirection(ani_env* env, ani_object object)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
        TEXT_LOGE("Run is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    TextDirection textDirection = aniRun->run_->GetTextDirection();
    ani_enum_item textDirectionEnum = AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_DIRECTION, static_cast<int>(textDirection));
    return static_cast<ani_object>(textDirectionEnum);
}

ani_object AniRun::GetAdvances(ani_env* env, ani_object object, ani_object range)
{
    AniRun* aniRun = AniTextUtils::GetNativeFromObj<AniRun>(env, object);
    if (aniRun == nullptr || aniRun->run_ == nullptr) {
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
    if (rectRange.start < 0 || rectRange.end < 0) {
        TEXT_LOGE("Invalid range, start %{public}zu, end %{public}zu", rectRange.start, rectRange.end);
        return AniTextUtils::CreateAniUndefined(env);
    }
    std::vector<Drawing::Point> advances = aniRun->run_->GetAdvances(rectRange.start, rectRange.end);
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, advances.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& point : advances) {
        ani_object aniObj = nullptr;
        ani_status status = OHOS::Rosen::Drawing::CreatePointObj(env, point, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to create point ani obj, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj);
        if (ANI_OK != status) {
            TEXT_LOGE("Failed to set points item, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}
} // namespace OHOS::Text::ANI