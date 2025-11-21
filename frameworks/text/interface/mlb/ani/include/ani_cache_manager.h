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
#include <unordered_map>
#include <string_view>
#include <memory>
#include <shared_mutex>

namespace OHOS::Text::ANI {
struct AniRefCache {
    std::unordered_map<std::string_view, ani_ref> store;
    std::shared_mutex mtx;
};

struct CacheKey {
    std::string_view d;
    std::string_view n;
    std::string_view s;
    bool operator==(const CacheKey& other) const {
        return d == other.d && n == other.n && s == other.s;
    }
};

struct CacheKeyHash {
    size_t operator()(const CacheKey& key) const {
        size_t h1 = std::hash<std::string_view>{}(key.d);
        size_t h2 = std::hash<std::string_view>{}(key.n);
        size_t h3 = std::hash<std::string_view>{}(key.s);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

static AniRefCache g_nsCache;
static AniRefCache g_classCache;
static AniRefCache g_enumCache;

static std::unordered_map<CacheKey, ani_method, CacheKeyHash> g_methodCache;
static std::unordered_map<CacheKey, ani_function, CacheKeyHash> g_functionCache;
static std::shared_mutex g_methodMutex;
static std::shared_mutex g_functionMutex;

ani_namespace AniFindNamespace(ani_env* env, const char* descriptor);

ani_class AniFindClass(ani_env* env, const char* descriptor);

ani_enum AniFindEnum(ani_env* env, const char* descriptor);

ani_method AniClassFindMethod(ani_env* env, const CacheKey& key);

ani_function AniNamespaceFindFunction(ani_env* env, const CacheKey& key);
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_CACHE_MANAGER_H