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

#ifndef OHOS_ROSEN_JS_FONTCOLLECTION_H
#define OHOS_ROSEN_JS_FONTCOLLECTION_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "font_collection.h"
#include "js_text_utils.h"

namespace OHOS::Rosen {
class JsFontCollection final {
public:
    JsFontCollection();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    static napi_value DisableFallback(napi_env env, napi_callback_info info);
    static napi_value LoadFont(napi_env env, napi_callback_info info);

    std::shared_ptr<FontCollection> GetFontCollection();
private:
    static thread_local napi_ref constructor_;
    napi_value OnDisableFallback(napi_env env, napi_callback_info info);
    napi_value OnLoadFont(napi_env env, napi_callback_info info);
    bool SpiltAbsoluteFontPath(std::string& absolutePath);
    bool GetFontFileProperties(uint8_t* data, size_t& datalen, const std::string path);
    bool AddTypefaceInformation(Drawing::Typeface& typeface, const std::string familyName);
    std::shared_ptr<FontCollection> m_fontCollection = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_FONTCOLLECTION_H