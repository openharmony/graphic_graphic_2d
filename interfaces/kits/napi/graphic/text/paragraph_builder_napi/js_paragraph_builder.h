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

#ifndef OHOS_ROSEN_JS_PARAGRAPH_BUILDER_H
#define OHOS_ROSEN_JS_PARAGRAPH_BUILDER_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "typography_create.h"
#include "text_style.h"
#include "font_collection.h"

namespace OHOS::Rosen {
class JsParagraphBuilder final {
public:
    JsParagraphBuilder() {}
    void SetTypographyCreate(std::unique_ptr<TypographyCreate> typographyCreate);

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    static napi_value PushStyle(napi_env env, napi_callback_info info);
    static napi_value AddText(napi_env env, napi_callback_info info);
    static napi_value PopStyle(napi_env env, napi_callback_info info);
    static napi_value AddPlaceholder(napi_env env, napi_callback_info info);
    static napi_value Build(napi_env env, napi_callback_info info);
    std::unique_ptr<TypographyCreate> GetTypographyCreate();

    std::shared_ptr<FontCollection> GetFontCollection();
private:
    napi_value OnPushStyle(napi_env env, napi_callback_info info);
    napi_value OnAddText(napi_env env, napi_callback_info info);
    napi_value OnPopStyle(napi_env env, napi_callback_info info);
    napi_value OnAddPlaceholder(napi_env env, napi_callback_info info);
    napi_value OnBuild(napi_env env, napi_callback_info info);
    static thread_local napi_ref constructor_;
    std::unique_ptr<TypographyCreate> typographyCreate_ = nullptr;
    std::shared_ptr<FontCollection> fontCollection_ = nullptr;
    
};

} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PARAGRAPH_BUILDER_H