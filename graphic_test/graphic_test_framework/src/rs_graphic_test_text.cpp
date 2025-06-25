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

#include "rs_graphic_test_text.h"

#include "draw/brush.h"
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "text/font.h"
#include "text/font_types.h"
#include "text/text_blob.h"

namespace OHOS {
namespace Rosen {
void TextCustomModifier::SetPosition(Vector2f position)
{
    if (position_ == nullptr) {
        position_ = std::make_shared<RSProperty<Vector2f>>(position);
        AttachProperty(position_);
    } else {
        position_->Set(position);
    }
}

void TextCustomModifier::SetBrushColor(uint32_t color)
{
    if (color_ == nullptr) {
        color_ = std::make_shared<RSProperty<uint32_t>>(color);
        AttachProperty(color_);
    } else {
        color_->Set(color);
    }
}

void TextCustomModifier::SetFontSize(float size)
{
    if (size_ == nullptr) {
        size_ = std::make_shared<RSProperty<float>>(size);
        AttachProperty(size_);
    } else {
        size_->Set(size);
    }
}

void TextCustomModifier::SetText(std::string text)
{
    if (text_ == nullptr) {
        text_ = std::make_shared<RSProperty<std::string>>(text);
        AttachProperty(text_);
    } else {
        text_->Set(text);
    }
}

void TextCustomModifier::Draw(DrawingContext& context) const
{
    if (text_ == nullptr)
        return;
    uint32_t colorInt = 0xff000000;
    Vector2f position = { 0.0, 0.0 };
    float fontSize = 16;
    if (color_ != nullptr)
        colorInt = color_->Get();
    if (position_ != nullptr)
        position = position_->Get();
    if (size_ != nullptr)
        fontSize = size_->Get();
    auto brushColor = OHOS::Rosen::Drawing::Color(colorInt);
    Drawing::Brush brush;
    brush.SetColor(brushColor);
    brush.SetAntiAlias(true);

    Drawing::Font font = Drawing::Font();
    font.SetSize(fontSize); // text size 16
    std::shared_ptr<Drawing::TextBlob> scaleInfoTextBlob =
        Drawing::TextBlob::MakeFromString(text_->Get().c_str(), font, Drawing::TextEncoding::UTF8);
    context.canvas->AttachBrush(brush);
    context.canvas->DrawTextBlob(scaleInfoTextBlob.get(), position[0], position[1]);
    context.canvas->DetachBrush();
}
} // namespace Rosen
} // namespace OHOS