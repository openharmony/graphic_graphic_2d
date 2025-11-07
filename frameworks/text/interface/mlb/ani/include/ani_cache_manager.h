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

struct ani_cache_param
{
    const char* clsName = nullptr;
    const char* methodName = nullptr;
    const char* signature = nullptr;

    bool IsValid() const
    {
        return clsName != nullptr && methodName != nullptr && signature != nullptr;
    }

    std::string BuildCacheKey() const
    {
        std::string_view clsNameView(clsName);
        std::string_view methodNameView(methodName);
        std::string_view signatureView(signature);
        std::string key;
        key.reserve(clsNameView.size() + methodNameView.size() + signatureView.size() + 3);
        key.append(clsNameView);
        key.append(methodNameView);
        key.append(signatureView);
        return key;
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