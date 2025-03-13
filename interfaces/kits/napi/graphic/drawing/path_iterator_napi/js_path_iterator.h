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

#ifndef OHOS_ROSEN_JS_PATH_ITERATOR_H
#define OHOS_ROSEN_JS_PATH_ITERATOR_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "draw/path_iterator.h"
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsPathIterator final {
public:
    explicit JsPathIterator(PathIterator* iter) : m_iter(iter) {};
    ~JsPathIterator();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsPathIterator(napi_env env, PathIterator* iter);
    static napi_value Next(napi_env env, napi_callback_info info);
    static napi_value HasNext(napi_env env, napi_callback_info info);
    static napi_value Peek(napi_env env, napi_callback_info info);
    PathIterator* GetPathIterator();

private:
    napi_value OnNext(napi_env env, napi_callback_info info);
    napi_value OnHasNext(napi_env env, napi_callback_info info);
    napi_value OnPeek(napi_env env, napi_callback_info info);

    PathVerb nextInternal();
    PathVerb getReturnVerb(const PathVerb& cachedVerb);

    static thread_local napi_ref constructor_;
    PathIterator* m_iter = nullptr;
    PathVerb m_verb = PathVerb::UNINIT;
    bool m_done = false;
    Point m_points[MAX_PAIRS_PATHVERB] = {{0, 0}};
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PATH_ITERATOR_H
