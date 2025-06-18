/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_JS_CANVAS_H
#define OHOS_ROSEN_JS_CANVAS_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#ifdef ROSEN_OHOS
#include "pixel_map.h"
#endif

namespace OHOS::Rosen {
namespace Drawing {

#ifndef DRAWING_API
#ifdef _WIN32
#define DRAWING_EXPORT __attribute__((dllexport))
#define DRAWING_IMPORT __attribute__((dllimport))
#else
#define DRAWING_EXPORT __attribute__((visibility("default")))
#define DRAWING_IMPORT __attribute__((visibility("default")))
#endif
#ifdef MODULE_DRAWING
#define DRAWING_API DRAWING_EXPORT
#else
#define DRAWING_API DRAWING_IMPORT
#endif
#endif

class Canvas;
class JsCanvas final {
public:
    explicit JsCanvas(Canvas* canvas, bool owned = false) : m_canvas(canvas), owned_(owned) {};
    ~JsCanvas();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);

    DRAWING_API static napi_value CreateJsCanvas(napi_env env, Canvas* canvas);

    static napi_value AttachBrush(napi_env env, napi_callback_info info);
    static napi_value AttachPen(napi_env env, napi_callback_info info);
    static napi_value Clear(napi_env env, napi_callback_info info);
    static napi_value ClipPath(napi_env env, napi_callback_info info);
    static napi_value ClipRegion(napi_env env, napi_callback_info info);
    static napi_value ClipRect(napi_env env, napi_callback_info info);
    static napi_value ConcatMatrix(napi_env env, napi_callback_info info);
    static napi_value DrawArc(napi_env env, napi_callback_info info);
    static napi_value DrawArcWithCenter(napi_env env, napi_callback_info info);
    static napi_value ClipRoundRect(napi_env env, napi_callback_info info);
    static napi_value DrawCircle(napi_env env, napi_callback_info info);
    static napi_value DrawColor(napi_env env, napi_callback_info info);
    static napi_value DrawImage(napi_env env, napi_callback_info info);
    static napi_value DrawImageLattice(napi_env env, napi_callback_info info);
    static napi_value DrawImageNine(napi_env env, napi_callback_info info);
    static napi_value DrawImageRect(napi_env env, napi_callback_info info);
    static napi_value DrawImageRectWithSrc(napi_env env, napi_callback_info info);
    static napi_value DrawLine(napi_env env, napi_callback_info info);
    static napi_value DrawRect(napi_env env, napi_callback_info info);
    static napi_value DrawOval(napi_env env, napi_callback_info info);
    static napi_value DrawPoint(napi_env env, napi_callback_info info);
    static napi_value DrawPoints(napi_env env, napi_callback_info info);
    static napi_value DrawPath(napi_env env, napi_callback_info info);
    static napi_value DrawPixelMapMesh(napi_env env, napi_callback_info info);
    static napi_value DrawShadow(napi_env env, napi_callback_info info);
    static napi_value DrawRegion(napi_env env, napi_callback_info info);
    static napi_value DrawSingleCharacter(napi_env env, napi_callback_info info);
    static napi_value DrawSingleCharacterWithFeatures(napi_env env, napi_callback_info info);
    static napi_value DrawText(napi_env env, napi_callback_info info);
    static napi_value DrawBackground(napi_env env, napi_callback_info info);
    static napi_value DrawRoundRect(napi_env env, napi_callback_info info);
    static napi_value GetTotalMatrix(napi_env env, napi_callback_info info);
    static napi_value DrawNestedRoundRect(napi_env env, napi_callback_info info);
    static napi_value DetachBrush(napi_env env, napi_callback_info info);
    static napi_value DetachPen(napi_env env, napi_callback_info info);
    static napi_value GetSaveCount(napi_env env, napi_callback_info info);
    static napi_value GetWidth(napi_env env, napi_callback_info info);
    static napi_value GetHeight(napi_env env, napi_callback_info info);
    static napi_value IsClipEmpty(napi_env env, napi_callback_info info);
    static napi_value Rotate(napi_env env, napi_callback_info info);
    static napi_value RestoreToCount(napi_env env, napi_callback_info info);
    static napi_value Restore(napi_env env, napi_callback_info info);
    static napi_value Skew(napi_env env, napi_callback_info info);
    static napi_value Save(napi_env env, napi_callback_info info);
    static napi_value SaveLayer(napi_env env, napi_callback_info info);
    static napi_value Scale(napi_env env, napi_callback_info info);
    static napi_value SetMatrix(napi_env env, napi_callback_info info);
    static napi_value ResetMatrix(napi_env env, napi_callback_info info);
    static napi_value Translate(napi_env env, napi_callback_info info);
    static napi_value GetLocalClipBounds(napi_env env, napi_callback_info info);
    static napi_value QuickRejectPath(napi_env env, napi_callback_info info);
    static napi_value QuickRejectRect(napi_env env, napi_callback_info info);

