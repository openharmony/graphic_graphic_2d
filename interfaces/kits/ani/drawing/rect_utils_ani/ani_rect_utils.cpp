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

#include "ani_rect_utils.h"

namespace OHOS::Rosen {
namespace Drawing {

ani_status AniRectUtils::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().rectUtils;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_RECT_UTILS_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "contains",
            "C{@ohos.graphics.common2D.common2D.Rect}C{@ohos.graphics.common2D.common2D.Rect}:z",
            reinterpret_cast<void*>(ContainsWithObject) },
        ani_native_function { "contains", "C{@ohos.graphics.common2D.common2D.Rect}dd:z",
            reinterpret_cast<void*>(Contains) },
        ani_native_function { "contains", "C{@ohos.graphics.common2D.common2D.Rect}dddd:z",
            reinterpret_cast<void*>(ContainsWithVaule) },
        ani_native_function { "isIntersect", nullptr, reinterpret_cast<void*>(IsIntersect) },
        ani_native_function { "centerX", nullptr, reinterpret_cast<void*>(CenterX) },
        ani_native_function { "centerY", nullptr, reinterpret_cast<void*>(CenterY) },
        ani_native_function { "makeCopy", nullptr, reinterpret_cast<void*>(MakeCopy) },
        ani_native_function { "makeLtrb", nullptr, reinterpret_cast<void*>(MakeLtrb) },
        ani_native_function { "makeEmpty", nullptr, reinterpret_cast<void*>(MakeEmpty) },
        ani_native_function { "union", nullptr, reinterpret_cast<void*>(Union) },
        ani_native_function { "offsetTo", nullptr, reinterpret_cast<void*>(OffsetTo) },
        ani_native_function { "setLtrb", nullptr, reinterpret_cast<void*>(SetLtrb) },
        ani_native_function { "setEmpty", nullptr, reinterpret_cast<void*>(SetEmpty) },
        ani_native_function { "setRect", nullptr, reinterpret_cast<void*>(SetRect) },
        ani_native_function { "inset", nullptr, reinterpret_cast<void*>(Inset) },
        ani_native_function { "sort", nullptr, reinterpret_cast<void*>(Sort) },
        ani_native_function { "isEqual", nullptr, reinterpret_cast<void*>(IsEqual) },
        ani_native_function { "getHeight", nullptr, reinterpret_cast<void*>(GetHeight) },
        ani_native_function { "getWidth", nullptr, reinterpret_cast<void*>(GetWidth) },
        ani_native_function { "isEmpty", nullptr, reinterpret_cast<void*>(IsEmpty) },
        ani_native_function { "intersect", nullptr, reinterpret_cast<void*>(Intersect) },
        ani_native_function { "offset", nullptr, reinterpret_cast<void*>(Offset) },
    };

    ani_status ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_RECT_UTILS_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

ani_boolean AniRectUtils::ContainsWithObject(ani_env* env, ani_object obj, ani_object aniRectObj,
    ani_object aniOtherRect)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::ContainsWithObject invalid params: aniRectObj. ");
        return false;
    }
    Drawing::Rect otherRect;
    if (!GetRectFromAniRectObj(env, aniOtherRect, otherRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::ContainsWithObject invalid params: aniOtherRect. ");
        return false;
    }
    bool isContains = drawingRect.Contains(otherRect);
    return isContains;
}

ani_boolean AniRectUtils::Contains(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double x, ani_double y)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::Contains invalid params: aniRectObj. ");
        return false;
    }
 
    bool isContains = drawingRect.Contains(x, y);
    return isContains;
}
 
ani_boolean AniRectUtils::ContainsWithVaule(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double left,
    ani_double top, ani_double right, ani_double bottom)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::ContainsWithVaule invalid params: aniRectObj. ");
        return false;
    }
    Drawing::Rect other = Drawing::Rect(left, top, right, bottom);
    bool isContains = drawingRect.Contains(other);
    return isContains;
}

void AniRectUtils::Inset(ani_env* env, ani_object obj, ani_object aniRectObj,
    ani_double left, ani_double top, ani_double right, ani_double bottom)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::Inset invalid params: aniRectObj. ");
        return;
    }

    float originalLeft = drawingRect.GetLeft();
    float originalTop = drawingRect.GetTop();
    float originalRight = drawingRect.GetRight();
    float originalBottom = drawingRect.GetBottom();
    drawingRect.SetLeft(originalLeft + left);
    drawingRect.SetTop(originalTop + top);
    drawingRect.SetRight(originalRight - right);
    drawingRect.SetBottom(originalBottom - bottom);
    DrawingRectConvertToAniRect(env, aniRectObj, drawingRect);
}

void AniRectUtils::Sort(ani_env* env, ani_object obj, ani_object aniRectObj)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return;
    }
    drawingRect.Sort();
    DrawingRectConvertToAniRect(env, aniRectObj, drawingRect);
}

ani_boolean AniRectUtils::IsEqual(ani_env* env, ani_object obj, ani_object aniRectObj, ani_object aniOtherRect)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return false;
    }
    Drawing::Rect otherRect;
    if (!GetRectFromAniRectObj(env, aniOtherRect, otherRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
        return false;
    }
    return drawingRect == otherRect;
}

