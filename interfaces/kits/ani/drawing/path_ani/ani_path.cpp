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

#include "ani_path.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/path_napi/js_path.h"
#include "path_iterator_ani/ani_path_iterator.h"
#include "ani_drawing_utils.h"
#include "round_rect_ani/ani_round_rect.h"
#include "matrix_ani/ani_matrix.h"

namespace OHOS::Rosen {
namespace Drawing {

static const std::array g_methods = {
    ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(AniPath::Constructor) },
    ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.Path}:",
        reinterpret_cast<void*>(AniPath::ConstructorWithPath) },
    ani_native_function { "arcTo", "dddddd:", reinterpret_cast<void*>(AniPath::ArcTo) },
    ani_native_function { "reset", ":", reinterpret_cast<void*>(AniPath::Reset) },
    ani_native_function { "isRect", nullptr, reinterpret_cast<void*>(AniPath::IsRect) },
    ani_native_function { "getSegment", nullptr, reinterpret_cast<void*>(AniPath::GetSegment) },
    ani_native_function { "getPathIterator", ":C{@ohos.graphics.drawing.drawing.PathIterator}",
        reinterpret_cast<void*>(AniPath::GetPathIterator) },
    ani_native_function { "rLineTo", "dd:", reinterpret_cast<void*>(AniPath::RLineTo) },
    ani_native_function { "addRoundRect", nullptr, reinterpret_cast<void*>(AniPath::AddRoundRect) },
    ani_native_function { "rQuadTo", "dddd:", reinterpret_cast<void*>(AniPath::RQuadTo) },
    ani_native_function { "transform", "C{@ohos.graphics.drawing.drawing.Matrix}:",
        reinterpret_cast<void*>(AniPath::Transform) },
    ani_native_function { "offset", "dd:C{@ohos.graphics.drawing.drawing.Path}",
        reinterpret_cast<void*>(AniPath::Offset) },
    ani_native_function { "addArc", "C{@ohos.graphics.common2D.common2D.Rect}dd:",
        reinterpret_cast<void*>(AniPath::AddArc) },
    ani_native_function { "rMoveTo", "dd:", reinterpret_cast<void*>(AniPath::RMoveTo) },
    ani_native_function { "getPositionAndTangent", nullptr, reinterpret_cast<void*>(AniPath::GetPositionAndTangent) },
    ani_native_function { "getLength", nullptr, reinterpret_cast<void*>(AniPath::GetLength) },
    ani_native_function { "op", nullptr, reinterpret_cast<void*>(AniPath::Op) },
    ani_native_function { "addPolygon", nullptr, reinterpret_cast<void*>(AniPath::AddPolygon) },
    ani_native_function { "conicTo", nullptr, reinterpret_cast<void*>(AniPath::ConicTo) },
    ani_native_function { "addCircle", nullptr, reinterpret_cast<void*>(AniPath::AddCircle) },
    ani_native_function { "rCubicTo", nullptr, reinterpret_cast<void*>(AniPath::RCubicTo) },
    ani_native_function { "rConicTo", nullptr, reinterpret_cast<void*>(AniPath::RConicTo) },
    ani_native_function { "setFillType", nullptr, reinterpret_cast<void*>(AniPath::SetFillType) },
    ani_native_function { "addRect", nullptr, reinterpret_cast<void*>(AniPath::AddRect) },
    ani_native_function { "getMatrix", nullptr, reinterpret_cast<void*>(AniPath::GetMatrix) },
    ani_native_function { "getBounds", nullptr, reinterpret_cast<void*>(AniPath::GetBounds) },
    ani_native_function { "addOval", nullptr, reinterpret_cast<void*>(AniPath::AddOval) },
    ani_native_function { "contains", nullptr, reinterpret_cast<void*>(AniPath::Contains) },
    ani_native_function { "addPath", nullptr, reinterpret_cast<void*>(AniPath::AddPath) },
    ani_native_function { "isClosed", nullptr, reinterpret_cast<void*>(AniPath::IsClosed) },
    ani_native_function { "buildFromSvgString", nullptr, reinterpret_cast<void*>(AniPath::BuildFromSVGString) },
    ani_native_function { "cubicTo", nullptr, reinterpret_cast<void*>(AniPath::CubicTo) },
    ani_native_function { "quadTo", nullptr, reinterpret_cast<void*>(AniPath::QuadTo) },
    ani_native_function { "lineTo", nullptr, reinterpret_cast<void*>(AniPath::LineTo) },
    ani_native_function { "moveTo", nullptr, reinterpret_cast<void*>(AniPath::MoveTo) },
    ani_native_function { "close", nullptr, reinterpret_cast<void*>(AniPath::Close) },
    ani_native_function { "isInverseFillType", nullptr, reinterpret_cast<void*>(AniPath::IsInverseFillType) },
    ani_native_function { "toggleInverseFillType", nullptr, reinterpret_cast<void*>(AniPath::ToggleInverseFillType) },
    ani_native_function { "approximate", nullptr, reinterpret_cast<void*>(AniPath::Approximate) },
    ani_native_function { "setLastPoint", nullptr, reinterpret_cast<void*>(AniPath::SetLastPoint) },
    ani_native_function { "rewind", nullptr, reinterpret_cast<void*>(AniPath::ReWind) },
    ani_native_function { "interpolate", nullptr, reinterpret_cast<void*>(AniPath::Interpolate) },
    ani_native_function { "isEmpty", nullptr, reinterpret_cast<void*>(AniPath::IsEmpty) },
    ani_native_function { "set", nullptr, reinterpret_cast<void*>(AniPath::Set) },
    ani_native_function { "isInterpolate", nullptr, reinterpret_cast<void*>(AniPath::IsInterpolate) },
    ani_native_function { "getFillType", nullptr, reinterpret_cast<void*>(AniPath::GetFillType) },
};

