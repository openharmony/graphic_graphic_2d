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

#ifndef OHOS_ROSEN_JS_ROUNDRECT_H
#define OHOS_ROSEN_JS_ROUNDRECT_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "utils/round_rect.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsRoundRect final {
public:
    explicit JsRoundRect(const Drawing::Rect& rect, float x, float y)
        : m_roundRect(rect, x, y) {};
    explicit JsRoundRect(const RoundRect& rrect) : m_roundRect(RoundRect(rrect)) {};

    ~JsRoundRect() {};

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

    static napi_value SetCorner(napi_env env, napi_callback_info info);
    static napi_value GetCorner(napi_env env, napi_callback_info info);
    static napi_value Offset(napi_env env, napi_callback_info info);

    const RoundRect& GetRoundRect();
private:
    napi_value OnSetCorner(napi_env env, napi_callback_info info);
    napi_value OnGetCorner(napi_env env, napi_callback_info info);
    napi_value OnOffset(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;

    RoundRect m_roundRect;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_ROUNDRECT_H