/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef MASK_FILTER_IMPL_H
#define MASK_FILTER_IMPL_H

#include <memory>

#include "base_impl.h"

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class BlurType;
class MaskFilterImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    MaskFilterImpl() noexcept {}
    ~MaskFilterImpl() override {}
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual void InitWithBlur(BlurType t, scalar sigma) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif