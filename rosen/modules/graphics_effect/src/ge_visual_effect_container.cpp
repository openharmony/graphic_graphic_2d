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

#include "ge_visual_effect_container.h"

#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEVisualEffectContainer::GEVisualEffectContainer() {}

void GEVisualEffectContainer::AddToChainedFilter(std::shared_ptr<Drawing::GEVisualEffect> visualEffect)
{
    LOGD("GERender::AddToChainedFilter %{public}s", visualEffect->GetName().c_str());
    filterVec_.push_back(visualEffect);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
