/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef SKIA_PRIM_LIST_H
#define SKIA_PRIM_LIST_H

#include <vector>

#include "impl_interface/prim_list_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API SkiaPrimList : public PrimListImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaPrimList() noexcept;
    ~SkiaPrimList() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void UpdateAlphaFactor(float alphaFactor) override;
    size_t GetPrimCount() const override;
    std::vector<std::pair<const char*, size_t>> GetPrimCountPerType() const override;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // SKIA_PRIM_LIST_H
