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

#ifndef OHOS_ROSEN_ANI_PATH_H
#define OHOS_ROSEN_ANI_PATH_H

#include "ani_drawing_utils.h"
#include "draw/path.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniPath final {
public:
    explicit AniPath(std::shared_ptr<Path> path = nullptr) : path_(path) {}
    ~AniPath();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithPath(ani_env* env, ani_object obj, ani_object aniPathObj);
    static void ArcTo(ani_env* env, ani_object obj, ani_double x1, ani_double y1, ani_double x2,
        ani_double y2, ani_double startDeg, ani_double sweepDeg);
    static void Reset(ani_env* env, ani_object obj);
    static ani_boolean IsRect(ani_env* env, ani_object obj, ani_object aniRectObj);
    static ani_boolean GetSegment(ani_env* env, ani_object obj, ani_boolean forceClosed, ani_double start,
        ani_double stop, ani_boolean startWithMoveTo, ani_object aniPathObj);
    static ani_object GetPathIterator(ani_env* env, ani_object obj);
    static void RLineTo(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static void AddRoundRect(ani_env* env, ani_object obj, ani_object aniRoundRectObj, ani_object aniPathDirection);
    static void RQuadTo(ani_env* env, ani_object obj, ani_double dx1, ani_double dy1, ani_double dx2, ani_double dy2);
    static void Transform(ani_env* env, ani_object obj, ani_object aniMatrix);
    static ani_object Offset(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static void AddArc(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double startAngle, ani_double endAngle);
    static void RMoveTo(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static ani_boolean GetPositionAndTangent(ani_env* env, ani_object obj, ani_boolean forceClosed,
        ani_double distance, ani_object aniPosition, ani_object aniTangent);
    static ani_double GetLength(ani_env* env, ani_object obj, ani_boolean forceClosed);
    static ani_boolean Op(ani_env* env, ani_object obj, ani_object aniPathObj, ani_enum_item aniPathOpEnum);
    static void AddPolygon(ani_env* env, ani_object obj, ani_array aniPointArray, ani_boolean aniClose);
    static void ConicTo(ani_env* env, ani_object obj, ani_double ctrlX, ani_double ctrlY, ani_double endX,
        ani_double endY, ani_double weight);
    static void AddCircle(ani_env* env, ani_object obj,
        ani_double x, ani_double y, ani_double radius, ani_object aniPathDirectionObj);
    static void RCubicTo(ani_env* env, ani_object obj, ani_double ctrlX1, ani_double ctrlY1,
        ani_double ctrlX2, ani_double ctrlY2, ani_double endX, ani_double endY);
    static void RConicTo(ani_env* env, ani_object obj, ani_double ctrlX, ani_double ctrlY, ani_double endX,
        ani_double endY, ani_double weight);
    static void SetFillType(ani_env* env, ani_object obj, ani_enum_item aniPathFillType);
    static void AddRect(ani_env* env, ani_object obj, ani_object aniRectObj, ani_object aniPathDirectionObj);
    static ani_boolean GetMatrix(ani_env* env, ani_object obj, ani_boolean aniForceClosed, ani_double distance,
            ani_object aniMatrixObj, ani_enum_item aniPathMeasureMatrixFlags);
    static ani_object GetBounds(ani_env* env, ani_object obj);
    static void AddOval(
        ani_env* env, ani_object obj, ani_object aniRectObj, ani_int aniStart, ani_object aniPathDirectionObj);
    static ani_boolean Contains(ani_env* env, ani_object obj, ani_double x, ani_double y);
    static void AddPath(ani_env* env, ani_object obj, ani_object aniPathObj, ani_object aniMatrixObj);
    static ani_boolean IsClosed(ani_env* env, ani_object obj);
    static ani_boolean BuildFromSVGString(ani_env* env, ani_object obj, ani_string aniStringObj);
    static void CubicTo(ani_env* env, ani_object obj, ani_double ctrlX1, ani_double ctrlY1, ani_double ctrlX2,
        ani_double ctrlY2, ani_double endX, ani_double endY);
    static void QuadTo(ani_env* env, ani_object obj, ani_double ctrlX, ani_double ctrlY, ani_double ctrlX2,
        ani_double ctrlY2);
    static void LineTo(ani_env* env, ani_object obj, ani_double x, ani_double y);
    static void MoveTo(ani_env* env, ani_object obj, ani_double x, ani_double y);
    static void Close(ani_env* env, ani_object obj);
    static ani_boolean IsInverseFillType(ani_env* env, ani_object obj);
    static void ToggleInverseFillType(ani_env* env, ani_object obj);
    static ani_object Approximate(ani_env* env, ani_object obj, ani_double acceptableErrorobj);
    static void SetLastPoint(ani_env* env, ani_object obj, ani_double x, ani_double y);
    static void ReWind(ani_env* env, ani_object obj);
    static ani_boolean Interpolate(ani_env* env, ani_object obj, ani_object otherobj, ani_double weight,
        ani_object interpolatedPathobj);
    static ani_boolean IsEmpty(ani_env* env, ani_object obj);
    static void Set(ani_env* env, ani_object obj, ani_object srcobj);
    static ani_boolean IsInterpolate(ani_env* env, ani_object obj, ani_object otherobj);
    static ani_enum_item GetFillType(ani_env* env, ani_object obj);

    std::shared_ptr<Path> GetPath();

private:
    static ani_object PathTransferStatic(
        ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input);
    static ani_long GetPathAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<Path>* GetPathPtrAddr();
    std::shared_ptr<Path> path_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_PATH_H