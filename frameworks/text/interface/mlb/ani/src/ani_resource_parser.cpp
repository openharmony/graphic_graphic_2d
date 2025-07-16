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

#include "ability.h"
#include "ani_resource_parser.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
namespace {
std::vector<std::string> AniToStdVectorString(ani_env* env, ani_array array)
{
    std::vector<std::string> result;

    ani_size length;
    ani_status ret = env->Array_GetLength(array, &length);
    if (ret != ANI_OK) {
        return result;
    }
    for (ani_size i = 0; i < length; i++) {
        ani_ref aniString = nullptr;
        ret = env->Array_Get_Ref(reinterpret_cast<ani_array_ref>(array), i, &aniString);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to get array element %{public}zu", i);
            continue;
        }
        std::string utf8Str;
        ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(aniString), utf8Str);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to get str %{public}zu", i);
            continue;
        }
        result.push_back(utf8Str);
    }

    return result;
}
} // namespace

AniResource AniResourceParser::ParseResource(ani_env* env, ani_object obj)
{
    AniResource result;
    ani_double aniId = 0;
    ani_ref aniBundleName{nullptr};
    ani_ref aniModuleName{nullptr};
    ani_ref aniParams{nullptr};
    ani_double aniType = 0;
    env->Object_GetPropertyByName_Double(obj, "id", &aniId);
    env->Object_GetPropertyByName_Ref(obj, "bundleName", &aniBundleName);
    env->Object_GetPropertyByName_Ref(obj, "moduleName", &aniModuleName);
    env->Object_GetPropertyByName_Ref(obj, "params", &aniParams);
    env->Object_GetPropertyByName_Double(obj, "type", &aniType);

    result.type = static_cast<int32_t>(aniType);
    result.id = static_cast<int32_t>(aniId);
    if (aniBundleName != nullptr) {
        AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(aniBundleName), result.bundleName);
    }
    if (aniModuleName != nullptr) {
        AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(aniModuleName), result.moduleName);
    }
    if (aniParams != nullptr) {
        result.params = AniToStdVectorString(env, reinterpret_cast<ani_array>(aniParams));
    }

    return result;
}

bool AniResourceParser::ResolveResource(const AniResource& resource, size_t& dataLen, std::unique_ptr<uint8_t[]>& data)
{
    auto context = AbilityRuntime::ApplicationContext::GetApplicationContext();
    TEXT_ERROR_CHECK(context != nullptr, return false, "Failed to get application context");
    auto resourceManager = context->GetResourceManager();
    TEXT_ERROR_CHECK(resourceManager != nullptr, return false, "Failed to get resource manager");

    if (resource.type == static_cast<int32_t>(Global::Resource::ResType::STRING)) {
        std::string rPath;
        if (resource.id < 0 && !resource.params.empty() && !resource.params[0].empty()) {
            rPath = resource.params[0];
        } else {
            uint32_t state =
                static_cast<uint32_t>(resourceManager->GetStringById(static_cast<uint32_t>(resource.id), rPath));
            if (state >= static_cast<uint32_t>(Global::Resource::RState::ERROR)) {
                return false;
            }
            if (!AniTextUtils::SplitAbsoluteFontPath(rPath) || !AniTextUtils::ReadFile(rPath, dataLen, data)) {
                return false;
            }
        }
    } else if (resource.type == static_cast<int32_t>(Global::Resource::ResType::RAW)) {
        if (resource.params.empty()) {
            return false;
        }

        uint32_t state = resourceManager->GetRawFileFromHap(resource.params[0], dataLen, data);
        if (state >= static_cast<uint32_t>(Global::Resource::RState::ERROR)) {
            return false;
        }
    } else {
        TEXT_LOGE("Unsupported resource type");
    }

    return true;
}
} // namespace OHOS::Text::ANI
