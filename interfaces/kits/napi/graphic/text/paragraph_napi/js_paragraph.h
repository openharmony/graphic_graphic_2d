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

#ifndef OHOS_ROSEN_JS_PARAGRAPH_H
#define OHOS_ROSEN_JS_PARAGRAPH_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "typography.h"

namespace OHOS::Rosen {
class JsParagraph final {
public:
    explicit JsParagraph(std::shared_ptr<Typography> paragraph);
    ~JsParagraph();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Layout(napi_env env, napi_callback_info info);
    static napi_value Paint(napi_env env, napi_callback_info info);
    static napi_value GetMaxWidth(napi_env env, napi_callback_info info);
    static napi_value GetHeight(napi_env env, napi_callback_info info);
    static napi_value GetLongestLine(napi_env env, napi_callback_info info);
    static napi_value GetMinIntrinsicWidth(napi_env env, napi_callback_info info);
    static napi_value GetMaxIntrinsicWidth(napi_env env, napi_callback_info info);
    static napi_value GetAlphabeticBaseline(napi_env env, napi_callback_info info);
    static napi_value GetIdeographicBaseline(napi_env env, napi_callback_info info);
    static napi_value GetRectsForRange(napi_env env, napi_callback_info info);
    static napi_value GetRectsForPlaceholders(napi_env env, napi_callback_info info);
    static napi_value GetGlyphPositionAtCoordinate(napi_env env, napi_callback_info info);
    static napi_value GetWordBoundary(napi_env env, napi_callback_info info);
    static napi_value GetLineCount(napi_env env, napi_callback_info info);
    static napi_value GetLineHeight(napi_env env, napi_callback_info info);
    static napi_value GetLineWidth(napi_env env, napi_callback_info info);
    static napi_value GetTextLines(napi_env env, napi_callback_info info);
    static napi_value DidExceedMaxLines(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsTypography(napi_env env, std::unique_ptr<Typography> typography);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    std::shared_ptr<Typography> GetParagraph();

private:
    static thread_local napi_ref constructor_;
    napi_value OnLayout(napi_env env, napi_callback_info info);
    napi_value OnPaint(napi_env env, napi_callback_info info);
    napi_value OnGetMaxWidth(napi_env env, napi_callback_info info);
    napi_value OnGetHeight(napi_env env, napi_callback_info info);
    napi_value OnGetLongestLine(napi_env env, napi_callback_info info);
    napi_value OnGetMinIntrinsicWidth(napi_env env, napi_callback_info info);
    napi_value OnGetMaxIntrinsicWidth(napi_env env, napi_callback_info info);
    napi_value OnGetAlphabeticBaseline(napi_env env, napi_callback_info info);
    napi_value OnGetIdeographicBaseline(napi_env env, napi_callback_info info);
    napi_value OnGetRectsForRange(napi_env env, napi_callback_info info);
    napi_value OnGetRectsForPlaceholders(napi_env env, napi_callback_info info);
    napi_value OnGetGlyphPositionAtCoordinate(napi_env env, napi_callback_info info);
    napi_value OnGetWordBoundary(napi_env env, napi_callback_info info);
    napi_value OnGetLineCount(napi_env env, napi_callback_info info);
    napi_value OnGetLineHeight(napi_env env, napi_callback_info info);
    napi_value OnGetLineWidth(napi_env env, napi_callback_info info);
    napi_value OnDidExceedMaxLines(napi_env env, napi_callback_info info);
    napi_value OnGetTextLines(napi_env env, napi_callback_info info);
    std::shared_ptr<Typography> paragraph_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PARAGRAPH_H