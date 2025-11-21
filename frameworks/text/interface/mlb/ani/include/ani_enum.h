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

#include <string_view>
#include <vector>
#include <unordered_map>

namespace OHOS::Text::ANI {
class AniTextEnum {
public:
    static const std::vector<uint32_t> ENUM_TEXT_ALIGN;
    static const std::vector<uint32_t> ENUM_TEXT_DECORATION;
    static const std::vector<uint32_t> ENUM_TEXT_DECORATION_STYLE;
    static const std::vector<uint32_t> ENUM_FONT_WEIGHT;
    static const std::vector<uint32_t> ENUM_FONT_STYLE;
    static const std::vector<uint32_t> ENUM_TEXT_BASE_LINE;
    static const std::vector<uint32_t> ENUM_TEXT_DIRECTION;
    static const std::vector<uint32_t> ENUM_WORD_BREAK_TYPE;
    static const std::vector<uint32_t> ENUM_BREAK_STRATEGY;
    static const std::vector<uint32_t> ENUM_ELLIPSIS_MODAL;
    static const std::vector<uint32_t> ENUM_PLACEHOLDER_ALIGNMENT;
    static const std::vector<uint32_t> ENUM_RECT_WIDTH_STYLE;
    static const std::vector<uint32_t> ENUM_RECT_HEIGHT_STYLE;
    static const std::vector<uint32_t> ENUM_AFFINITY;
    static const std::vector<uint32_t> ENUM_FONT_WIDTH;
    static const std::vector<uint32_t> ENUM_TEXTHEIGHTBEHAVIOR;
    static const std::vector<uint32_t> ENUM_SYSTEM_FONT_TYPE;
    static const std::vector<uint32_t> ENUM_TEXT_HIGH_CONTRAST;
    static const std::vector<uint32_t> ENUM_TEXT_BADGE_TYPE;
    static const std::vector<uint32_t> ENUM_TEXT_UNDEFINED_GLYPH_DISPLAY;
    static const std::vector<uint32_t> ENUM_TEXT_VERTICAL_ALIGN;
};

size_t getEnumIndex(const std::vector<uint32_t>& enumValues, uint32_t enumValue);
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_ENUM_H