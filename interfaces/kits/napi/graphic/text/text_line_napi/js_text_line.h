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

#ifndef OHOS_ROSEN_JS_TEXT_LINE_H
#define OHOS_ROSEN_JS_TEXT_LINE_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "js_text_utils.h"
#include "text_line_base.h"
#include "typography_style.h"
#include "typography.h"

namespace OHOS::Rosen {
class JsTextLine final {
public:
    JsTextLine();

    void SetTextLine(std::unique_ptr<TextLineBase> textLine);
    static napi_value CreateTextLine(napi_env env, napi_callback_info info);
    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value GetGlyphCount(napi_env env, napi_callback_info info);
    static napi_value GetGlyphRuns(napi_env env, napi_callback_info info);
    static napi_value GetTextRange(napi_env env, napi_callback_info info);
    static napi_value Paint(napi_env env, napi_callback_info info);
    std::unique_ptr<TextLineBase> GetTextLineBase();
    void SetParagraph(std::shared_ptr<Typography> paragraph);

private:
    napi_value OnGetGlyphCount(napi_env env, napi_callback_info info);
    napi_value OnGetGlyphRuns(napi_env env, napi_callback_info info);
    napi_value OnGetTextRange(napi_env env, napi_callback_info info);
    napi_value OnPaint(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    std::unique_ptr<TextLineBase> textLine_ = nullptr;
    std::shared_ptr<Typography> paragraph_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_TEXT_LINE_H