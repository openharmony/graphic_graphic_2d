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

#ifndef OHOS_ROSEN_JS_TOOL_H
#define OHOS_ROSEN_JS_TOOL_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "resource_manager.h"

namespace OHOS::Rosen {
namespace Drawing {

struct ResourceInfo {
    int32_t resId = 0;
    int32_t type = 0;
    std::vector<std::string> params;
    std::string bundleName;
    std::string moduleName;
};

enum class ResourceType {
    RAWFILE = 30000,
};

class JsTool final {
public:
    static size_t GetParamLen(napi_env env, napi_value param);
    static std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager();
    static bool GetResourceType(napi_env env, napi_value value, ResourceInfo& info);
    static bool GetResourcePartData(napi_env env, ResourceInfo& info, napi_value paramsNApi,
        napi_value bundleNameNApi, napi_value moduleNameNApi);
    static bool GetResourceRawFileDataBuffer(std::unique_ptr<uint8_t[]>&& buffer,
        size_t* len, ResourceInfo& info);
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_TOOL_H