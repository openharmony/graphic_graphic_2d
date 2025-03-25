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

#ifndef SAFUZZ_RANDOM_RS_RENDER_PROPERTY_BASE_H
#define SAFUZZ_RANDOM_RS_RENDER_PROPERTY_BASE_H

#include <memory>

#include "customized/random_draw_cmd_list.h"
#include "customized/random_rs_filter.h"
#include "customized/random_rs_image.h"
#include "customized/random_rs_gradient_blur_para.h"
#include "customized/random_rs_mask.h"
#include "customized/random_rs_path.h"
#include "customized/random_rs_render_particle.h"
#include "customized/random_rs_shader.h"
#include "random/random_data.h"

namespace OHOS {
namespace Rosen {
class RandomRSRenderPropertyBase {
public:
    static std::shared_ptr<RSRenderPropertyBase> GetRandomRSRenderPropertyBase();

    // 1. if there is no explicit implementation of the corresponding template parameter, return RSRenderPropertyBase.
    // 2. when there is an explicit implementation: if isAnimatable is false, return RSRenderProperty<T>.
    // otherwise, RSRenderAnimatableProperty<T> is returned.
    template<typename T, bool isAnimatable>
    static std::shared_ptr<RSRenderPropertyBase> GetRandom();

private:
#define DECLARE_RENDER_PROPERTY(TYPE, RANDOM_FACTORY, FUNC_NAME)                                \
    static std::shared_ptr<RSRenderPropertyBase> GetRandom##FUNC_NAME##PropertyBase()           \
    {                                                                                           \
        return std::make_shared<RSRenderProperty<TYPE>>(                                        \
            RANDOM_FACTORY::GetRandom##FUNC_NAME(), RandomDataBasicType::GetRandomUint64());    \
    }
#define DECLARE_ANIMATABLE_RENDER_PROPERTY(TYPE, RANDOM_FACTORY, FUNC_NAME)                     \
    static std::shared_ptr<RSRenderPropertyBase> GetRandomAnimatable##FUNC_NAME##PropertyBase() \
    {                                                                                           \
        return std::make_shared<RSRenderAnimatableProperty<TYPE>>(                              \
            RANDOM_FACTORY::GetRandom##FUNC_NAME(), RandomDataBasicType::GetRandomUint64());    \
    }

#include "customized/rs_render_property.in"

#undef DECLARE_RENDER_PROPERTY
#undef DECLARE_ANIMATABLE_RENDER_PROPERTY
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_RS_RENDER_PROPERTY_BASE_H
