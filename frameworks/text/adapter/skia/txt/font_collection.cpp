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
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "text_bundle_config_parser.h"
#include "text/typeface.h"
#include "txt/platform.h"
#include "txt/text_style.h"
#include "txt/variation_font_cache.h"

namespace txt {
FontCollection::FontCollection()
    : enableFontFallback_(true),
      variationFontCache_(std::make_unique<VariationFontCache>()) {}

FontCollection::~FontCollection()
{
    variationFontCache_->SetCacheRemoveCallback(nullptr);
    if (sktFontCollection_ != nullptr) {
        sktFontCollection_->registerCloneTypefaceCallback(nullptr);
    }
}

size_t FontCollection::GetFontManagersCount() const
{
    return GetFontManagerOrder().size();
}

void FontCollection::SetupDefaultFontManager()
{
    std::unique_lock lock(collectionMutex_);
    if (!defaultFontManager_) {
        defaultFontManager_ = OHOS::Rosen::SPText::GetDefaultFontManager();
        sktFontCollection_.reset();
    }
}

void FontCollection::SetDefaultFontManager(std::shared_ptr<RSFontMgr> fontManager)
{
    std::unique_lock lock(collectionMutex_);
    if (defaultFontManager_ != fontManager) {
        defaultFontManager_ = fontManager;
        sktFontCollection_.reset();
    }
}

void FontCollection::SetAssetFontManager(std::shared_ptr<RSFontMgr> fontManager)
{
    std::unique_lock lock(collectionMutex_);
    if (assetFontManager_ != fontManager) {
        assetFontManager_ = fontManager;
        sktFontCollection_.reset();
    }
}

void FontCollection::SetDynamicFontManager(std::shared_ptr<RSFontMgr> fontManager)
{
    std::unique_lock lock(collectionMutex_);
    if (dynamicFontManager_ != fontManager) {
        dynamicFontManager_ = fontManager;
        sktFontCollection_.reset();
    }
}

void FontCollection::SetGlobalFontManager(std::shared_ptr<RSFontMgr> fontManager)
{
    std::unique_lock lock(collectionMutex_);
    if (globalFontManager_ != fontManager) {
        globalFontManager_ = fontManager;
        sktFontCollection_.reset();
    }
}

void FontCollection::SetTestFontManager(std::shared_ptr<RSFontMgr> fontManager)
{
    std::unique_lock lock(collectionMutex_);
    if (testFontManager_ != fontManager) {
        testFontManager_ = fontManager;
        sktFontCollection_.reset();
    }
}

std::vector<std::shared_ptr<RSFontMgr>> FontCollection::GetFontManagerOrder() const
{
    std::shared_lock lock(collectionMutex_);
    std::vector<std::shared_ptr<RSFontMgr>> order;
    if (dynamicFontManager_)
        order.push_back(dynamicFontManager_);
    if (globalFontManager_ != nullptr)
        order.push_back(globalFontManager_);
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
    std::unique_lock lock(collectionMutex_);
    enableFontFallback_ = false;
    if (sktFontCollection_) {
        sktFontCollection_->disableFontFallback();
    }
}

void FontCollection::ClearFontFamilyCache()
{
    std::shared_lock lock(collectionMutex_);
    if (sktFontCollection_) {
        sktFontCollection_->clearCaches();
    }
}

sk_sp<skia::textlayout::FontCollection> FontCollection::CreateSktFontCollection()
{
    std::unique_lock lock(collectionMutex_);
    if (!sktFontCollection_) {
        skia::textlayout::FontCollection::SetAdapterTextHeightEnabled(
            OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().IsAdapterTextHeightEnabled());
        sktFontCollection_ = sk_make_sp<skia::textlayout::FontCollection>();
        UpdateDefaultFamiliesInner();
        sktFontCollection_->setAssetFontManager(assetFontManager_);
        sktFontCollection_->setDynamicFontManager(dynamicFontManager_);
        sktFontCollection_->setTestFontManager(testFontManager_);
        sktFontCollection_->setGlobalFontManager(globalFontManager_);
        if (!enableFontFallback_) {
            sktFontCollection_->disableFontFallback();
        }
        // Register internal clone callback using std::bind
        // This callback handles variation typeface cloning through VariationFontCache
        auto internalCallback = [this](
            std::shared_ptr<RSTypeface> typeface,
            const std::optional<skia::textlayout::FontArguments>& fontArgs)
            -> std::optional<std::shared_ptr<RSTypeface>> {
            if (variationFontCache_ == nullptr) {
                return std::nullopt;
            }
            // Call VariationFontCache to register/retrieve variation typeface
            auto result = variationFontCache_->RegisterVariationTypeface(typeface, fontArgs);
            // Return nullopt if result is same as original (no new variation created)
            // Return the variation typeface otherwise
            if (result != nullptr && result != typeface) {
                return result;
            }
            return std::nullopt;
        };
        sktFontCollection_->registerCloneTypefaceCallback(internalCallback);
        auto removeCallback = [this](uint32_t uniqueId) {
            if (sktFontCollection_) {
                sktFontCollection_->removeCacheByUniqueId(uniqueId);
            }
        };
        variationFontCache_->SetCacheRemoveCallback(removeCallback);
    }

    return sktFontCollection_;
}

void FontCollection::UpdateDefaultFamiliesInner()
{
    if (!sktFontCollection_) {
        return;
    }
    std::vector<SkString> defaultFontFamilies;
    for (const std::string& family :
        OHOS::Rosen::SPText::DefaultFamilyNameMgr::GetInstance().GetDefaultFontFamilies()) {
        defaultFontFamilies.emplace_back(family);
    }
    sktFontCollection_->setDefaultFontManager(defaultFontManager_, std::move(defaultFontFamilies));
}

void FontCollection::UpdateDefaultFamilies()
{
    std::shared_lock lock(collectionMutex_);
    UpdateDefaultFamiliesInner();
}

void FontCollection::RemoveCacheByUniqueId(uint32_t uniqueId)
{
    std::shared_lock lock(collectionMutex_);
    if (sktFontCollection_) {
        sktFontCollection_->removeCacheByUniqueId(uniqueId);
    }
}

void FontCollection::RemoveVariationCacheByOriginalUniqueId(uint32_t originalUniqueId)
{
    if (variationFontCache_ != nullptr) {
        variationFontCache_->RemoveByOriginalUniqueId(originalUniqueId);
    }
}
} // namespace txt