ani_status AniPath::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().path;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_PATH_NAME);
        return ANI_NOT_FOUND;
    }

    ani_status ret = env->Class_BindNativeMethods(cls, g_methods.data(), g_methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: ret %{public}d %{public}s", ret, ANI_CLASS_PATH_NAME);
        return ret;
    }

    std::array staticMethods = {
        ani_native_function { "pathTransferStaticNative", nullptr, reinterpret_cast<void*>(PathTransferStatic) },
        ani_native_function { "getPathAddr", nullptr, reinterpret_cast<void*>(GetPathAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: ret %{public}d %{public}s", ret, ANI_CLASS_PATH_NAME);
        return ret;
    }

    return ANI_OK;
}

void AniPath::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Path> path = std::make_shared<Path>();
    AniPath* aniPath = new AniPath(path);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().pathNativeObj, reinterpret_cast<ani_long>(aniPath))) {
        ROSEN_LOGE("AniPath::Constructor failed create AniPath");
        delete aniPath;
        return;
    }
}

void AniPath::ConstructorWithPath(ani_env* env, ani_object obj, ani_object aniPathObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, aniPathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    std::shared_ptr<Path> other = aniPath->GetPath();
    std::shared_ptr<Path> path = other == nullptr ? std::make_shared<Path>() : std::make_shared<Path>(*other);
    AniPath* newAniPath = new AniPath(path);
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().pathNativeObj, reinterpret_cast<ani_long>(newAniPath))) {
        ROSEN_LOGE("AniPath::Constructor failed create AniPath");
        delete newAniPath;
        return;
    }
}

void AniPath::Reset(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    aniPath->GetPath()->Reset();
}

void AniPath::ArcTo(ani_env* env, ani_object obj, ani_double x1, ani_double y1, ani_double x2, ani_double y2,
    ani_double startDeg, ani_double sweepDeg)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    aniPath->GetPath()->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg);
    return;
}

ani_boolean AniPath::IsRect(ani_env* env, ani_object obj, ani_object aniRectObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return false;
    }
    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(aniRectObj, &isNull);
    if (isNull) {
        return aniPath->GetPath()->IsRect(nullptr);
    }
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        return false;
    }
    bool result = aniPath->GetPath()->IsRect(&drawingRect);
    DrawingRectConvertToAniRect(env, aniRectObj, drawingRect);
    return result;
}

