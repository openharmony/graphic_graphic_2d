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

#ifndef LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_TXT_H
#define LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_TXT_H

#include <iostream>
#include <vector>

#ifndef USE_ROSEN_DRAWING
#include "include/core/HMSymbol.h"
#else
#include "drawing.h"
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {

class HMSymbolTxt {
public:
    HMSymbolTxt() {}
    ~HMSymbolTxt() {}

#ifndef USE_ROSEN_DRAWING
    void SetRenderColor(const std::vector<SColor>& colorList)
#else
    void SetRenderColor(const std::vector<RSSColor>& colorList)
#endif
    {
        colorList_ = colorList;
    }

#ifndef USE_ROSEN_DRAWING
    void SetRenderColor(const SColor& colorList)
#else
    void SetRenderColor(const RSSColor& colorList)
#endif
    {
        colorList_ = {colorList};
    }

#ifndef USE_ROSEN_DRAWING
    void SetRenderMode(SymbolRenderingStrategy renderMode)
#else
    void SetRenderMode(RSSymbolRenderingStrategy renderMode)
#endif
    {
        renderMode_ = renderMode;
    }

#ifndef USE_ROSEN_DRAWING
    void SetSymbolEffect(const EffectStrategy& effectStrategy)
#else
    void SetSymbolEffect(const RSEffectStrategy& effectStrategy)
#endif
    {
        effectStrategy_ = effectStrategy;
    }

#ifndef USE_ROSEN_DRAWING
    std::vector<SColor> GetRenderColor() const
#else
    std::vector<RSSColor> GetRenderColor() const
#endif
    {
        return colorList_;
    }

#ifndef USE_ROSEN_DRAWING
    SymbolRenderingStrategy GetRenderMode() const
#else
    RSSymbolRenderingStrategy GetRenderMode() const
#endif
    {
        return renderMode_;
    }

#ifndef USE_ROSEN_DRAWING
    EffectStrategy GetEffectStrategy() const
#else
    RSEffectStrategy GetEffectStrategy() const
#endif
    {
        return effectStrategy_;
    }

    bool operator ==(HMSymbolTxt const &sym) const;
     
private:
#ifndef USE_ROSEN_DRAWING
    std::vector<SColor> colorList_;
    SymbolRenderingStrategy renderMode_ = SymbolRenderingStrategy::SINGLE;
    EffectStrategy effectStrategy_ = EffectStrategy::NONE;
#else
    std::vector<RSSColor> colorList_;
    RSSymbolRenderingStrategy renderMode_ = RSSymbolRenderingStrategy::SINGLE;
    RSEffectStrategy effectStrategy_ = RSEffectStrategy::NONE;
#endif
};
}
}
}
#endif