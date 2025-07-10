/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_JS_PATH_H
#define OHOS_ROSEN_JS_PATH_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "draw/path.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsPath final {
public:
    explicit JsPath(Path* path) : m_path(path) {};
    ~JsPath();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsPath(napi_env env, Path* path);
    static napi_value MoveTo(napi_env env, napi_callback_info info);
    static napi_value LineTo(napi_env env, napi_callback_info info);
    static napi_value ArcTo(napi_env env, napi_callback_info info);
    static napi_value QuadTo(napi_env env, napi_callback_info info);
    static napi_value ConicTo(napi_env env, napi_callback_info info);
    static napi_value CubicTo(napi_env env, napi_callback_info info);
    static napi_value RMoveTo(napi_env env, napi_callback_info info);
    static napi_value RLineTo(napi_env env, napi_callback_info info);
    static napi_value RQuadTo(napi_env env, napi_callback_info info);
    static napi_value RConicTo(napi_env env, napi_callback_info info);
    static napi_value RCubicTo(napi_env env, napi_callback_info info);
    static napi_value AddPolygon(napi_env env, napi_callback_info info);
    static napi_value AddOval(napi_env env, napi_callback_info info);
    static napi_value AddCircle(napi_env env, napi_callback_info info);
    static napi_value AddArc(napi_env env, napi_callback_info info);
    static napi_value AddRect(napi_env env, napi_callback_info info);
    static napi_value AddRoundRect(napi_env env, napi_callback_info info);
    static napi_value AddPath(napi_env env, napi_callback_info info);
    static napi_value Transform(napi_env env, napi_callback_info info);
    static napi_value Contains(napi_env env, napi_callback_info info);
    static napi_value SetFillType(napi_env env, napi_callback_info info);
    static napi_value GetBounds(napi_env env, napi_callback_info info);
    static napi_value Close(napi_env env, napi_callback_info info);
    static napi_value Offset(napi_env env, napi_callback_info info);
    static napi_value Reset(napi_env env, napi_callback_info info);
    static napi_value Op(napi_env env, napi_callback_info info);
    static napi_value GetLength(napi_env env, napi_callback_info info);
    static napi_value GetPositionAndTangent(napi_env env, napi_callback_info info);
    static napi_value GetSegment(napi_env env, napi_callback_info info);
    static napi_value Set(napi_env env, napi_callback_info info);
    static napi_value GetFillType(napi_env env, napi_callback_info info);
    static napi_value SetLastPoint(napi_env env, napi_callback_info info);
    static napi_value ReWind(napi_env env, napi_callback_info info);
    static napi_value GetMatrix(napi_env env, napi_callback_info info);
    static napi_value BuildFromSvgString(napi_env env, napi_callback_info info);
    static napi_value IsClosed(napi_env env, napi_callback_info info);
    static napi_value IsEmpty(napi_env env, napi_callback_info info);
    static napi_value IsRect(napi_env env, napi_callback_info info);
    static napi_value GetPathIterator(napi_env env, napi_callback_info info);
    static napi_value Approximate(napi_env env, napi_callback_info info);
    static napi_value Interpolate(napi_env env, napi_callback_info info);
    static napi_value IsInterpolate(napi_env env, napi_callback_info info);
    Path* GetPath();

private:
    napi_value OnMoveTo(napi_env env, napi_callback_info info);
    napi_value OnLineTo(napi_env env, napi_callback_info info);
    napi_value OnArcTo(napi_env env, napi_callback_info info);
    napi_value OnQuadTo(napi_env env, napi_callback_info info);
    napi_value OnConicTo(napi_env env, napi_callback_info info);
    napi_value OnCubicTo(napi_env env, napi_callback_info info);
    napi_value OnRMoveTo(napi_env env, napi_callback_info info);
    napi_value OnRLineTo(napi_env env, napi_callback_info info);
    napi_value OnRQuadTo(napi_env env, napi_callback_info info);
    napi_value OnRConicTo(napi_env env, napi_callback_info info);
    napi_value OnRCubicTo(napi_env env, napi_callback_info info);
    napi_value OnAddPolygon(napi_env env, napi_callback_info info);
    napi_value OnAddOval(napi_env env, napi_callback_info info);
    napi_value OnAddCircle(napi_env env, napi_callback_info info);
    napi_value OnAddArc(napi_env env, napi_callback_info info);
    napi_value OnAddRect(napi_env env, napi_callback_info info);
    napi_value OnAddRoundRect(napi_env env, napi_callback_info info);
    napi_value OnAddPath(napi_env env, napi_callback_info info);
    napi_value OnTransform(napi_env env, napi_callback_info info);
    napi_value OnContains(napi_env env, napi_callback_info info);
    napi_value OnSetFillType(napi_env env, napi_callback_info info);
    napi_value OnGetBounds(napi_env env, napi_callback_info info);
    napi_value OnClose(napi_env env, napi_callback_info info);
    napi_value OnOffset(napi_env env, napi_callback_info info);
    napi_value OnReset(napi_env env, napi_callback_info info);
    napi_value OnOp(napi_env env, napi_callback_info info);
    napi_value OnGetLength(napi_env env, napi_callback_info info);
    napi_value OnGetPositionAndTangent(napi_env env, napi_callback_info info);
    napi_value OnGetSegment(napi_env env, napi_callback_info info);
    napi_value OnSet(napi_env env, napi_callback_info info);
    napi_value OnGetFillType(napi_env env, napi_callback_info info);
    napi_value OnSetLastPoint(napi_env env, napi_callback_info info);
    napi_value OnReWind(napi_env env, napi_callback_info info);
    napi_value OnIsRect(napi_env env, napi_callback_info info);
    napi_value OnIsEmpty(napi_env env, napi_callback_info info);
    napi_value OnGetMatrix(napi_env env, napi_callback_info info);
    napi_value OnBuildFromSvgString(napi_env env, napi_callback_info info);
    napi_value OnIsClosed(napi_env env, napi_callback_info info);
    napi_value OnGetPathIterator(napi_env env, napi_callback_info info);
    napi_value OnApproximate(napi_env env, napi_callback_info info);
    napi_value OnInterpolate(napi_env env, napi_callback_info info);
    napi_value OnIsInterpolate(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    Path* m_path = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PATH_H
