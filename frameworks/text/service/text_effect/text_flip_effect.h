/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef OHOS_ROSEN_TEXT_TEXT_FLIP_EFFECT_H
#define OHOS_ROSEN_TEXT_TEXT_FLIP_EFFECT_H

#include <functional>
#include <vector>

#include "rosen_text/symbol_animation_config.h"
#include "text_effect.h"

namespace OHOS::Rosen {
struct TextFlipEffectCharConfig {
    std::vector<std::pair<float, int>> yUpOffsets;
    std::vector<std::pair<float, int>> yDownOffsets;
    std::vector<std::pair<float, int>> scales;
    std::vector<std::pair<float, int>> opacity;
    std::vector<std::pair<int, int>> blur;
};

enum class TextEffectFlipDirection {
    UP,
    DOWN,
};

class TextFlipEffect : public TextEffect {
public:
    TextFlipEffect();
    ~TextFlipEffect() override;

    int UpdateEffectConfig(const std::unordered_map<TextEffectAttribute, std::string>& config) override;
    int AppendTypography(const std::vector<TypographyConfig>& typographyConfigs) override;
    void RemoveTypography(const std::vector<TypographyConfig>& typographyConfigs) override;
    int UpdateTypography(std::vector<std::pair<TypographyConfig, TypographyConfig>>& typographyConfigs) override;
    void StartEffect(Drawing::Canvas* canvas, double x, double y) override;
    void StopEffect() override;
    void NoEffect(Drawing::Canvas* canvas, double x, double y) override;

private:
    bool CheckInputParams(const std::unordered_map<TextEffectAttribute, std::string>& config);
    bool CheckDirection(const std::string& direction);
    void SetDirection(const std::string& direction);
    bool CheckBlurEnable(const std::string& enable);
    void SetBlurEnable(const std::string& enable);
    void DrawTextFlip(std::vector<TextBlobRecordInfo>& infos, Drawing::Canvas* canvas, double x, double y);
    void DrawResidualText(Drawing::Canvas* canvas, double height,
        const std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& func);
    void DrawTextFlipElements(Drawing::Canvas* canvas, double height, const Drawing::Color& color,
        const std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& func,
        const std::vector<std::vector<TextEngine::TextEffectElement>>& effectElements);
    void ClearTypography();
    std::vector<std::vector<TextEngine::TextEffectElement>> GenerateChangeElements(
        const std::shared_ptr<Drawing::TextBlob>& blob, double x, double y);
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> GenerateFlipConfig(double height);

    TypographyConfig typographyConfig_;
    TextEffectFlipDirection direction_{TextEffectFlipDirection::UP};
    bool blurEnable_{false};
    TextFlipEffectCharConfig inChar_;
    TextFlipEffectCharConfig outChar_;
    std::vector<uint16_t> lastAllBlobGlyphIds_;
    std::vector<uint16_t> currentBlobGlyphIds_;
    size_t currentGlyphIndex_{0};
    int changeNumber_{0};

    struct FlipAttributeFunction {
        FlipAttributeFunction() = default;
        FlipAttributeFunction(TextEffectAttribute inputAttribute,
            std::function<bool(TextFlipEffect*, const std::string&)> inputCheck,
            std::function<void(TextFlipEffect*, const std::string&)> inputSet)
            : attribute(inputAttribute), checkFunc(inputCheck), setFunc(inputSet) {}

        TextEffectAttribute attribute{TextEffectAttribute::FLIP_DIRECTION};
        std::function<bool(TextFlipEffect*, const std::string&)> checkFunc{nullptr};
        std::function<void(TextFlipEffect*, const std::string&)> setFunc{nullptr};
    };
    const std::vector<FlipAttributeFunction> supportAttributes_ = {
        {TextEffectAttribute::FLIP_DIRECTION, &TextFlipEffect::CheckDirection, &TextFlipEffect::SetDirection},
        {TextEffectAttribute::BLUR_ENABLE, &TextFlipEffect::CheckBlurEnable, &TextFlipEffect::SetBlurEnable},
    };
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_TEXT_TEXT_FLIP_EFFECT_H