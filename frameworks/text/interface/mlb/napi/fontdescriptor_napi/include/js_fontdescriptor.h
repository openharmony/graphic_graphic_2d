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

#ifndef OHOS_ROSEN_JS_FONTDESCRIPTOR_H
#define OHOS_ROSEN_JS_FONTDESCRIPTOR_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <set>

#include "font_parser.h"

namespace OHOS::Rosen {
using FontDescSharedPtr = std::shared_ptr<TextEngine::FontParser::FontDescriptor>;
class JsFontDescriptor final {
public:
    JsFontDescriptor();
    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value MatchFontDescriptorsAsync(napi_env env, napi_callback_info info);
    static napi_value CreateFontDescriptorArray(napi_env env, std::set<FontDescSharedPtr>& result);
    static napi_value CreateFontDescriptor(napi_env env, FontDescSharedPtr& result);
    static napi_value GetSystemFontFullNamesByType(napi_env env, napi_callback_info info);
    static napi_value GetFontDescriptorByFullName(napi_env env, napi_callback_info info);
    static napi_value CreateFontList(napi_env env, std::unordered_set<std::string>& fontList);

private:
    template <typename T>
    static bool CheckAndConvertProperty(napi_env env, napi_value obj, const std::string& fieldName, T& out);
    static bool CreateAndSetProperties(napi_env env, napi_value fontDescriptor, FontDescSharedPtr item);
    static bool SetProperty(napi_env env, napi_value object, const char* name, napi_value value);
    static bool ParseFontDescWeight(napi_env env, napi_value obj, int& weight);
    static bool ConvertFontDescWeight(napi_env env, napi_value obj, int weight);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_FONTDESCRIPTOR_H