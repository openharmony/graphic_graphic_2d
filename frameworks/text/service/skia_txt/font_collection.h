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

#ifndef ROSEN_TEXT_ADAPTER_TXT_FONT_COLLECTION_H
#define ROSEN_TEXT_ADAPTER_TXT_FONT_COLLECTION_H

#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "rosen_text/font_collection.h"
#include "rosen_text/symbol_constants.h"
#include "txt/asset_font_manager.h"
#include "txt/font_collection.h"

#include "text/font_mgr.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
class TypefaceWithAlias {
public:
    TypefaceWithAlias(const std::string& alias, const std::shared_ptr<Drawing::Typeface>& typeface);
    uint32_t GetHash() const;
    const std::string& GetAlias() const;
    const std::shared_ptr<Drawing::Typeface>& GetTypeface() const;
    bool operator==(const TypefaceWithAlias& other) const;

    struct Hasher {
        uint32_t operator()(const TypefaceWithAlias& ta) const;
    };

private:
    std::string alias_;
    std::shared_ptr<Drawing::Typeface> typeface_;
    mutable uint32_t hash_ { 0 };
};

enum class RegisterError {
    SUCCESS,
    ALREADY_EXIST,
    INVALID_INPUT,
    REGISTER_FAILED,
};

class FontCollection : public ::OHOS::Rosen::FontCollection {
public:
    explicit FontCollection(std::shared_ptr<txt::FontCollection> fontCollection = nullptr);
    ~FontCollection();
    std::shared_ptr<txt::FontCollection> Get();

    void DisableFallback() override;
    void DisableSystemFont() override;
    std::shared_ptr<Drawing::Typeface> LoadFont(
        const std::string& familyName, const uint8_t* data, size_t datalen) override;
    std::shared_ptr<Drawing::Typeface> LoadThemeFont(
        const std::string& familyName, const uint8_t* data, size_t datalen) override;
    std::vector<std::shared_ptr<Drawing::Typeface>> LoadThemeFont(
        const std::string& familyName, const std::vector<std::pair<const uint8_t*, size_t>>& data) override;
    void ClearThemeFont() override;
    std::shared_ptr<Drawing::FontMgr> GetFontMgr() override;
    LoadSymbolErrorCode LoadSymbolFont(const std::string& familyName, const uint8_t* data, size_t datalen) override;
    LoadSymbolErrorCode LoadSymbolJson(const std::string& familyName, const uint8_t* data, size_t datalen) override;

    void ClearCaches() override;

private:
    RegisterError RegisterTypeface(const TypefaceWithAlias& ta);

    std::shared_ptr<txt::FontCollection> fontCollection_ = nullptr;
    std::shared_ptr<Drawing::FontMgr> dfmanager_ = nullptr;
    std::unordered_set<TypefaceWithAlias, TypefaceWithAlias::Hasher> typefaceSet_;
    std::unordered_map<uint32_t, std::string> familyNames_;
    std::shared_mutex mutex_;
};
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TXT_FONT_COLLECTION_H
