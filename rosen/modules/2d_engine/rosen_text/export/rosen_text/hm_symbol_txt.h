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
#pragma once
#include <iostream>
#include <vector>
#include "draw/color.h"

#include "include/core/SkHMSymbol.h"

#define EFFECT_NONE 0
#define EFFECT_SCALE 1
#define EFFECT_HIERARCHICAL 2
#define RENDER_SINGLE 1
#define RENDER_MULTIPLE_OPACITY 2
#define RENDER_MULTIPLE_COLOR 3

namespace OHOS {
namespace Rosen {

class RS_EXPORT HMSymbolTxt {
public:
    HMSymbolTxt() {}
    ~HMSymbolTxt() {}

    void SetRenderColor(const std::vector<SColor>& colorList)
    {
        colorList_ = colorList;
    }

    void SetRenderColor(const std::vector<Drawing::Color>& colorList)
    {
        colorList_.clear();
        for (auto color: colorList) {
            SColor colorIt = {color.GetAlphaF(), color.GetRed(), color.GetGreen(), color.GetBlue()};
            colorList_.push_back(colorIt);
        }
    }

    void SetRenderColor(const Drawing::Color& color)
    {
        SColor colorIt = {color.GetAlphaF(), color.GetRed(), color.GetGreen(), color.GetBlue()};
        colorList_ = {colorIt};
    }

    void SetRenderColor(const SColor& colorList)
    {
        colorList_ = {colorList};
    }

    void SetRenderMode(const uint32_t& renderMode)
    {
        switch (renderMode) {
            case RENDER_SINGLE:
                renderMode_ = SymbolRenderingStrategy::SINGLE;
                break;
            case RENDER_MULTIPLE_OPACITY:
                renderMode_ = SymbolRenderingStrategy::MULTIPLE_OPACITY;
                break;
            case RENDER_MULTIPLE_COLOR:
                renderMode_ = SymbolRenderingStrategy::MULTIPLE_COLOR;
                break;
            default:
                break;
        }
    }

    void SetSymbolEffect(const uint32_t& effectStrategy)
    {
        switch (effectStrategy) {
            case EFFECT_NONE:
                effectStrategy_ = EffectStrategy::NONE;
                break;
            case EFFECT_SCALE:
                effectStrategy_ = EffectStrategy::SCALE;
                break;
            case EFFECT_HIERARCHICAL:
                effectStrategy_ = EffectStrategy::HIERARCHICAL;
                break;
            default:
                break;
        }
    }

    std::vector<SColor> GetRenderColor() const
    {
        return colorList_;
    }

    SymbolRenderingStrategy GetRenderMode() const
    {
        return renderMode_;
    }

    EffectStrategy GetEffectStrategy() const
    {
        return effectStrategy_;
    }

private:
    std::vector<SColor> colorList_;
    SymbolRenderingStrategy renderMode_ = SymbolRenderingStrategy::SINGLE;
    EffectStrategy effectStrategy_ = EffectStrategy::NONE;
};

}
}
#endif