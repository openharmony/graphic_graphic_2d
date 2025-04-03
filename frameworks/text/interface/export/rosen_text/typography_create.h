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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_CREATE_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_CREATE_H

#include <memory>

#include "common/rs_macros.h"
#include "font_collection.h"
#include "line_typography.h"
#include "text_style.h"
#include "typography.h"
#include "typography_style.h"

namespace OHOS {
namespace Rosen {
enum class PlaceholderVerticalAlignment {
    OFFSET_AT_BASELINE,
    ABOVE_BASELINE,
    BELOW_BASELINE,
    TOP_OF_ROW_BOX,
    BOTTOM_OF_ROW_BOX,
    CENTER_OF_ROW_BOX,
};

struct PlaceholderSpan {
    double width = 0.0;
    double height = 0.0;
    PlaceholderVerticalAlignment alignment;
    TextBaseline baseline;
    double baselineOffset = 0.0;
};

class RS_EXPORT TypographyCreate {
public:
    static std::unique_ptr<TypographyCreate> Create(const TypographyStyle& style,
        std::shared_ptr<FontCollection> collection);
    virtual ~TypographyCreate() = default;
    virtual void PushStyle(const TextStyle& style) = 0;
    virtual void PopStyle() = 0;
    virtual void AppendText(const std::u16string& text) = 0;
    virtual void AppendPlaceholder(const PlaceholderSpan& span) = 0;
    virtual std::unique_ptr<Typography> CreateTypography() = 0;
    virtual std::unique_ptr<LineTypography> CreateLineTypography() = 0;
    virtual void AppendSymbol(const uint32_t& symbolId) {}
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_CREATE_H
