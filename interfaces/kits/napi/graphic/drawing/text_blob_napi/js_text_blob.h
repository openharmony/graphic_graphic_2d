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

#ifndef OHOS_ROSEN_JS_TEXT_BLOB_H
#define OHOS_ROSEN_JS_TEXT_BLOB_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "text/text_blob.h"
#include "text/text_blob_builder.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsTextBlob final {
public:
    explicit JsTextBlob(napi_env env, std::shared_ptr<TextBlob> textBlob) : m_textBlob(textBlob) {}
    ~JsTextBlob();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsTextBlob(napi_env env, const std::shared_ptr<TextBlob> textBlob);

    static napi_value MakeFromString(napi_env env, napi_callback_info info);
    static napi_value MakeFromRunBuffer(napi_env env, napi_callback_info info);
    static napi_value Bounds(napi_env env, napi_callback_info info);

    std::shared_ptr<TextBlob> GetTextBlob();

private:
    napi_value OnBounds(napi_env env, napi_callback_info info);
    static bool OnMakeDrawingRect(napi_env& env, napi_value& argv, Rect& drawingRect, napi_valuetype& isRectNullptr);
    static bool OnMakeRunBuffer(napi_env& env, TextBlobBuilder::RunBuffer& runBuffer, uint32_t size, napi_value& array);
    std::shared_ptr<TextBlob> m_textBlob = nullptr;
    static thread_local napi_ref constructor_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_TEXT_BLOB_H