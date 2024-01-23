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

#include "hm_symbol_txt.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
    bool HMSymbolTxt::operator ==(HMSymbolTxt const &sym) const
    {
        if (colorList_.size() != sym.colorList_.size()) {
            return false;
        }
        for (size_t i = 0; i < colorList_.size(); i++) {
            if (colorList_[i].a != sym.colorList_[i].a ||
                colorList_[i].r != sym.colorList_[i].r ||
                colorList_[i].g != sym.colorList_[i].g ||
                colorList_[i].b != sym.colorList_[i].b) {
                    return false;
                }
        }
        if (renderMode_ != sym.renderMode_ || effectStrategy_ != sym.effectStrategy_) {
            return false;
        }
        return true;
    }
}
}
}
