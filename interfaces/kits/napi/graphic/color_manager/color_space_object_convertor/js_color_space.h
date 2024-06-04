/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_COLOR_SPACE_H
#define OHOS_JS_COLOR_SPACE_H

#include <memory>

#include "color_space.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ColorManager {
void BindFunctions(napi_env env, napi_value object);
class JsColorSpace final {
public:
    explicit JsColorSpace(const std::shared_ptr<ColorSpace>& colorSpace) : colorSpaceToken_(colorSpace) {};
    ~JsColorSpace() {};
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value GetColorSpaceName(napi_env env, napi_callback_info info);
    static napi_value GetWhitePoint(napi_env env, napi_callback_info info);
    static napi_value GetGamma(napi_env env, napi_callback_info info);
    static napi_value GetSendableColorSpaceName(napi_env env, napi_callback_info info);
    static napi_value GetSendableWhitePoint(napi_env env, napi_callback_info info);
    static napi_value GetSendableGamma(napi_env env, napi_callback_info info);
    inline const std::shared_ptr<ColorSpace>& GetColorSpaceToken() const
    {
        return colorSpaceToken_;
    }

private:
    napi_value OnGetColorSpaceName(napi_env env, napi_callback_info info);
    napi_value OnGetWhitePoint(napi_env env, napi_callback_info info);
    napi_value OnGetGamma(napi_env env, napi_callback_info info);
    napi_value OnGetSendableColorSpaceName(napi_env env, napi_callback_info info);
    napi_value OnGetSendableWhitePoint(napi_env env, napi_callback_info info);
    napi_value OnGetSendableGamma(napi_env env, napi_callback_info info);

    std::shared_ptr<ColorSpace> colorSpaceToken_;
};
}  // namespace ColorManager
}  // namespace OHOS

#endif // OHOS_JS_COLOR_SPACE_H
