/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_TEXT_ANI_ENUM_H
#define OHOS_TEXT_ANI_ENUM_H

#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>


namespace OHOS::Text::ANI {
class AniTextEnum {
public:
    static const std::vector<uint32_t> textAlign;
    static const std::vector<uint32_t> textDecoration;
    static const std::vector<uint32_t> textDecorationStyle;
    static const std::vector<uint32_t> fontWeight;
    static const std::vector<uint32_t> fontStyle;
    static const std::vector<uint32_t> textBaseLine;
    static const std::vector<uint32_t> textDirection;
    static const std::vector<uint32_t> wordBreakType;
    static const std::vector<uint32_t> breakStrategy;
    static const std::vector<uint32_t> ellipsisModal;
    static const std::vector<uint32_t> placeHolderAlignment;
    static const std::vector<uint32_t> rectWidthStyle;
    static const std::vector<uint32_t> rectHeightStyle;
    static const std::vector<uint32_t> affinity;
    static const std::vector<uint32_t> fontWidth;
    static const std::vector<uint32_t> textHeightBehavior;
    static const std::vector<uint32_t> systemFontType;
    static const std::vector<uint32_t> textHighContrast;
    static const std::vector<uint32_t> textBadgeType;
    static const std::vector<uint32_t> textUndefinedGlyphDisplay;
    static const std::vector<uint32_t> textVerticalAlign;
    static const std::vector<uint32_t> lineHeightStyle;
    static const std::vector<uint32_t> fontEdging;
};

std::optional<size_t> aniGetEnumIndex(const std::vector<uint32_t>& enumValues, uint32_t enumValue);
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_ENUM_H