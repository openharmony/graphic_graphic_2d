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
#ifndef UIEFFECT_EFFECT_VISUAL_EFFECT_PARA_H
#define UIEFFECT_EFFECT_VISUAL_EFFECT_PARA_H

namespace OHOS {
namespace Rosen {
class VisualEffectPara {
public:
    enum ParaType {
        NONE,
        BACKGROUND_COLOR_EFFECT,
        BORDER_LIGHT_EFFECT,
    };

    VisualEffectPara()  = default;
    virtual ~VisualEffectPara() = default;
    ParaType GetParaType()
    {
        return type_;
    }
protected:
    ParaType type_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_VISUAL_EFFECT_PARA_H
