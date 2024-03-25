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

#include "text/hm_symbol_config_ohos.h"

#include "static_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<DrawingSymbolLayersGroups> HmSymbolConfigOhos::GetSymbolLayersGroups(uint32_t glyphId)
{
    return StaticFactory::GetSymbolLayersGroups(glyphId);
}

std::shared_ptr<std::vector<std::vector<DrawingPiecewiseParameter>>> HmSymbolConfigOhos::GetGroupParameters(
    DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode)
{
    return StaticFactory::GetGroupParameters(type, subType, animationMode);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS