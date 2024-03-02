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

#include "texgine_path_1d_path_effect.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::shared_ptr<TexginePathEffect> TexginePath1DPathEffect::Make(const TexginePath &path,
    float advance, float phase, Style style)
{
    auto effect = RSPathEffect::CreatePathDashEffect(*path.GetPath().get(), advance, phase,
        static_cast<RSPathDashStyle>(style));
    auto pathEffect = std::make_shared<TexginePathEffect>();
    pathEffect->SetPathEffect(effect);
    return pathEffect;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
