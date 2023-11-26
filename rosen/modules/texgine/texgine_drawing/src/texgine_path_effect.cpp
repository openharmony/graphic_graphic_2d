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

#include "texgine_path_effect.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
sk_sp<SkPathEffect> TexginePathEffect::GetPathEffect() const
#else
std::shared_ptr<RSPathEffect> TexginePathEffect::GetPathEffect() const
#endif
{
    return pathEffect_;
}

#ifndef USE_ROSEN_DRAWING
void TexginePathEffect::SetPathEffect(const sk_sp<SkPathEffect> effect)
#else
void TexginePathEffect::SetPathEffect(const std::shared_ptr<RSPathEffect> effect)
#endif
{
    pathEffect_ = effect;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