ani_boolean AniPath::GetSegment(ani_env* env, ani_object obj, ani_boolean forceClosed, ani_double start,
    ani_double stop, ani_boolean startWithMoveTo, ani_object aniPathObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    auto dst = GetNativeFromObj<AniPath>(env, aniPathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (dst == nullptr || dst->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param dst.");
        return false;
    }
    bool result = aniPath->GetPath()->GetSegment(start, stop, dst->GetPath().get(), startWithMoveTo, forceClosed);
    return static_cast<ani_boolean>(result);
}

ani_object AniPath::GetPathIterator(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }
    AniPathIterator* aniPathItertor = new AniPathIterator(*aniPath->GetPath());
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().pathIterator,
        AniGlobalMethod::GetInstance().pathIteratorCtor, obj);
    if (ANI_OK != env->Object_SetField_Long(
        aniObj, AniGlobalField::GetInstance().pathIteratorNativeObj, reinterpret_cast<ani_long>(aniPathItertor))) {
        ROSEN_LOGE("AniPath::GetPathIterator failed create PathIntertor.");
        delete aniPathItertor;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniPath::RLineTo(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniPath->GetPath()->RLineTo(dx, dy);
}

void AniPath::AddRoundRect(ani_env* env, ani_object obj, ani_object aniRoundRectObj, ani_object aniPathDirectionObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, aniRoundRectObj,
        AniGlobalField::GetInstance().roundRectNativeObj);
    if (aniRoundRect == nullptr || aniRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param roundRect.");
        return;
    }

    if (IsUndefined(env, aniPathDirectionObj)) {
        aniPath->GetPath()->AddRoundRect(*aniRoundRect->GetRoundRect());
        return;
    }
    ani_int pathDirection = 0;
    if (ANI_OK != env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(aniPathDirectionObj), &pathDirection)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param pathDirection.");
        return;
    }
    aniPath->GetPath()->AddRoundRect(*aniRoundRect->GetRoundRect(), static_cast<PathDirection>(pathDirection));
}

void AniPath::RQuadTo(ani_env* env, ani_object obj, ani_double dx1, ani_double dy1, ani_double dx2, ani_double dy2)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniPath->GetPath()->RQuadTo(dx1, dy1, dx2, dy2);
}

void AniPath::Transform(ani_env* env, ani_object obj, ani_object aniMatrix)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    auto matrix = GetNativeFromObj<AniMatrix>(env, aniMatrix, AniGlobalField::GetInstance().matrixNativeObj);
    if (matrix == nullptr || matrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param matrix.");
        return;
    }
    aniPath->GetPath()->Transform(*matrix->GetMatrix());
}

ani_object AniPath::Offset(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }
    std::shared_ptr<Path> path = std::make_shared<Path>();
    aniPath->GetPath()->Offset(path.get(), dx, dy);
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().path,
        AniGlobalMethod::GetInstance().pathCtor);
    if (IsUndefined(env, aniObj)) {
        ROSEN_LOGE("AniPath::Offset failed cause aniObj is undefined");
        return aniObj;
    }
    AniPath* newAniPath = new AniPath(path);
    ani_status ret = env->Object_SetField_Long(
        aniObj, AniGlobalField::GetInstance().pathNativeObj, reinterpret_cast<ani_long>(newAniPath));
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPath::Offset create new path failed %{public}d", ret);
        delete newAniPath;
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param path.");
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniPath::AddArc(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double startAngle, ani_double endAngle)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param rect.");
        return;
    }
    aniPath->GetPath()->AddArc(drawingRect, startAngle, endAngle);
}

void AniPath::RMoveTo(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniPath->GetPath()->RMoveTo(dx, dy);
}

ani_boolean AniPath::GetPositionAndTangent(ani_env* env, ani_object obj, ani_boolean forceClosed,
    ani_double distance, ani_object aniPosition, ani_object aniTangent)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    Drawing::Point position;
    if (GetPointFromPointObj(env, aniPosition, position) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param position.");
        return false;
    }

    Drawing::Point tangent;
    if (GetPointFromPointObj(env, aniTangent, tangent) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param tangent.");
        return false;
    }
    bool result = aniPath->GetPath()->GetPositionAndTangent(distance, position, tangent, forceClosed);
    DrawingPointConvertToAniPoint(env, aniPosition, position);
    DrawingPointConvertToAniPoint(env, aniTangent, tangent);
    return static_cast<ani_boolean>(result);
}

ani_double AniPath::GetLength(ani_env* env, ani_object obj, ani_boolean forceClosed)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return 0.0;
    }
    return aniPath->GetPath()->GetLength(forceClosed);
}

ani_boolean AniPath::Op(ani_env* env, ani_object obj, ani_object aniPathObj, ani_enum_item aniPathOpEnum)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return false;
    }

    auto aniNewPath = GetNativeFromObj<AniPath>(env, aniPathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniNewPath == nullptr  || aniNewPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param path.");
        return false;
    }

    ani_int pathOp;
    if (ANI_OK != env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(aniPathOpEnum), &pathOp)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param pathOp.");
        return false;
    }

    return aniPath->GetPath()->Op(*aniPath->GetPath(), *aniNewPath->GetPath(), static_cast<Drawing::PathOp>(pathOp));
}


