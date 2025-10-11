/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef OHOS_ROSEN_TEXT_FONT_COLLECTION_MGR_H
#define OHOS_ROSEN_TEXT_FONT_COLLECTION_MGR_H

#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "font_collection.h"

namespace OHOS::Rosen {

class FontCollectionMgr final {
public:
    static FontCollectionMgr& GetInstance();
    ~FontCollectionMgr() = default;
    
    void InsertLocalInstance(uint64_t envId, std::shared_ptr<FontCollection> fontCollection);
    std::shared_ptr<FontCollection> GetLocalInstance(uint64_t envId);
    void DestroyLocalInstance(uint64_t envId);
    bool CheckInstanceIsValid(uint64_t envId, std::shared_ptr<FontCollection> fontCollection);
    uint64_t GetEnvByFontCollection(const FontCollection* fontCollection);

    void InsertSharedFontColleciton(void* key, std::shared_ptr<FontCollection> fontCollection);
    std::shared_ptr<FontCollection> FindSharedFontColleciton(void* key);
    bool RemoveSharedFontColleciton(void* key);

    bool InsertHapPath(const std::string& bundle, const std::string& module, const std::string& path);
    void DestoryHapPath(const std::string& bundle, const std::string& module);
    std::string GetHapPath(const std::string& bundle, const std::string& module);

private:
    FontCollectionMgr() = default;
    FontCollectionMgr(const FontCollectionMgr&) = delete;
    FontCollectionMgr& operator=(const FontCollectionMgr&) = delete;
    FontCollectionMgr(FontCollectionMgr&&) = delete;
    FontCollectionMgr& operator=(FontCollectionMgr&&) = delete;

    std::unordered_map<uint64_t, std::shared_ptr<FontCollection>> localCollections_;
    std::unordered_map<void*, std::shared_ptr<FontCollection>> sharedCollections_;
    std::shared_mutex mutex_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hapPaths_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_TEXT_FONT_COLLECTION_MGR_H