    Canvas* GetCanvas();
    DRAWING_API void ResetCanvas();
    DRAWING_API void ClipCanvas(float width, float height);
    DRAWING_API void SaveCanvas();
    DRAWING_API void RestoreCanvas();

private:
    napi_value OnClear(napi_env env, napi_callback_info info);
    napi_value OnClipPath(napi_env env, napi_callback_info info);
    napi_value OnClipRegion(napi_env env, napi_callback_info info);
    napi_value OnClipRect(napi_env env, napi_callback_info info);
    napi_value OnConcatMatrix(napi_env env, napi_callback_info info);
    napi_value OnDrawArc(napi_env env, napi_callback_info info);
    napi_value OnDrawArcWithCenter(napi_env env, napi_callback_info info);
    napi_value OnClipRoundRect(napi_env env, napi_callback_info info);
    napi_value OnDrawCircle(napi_env env, napi_callback_info info);
    napi_value OnDrawColor(napi_env env, napi_callback_info info);
    napi_value OnDrawImage(napi_env env, napi_callback_info info);
    napi_value OnDrawImageLattice(napi_env env, napi_callback_info info);
    napi_value OnDrawImageNine(napi_env env, napi_callback_info info);
    napi_value OnDrawImageRect(napi_env env, napi_callback_info info);
    napi_value OnDrawImageRectWithSrc(napi_env env, napi_callback_info info);
    napi_value OnDrawLine(napi_env env, napi_callback_info info);
    napi_value OnDrawRect(napi_env env, napi_callback_info info);
    napi_value OnDrawOval(napi_env env, napi_callback_info info);
    napi_value OnDrawPoint(napi_env env, napi_callback_info info);
    napi_value OnDrawPoints(napi_env env, napi_callback_info info);
    napi_value OnDrawPath(napi_env env, napi_callback_info info);
    napi_value OnDrawPixelMapMesh(napi_env env, napi_callback_info info);
    napi_value OnDrawShadow(napi_env env, napi_callback_info info);
    napi_value OnDrawRegion(napi_env env, napi_callback_info info);
    napi_value OnDrawSingleCharacter(napi_env env, napi_callback_info info);
    napi_value OnDrawSingleCharacterWithFeatures(napi_env env, napi_callback_info info);
    napi_value OnDrawText(napi_env env, napi_callback_info info);
    napi_value OnGetSaveCount(napi_env env, napi_callback_info info);
    napi_value OnGetWidth(napi_env env, napi_callback_info info);
    napi_value OnGetHeight(napi_env env, napi_callback_info info);
    napi_value OnIsClipEmpty(napi_env env, napi_callback_info info);
    napi_value OnRotate(napi_env env, napi_callback_info info);
    napi_value OnRestoreToCount(napi_env env, napi_callback_info info);
    napi_value OnRestore(napi_env env, napi_callback_info info);
    napi_value OnSkew(napi_env env, napi_callback_info info);
    napi_value OnSave(napi_env env, napi_callback_info info);
    napi_value OnSaveLayer(napi_env env, napi_callback_info info);
    napi_value OnScale(napi_env env, napi_callback_info info);
    napi_value OnSetMatrix(napi_env env, napi_callback_info info);
    napi_value OnResetMatrix(napi_env env, napi_callback_info info);
    napi_value OnTranslate(napi_env env, napi_callback_info info);
    napi_value OnDrawBackground(napi_env env, napi_callback_info info);
    napi_value OnDrawRoundRect(napi_env env, napi_callback_info info);
    napi_value OnDrawNestedRoundRect(napi_env env, napi_callback_info info);
    napi_value OnGetLocalClipBounds(napi_env env, napi_callback_info info);
    napi_value OnGetTotalMatrix(napi_env env, napi_callback_info info);
    napi_value OnQuickRejectPath(napi_env env, napi_callback_info info);
    napi_value OnQuickRejectRect(napi_env env, napi_callback_info info);

    static bool CreateConstructor(napi_env env);
    static thread_local napi_ref constructor_;
    Canvas* m_canvas = nullptr;
    bool owned_ = false;
#ifdef ROSEN_OHOS
    std::shared_ptr<Media::PixelMap> mPixelMap_ = nullptr;
#endif
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_CANVAS_H