void AniRectUtils::Union(ani_env* env, ani_object obj, ani_object rectobj, ani_object otherobj)
{
    std::vector<double> ltrb;
    if (!GetValueFromAniRectObj(env, rectobj, ltrb)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return;
    }

    std::vector<double> ltrb2;
    if (!GetValueFromAniRectObj(env, otherobj, ltrb2)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return;
    }
    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    Drawing::Rect other = Drawing::Rect(ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
 
    bool isJoin = rect.Join(other);
    if (isJoin) {
        DrawingRectConvertToAniRect(env, rectobj, rect);
    }
    return;
}
 
void AniRectUtils::OffsetTo(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double newLeft, ani_double newTop)
{
    std::vector<double> ltrb;
    if (!GetValueFromAniRectObj(env, aniRectObj, ltrb)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return;
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    ani_double dx = newLeft - ltrb[ARGC_ZERO];
    ani_double dy = newTop - ltrb[ARGC_ONE];
 
    rect.Offset(dx, dy);
    DrawingRectConvertToAniRect(env, aniRectObj, rect);
    return;
}

ani_double AniRectUtils::CenterX(ani_env* env, ani_object obj, ani_object aniRectObj)
{

    std::vector<double> ltrb;
    if (!GetValueFromAniRectObj(env, aniRectObj, ltrb)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return -1;
    }

    return (ltrb[ARGC_ZERO] + ltrb[ARGC_TWO]) / 2; // 2 represents the average of left and right
}
 
ani_double  AniRectUtils::CenterY(ani_env* env, ani_object obj, ani_object aniRectObj)
{
    std::vector<double> ltrb;
    if (!GetValueFromAniRectObj(env, aniRectObj, ltrb)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return -1;
    }
    return (ltrb[ARGC_ONE] + ltrb[ARGC_THREE]) / 2; // 2 represents the average of top and bottom
}
 
ani_object AniRectUtils::MakeCopy(ani_env* env, ani_object obj, ani_object aniSrcRectObj)
{
    std::vector<double> ltrb;
    if (!GetValueFromAniRectObj(env, aniSrcRectObj, ltrb)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return CreateAniUndefined(env);
    }

    return MakeLtrb(env, obj, ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
}
 
ani_object AniRectUtils::MakeLtrb(ani_env* env, ani_object obj, ani_double left, ani_double top, ani_double right,
    ani_double bottom)
{
    return CreateAniObject(env, AniGlobalClass::GetInstance().rect, AniGlobalMethod::GetInstance().rectCtor,
        left, top, right, bottom);
}
 
ani_object AniRectUtils::MakeEmpty(ani_env* env, ani_object obj)
{
    Drawing::Rect drawingRect = Drawing::Rect();
    ani_object aniRectObj;
    ani_status ret = CreateRectObj(env, drawingRect, aniRectObj);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Set AniRectObj From rect failed.");
        return CreateAniUndefined(env);
    }
    return aniRectObj;
}

ani_boolean AniRectUtils::IsIntersect(ani_env* env, ani_object obj, ani_object aniRectObj, ani_object aniOtherRect)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::IsIntersect Incorrect parameter0 type. The type of value must be number.");
        return false;
    }

    Drawing::Rect otherRect;
    if (!GetRectFromAniRectObj(env, aniOtherRect, otherRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::IsIntersect Incorrect parameter1 type. The type of value must be number.");
        return false;
    }
    bool isIntersect = drawingRect.IsIntersect(otherRect);
    return isIntersect;
}

void AniRectUtils::SetLtrb(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double left, ani_double top,
    ani_double right, ani_double bottom)
{
    DrawingValueConvertToAniRect(env, aniRectObj, left, top, right, bottom);
}
 
void AniRectUtils::SetEmpty(ani_env* env, ani_object obj, ani_object aniRectObj)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::SetEmpty Incorrect parameter0 type. The type of value must be number.");
        return;
    }
    DrawingRectConvertToAniRect(env, aniRectObj, Drawing::Rect());
}
 
void AniRectUtils::SetRect(ani_env* env, ani_object obj, ani_object rectobj, ani_object otherobj)
{
    std::vector<double> ltrb;
    if (!GetValueFromAniRectObj(env, rectobj, ltrb)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRectUtils::SetRect Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return;
    }

    std::vector<double> ltrb2;
    if (!GetValueFromAniRectObj(env, otherobj, ltrb2)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return;
    }

    DrawingValueConvertToAniRect(env, rectobj, ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
}

ani_double AniRectUtils::GetHeight(ani_env* env, ani_object object, ani_object aniRectObj)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return -1;
    }
    return drawingRect.GetHeight();
}

ani_double AniRectUtils::GetWidth(ani_env* env, ani_object object, ani_object aniRectObj)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return -1;
    }
    return drawingRect.GetWidth();
}

ani_boolean AniRectUtils::IsEmpty(ani_env* env, ani_object obj, ani_object aniRectObj)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return false;
    }
    return drawingRect.IsEmpty();
}

ani_boolean AniRectUtils::Intersect(ani_env* env, ani_object obj, ani_object aniRectObj, ani_object aniOtherRect)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return false;
    }
    Drawing::Rect otherRect;
    if (!GetRectFromAniRectObj(env, aniOtherRect, otherRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
        return false;
    }
    bool isIntersect = drawingRect.Intersect(otherRect);
    if (isIntersect) {
        DrawingRectConvertToAniRect(env, aniRectObj, drawingRect);
    }
    return isIntersect;
}

void AniRectUtils::Offset(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double dx, ani_double dy)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return;
    }
    drawingRect.Offset(dx, dy);
    DrawingRectConvertToAniRect(env, aniRectObj, drawingRect);
}
} // namespace Drawing
} // namespace OHOS::Rosen