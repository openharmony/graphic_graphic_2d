/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "texgine/font_providers.h"
#include "texgine/system_font_provider.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::shared_ptr<FontProviders> FontProviders::Create() noexcept(true)
{
    return std::make_shared<FontProviders>();
}

std::shared_ptr<FontProviders> FontProviders::SystemFontOnly() noexcept(true)
{
    auto fps = FontProviders::Create();
    fps->AppendFontProvider(SystemFontProvider::GetInstance());
    return fps;
}

void FontProviders::AppendFontProvider(const std::shared_ptr<IFontProvider> &provider) noexcept(true)
{
    if (provider == nullptr) {
        return;
    }

    for (const auto &p : providers_) {
        if (p == provider) {
            return;
        }
    }

    providers_.push_back(provider);
}

void FontProviders::DisableFallback()
{
    enableFallback_ = false;
}

std::shared_ptr<FontCollection> FontProviders::GenerateFontCollection(
    const std::vector<std::string> &families) const noexcept(true)
{
    std::vector<std::shared_ptr<VariantFontStyleSet>> sets;
    for (const auto &familyName : families) {
        for (const auto &provider : providers_) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = fontStyleSetCache_.find(familyName);
            if (it == fontStyleSetCache_.end()) {
                auto set = provider->MatchFamily(familyName);
                if (set != nullptr) {
                    fontStyleSetCache_[familyName] = set;
                    it = fontStyleSetCache_.find(familyName);
                }
            }

            if (it != fontStyleSetCache_.end()) {
                sets.push_back(it->second);
                break;
            }
        }
    }

    auto collection = std::make_shared<FontCollection>(std::move(sets));
    if (!enableFallback_) {
        collection->DisableFallback();
    }
    return collection;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
