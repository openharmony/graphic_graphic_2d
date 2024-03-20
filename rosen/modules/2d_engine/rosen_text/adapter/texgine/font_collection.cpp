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
#include "texgine/dynamic_font_provider.h"
#include "texgine/font_providers.h"
#include "texgine/system_font_provider.h"
#include "texgine/theme_font_provider.h"

#include "convert.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<FontCollection> g_instance;

std::shared_ptr<FontCollection> FontCollection::Create()
{
    if (g_instance == nullptr) {
        g_instance = std::make_shared<AdapterTextEngine::FontCollection>();
    }
    return g_instance;
}

std::shared_ptr<FontCollection> FontCollection::From(std::shared_ptr<txt::FontCollection> fontCollection)
{
    return std::make_shared<AdapterTextEngine::FontCollection>();
}

namespace AdapterTextEngine {
FontCollection::FontCollection()
{
    fontProviders_ = TextEngine::FontProviders::Create();
    dfprovider_ = TextEngine::DynamicFontProvider::Create();
    tfprovider_ = TextEngine::ThemeFontProvider::GetInstance();
}

std::shared_ptr<TextEngine::FontProviders> FontCollection::Get()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (fontProviders_ == nullptr) {
        fontProviders_ = TextEngine::FontProviders::Create();
    }
    if (dfprovider_ == nullptr) {
        dfprovider_ = TextEngine::DynamicFontProvider::Create();
    }
    if (tfprovider_ == nullptr) {
        tfprovider_ = TextEngine::ThemeFontProvider::GetInstance();
    }
    fontProviders_->AppendFontProvider(dfprovider_);
    fontProviders_->AppendFontProvider(tfprovider_);
    if (!disableSystemFont_) {
        if (sysprovider_ == nullptr) {
            sysprovider_ = TextEngine::SystemFontProvider::GetInstance();
        }
        fontProviders_->AppendFontProvider(sysprovider_);
    }
    return fontProviders_;
}

void FontCollection::DisableFallback()
{
    fontProviders_->DisableFallback();
}

void FontCollection::DisableSystemFont()
{
    disableSystemFont_ = true;
}

void FontCollection::LoadFont(const std::string &familyName, const uint8_t *data, size_t datalen)
{
    dfprovider_->LoadFont(familyName, data, datalen);
    fontProviders_->Clear();
}

void FontCollection::LoadThemeFont(const std::string &familyName, const uint8_t *data, size_t datalen)
{
    tfprovider_->LoadFont(familyName, data, datalen);
    fontProviders_->Clear();
}
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS
