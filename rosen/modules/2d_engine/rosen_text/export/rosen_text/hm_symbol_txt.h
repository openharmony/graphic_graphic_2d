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

#define EFFECT_NONE 0
#define EFFECT_SCALE 1
#define EFFECT_HIERARCHICAL 2
#define RENDER_SINGLE 0
#define RENDER_MULTIPLE_COLOR 1
#define RENDER_MULTIPLE_OPACITY 2

namespace OHOS {
namespace Rosen {

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
        switch (effectStrategy) {
            case EFFECT_NONE:
                effectStrategy_ = Drawing::DrawingEffectStrategy::NONE;
                break;
            case EFFECT_SCALE:
                effectStrategy_ = Drawing::DrawingEffectStrategy::SCALE;
                break;
            case EFFECT_HIERARCHICAL:
                effectStrategy_ = Drawing::DrawingEffectStrategy::HIERARCHICAL;
                break;
            default:
                break;
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

private:
    std::vector<Drawing::DrawingSColor> colorList_;
    Drawing::DrawingSymbolRenderingStrategy renderMode_ = Drawing::DrawingSymbolRenderingStrategy::SINGLE;
    Drawing::DrawingEffectStrategy effectStrategy_ = Drawing::DrawingEffectStrategy::NONE;
};
}
}
#endif