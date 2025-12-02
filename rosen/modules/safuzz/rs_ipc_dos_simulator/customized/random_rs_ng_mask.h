/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SAFUZZ_RANDOM_RS_NG_MASK_H
#define SAFUZZ_RANDOM_RS_NG_MASK_H

#include "effect/rs_render_mask_base.h"

namespace OHOS {
namespace Rosen {
class RandomRSNGMaskPtr {
public:
    static std::shared_ptr<RSNGRenderMaskBase> GetRandomValue();

private:
    static std::shared_ptr<RSNGRenderMaskBase> GetRandomSingleMask();
    static std::shared_ptr<RSNGRenderMaskBase> GetRandomMaskChain();

#define DECLARE_MASK(MaskName, MaskType, ...) \
    static std::shared_ptr<RSNGRenderMaskBase> GetRandom##MaskName()

#include "effect/rs_render_mask_def.in"

#undef DECLARE_MASK
};

} // namespace Rosen
} // namespace OHOS
#endif // SAFUZZ_RANDOM_RS_NG_MASK_H
