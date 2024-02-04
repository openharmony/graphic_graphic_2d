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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXT_SPAN_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXT_SPAN_H

#include <any>

#include "char_groups.h"
#include "texgine_canvas.h"
#include "texgine_font_metrics.h"
#include "texgine_text_blob.h"
#include "texgine/typography.h"
#include "texgine/typography_style.h"
#include "symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TextSpan : public std::enable_shared_from_this<TextSpan> {
public:
    static std::shared_ptr<TextSpan> MakeEmpty();
    static std::shared_ptr<TextSpan> MakeFromText(std::string const &text);
    static std::shared_ptr<TextSpan> MakeFromText(std::u16string const &text);
    static std::shared_ptr<TextSpan> MakeFromText(std::u32string const &text);
    static std::shared_ptr<TextSpan> MakeFromText(std::vector<uint16_t> const &text);
    static std::shared_ptr<TextSpan> MakeFromText(std::vector<uint32_t> const &text);
    static std::shared_ptr<TextSpan> MakeFromCharGroups(const CharGroups &cgs);

    TextSpan() = default;
    virtual ~TextSpan() = default;
    void AddUTF16Text(std::vector<uint16_t> const &text);

    double GetWidth() const;
    double GetHeight() const;
    double GetPostBreak() const;
    double GetPreBreak() const;
    bool IsRTL() const;

    void PaintDecoration(TexgineCanvas &canvas, double offsetX, double offsetY, const TextStyle &xs);
    void PaintDecorationStyle(TexgineCanvas &canvas, double left, double right, double y, const TextStyle &xs);
    void Paint(TexgineCanvas &canvas, double offsetX, double offsetY, const TextStyle &xs, const RoundRectType &rType);
    void PaintShadow(TexgineCanvas &canvas, double offsetX, double offsetY, const std::vector<TextShadow> &shadows);
    void SymbolAnimation(const TextStyle &xs);

    std::shared_ptr<TextSpan> CloneWithCharGroups(CharGroups const &cgs);

    void operator+=(TextSpan const &textSpan)
    {
        u16vect_.insert(u16vect_.end(), textSpan.u16vect_.begin(), textSpan.u16vect_.end());
    }

    std::shared_ptr<TexgineFontMetrics> tmetrics_ = std::make_shared<TexgineFontMetrics>();
    bool rtl_ = false;
    std::shared_ptr<Typeface> typeface_ = nullptr;

    std::vector<uint16_t> u16vect_;
    std::shared_ptr<TexgineTextBlob> textBlob_ = nullptr;
    std::vector<double> glyphWidths_;
    CharGroups cgs_;

    double preBreak_ = 0.0;
    double postBreak_ = 0.0;
    double width_ = 0.0;
    double topInGroup_ = 0.0;
    double bottomInGroup_ = 0.0;
    double maxRoundRectRadius_ = 0.0;

    uint64_t symbolId_ = 0; // 0: text_span, > 0: symbol

    void SetAnimation(
        const std::function<bool(
            const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>& animationFunc)
    {
        if (animationFunc) {
            animationFunc_ = animationFunc;
        }
    }

    void SetSymbolId(const uint64_t& symbolId)
    {
        symbolId_ = symbolId;
    }
private:

    std::function<bool(
        const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc_ = nullptr;

    friend class TextBreaker;
    friend class BidiProcesser;
    friend class ControllerForTest;
    friend class TextMerger;
    friend class LineBreaker;
    friend class TypographyImpl;
    friend class VariantSpan;
    friend class TextShaper;
    friend class TextReverser;
    friend void ReportMemoryUsage(std::string const &member, TextSpan const &that, bool needThis);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXT_SPAN_H
