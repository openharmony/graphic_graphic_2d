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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_FONT_MANAGER_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_FONT_MANAGER_H

#include <texgine/typography_types.h>

#include "font_parser.h"
#include "texgine_font_style.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

class FontManager {
public:
    static std::shared_ptr<FontManager> GetInstance();
    using ResultSet = std::vector<FontParser::FontDescriptor>;
    FontParser::FontDescriptor* FindFont(ResultSet* fontSrc, FontParser::FontDescriptor& descFind);
    FontManager() = default;
    ~FontManager() = default;

private:
    static inline std::shared_ptr<FontManager> instance = nullptr;
    using ResultCache = std::vector<FontParser::FontDescriptor>;
    ResultCache resultCache;
    bool ResAccurateMatch(FontParser::FontDescriptor& descSrc, FontParser::FontDescriptor& descFind);
    bool ResFallbackMatch(FontParser::FontDescriptor& descSrc, FontParser::FontDescriptor& descFind);
    int ScoreFallbackMatch(FontParser::FontDescriptor& descSrc, FontParser::FontDescriptor& descFind);
    FontParser::FontDescriptor* FindAccurateResult(ResultSet* fontSrc, FontParser::FontDescriptor& descFind);
    FontParser::FontDescriptor* FindFallbackResult(ResultSet* fontSrc, FontParser::FontDescriptor& descFind);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_FONT_MANAGER_H
