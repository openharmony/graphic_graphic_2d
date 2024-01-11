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

#include <hilog/log.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "draw/canvas.h"
#include "image/image.h"
#include "draw/path.h"
#include "text/text.h"
#include "text/text_blob.h"
#include "utils/point.h"
#include "utils/sampling_options.h"
#include "utils/rect.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsCanvas final {
public:
    explicit JsCanvas(Canvas* canvas) : m_canvas(canvas) {};
    ~JsCanvas();

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsCanvas(napi_env env, Canvas* canvas);

    static napi_value Init(napi_env env, napi_value exportObj);

    static napi_value DrawRect(napi_env env, napi_callback_info info);
    static napi_value DrawCircle(napi_env env, napi_callback_info info);
    static napi_value DrawImage(napi_env env, napi_callback_info info);
    static napi_value DrawColor(napi_env env, napi_callback_info info);
    static napi_value DrawPoint(napi_env env, napi_callback_info info);
    static napi_value DrawPath(napi_env env, napi_callback_info info);
    static napi_value DrawLine(napi_env env, napi_callback_info info);
    static napi_value DrawText(napi_env env, napi_callback_info info);
    static napi_value AttachPen(napi_env env, napi_callback_info info);
    static napi_value AttachBrush(napi_env env, napi_callback_info info);
    static napi_value DetachPen(napi_env env, napi_callback_info info);
    static napi_value DetachBrush(napi_env env, napi_callback_info info);

    Canvas* GetCanvas();

private:
    napi_value OnDrawRect(napi_env env, napi_callback_info info);
    napi_value OnDrawCircle(napi_env env, napi_callback_info info);
    napi_value OnDrawImage(napi_env env, napi_callback_info info);
    napi_value OnDrawColor(napi_env env, napi_callback_info info);
    napi_value OnDrawPoint(napi_env env, napi_callback_info info);
    napi_value OnDrawPath(napi_env env, napi_callback_info info);
    napi_value OnDrawLine(napi_env env, napi_callback_info info);
    napi_value OnDrawText(napi_env env, napi_callback_info info);

    static bool DeclareFuncAndCreateConstructor(napi_env env);
    static thread_local napi_ref constructor_;
    Canvas* m_canvas = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_CANVAS_H