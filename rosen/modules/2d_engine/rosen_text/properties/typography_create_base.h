/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_PROPERTIES_TYPOGRAPHY_CREATE_BASE_H
#define ROSEN_TEXT_PROPERTIES_TYPOGRAPHY_CREATE_BASE_H

#include <memory>
#include <string>
#include "rosen_text/ui/font_collection.h"
#include "rosen_text/ui/typography.h"

#include "rosen_text/properties/placeholder_run.h"
#include "rosen_text/properties/text_style.h"
#include "rosen_text/properties/typography_style.h"

namespace rosen {
class Typography;
class TypographyCreateBase {
public:
    TypographyCreateBase() {};
    TypographyCreateBase(const TypographyStyle& style, std::shared_ptr<FontCollection> font_collection);
    virtual ~TypographyCreateBase() {};
    virtual void PushStyle(const TextStyle& style);
    virtual void Pop();
    virtual void AddText(const std::u16string& text);
    virtual void AddPlaceholder(PlaceholderRun& span);
    virtual std::unique_ptr<Typography> Build();
    TypographyCreateBase(const TypographyCreateBase&) = delete;
    TypographyCreateBase& operator=(const TypographyCreateBase&) = delete;
};
} // namespace rosen
#endif // ROSEN_TEXT_PROPERTIES_TYPOGRAPHY_CREATE_BASE_H
