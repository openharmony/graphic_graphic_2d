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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_BUILDER_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_BUILDER_H

#include <memory>
#include <string>
#include <vector>

#include "texgine/any_span.h"
#include "texgine/typography.h"
#include "texgine/typography_style.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief TypographyBuilder can build Typography by styles and texts.
 */
class TypographyBuilder {
public:
    /*
     * @brief Create TypographyBuilder instance.
     * @param ys             the style used by Typography.
     * @param fontProviders  the font providers used by Typography.
     * @return TypographyBuilder
     */
    static std::unique_ptr<TypographyBuilder> Create(const TypographyStyle& ys = {},
        std::shared_ptr<FontProviders> fontProviders = FontProviders::SystemFontOnly());

    virtual ~TypographyBuilder() = default;

    /*
     * @brief Push TextStyle in style stack.
     * @param xs The style to be pushed onto the stack.
     */
    virtual void PushStyle(const TextStyle& xs) = 0;

    /*
     * @brief Pop a style in the style stack.
     */
    virtual void PopStyle() = 0;

    /*
     * @brief Append a span.
     * @param span The appended span.
     */
    virtual void AppendSpan(const std::shared_ptr<AnySpan>& span) = 0;
    virtual void AppendSpan(const std::string& text) = 0;
    virtual void AppendSpan(const std::u16string& text) = 0;
    virtual void AppendSpan(const std::u32string& text) = 0;
    virtual void AppendSpan(const std::vector<uint16_t>& text) = 0;
    virtual void AppendSpan(const std::vector<uint32_t>& text) = 0;

    /*
     * @brief Use styles and spans to build a Typography.
     * @return Typography
     */
    virtual std::shared_ptr<Typography> Build() = 0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_BUILDER_H
