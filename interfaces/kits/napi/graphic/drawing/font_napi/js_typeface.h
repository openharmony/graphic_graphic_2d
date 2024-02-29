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

#ifndef OHOS_ROSEN_JS_TYPEFACE_H
#define OHOS_ROSEN_JS_TYPEFACE_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "text/typeface.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsTypeface final {
public:
    static constexpr char ZH_CN_TTF[] = "/system/fonts/HarmonyOS_Sans_SC_Regular.ttf";

    explicit JsTypeface(std::shared_ptr<Typeface> typeface) : m_typeface(typeface) {};
    ~JsTypeface();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsTypeface(napi_env env, const std::shared_ptr<Typeface> typeface);

    static napi_value GetFamilyName(napi_env env, napi_callback_info info);

    // Default typeface does not support chinese characters, needs to load chinese character ttf file.
    static std::shared_ptr<Typeface> LoadZhCnTypeface();

    std::shared_ptr<Typeface> GetTypeface();

private:
    napi_value OnGetFamilyName(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    std::shared_ptr<Typeface> m_typeface = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_TYPEFACE_H