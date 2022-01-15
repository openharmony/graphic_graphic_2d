/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSENRENDER_ROSEN_WEBGL_UTIL
#define ROSENRENDER_ROSEN_WEBGL_UTIL

#include <vector>
#include <string>
#include "log.h"
#include "object_manager.h"
#include "../../../common/napi/n_exporter.h"
#include "../../include/context/webgl_rendering_context_basic_base.h"
#include "../../include/context/webgl_context_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class Util {
public:
    static void SplitString(const std::string& str, std::vector<std::string>& vec, const std::string& pattern);

    static WebGLRenderingContextBasicBase *GetContextObject(napi_env env, napi_value thisVar,
        const std::string& contextType);

    static std::string GetContextAttr(const std::string& str, const std::string& key, int keyLength, int value);

    static void SetContextAttr(std::vector<std::string>& vec, WebGLContextAttributes *webGlContextAttributes);
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL_UTIL
