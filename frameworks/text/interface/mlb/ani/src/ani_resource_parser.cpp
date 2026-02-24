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

#include "ani_resource_parser.h"

#include "ability.h"
#include "ani_common.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
namespace {
constexpr int32_t RESOURCE_STRING = 10003;
constexpr int32_t RESOURCE_RAWFILE = 30000;
} // namespace

AniResource AniResourceParser::ParseResource(ani_env* env, ani_object obj)
{
    AniResource result;
    ani_long aniId = 0;
    ani_ref aniBundleName = nullptr;
    ani_ref aniModuleName = nullptr;
    ani_int aniType = 0;
    env->Object_CallMethod_Long(obj, AniGlobalMethod::GetInstance().globalResourceId, &aniId);
    env->Object_CallMethod_Ref(obj, AniGlobalMethod::GetInstance().globalResourceBundleName, &aniBundleName);
    env->Object_CallMethod_Ref(obj, AniGlobalMethod::GetInstance().globalResourceModuleName, &aniModuleName);
    AniTextUtils::ReadOptionalArrayField(
        env, obj, AniGlobalMethod::GetInstance().globalResourceParams, result.params, [](ani_env* env, ani_ref ref) {
            std::string utf8Str;
            AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), utf8Str);
            return utf8Str;
        });
    AniTextUtils::ReadOptionalIntField(env, obj, AniGlobalMethod::GetInstance().globalResourceType, aniType);

    result.type = static_cast<int32_t>(aniType);
    result.id = static_cast<int32_t>(aniId);
    if (aniBundleName != nullptr) {
        AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(aniBundleName), result.bundleName);
    }
    if (aniModuleName != nullptr) {
        AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(aniModuleName), result.moduleName);
    }

    return result;
}

AniTextResult AniResourceParser::ResolveResource(
    const AniResource& resource, size_t& dataLen, std::unique_ptr<uint8_t[]>& data)
{
    auto context = AbilityRuntime::ApplicationContext::GetApplicationContext();
    TEXT_ERROR_CHECK(context != nullptr, return AniTextResult::Invalid(), "Failed to get application context");
    auto moduleContext = context->CreateModuleContext(resource.bundleName, resource.moduleName);
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager;
    if (moduleContext != nullptr) {
        resourceManager = moduleContext->GetResourceManager();
    } else {
        TEXT_LOGW("Failed to get module context, bundle: %{public}s, module: %{public}s",
            context->GetBundleName().c_str(), resource.moduleName.c_str());
        resourceManager = context->GetResourceManager();
    }
    TEXT_ERROR_CHECK(resourceManager != nullptr, return AniTextResult::Invalid(), "Failed to get resource manager");

    if (resource.type == RESOURCE_STRING) {
        std::string rPath;
        if (resourceManager->GetStringById(static_cast<uint32_t>(resource.id), rPath) !=
            Global::Resource::RState::SUCCESS) {
            return AniTextResult::Invalid("Failed to get string");
        }
        if (!AniTextUtils::SplitAbsoluteFontPath(rPath)) {
            TEXT_LOGE("Failed to split absolute font path");
            return AniTextResult::Invalid("File path must start with 'file://', e.g., file:///path/to/font.ttf");
        }
        return AniTextUtils::ReadFile(rPath, dataLen, data);
    } else if (resource.type == RESOURCE_RAWFILE) {
        TEXT_ERROR_CHECK(!resource.params.empty(), return false, "Failed to get raw file path");
        if (resourceManager->GetRawFileFromHap(resource.params[0], dataLen, data) ==
            Global::Resource::RState::SUCCESS) {
            return AniTextResult::Success();
        } else {
            return AniTextResult::Invalid("Failed to get raw file");
        }
    }

    TEXT_LOGE("Unsupported resource type");
    return AniTextResult::Invalid("Unsupported resource type");
}
} // namespace OHOS::Text::ANI
