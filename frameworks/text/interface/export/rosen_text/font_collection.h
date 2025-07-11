/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_FONT_COLLECTION_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_FONT_COLLECTION_H

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>

#include "common/rs_macros.h"
#include "symbol_constants.h"
#include "text/font_mgr.h"

namespace txt {
class FontCollection;
} // namespace txt

namespace OHOS {
namespace Rosen {
class RS_EXPORT FontCollection {
    using FontCallbackType = void (*)(const FontCollection*, const std::string&);

public:
    static std::shared_ptr<FontCollection> From(std::shared_ptr<txt::FontCollection> fontCollection);
    static std::shared_ptr<FontCollection> Create();
    virtual ~FontCollection() = default;

    virtual void DisableFallback() = 0;
    virtual void DisableSystemFont() = 0;
    virtual std::shared_ptr<Drawing::Typeface> LoadFont(
        const std::string& familyName, const uint8_t* data, size_t datalen) = 0;
    virtual std::shared_ptr<Drawing::Typeface> LoadThemeFont(
        const std::string& familyName, const uint8_t* data, size_t datalen) = 0;
    virtual std::vector<std::shared_ptr<Drawing::Typeface>> LoadThemeFont(
        const std::string& familyName, const std::vector<std::pair<const uint8_t*, size_t>>& data) = 0;
    virtual void ClearThemeFont() = 0;
    virtual std::shared_ptr<Drawing::FontMgr> GetFontMgr() = 0;
    virtual LoadSymbolErrorCode LoadSymbolFont(const std::string& familyName, const uint8_t* data, size_t datalen) = 0;
    virtual LoadSymbolErrorCode LoadSymbolJson(const std::string& familyName, const uint8_t* data, size_t datalen) = 0;
    virtual void ClearCaches() = 0;
    virtual bool UnloadFont(const std::string& familyName) = 0;

    static void RegisterUnloadFontStartCallback(FontCallbackType cb);
    static void RegisterUnloadFontFinishCallback(FontCallbackType cb);
    static void RegisterLoadFontStartCallback(FontCallbackType cb);
    static void RegisterLoadFontFinishCallback(FontCallbackType cb);

protected:
    struct FontCallback {
        mutable std::mutex mutex_;
        std::unordered_set<FontCallbackType> callback_;

    public:
        void AddCallback(FontCallbackType cb);
        void ExecuteCallback(const FontCollection* fc, const std::string& family) const;
    };
    static FontCallback unloadFontStartCallback_;
    static FontCallback unloadFontFinishCallback_;
    static FontCallback loadFontStartCallback_;
    static FontCallback loadFontFinishCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_FONT_COLLECTION_H
