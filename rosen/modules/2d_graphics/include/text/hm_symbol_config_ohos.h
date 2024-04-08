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

#ifndef HM_SYMBOL_CONFIG_OHOS_H
#define HM_SYMBOL_CONFIG_OHOS_H

#include "hm_symbol.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class HmSymbolConfigOhosImpl;

class DRAWING_API HmSymbolConfigOhos {
public:
    static DrawingSymbolLayersGroups GetSymbolLayersGroups(uint32_t glyphId);

    static std::vector<std::vector<DrawingPiecewiseParameter>> GetGroupParameters(
        DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode = 0,
        DrawingCommonSubType commonSubType = DrawingCommonSubType::UP);
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif