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

#include "font_collection.h"

#include <algorithm>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "txt/platform.h"
#include "txt/text_style.h"

namespace txt {
FontCollection::FontCollection() : enableFontFallback_(true) {}

FontCollection::~FontCollection()
{
    if (sktFontCollection_) {
        sktFontCollection_->clearCaches();
    }
}

size_t FontCollection::GetFontManagersCount() const
{
    return GetFontManagerOrder().size();
}

void FontCollection::SetupDefaultFontManager()
{
    defaultFontManager_ = OHOS::Rosen::SPText::GetDefaultFontManager();
    sktFontCollection_.reset();
}

#ifndef USE_ROSEN_DRAWING
void FontCollection::SetDefaultFontManager(sk_sp<SkFontMgr> fontManager)
#else
void FontCollection::SetDefaultFontManager(std::shared_ptr<RSFontMgr> fontManager)
#endif
{
    defaultFontManager_ = fontManager;
    sktFontCollection_.reset();
}

#ifndef USE_ROSEN_DRAWING
void FontCollection::SetAssetFontManager(sk_sp<SkFontMgr> fontManager)
#else
void FontCollection::SetAssetFontManager(std::shared_ptr<RSFontMgr> fontManager)
#endif
{
    assetFontManager_ = fontManager;
    sktFontCollection_.reset();
}

#ifndef USE_ROSEN_DRAWING
void FontCollection::SetDynamicFontManager(sk_sp<SkFontMgr> fontManager)
#else
void FontCollection::SetDynamicFontManager(std::shared_ptr<RSFontMgr> fontManager)
#endif
{
    dynamicFontManager_ = fontManager;
    sktFontCollection_.reset();
}

#ifndef USE_ROSEN_DRAWING
void FontCollection::SetTestFontManager(sk_sp<SkFontMgr> fontManager)
#else
void FontCollection::SetTestFontManager(std::shared_ptr<RSFontMgr> fontManager)
#endif
{
    testFontManager_ = fontManager;
    sktFontCollection_.reset();
}

#ifndef USE_ROSEN_DRAWING
std::vector<sk_sp<SkFontMgr>> FontCollection::GetFontManagerOrder() const
#else
std::vector<std::shared_ptr<RSFontMgr>> FontCollection::GetFontManagerOrder() const
#endif
{
#ifndef USE_ROSEN_DRAWING
    std::vector<sk_sp<SkFontMgr>> order;
#else
    std::vector<std::shared_ptr<RSFontMgr>> order;
#endif
    if (dynamicFontManager_)
        order.push_back(dynamicFontManager_);
    if (assetFontManager_)
        order.push_back(assetFontManager_);
    if (testFontManager_)
        order.push_back(testFontManager_);
    if (defaultFontManager_)
        order.push_back(defaultFontManager_);
    return order;
}

void FontCollection::DisableFontFallback()
{
    enableFontFallback_ = false;
    if (sktFontCollection_) {
        sktFontCollection_->disableFontFallback();
    }
}

void FontCollection::ClearFontFamilyCache()
{
    if (sktFontCollection_) {
        sktFontCollection_->clearCaches();
    }
}

sk_sp<skia::textlayout::FontCollection> FontCollection::CreateSktFontCollection()
{
    if (!sktFontCollection_) {
        sktFontCollection_ = sk_make_sp<skia::textlayout::FontCollection>();

        std::vector<SkString> defaultFontFamilies;
        for (const std::string& family : OHOS::Rosen::SPText::GetDefaultFontFamilies()) {
            defaultFontFamilies.emplace_back(family);
        }
        sktFontCollection_->setDefaultFontManager(defaultFontManager_, defaultFontFamilies);
        sktFontCollection_->setAssetFontManager(assetFontManager_);
        sktFontCollection_->setDynamicFontManager(dynamicFontManager_);
        sktFontCollection_->setTestFontManager(testFontManager_);
        if (!enableFontFallback_) {
            sktFontCollection_->disableFontFallback();
        }
    }

    return sktFontCollection_;
}
} // namespace txt
