/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "convert.h"
#include "custom_symbol_config.h"
#include "texgine/src/font_descriptor_mgr.h"
#include "txt/platform.h"
#include "text/typeface.h"
#include "utils/text_log.h"
#include "utils/text_trace.h"

namespace OHOS {
namespace Rosen {
FontCollection::FontCallback FontCollection::loadFontStartCallback_ {};
FontCollection::FontCallback FontCollection::loadFontFinishCallback_ {};
FontCollection::FontCallback FontCollection::unloadFontStartCallback_ {};
FontCollection::FontCallback FontCollection::unloadFontFinishCallback_ {};

std::shared_ptr<FontCollection> FontCollection::Create()
{
    static std::shared_ptr<FontCollection> instance = std::make_shared<AdapterTxt::FontCollection>();
    return instance;
}

std::shared_ptr<FontCollection> FontCollection::From(std::shared_ptr<txt::FontCollection> fontCollection)
{
    return std::make_shared<AdapterTxt::FontCollection>(fontCollection);
}

namespace AdapterTxt {
FontCollection::FontCollection(std::shared_ptr<txt::FontCollection> fontCollection)
    : fontCollection_(fontCollection), dfmanager_(Drawing::FontMgr::CreateDynamicFontMgr())
{
    if (fontCollection_ == nullptr) {
        fontCollection_ = std::make_shared<txt::FontCollection>();
    }
    fontCollection_->SetupDefaultFontManager();
    fontCollection_->SetDynamicFontManager(dfmanager_);
}

std::shared_ptr<txt::FontCollection> FontCollection::Get()
{
    return fontCollection_;
}

FontCollection::~FontCollection()
{
    if (Drawing::Typeface::GetTypefaceUnRegisterCallBack() == nullptr) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);
    for (const auto& ta : typefaceSet_) {
        Drawing::Typeface::GetTypefaceUnRegisterCallBack()(ta.GetTypeface());
    }
    for (const auto& family : familyNames_) {
        FontDescriptorMgrInstance.DeleteDynamicTypefaceFromCache(family.second);
    }
    typefaceSet_.clear();
    familyNames_.clear();
}

void FontCollection::DisableFallback()
{
    fontCollection_->DisableFontFallback();
}

void FontCollection::DisableSystemFont()
{
    fontCollection_->SetDefaultFontManager(nullptr);
}

std::shared_ptr<Drawing::FontMgr> FontCollection::GetFontMgr()
{
    return dfmanager_;
}

RegisterError FontCollection::RegisterTypeface(const TypefaceWithAlias& ta)
{
    if (ta.GetTypeface() == nullptr || Drawing::Typeface::GetTypefaceRegisterCallBack() == nullptr) {
        return RegisterError::INVALID_INPUT;
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (typefaceSet_.count(ta)) {
        TEXT_LOGI_LIMIT3_MIN(
            "Find same typeface: family name: %{public}s, hash: %{public}u", ta.GetAlias().c_str(), ta.GetHash());
        return RegisterError::ALREADY_EXIST;
    }
    if (!Drawing::Typeface::GetTypefaceRegisterCallBack()(ta.GetTypeface())) {
        return RegisterError::REGISTER_FAILED;
    }
    TEXT_LOGI("Succeed in registering typeface, family name: %{public}s, hash: %{public}u", ta.GetAlias().c_str(),
        ta.GetHash());
    typefaceSet_.insert(ta);
    return RegisterError::SUCCESS;
}

std::shared_ptr<Drawing::Typeface> FontCollection::LoadFont(
    const std::string& familyName, const uint8_t* data, size_t datalen)
{
    TEXT_TRACE_FUNC();
    std::shared_ptr<Drawing::Typeface> typeface(dfmanager_->LoadDynamicFont(familyName, data, datalen));
    if (typeface == nullptr) {
        TEXT_LOGE("Failed to load font %{public}s", familyName.c_str());
        return nullptr;
    }
    TypefaceWithAlias ta(familyName, typeface);
    FontCallbackGuard cb(this, ta.GetAlias(), loadFontStartCallback_, loadFontFinishCallback_);
    RegisterError err = RegisterTypeface(ta);
    if (err != RegisterError::SUCCESS && err != RegisterError::ALREADY_EXIST) {
        TEXT_LOGE("Failed to register typeface %{public}s", ta.GetAlias().c_str());
        return nullptr;
    }
    FontDescriptorMgrInstance.CacheDynamicTypeface(typeface, ta.GetAlias());
    familyNames_.emplace(ta.GetHash(), ta.GetAlias());
    fontCollection_->ClearFontFamilyCache();
    return typeface;
}

LoadSymbolErrorCode FontCollection::LoadSymbolFont(const std::string& familyName, const uint8_t* data, size_t datalen)
{
    TEXT_TRACE_FUNC();
    std::shared_ptr<Drawing::Typeface> typeface(dfmanager_->LoadDynamicFont(familyName, data, datalen));
    if (typeface == nullptr) {
        return LoadSymbolErrorCode::LOAD_FAILED;
    }
    std::unique_lock<std::shared_mutex> lock(mutex_);
    TypefaceWithAlias ta(familyName, typeface);
    FontCallbackGuard cb(this, familyName, loadFontStartCallback_, loadFontFinishCallback_);
    if (typefaceSet_.count(ta)) {
        return LoadSymbolErrorCode::SUCCESS;
    }
    typefaceSet_.insert(ta);
    return LoadSymbolErrorCode::SUCCESS;
}

LoadSymbolErrorCode FontCollection::LoadSymbolJson(const std::string& familyName, const uint8_t* data, size_t datalen)
{
    TEXT_TRACE_FUNC();
    return CustomSymbolConfig::GetInstance()->ParseConfig(familyName, data, datalen);
}


static std::shared_ptr<Drawing::Typeface> CreateTypeface(const uint8_t *data, size_t datalen)
{
    if (datalen != 0 && data != nullptr) {
        auto stream = std::make_unique<Drawing::MemoryStream>(data, datalen, true);
        return Drawing::Typeface::MakeFromStream(std::move(stream));
    }
    return nullptr;
}

std::shared_ptr<Drawing::Typeface> FontCollection::LoadThemeFont(
    const std::string& familyName, const uint8_t* data, size_t datalen)
{
    TEXT_TRACE_FUNC();
    auto res = LoadThemeFont(familyName, { { data, datalen } });
    return res.empty() ? nullptr : res[0];
}

std::vector<std::shared_ptr<Drawing::Typeface>> FontCollection::LoadThemeFont(
    const std::string& familyName, const std::vector<std::pair<const uint8_t*, size_t>>& data)
{
    if (familyName.empty()) {
        ClearThemeFont();
        return {};
    }

    std::vector<std::shared_ptr<Drawing::Typeface>> res;
    size_t index = 0;
    for (size_t i = 0; i < data.size(); i += 1) {
        std::string themeFamily = SPText::DefaultFamilyNameMgr::GetInstance().GenerateThemeFamilyName(index);
        auto face = CreateTypeface(data[i].first, data[i].second);
        TypefaceWithAlias ta(themeFamily, face);
        RegisterError err = RegisterTypeface(ta);
        if (err == RegisterError::ALREADY_EXIST) {
            res.emplace_back(face);
            continue;
        } else if (err != RegisterError::SUCCESS) {
            TEXT_LOGE("Failed to load font %{public}s", familyName.c_str());
            continue;
        }
        index += 1;
        res.emplace_back(face);
        dfmanager_->LoadThemeFont(themeFamily, face);
    }
    SPText::DefaultFamilyNameMgr::GetInstance().ModifyThemeFontFamilies(res.size());
    fontCollection_->ClearFontFamilyCache();
    fontCollection_->UpdateDefaultFamilies();
    return res;
}

void FontCollection::ClearThemeFont()
{
    std::unique_lock lock(mutex_);
    for (const auto& themeFamily : SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies()) {
        std::shared_ptr<Drawing::Typeface> face(dfmanager_->MatchFamilyStyle(themeFamily.c_str(), {}));
        TypefaceWithAlias ta(themeFamily, face);
        typefaceSet_.erase(ta);
        dfmanager_->LoadThemeFont("", themeFamily, nullptr, 0);
    }
    fontCollection_->ClearFontFamilyCache();
    SPText::DefaultFamilyNameMgr::GetInstance().ModifyThemeFontFamilies(0);
}

void FontCollection::ClearCaches()
{
    fontCollection_->ClearFontFamilyCache();
}

bool FontCollection::UnloadFont(const std::string& familyName)
{
    if (Drawing::Typeface::GetTypefaceUnRegisterCallBack() == nullptr ||
        SPText::DefaultFamilyNameMgr::IsThemeFontFamily(familyName) || familyName.empty()) {
        TEXT_LOGE("Failed to unload font: %{public}s", familyName.c_str());
        return false;
    }

    std::shared_lock readLock(mutex_);
    if (std::none_of(typefaceSet_.begin(), typefaceSet_.end(),
        [&familyName](const auto& ta) { return ta.GetAlias() == familyName; })) {
        TEXT_LOGE("Unload a font which is not loaded: %{public}s", familyName.c_str());
        return true;
    }
    readLock.unlock();

    FontCallbackGuard cb(this, familyName, unloadFontStartCallback_, unloadFontFinishCallback_);
    std::unique_lock<std::shared_mutex> lock(mutex_);
    for (auto it = typefaceSet_.begin(); it != typefaceSet_.end();) {
        if (it->GetAlias() == familyName) {
            dfmanager_->LoadDynamicFont(familyName, nullptr, 0);
            FontDescriptorMgrInstance.DeleteDynamicTypefaceFromCache(familyName);
            ClearCaches();
            Drawing::Typeface::GetTypefaceUnRegisterCallBack()(it->GetTypeface());
            familyNames_.erase(it->GetHash());
            typefaceSet_.erase(it++);
        } else {
            ++it;
        }
    }

    return true;
}

FontCollection::FontCallbackGuard::FontCallbackGuard(
    const FontCollection* fc, const std::string& familyName, const FontCallback& begin, const FontCallback& end)
    : fc_(fc), familyName_(familyName), begin_(begin), end_(end)
{
    begin_.ExcuteCallback(fc_, familyName_);
}

FontCollection::FontCallbackGuard::~FontCallbackGuard()
{
    end_.ExcuteCallback(fc_, familyName_);
}

TypefaceWithAlias::TypefaceWithAlias(const std::string& alias, const std::shared_ptr<Drawing::Typeface>& typeface)
    : alias_(alias), typeface_(typeface)
{
    if (alias.empty() && typeface != nullptr) {
        alias_ = typeface->GetFamilyName();
    }
}

uint32_t TypefaceWithAlias::GetHash() const
{
    if (hash_ != 0) {
        return hash_;
    }
    hash_ = Hasher()(*this);
    return hash_;
}

uint32_t TypefaceWithAlias::Hasher::operator()(const TypefaceWithAlias& ta) const
{
    uint32_t hashS = static_cast<uint32_t>(std::hash<std::string>()(ta.alias_));
    uint32_t hashT = ta.typeface_ == nullptr ? 0 : ta.typeface_->GetHash();
    ta.hash_ = static_cast<uint32_t>(std::hash<uint32_t>()(hashS ^ hashT));
    return ta.hash_;
}

const std::shared_ptr<Drawing::Typeface>& TypefaceWithAlias::GetTypeface() const
{
    return typeface_;
}

const std::string& TypefaceWithAlias::GetAlias() const
{
    return alias_;
}

bool TypefaceWithAlias::operator==(const TypefaceWithAlias& other) const
{
    return other.alias_ == this->alias_ && other.GetHash() == this->GetHash();
}
} // namespace AdapterTxt

void FontCollection::RegisterLoadFontStartCallback(FontCallbackType cb)
{
    loadFontStartCallback_.AddCallback(cb);
}

void FontCollection::RegisterUnloadFontStartCallback(FontCallbackType cb)
{
    unloadFontStartCallback_.AddCallback(cb);
}

void FontCollection::RegisterLoadFontFinishCallback(FontCallbackType cb)
{
    loadFontFinishCallback_.AddCallback(cb);
}

void FontCollection::RegisterUnloadFontFinishCallback(FontCallbackType cb)
{
    unloadFontFinishCallback_.AddCallback(cb);
}

void FontCollection::FontCallback::AddCallback(FontCallbackType cb)
{
    std::lock_guard lock(mutex_);
    if (cb != nullptr) {
        callback_.emplace(cb);
    } else {
        TEXT_LOGE("Failed to register callback, cb is nullptr");
    }
}

void FontCollection::FontCallback::ExcuteCallback(const FontCollection* fc, const std::string& family) const
{
    std::lock_guard lock(mutex_);
    for (auto& cb : callback_) {
        cb(fc, family);
    }
}
} // namespace Rosen
} // namespace OHOS
