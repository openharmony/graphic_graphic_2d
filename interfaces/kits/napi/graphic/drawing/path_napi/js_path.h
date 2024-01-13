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
    static napi_value MoveTo(napi_env env, napi_callback_info info);
    static napi_value LineTo(napi_env env, napi_callback_info info);
    static napi_value ArcTo(napi_env env, napi_callback_info info);
    static napi_value QuadTo(napi_env env, napi_callback_info info);
    static napi_value CubicTo(napi_env env, napi_callback_info info);
    static napi_value Close(napi_env env, napi_callback_info info);
    static napi_value Reset(napi_env env, napi_callback_info info);
    Path* GetPath();

private:
    napi_value OnMoveTo(napi_env env, napi_callback_info info);
    napi_value OnLineTo(napi_env env, napi_callback_info info);
    napi_value OnArcTo(napi_env env, napi_callback_info info);
    napi_value OnQuadTo(napi_env env, napi_callback_info info);
    napi_value OnCubicTo(napi_env env, napi_callback_info info);
    napi_value OnClose(napi_env env, napi_callback_info info);
    napi_value OnReset(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    Path* m_path = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PATH_H