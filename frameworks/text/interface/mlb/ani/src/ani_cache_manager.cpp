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

#include "ani_cache_manager.h"

namespace OHOS::Text::ANI {
void AniCacheManager::InsertClass(ani_env* env, const std::string& key, ani_class cls)
{
    std::unique_lock<std::shared_mutex> lock(clsMutex_);
    ani_ref savePtr = nullptr;
    ani_status status = env->GlobalReference_Create(reinterpret_cast<ani_ref>(cls), &savePtr);
    if (status == ANI_OK) {
        clsCache_.insert({ key, savePtr });
    } else {
        TEXT_LOGE("Failed to cache class %{public}s", key.c_str());
    }
}
bool AniCacheManager::FindClass(const std::string& key, ani_class& cls)
{
    std::shared_lock<std::shared_mutex> lock(clsMutex_);
    auto it = clsCache_.find(key);
    if (it != clsCache_.end()) {
        cls = reinterpret_cast<ani_class>(it->second);
        return true;
    }
    return false;
}
void AniCacheManager::InsertMethod(const std::string& key, ani_method method)
{
    std::unique_lock<std::shared_mutex> lock(methodMutex_);
    methodCache_.insert({ key, method });
}
bool AniCacheManager::FindMethod(const std::string& key, ani_method& method)
{
    std::shared_lock<std::shared_mutex> lock(methodMutex_);
    auto it = methodCache_.find(key);
    if (it != methodCache_.end()) {
        method = it->second;
        return true;
    }
    return false;
}
void AniCacheManager::InsertEnum(ani_env* env, const std::string& key, ani_enum enumType)
{
    std::unique_lock<std::shared_mutex> lock(enumMutex_);
    ani_ref savePtr = nullptr;
    ani_status status = env->GlobalReference_Create(reinterpret_cast<ani_ref>(enumType), &savePtr);
    if (status == ANI_OK) {
        enumCache_.insert({ key, savePtr });
    } else {
        TEXT_LOGE("Failed to cache enum %{public}s", key.c_str());
    }
}
bool AniCacheManager::FindEnum(const std::string& key, ani_enum& enumType)
{
    std::shared_lock<std::shared_mutex> lock(enumMutex_);
    auto it = enumCache_.find(key);
    if (it != enumCache_.end()) {
        enumType = reinterpret_cast<ani_enum>(it->second);
        return true;
    }
    return false;
}
void AniCacheManager::Clear(ani_env* env)
{
    {
        std::unique_lock<std::shared_mutex> lock(clsMutex_);
        for (auto& pair : clsCache_) {
            env->GlobalReference_Delete(pair.second);
        }
        clsCache_.clear();
    }
    {
        std::unique_lock<std::shared_mutex> lock(methodMutex_);
        methodCache_.clear();
    }
    {
        std::unique_lock<std::shared_mutex> lock(enumMutex_);
        for (auto& pair : enumCache_) {
            env->GlobalReference_Delete(pair.second);
        }
        enumCache_.clear();
    }
}
} // namespace OHOS::Text::ANI