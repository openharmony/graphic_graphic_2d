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
#include <set>
#include <string>
#include <unordered_map>

#include "modules/skparagraph/include/FontCollection.h"
#include "txt/asset_font_manager.h"
#include "txt/text_style.h"
#include "utils.h"

namespace txt {
class FontCollection : public std::enable_shared_from_this<FontCollection> {
public:
    FontCollection();

    ~FontCollection();

    size_t GetFontManagersCount() const;

    void SetupDefaultFontManager();
#ifndef USE_ROSEN_DRAWING
    void SetDefaultFontManager(sk_sp<SkFontMgr> fontManager);
    void SetAssetFontManager(sk_sp<SkFontMgr> fontManager);
    void SetDynamicFontManager(sk_sp<SkFontMgr> fontManager);
    void SetTestFontManager(sk_sp<SkFontMgr> fontManager);
#else
    void SetDefaultFontManager(std::shared_ptr<RSFontMgr> fontManager);
    void SetAssetFontManager(std::shared_ptr<RSFontMgr> fontManager);
    void SetDynamicFontManager(std::shared_ptr<RSFontMgr> fontManager);
    void SetTestFontManager(std::shared_ptr<RSFontMgr> fontManager);
#endif

    void DisableFontFallback();

    void ClearFontFamilyCache();

    sk_sp<skia::textlayout::FontCollection> CreateSktFontCollection();

private:
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkFontMgr> defaultFontManager_;
    sk_sp<SkFontMgr> assetFontManager_;
    sk_sp<SkFontMgr> dynamicFontManager_;
    sk_sp<SkFontMgr> testFontManager_;
#else
    std::shared_ptr<RSFontMgr> defaultFontManager_;
    std::shared_ptr<RSFontMgr> assetFontManager_;
    std::shared_ptr<RSFontMgr> dynamicFontManager_;
    std::shared_ptr<RSFontMgr> testFontManager_;
#endif
    bool enableFontFallback_;

    sk_sp<skia::textlayout::FontCollection> sktFontCollection_;

#ifndef USE_ROSEN_DRAWING
    std::vector<sk_sp<SkFontMgr>> GetFontManagerOrder() const;
#else
    std::vector<std::shared_ptr<RSFontMgr>> GetFontManagerOrder() const;
#endif

    DISALLOW_COPY_AND_ASSIGN(FontCollection);
};
} // namespace txt

#endif // ROSEN_MODULES_SPTEXT_FONT_COLLECTION_H
