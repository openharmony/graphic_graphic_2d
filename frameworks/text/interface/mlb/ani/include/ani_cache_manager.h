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

#ifndef OHOS_TEXT_ANI_CACHE_MANAGER_H
#define OHOS_TEXT_ANI_CACHE_MANAGER_H

#include <ani.h>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace OHOS::Text::ANI {
class AniCacheManager {
public:
    static AniCacheManager& Instance()
    {
        static AniCacheManager instance;
        return instance;
    }

    void InsertClass(ani_env* env, const std::string& key, ani_class cls)
    {
        ani_ref savePtr;
        env->GlobalReference_Create(reinterpret_cast<ani_ref>(cls), &savePtr);
        std::unique_lock<std::shared_mutex> lock(clsMutex_);
        clsCache_.insert({ key, savePtr });
    }

    bool FindClass(const std::string& key, ani_class& cls)
    {
        std::shared_lock<std::shared_mutex> lock(clsMutex_);
        auto it = clsCache_.find(key);
        if (it != clsCache_.end()) {
            cls = reinterpret_cast<ani_class>(it->second);
            return true;
        }
        return false;
    }

    void InsertMethod(const std::string& key, ani_method method)
    {
        std::unique_lock<std::shared_mutex> lock(methodMutex_);
        methodCache_.insert({ key, method });
    }

    bool FindMethod(const std::string& key, ani_method& method)
    {
        std::shared_lock<std::shared_mutex> lock(methodMutex_);
        auto it = methodCache_.find(key);
        if (it != methodCache_.end()) {
            method = it->second;
            return true;
        }
        return false;
    }

    void Clear(ani_env* env)
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
    }

private:
    AniCacheManager() = default;
    std::unordered_map<std::string, ani_ref> clsCache_;
    std::unordered_map<std::string, ani_method> methodCache_;
    mutable std::shared_mutex clsMutex_;
    mutable std::shared_mutex methodMutex_;
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_CACHE_MANAGER_H