void AniPath::AddPolygon(ani_env* env, ani_object obj, ani_array aniPointArray, ani_boolean aniClose)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    std::vector<Drawing::Point> points;
    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(aniPointArray, &aniLength)) {
        ROSEN_LOGE("AniPath::AddPolygon aniPointsObj are invalid");
        return;
    }
    uint32_t pointSize = static_cast<uint32_t>(aniLength);
    if (pointSize == 0) {
        ROSEN_LOGD("AniPath::AddPolygon aniPointArray size is empty.");
        return;
    }
    points.reserve(pointSize);
    for (uint32_t i = 0; i < pointSize; i++) {
        ani_ref pointRef;
        Drawing::Point point;
        ani_status ret = env->Array_Get(aniPointArray, static_cast<ani_size>(i), &pointRef);
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniPath::AddPolygon get point from array failed: %{public}d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param points.");
            return;
        }
        if (GetPointFromPointObj(env, static_cast<ani_object>(pointRef), point) != ANI_OK) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param points.");
            return;
        }
        points.push_back(point);
    }
    aniPath->GetPath()->AddPoly(points, pointSize, aniClose);
}

void AniPath::ConicTo(ani_env* env, ani_object obj, ani_double ctrlX, ani_double ctrlY, ani_double endX,
    ani_double endY, ani_double weight)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->ConicTo(ctrlX, ctrlY, endX, endY, weight);
}

void AniPath::AddCircle(
    ani_env* env, ani_object obj, ani_double x, ani_double y, ani_double radius, ani_object aniPathDirectionObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    if (IsUndefined(env, aniPathDirectionObj)) {
        aniPath->GetPath()->AddCircle(x, y, radius);
        return;
    }
    ani_int pathDirection = 0;
    ani_status ret = env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(aniPathDirectionObj), &pathDirection);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPath::AddPolygon get pathDirection failed: %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param pathDirection.");
        return;
    }
    aniPath->GetPath()->AddCircle(x, y, radius, static_cast<PathDirection>(pathDirection));
}

void AniPath::RCubicTo(ani_env* env, ani_object obj, ani_double ctrlX1, ani_double ctrlY1,
    ani_double ctrlX2, ani_double ctrlY2, ani_double endX, ani_double endY)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->RCubicTo(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
}

void AniPath::RConicTo(ani_env* env, ani_object obj, ani_double ctrlX, ani_double ctrlY, ani_double endX,
    ani_double endY, ani_double weight)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->RConicTo(ctrlX, ctrlY, endX, endY, weight);
}

void AniPath::SetFillType(ani_env* env, ani_object obj, ani_enum_item aniPathFillType)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    ani_int pathFillType = 0;
    ani_status ret = env->EnumItem_GetValue_Int(aniPathFillType, &pathFillType);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPath::SetFillType get pathFillType failed %{public}d.", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param pathFillType.");
        return;
    }
    aniPath->GetPath()->SetFillStyle(static_cast<PathFillType>(pathFillType));
}

void AniPath::AddRect(ani_env* env, ani_object obj, ani_object aniRectObj, ani_object aniPathDirectionObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param rect.");
        return;
    }

    if (IsUndefined(env, aniPathDirectionObj)) {
        aniPath->GetPath()->AddRect(drawingRect);
        return;
    }
    ani_int pathDirection = 0;
    ani_status ret = env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(aniPathDirectionObj), &pathDirection);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPath::AddRect get pathDirection failed %{public}d.", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param pathDirection.");
        return;
    }
    aniPath->GetPath()->AddRect(drawingRect, static_cast<PathDirection>(pathDirection));
}

ani_boolean AniPath::GetMatrix(ani_env* env, ani_object obj, ani_boolean aniForceClosed, ani_double distance,
    ani_object aniMatrixObj, ani_enum_item aniPathMeasureMatrixFlags)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return false;
    }
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, aniMatrixObj, AniGlobalField::GetInstance().matrixNativeObj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param matrix.");
        return false;
    }

    ani_int pathMeasureMatrixFlags = 0;
    ani_status ret = env->EnumItem_GetValue_Int(aniPathMeasureMatrixFlags, &pathMeasureMatrixFlags);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPath::GetMatrix pathMeasureMatrixFlags param error. ret: %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param pathMeasureMatrixFlags.");
        return false;
    }
    std::shared_ptr<Matrix> matrix = aniMatrix->GetMatrix();
    auto result = aniPath->GetPath()->GetMatrix(
        aniForceClosed, distance, matrix.get(), static_cast<PathMeasureMatrixFlags>(pathMeasureMatrixFlags));
    ret = env->Object_SetField_Long(
        aniMatrixObj, AniGlobalField::GetInstance().matrixNativeObj, reinterpret_cast<ani_long>(aniMatrix));
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPath::GetMatrix failed create AniMatrix ret: %{public}d", ret);
        return false;
    }
    return result;
}

