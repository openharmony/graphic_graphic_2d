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
extern const std::unordered_map<std::string_view, const std::vector<uint32_t>&> INT_ENUM_CLASS_MAP;
extern const std::vector<uint32_t> ENUM_TEXT_ALIGN;
extern const std::vector<uint32_t> ENUM_TEXT_DECORATION;
extern const std::vector<uint32_t> ENUM_TEXT_DECORATION_STYLE;
extern const std::vector<uint32_t> ENUM_FONT_WEIGHT;
extern const std::vector<uint32_t> ENUM_FONT_STYLE;
extern const std::vector<uint32_t> ENUM_TEXT_BASE_LINE;
extern const std::vector<uint32_t> ENUM_TEXT_DIRECTION;
extern const std::vector<uint32_t> ENUM_WORD_BREAK_TYPE;
extern const std::vector<uint32_t> ENUM_BREAK_STRATEGY;
extern const std::vector<uint32_t> ENUM_ELLIPSIS_MODAL;
extern const std::vector<uint32_t> ENUM_PLACEHOLDER_ALIGNMENT;
extern const std::vector<uint32_t> ENUM_RECT_WIDTH_STYLE;
extern const std::vector<uint32_t> ENUM_RECT_HEIGHT_STYLE;
extern const std::vector<uint32_t> ENUM_AFFINITY;
extern const std::vector<uint32_t> ENUM_FONTWIDTH;
extern const std::vector<uint32_t> ENUM_TEXTHEIGHTBEHAVIOR;
extern const std::vector<uint32_t> ENUM_SYSTEM_FONT_TYPE;
extern const std::vector<uint32_t> ENUM_TEXT_HIGH_CONTRAST;
extern const std::vector<uint32_t> ENUM_TEXT_BADGE_TYPE;
extern const std::vector<uint32_t> ENUM_TEXT_UNDEFINED_GLYPH_DISPLAY;
extern const std::vector<uint32_t> ENUM_TEXT_VERTICAL_ALIGN;

uint32_t getEnumValue(const std::string_view& enumName, size_t enumIndex);
size_t getEnumIndex(const std::string_view& enumName, uint32_t enumValue);
size_t getEnumIndex(const std::vector<uint32_t>& enumValues, uint32_t enumValue);
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_ENUM_H