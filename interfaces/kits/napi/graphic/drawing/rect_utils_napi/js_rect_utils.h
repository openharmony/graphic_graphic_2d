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

#ifndef OHOS_ROSEN_JS_RECT_UTILS_H
#define OHOS_ROSEN_JS_RECT_UTILS_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

namespace OHOS::Rosen {
namespace Drawing {
class JsRectUtils final {
public:
    JsRectUtils() = default;
    ~JsRectUtils() = default;

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

    static napi_value CenterX(napi_env env, napi_callback_info info);
    static napi_value CenterY(napi_env env, napi_callback_info info);
    static napi_value Contains(napi_env env, napi_callback_info info);
    static napi_value GetHeight(napi_env env, napi_callback_info info);
    static napi_value GetWidth(napi_env env, napi_callback_info info);
    static napi_value Inset(napi_env env, napi_callback_info info);
    static napi_value Intersect(napi_env env, napi_callback_info info);
    static napi_value IsEmpty(napi_env env, napi_callback_info info);
    static napi_value IsEqual(napi_env env, napi_callback_info info);
    static napi_value IsIntersect(napi_env env, napi_callback_info info);
    static napi_value MakeCopy(napi_env env, napi_callback_info info);
    static napi_value MakeEmpty(napi_env env, napi_callback_info info);
    static napi_value MakeLtrb(napi_env env, napi_callback_info info);
    static napi_value Offset(napi_env env, napi_callback_info info);
    static napi_value OffsetTo(napi_env env, napi_callback_info info);
    static napi_value SetEmpty(napi_env env, napi_callback_info info);
    static napi_value SetLtrb(napi_env env, napi_callback_info info);
    static napi_value SetRect(napi_env env, napi_callback_info info);
    static napi_value Sort(napi_env env, napi_callback_info info);
    static napi_value Union(napi_env env, napi_callback_info info);

private:
    static thread_local napi_ref constructor_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_RECT_UTILS_H
