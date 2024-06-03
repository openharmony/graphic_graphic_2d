/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_PARAGRAPH_BUILDER_H
#define ROSEN_MODULES_SPTEXT_PARAGRAPH_BUILDER_H

#include <memory>
#include <string>

#include "font_collection.h"
#include "paragraph.h"
#include "paragraph_style.h"
#include "placeholder_run.h"
#include "text_style.h"
#include "utils.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class ParagraphBuilder {
public:
    static std::unique_ptr<ParagraphBuilder> Create(const ParagraphStyle& style,
      std::shared_ptr<txt::FontCollection> fontCollection);

    virtual ~ParagraphBuilder() = default;

    virtual void PushStyle(const TextStyle& style) = 0;
    virtual void Pop() = 0;
    virtual void AddText(const std::u16string& text) = 0;
    virtual void AddPlaceholder(PlaceholderRun& span) = 0;

    virtual std::unique_ptr<Paragraph> Build() = 0;

protected:
    ParagraphBuilder() = default;

private:
    DISALLOW_COPY_AND_ASSIGN(ParagraphBuilder);
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PARAGRAPH_BUILDER_H
