/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_GRAPHIC_TEST_TEXT_H
#define RS_GRAPHIC_TEST_TEXT_H

#include "rs_graphic_test.h"

#if defined(MODIFIER_NG)
#include "modifier_ng/custom/rs_content_style_modifier.h"
#else
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_modifier.h"
#endif

namespace OHOS {
namespace Rosen {
#if defined(MODIFIER_NG)
using ContentStyleModifier = ModifierNG::RSContentStyleModifier;
using DrawingContext = ModifierNG::RSDrawingContext;
#else
using ContentStyleModifier = RSContentStyleModifier;
using DrawingContext = RSDrawingContext;
#endif
class TextCustomModifier : public ContentStyleModifier {
public:
    ~TextCustomModifier() = default;

    void Draw(DrawingContext& context) const;
    void SetPosition(OHOS::Rosen::Vector2f position);
    void SetBrushColor(uint32_t color);
    void SetFontSize(float size);
    void SetText(std::string text);

private:
    std::shared_ptr<OHOS::Rosen::RSProperty<OHOS::Rosen::Vector2f>> position_;
    std::shared_ptr<OHOS::Rosen::RSProperty<uint32_t>> color_;
    std::shared_ptr<OHOS::Rosen::RSProperty<float>> size_;
    std::shared_ptr<OHOS::Rosen::RSProperty<std::string>> text_;
};
} // namespace Rosen
} // namespace OHOS
#endif