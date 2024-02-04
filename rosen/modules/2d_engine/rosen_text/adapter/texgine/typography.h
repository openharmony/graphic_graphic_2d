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

#ifndef ROSEN_TEXT_ADAPTER_TEXGINE_TYPOGRAPHY_H
#define ROSEN_TEXT_ADAPTER_TEXGINE_TYPOGRAPHY_H

#include "rosen_text/typography.h"

#include "texgine/typography.h"
#include "rosen_text/symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTextEngine {
class Typography : public ::OHOS::Rosen::Typography {
public:
    explicit Typography(std::shared_ptr<TextEngine::Typography> typography);

    double GetMaxWidth() const override;
    double GetHeight() const override;
    double GetActualWidth() const override;
    double GetMinIntrinsicWidth() override;
    double GetMaxIntrinsicWidth() override;
    double GetAlphabeticBaseline() override;
    double GetIdeographicBaseline() override;
    bool DidExceedMaxLines() const override;
    int GetLineCount() const override;

    void SetIndents(const std::vector<float> &indents) override;
    void Layout(double width) override;
    void Paint(SkCanvas *canvas, double x, double y) override;
    void Paint(Drawing::Canvas *drawCanvas, double x, double y) override;

    std::vector<TextRect> GetTextRectsByBoundary(size_t left, size_t right,
        TextRectHeightStyle heightStyle, TextRectWidthStyle widthStyle) override;
    std::vector<TextRect> GetTextRectsOfPlaceholders() override;
    IndexAndAffinity GetGlyphIndexByCoordinate(double x, double y) override;
    Boundary GetWordBoundaryByIndex(size_t index) override;
    Boundary GetActualTextRange(int lineNumber, bool includeSpaces) override;

    double GetLineHeight(int lineNumber) override
    {
        return 0.0;
    }
    
    double GetLineWidth(int lineNumber) override
    {
        return 0.0;
    }

    void SetAnimation(
        std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc) override
    {
        if (animationFunc != nullptr && typography_ != nullptr) {
            typography_->SetAnimation(animationFunc);
        }
    }

private:
    std::shared_ptr<TextEngine::Typography> typography_ = nullptr;
};
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TEXGINE_TYPOGRAPHY_H