ani_object AniPath::GetBounds(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return CreateAniUndefined(env);
    }
    Drawing::Rect rect = aniPath->GetPath()->GetBounds();
    ani_object aniRectObj;
    ani_status ret = CreateRectObj(env, rect, aniRectObj);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Set AniRectObj From rect failed.");
        return CreateAniUndefined(env);
    }
    return aniRectObj;
}

void AniPath::AddOval(
    ani_env* env, ani_object obj, ani_object aniRectObj, ani_int aniStart, ani_object aniPathDirectionObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    Drawing::Rect oval;
    if (!GetRectFromAniRectObj(env, aniRectObj, oval)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param rect.");
        return;
    }
    if (aniStart < 0) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect AddOval paramter1 range. It should be greater than 0.");
        return;
    }
    if (IsUndefined(env, aniPathDirectionObj)) {
        aniPath->GetPath()->AddOval(oval, static_cast<int32_t>(aniStart));
        return;
    }
    ani_int pathDirection = 0;
    ani_status ret = env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(aniPathDirectionObj), &pathDirection);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPath::AddRect get pathDirection failed %{public}d.", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param pathDirection.");
        return;
    }
    aniPath->GetPath()->AddOval(
        oval, static_cast<int32_t>(aniStart), static_cast<Drawing::PathDirection>(pathDirection));
}

ani_boolean AniPath::Contains(ani_env* env, ani_object obj, ani_double x, ani_double y)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return false;
    }
    return aniPath->GetPath()->Contains(x, y);
}

void AniPath::AddPath(ani_env* env, ani_object obj, ani_object aniPathObj, ani_object aniMatrixObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    auto aniNewPath = GetNativeFromObj<AniPath>(env, aniPathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniNewPath == nullptr  || aniNewPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param path.");
        return;
    }

    if (IsUndefined(env, aniMatrixObj) || IsNull(env, aniMatrixObj)) {
        aniPath->GetPath()->AddPath(*aniNewPath->GetPath(), Drawing::Matrix());
        return;
    }
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, aniMatrixObj, AniGlobalField::GetInstance().matrixNativeObj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param matrix.");
        return;
    }
    aniPath->GetPath()->AddPath(*aniNewPath->GetPath(), *aniMatrix->GetMatrix());
}

ani_boolean AniPath::IsClosed(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return false;
    }
    return aniPath->GetPath()->IsClosed(false);
}

ani_boolean AniPath::BuildFromSVGString(ani_env* env, ani_object obj, ani_string aniStringObj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return false;
    }
    std::string str = CreateStdString(env, aniStringObj);
    return aniPath->GetPath()->BuildFromSVGString(str);
}

void AniPath::CubicTo(ani_env* env, ani_object obj, ani_double ctrlX1, ani_double ctrlY1, ani_double ctrlX2,
    ani_double ctrlY2, ani_double endX, ani_double endY)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->CubicTo(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
}

void AniPath::QuadTo(ani_env* env, ani_object obj, ani_double ctrlX, ani_double ctrlY, ani_double ctrlX2,
    ani_double ctrlY2)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->QuadTo(ctrlX, ctrlY, ctrlX2, ctrlY2);
}

void AniPath::LineTo(ani_env* env, ani_object obj, ani_double x, ani_double y)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->LineTo(x, y);
}

void AniPath::MoveTo(ani_env* env, ani_object obj, ani_double x, ani_double y)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->MoveTo(x, y);
}

void AniPath::Close(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->Close();
}

ani_boolean AniPath::IsInverseFillType(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return false;
    }
    return aniPath->GetPath()->IsInverseFillType();
}

void AniPath::ToggleInverseFillType(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return;
    }
    aniPath->GetPath()->ToggleInverseFillType();
}

