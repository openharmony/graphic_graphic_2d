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

#include "js_drawing_utils.h"
#include "js_tool.h"

#include "ability.h"

namespace OHOS::Rosen {
namespace Drawing {
const int32_t GLOBAL_ERROR = 10000;

size_t JsTool::GetParamLen(napi_env env, napi_value param)
{
    size_t buffSize = 0;
    napi_status status = napi_get_value_string_utf8(env, param, nullptr, 0, &buffSize);
    if (status != napi_ok || buffSize == 0) {
        return 0;
    }
    return buffSize;
}

std::shared_ptr<Global::Resource::ResourceManager> JsTool::GetResourceManager()
{
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (context == nullptr) {
        ROSEN_LOGE("JsTool::Failed to get application context");
        return nullptr;
    }
    auto resourceManager = context->GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("JsTool::Failed to get resource manager");
        return nullptr;
    }
    return resourceManager;
}

bool JsTool::GetResourceInfo(napi_env env, napi_value value, ResourceInfo& info)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_object) {
        ROSEN_LOGE("JsTool::GetResourceInfo the value is not object!");
        return false;
    }

    napi_value idNApi = nullptr;
    napi_value typeNApi = nullptr;
    napi_value paramsNApi = nullptr;
    napi_get_named_property(env, value, "id", &idNApi);
    napi_get_named_property(env, value, "type", &typeNApi);
    napi_get_named_property(env, value, "params", &paramsNApi);

    napi_typeof(env, idNApi, &valueType);
    if (valueType != napi_number) {
        ROSEN_LOGE("JsTool::GetResourceInfo id is not number!");
        return false;
    }
    napi_get_value_int32(env, idNApi, &info.resId);

    napi_typeof(env, typeNApi, &valueType);
    if (valueType == napi_number) {
        ROSEN_LOGE("JsTool::GetResourceInfo type is not number!");
        return false;
    }
    napi_get_value_int32(env, typeNApi, &info.type);

    return GetResourceInfoParams(env, info, paramsNApi);
}

bool JsTool::GetResourceInfoParams(napi_env env, ResourceInfo& info, napi_value paramsNApi)
{
    napi_valuetype valueType = napi_undefined;
    bool isArray = false;
    if (napi_is_array(env, paramsNApi, &isArray) != napi_ok) {
        ROSEN_LOGE("JsTool::Failed to get array type");
        return false;
    }
    if (!isArray) {
        ROSEN_LOGE("JsTool::Invalid array type");
        return false;
    }

    // Here we use 'for' to get all params
    uint32_t arrayLength = 0;
    napi_get_array_length(env, paramsNApi, &arrayLength);
    for (uint32_t i = 0; i < arrayLength; i++) {
        size_t ret = 0;
        napi_value indexValue = nullptr;
        napi_get_element(env, paramsNApi, i, &indexValue);
        napi_typeof(env, indexValue, &valueType);
        if (valueType == napi_string) {
            size_t strlen = GetParamLen(env, indexValue) + 1;
            std::unique_ptr<char[]> indexStr = std::make_unique<char[]>(strlen);
            napi_get_value_string_utf8(env, indexValue, indexStr.get(), strlen, &ret);
            info.params.emplace_back(indexStr.get());
        } else if (valueType == napi_number) {
            int32_t num = 0;
            napi_get_value_int32(env, indexValue, &num);
            info.params.emplace_back(std::to_string(num));
        } else {
            ROSEN_LOGE("JsTool::Invalid value type %{public}d", valueType);
            return false;
        }
    }

    return true;
}

bool JsTool::GetResourceRawFileDataBuffer(std::unique_ptr<uint8_t[]>&& buffer, size_t& len, ResourceInfo& info)
{
    auto resourceManager = GetResourceManager();
    if (resourceManager == nullptr) {
        ROSEN_LOGE("JsTool::Failed to get resourceManager, resourceManager is nullptr");
        return false;
    }
    if (info.type != static_cast<int32_t>(ResourceType::RAWFILE)) {
        ROSEN_LOGE("JsTool::Invalid resource type %{public}d", info.type);
        return false;
    }

    int32_t state = 0;

    if (info.params.empty()) {
        ROSEN_LOGE("JsTool::Failed to get RawFile resource, RawFile is null");
        return false;
    }

    state = resourceManager->GetRawFileFromHap(info.params[0], len, buffer);
    if (state >= GLOBAL_ERROR || state < 0) {
        ROSEN_LOGE("JsTool::Failed to get Rawfile buffer");
        return false;
    }
    return true;
}
} // namespace Drawing
} // namespace OHOS::Rosen