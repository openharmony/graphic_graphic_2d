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
#include <string>

#include "common/rs_macros.h"
#include "text/font_mgr.h"

namespace txt {
class FontCollection;
} // namespace txt

namespace OHOS {
namespace Rosen {
class RS_EXPORT FontCollection {
public:
    static std::shared_ptr<FontCollection> From(std::shared_ptr<txt::FontCollection> fontCollection);
    static std::shared_ptr<FontCollection> Create();
    virtual ~FontCollection() = default;

    virtual void DisableFallback() = 0;
    virtual void DisableSystemFont() = 0;
    virtual Drawing::Typeface* LoadFont(const std::string &familyName, const uint8_t *data, size_t datalen) = 0;
    virtual void LoadThemeFont(const std::string &familyName, const uint8_t *data, size_t datalen) = 0;
    virtual std::shared_ptr<Drawing::FontMgr> GetFontMgr() = 0;
    virtual void AddLoadedFamilyName(const std::string& name) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_FONT_COLLECTION_H
