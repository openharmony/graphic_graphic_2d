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
#include <string_view>
#include <unordered_map>

#include "utils/text_log.h"

namespace OHOS::Text::ANI {

struct AniCacheParam
{
    std::string cacheKey{""};
    const char* clsName{nullptr};
    const char* methodName{nullptr};
    const char* signature{nullptr};

    AniCacheParam(const char* cls, const char* method, const char* sig)
        : clsName(cls), methodName(method), signature(sig)
    {
        if (IsValid()) {
            BuildCacheKey();
        }
    }

    bool IsValid() const
    {
        return clsName != nullptr && methodName != nullptr && signature != nullptr;
    }

    const char* GetCacheKey() const
    {
        return cacheKey.c_str();
    }

private:
    void BuildCacheKey()
    {
        cacheKey.clear();
        size_t clsLen = std::strlen(clsName);
        size_t methodLen = std::strlen(methodName);
        size_t sigLen = std::strlen(signature);
        cacheKey.reserve(clsLen + methodLen + sigLen);
        cacheKey.append(clsName, clsLen);
        cacheKey.append(methodName, methodLen);
        cacheKey.append(signature, sigLen);
    }
};

class AniCacheManager {
public:
    static AniCacheManager& Instance()
    {
        static AniCacheManager instance;
        return instance;
    }

    void InsertClass(ani_env* env, const std::string& key, ani_class cls);
    bool FindClass(const std::string& key, ani_class& cls);
    void InsertMethod(const std::string& key, ani_method method);
    bool FindMethod(const std::string& key, ani_method& method);
    void InsertEnum(ani_env* env, const std::string& key, ani_enum enumType);
    bool FindEnum(const std::string& key, ani_enum& enumType);
    void Clear(ani_env* env);

private:
    AniCacheManager() = default;
    ~AniCacheManager() = default;
    AniCacheManager(AniCacheManager&&) = delete;
    AniCacheManager& operator=(AniCacheManager&&) = delete;
    AniCacheManager(const AniCacheManager&) = delete;
    AniCacheManager& operator=(const AniCacheManager&) = delete;

    std::unordered_map<std::string, ani_ref> clsCache_;
    std::unordered_map<std::string, ani_method> methodCache_;
    std::unordered_map<std::string, ani_ref> enumCache_;
    mutable std::shared_mutex clsMutex_;
    mutable std::shared_mutex methodMutex_;
    mutable std::shared_mutex enumMutex_;
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_CACHE_MANAGER_H