ani_object AniPath::Approximate(ani_env* env, ani_object obj, ani_double acceptableErrorobj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return CreateAniUndefined(env);
    }
    if (acceptableErrorobj < 0.0) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniPath::approximate acceptableError is invaild.");
        return CreateAniUndefined(env);
    }
    std::vector<scalar> points;
    aniPath->GetPath()->Approximate(static_cast<scalar>(acceptableErrorobj), points);

    uint32_t arrayLength = static_cast<uint32_t>(points.size());
    ani_array arrayObj = CreateAniArrayWithSize(env, arrayLength);
    if (arrayObj == nullptr) {
        return CreateAniUndefined(env);
    }
    
    for (size_t i = 0; i < points.size(); ++i) {
        ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().doubleCls,
            AniGlobalMethod::GetInstance().doubleCtor, points[i]);
        if (IsUndefined(env, aniObj)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniPath::approximate Failed to create width item .");
            return aniObj;
        }
        ani_status ret = env->Array_Set(arrayObj, static_cast<ani_size>(i), aniObj);
        if (ret != ANI_OK) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniPath::approximate Failed to set width item.");
            return CreateAniUndefined(env);
        }
    }
    return arrayObj;
}

void AniPath::SetLastPoint(ani_env* env, ani_object obj, ani_double x, ani_double y)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return ;
    }
    aniPath->GetPath()->SetLastPoint(x, y);
}

void AniPath::ReWind(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param.");
        return ;
    }
    aniPath->GetPath()->ReWind();
}

ani_boolean AniPath::Interpolate(ani_env* env, ani_object obj, ani_object otherobj, ani_double weight,
    ani_object interpolatedPathobj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }

    auto other = GetNativeFromObj<AniPath>(env, otherobj, AniGlobalField::GetInstance().pathNativeObj);
    if (other == nullptr || other->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param dst.");
        return false;
    }

    auto interpolatedPath = GetNativeFromObj<AniPath>(env, interpolatedPathobj,
        AniGlobalField::GetInstance().pathNativeObj);
    if (interpolatedPath == nullptr || interpolatedPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param dst.");
        return false;
    }

    return aniPath->GetPath()->Interpolate(*other->GetPath(), weight, *interpolatedPath->GetPath());
}

void AniPath::Set(ani_env* env, ani_object obj, ani_object srcobj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return ;
    }

    auto srcPath = GetNativeFromObj<AniPath>(env, srcobj, AniGlobalField::GetInstance().pathNativeObj);
    if (srcPath == nullptr || srcPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param dst.");
        return ;
    }
    aniPath->GetPath()->SetPath(*srcPath->GetPath());
}

ani_boolean AniPath::IsInterpolate(ani_env* env, ani_object obj, ani_object otherobj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }

    auto other = GetNativeFromObj<AniPath>(env, otherobj, AniGlobalField::GetInstance().pathNativeObj);
    if (other == nullptr || other->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param dst.");
        return false;
    }
    return aniPath->GetPath()->IsInterpolate(*other->GetPath());
}

ani_enum_item AniPath::GetFillType(ani_env* env, ani_object obj)
{
    ani_enum_item value = nullptr;
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return value;
    }

    PathFillType fillType = aniPath->GetPath()->GetFillStyle();
    if (!CreateAniEnumByEnumIndex(
        env, AniGlobalEnum::GetInstance().pathFillType, static_cast<ani_size>(fillType), value)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Find enum for PathFillType failed.");
        return value;
    }
    return value;
}

ani_boolean AniPath::IsEmpty(ani_env* env, ani_object obj)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, obj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr  || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    return aniPath->GetPath()->IsEmpty();
}

ani_object AniPath::PathTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniPath::PathTransferStatic failed to unwrap");
        return CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniPath::PathTransferStatic unwrapResult is null");
        return CreateAniUndefined(env);
    }
    auto jsPath = reinterpret_cast<JsPath*>(unwrapResult);
    if (jsPath->GetPathPtr() == nullptr) {
        ROSEN_LOGE("AniPath::PathTransferStatic jsPath is null");
        return CreateAniUndefined(env);
    }

    auto aniPath = new AniPath(jsPath->GetPathPtr());
    if (ANI_OK != env->Object_SetField_Long(
        output, AniGlobalField::GetInstance().pathNativeObj, reinterpret_cast<ani_long>(aniPath))) {
        ROSEN_LOGE("AniPath::PathTransferStatic failed create aniPath");
        delete aniPath;
        return CreateAniUndefined(env);
    }
    return output;
}

ani_long AniPath::GetPathAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, input, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ROSEN_LOGE("AniPath::GetPathAddr aniPath is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniPath->GetPathPtrAddr());
}

std::shared_ptr<Path>* AniPath::GetPathPtrAddr()
{
    return &path_;
}

std::shared_ptr<Path> AniPath::GetPath()
{
    return path_;
}

AniPath::~AniPath()
{
    path_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen