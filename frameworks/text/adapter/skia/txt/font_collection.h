/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_FONT_COLLECTION_H
#define ROSEN_MODULES_SPTEXT_FONT_COLLECTION_H

#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <shared_mutex>
#include <unordered_map>

#include "modules/skparagraph/include/FontCollection.h"
#include "txt/asset_font_manager.h"
#include "txt/text_style.h"
#include "txt/variation_font_cache.h"
#include "utils.h"

namespace txt {
class FontCollection : public std::enable_shared_from_this<FontCollection> {
public:
    FontCollection();

    ~FontCollection();

    size_t GetFontManagersCount() const;

    void SetupDefaultFontManager();
    void SetDefaultFontManager(std::shared_ptr<RSFontMgr> fontManager);
    void SetAssetFontManager(std::shared_ptr<RSFontMgr> fontManager);
    void SetDynamicFontManager(std::shared_ptr<RSFontMgr> fontManager);
    void SetTestFontManager(std::shared_ptr<RSFontMgr> fontManager);
    void SetGlobalFontManager(std::shared_ptr<RSFontMgr> fontManager);

    void DisableFontFallback();

    void ClearFontFamilyCache();

    sk_sp<skia::textlayout::FontCollection> CreateSktFontCollection();

    void UpdateDefaultFamilies();

    void RemoveCacheByUniqueId(uint32_t uniqueId);

    // Remove all cached variations for a given original uniqueId
    void RemoveVariationCacheByOriginalUniqueId(uint32_t originalUniqueId);
private:
    std::shared_ptr<RSFontMgr> defaultFontManager_{nullptr};
    std::shared_ptr<RSFontMgr> assetFontManager_{nullptr};
    std::shared_ptr<RSFontMgr> dynamicFontManager_{nullptr};
    std::shared_ptr<RSFontMgr> testFontManager_{nullptr};
    std::shared_ptr<RSFontMgr> globalFontManager_{nullptr};
    bool enableFontFallback_;

    sk_sp<skia::textlayout::FontCollection> sktFontCollection_;
    mutable std::shared_mutex collectionMutex_;

    std::vector<std::shared_ptr<RSFontMgr>> GetFontManagerOrder() const;
    void UpdateDefaultFamiliesInner();

    // Variation font cache - owned by txt::FontCollection
    std::unique_ptr<VariationFontCache> variationFontCache_{nullptr};

    DISALLOW_COPY_AND_ASSIGN(FontCollection);
};
} // namespace txt

#endif // ROSEN_MODULES_SPTEXT_FONT_COLLECTION_H
