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

#ifndef OH_ROSEN_JS_TYPEFACE_ARGUMENTS_H
#define OH_ROSEN_JS_TYPEFACE_ARGUMENTS_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "text/font_arguments.h"

struct TypeFaceArgumentsHelper {
    struct Coordinate {
        uint32_t axis;
        float value;
    };

    int fontCollectionIndex = 0;
    std::vector<Coordinate> coordinate;
};

namespace OHOS::Rosen {
namespace Drawing {
class JsTypeFaceArguments final {
public:
    explicit JsTypeFaceArguments(std::shared_ptr<TypeFaceArgumentsHelper> typeFaceArguments)
        : m_typeFaceArguments(typeFaceArguments) {}
    ~JsTypeFaceArguments();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

    std::shared_ptr<TypeFaceArgumentsHelper> GetTypeFaceArgumentsHelper();
    static void ConvertToFontArguments(std::shared_ptr<TypeFaceArgumentsHelper> typeFaceArgumentsHelper,
        FontArguments& fontArguments);
    static uint32_t ConvertAxisToNumber(const std::string& axis);

    static napi_value AddVariation(napi_env env, napi_callback_info info);
private:
    static thread_local napi_ref constructor_;
    std::shared_ptr<TypeFaceArgumentsHelper> m_typeFaceArguments = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif //OH_ROSEN_JS_TYPEFACE_ARGUMENTS_H