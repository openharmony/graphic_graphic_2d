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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_HM_SYMBOL_TXT_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_HM_SYMBOL_TXT_H

#include <iostream>
#include <vector>
#include "draw/color.h"

#include "text/hm_symbol.h"

#define RENDER_SINGLE 0
#define RENDER_MULTIPLE_COLOR 1
#define RENDER_MULTIPLE_OPACITY 2

namespace OHOS {
namespace Rosen {

static const std::map<uint32_t, Drawing::DrawingEffectStrategy> EFFECT_TYPES = {
    {0, Drawing::DrawingEffectStrategy::NONE},
    {1, Drawing::DrawingEffectStrategy::SCALE},
    {2, Drawing::DrawingEffectStrategy::VARIABLE_COLOR},
    {3, Drawing::DrawingEffectStrategy::APPEAR},
    {4, Drawing::DrawingEffectStrategy::DISAPPEAR},
    {5, Drawing::DrawingEffectStrategy::BOUNCE}};

enum VisualMode {
    VISUAL_MEDIUM = 0,
    VISUAL_SMALL = 1,
    VISUAL_LARGER = 2
};

class RS_EXPORT HMSymbolTxt {
public:
    HMSymbolTxt() {}
    ~HMSymbolTxt() {}

    void SetRenderColor(const std::vector<Drawing::DrawingSColor>& colorList)
    {
        colorList_ = colorList;
    }

    void SetRenderColor(const std::vector<Drawing::Color>& colorList)
    {
        colorList_.clear();
        for (auto color: colorList) {
            Drawing::DrawingSColor colorIt = {color.GetAlphaF(), color.GetRed(), color.GetGreen(), color.GetBlue()};
            colorList_.push_back(colorIt);
        }
    }

    void SetRenderColor(const Drawing::Color& color)
    {
        Drawing::DrawingSColor colorIt = {color.GetAlphaF(), color.GetRed(), color.GetGreen(), color.GetBlue()};
        colorList_ = {colorIt};
    }

    void SetRenderColor(const Drawing::DrawingSColor& colorList)
    {
        colorList_ = {colorList};
    }

    void SetRenderMode(const uint32_t& renderMode)
    {
        switch (renderMode) {
            case RENDER_SINGLE:
                renderMode_ = Drawing::DrawingSymbolRenderingStrategy::SINGLE;
                break;
            case RENDER_MULTIPLE_OPACITY:
                renderMode_ = Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY;
                break;
            case RENDER_MULTIPLE_COLOR:
                renderMode_ = Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR;
                break;
            default:
                break;
        }
    }

    void SetSymbolEffect(const uint32_t& effectStrategy)
    {
        auto iter = EFFECT_TYPES.find(effectStrategy);
        if (iter != EFFECT_TYPES.end()) {
            effectStrategy_ = iter->second;
        }
    }

    std::vector<Drawing::DrawingSColor> GetRenderColor() const
    {
        return colorList_;
    }

    Drawing::DrawingSymbolRenderingStrategy GetRenderMode() const
    {
        return renderMode_;
    }

    Drawing::DrawingEffectStrategy GetEffectStrategy() const
    {
        return effectStrategy_;
    }

    void SetAnimationMode(const uint16_t animationMode)
    {
        animationMode_ = animationMode > 0 ? 1 : 0; // 1 is whole or add, 0 is hierarchical or iterate
    }

    void SetRepeatCount(const int repeatCount)
    {
        repeatCount_ = repeatCount;
    }

    void SetAminationStart(const bool aminationStart)
    {
        aminationStart_ = aminationStart;
    }

    uint16_t GetAnimationMode() const
    {
        return animationMode_;
    }

    int GetRepeatCount() const
    {
        return repeatCount_;
    }

    bool GetAminationStart() const
    {
        return aminationStart_;
    }

    void SetVisualMode(const VisualMode visual)
    {
        visualMap_.clear();
        if (visual == VisualMode::VISUAL_SMALL) {
            visualMap_["ss01"] = 1;
        }

        if (visual == VisualMode::VISUAL_LARGER) {
            visualMap_["ss02"] = 1;
        }
    }

    std::map<std::string, int> GetVisualMap() const
    {
        return visualMap_;
    }

private:
    std::vector<Drawing::DrawingSColor> colorList_;
    Drawing::DrawingSymbolRenderingStrategy renderMode_ = Drawing::DrawingSymbolRenderingStrategy::SINGLE;
    Drawing::DrawingEffectStrategy effectStrategy_ = Drawing::DrawingEffectStrategy::NONE;

    // animationMode_ is the implementation mode of the animation effect:
    // common_animations: the 0 is the wholeSymbol effect and 1 is the byLayer effect;
    // variable_color : the 0 is the cumulative  effect and 1 is the iteratuve effect.
    uint16_t animationMode_ = 1;
    int repeatCount_ = 1;
    bool aminationStart_ = true;
    std::map<std::string, int> visualMap_;
};
}
}
#endif