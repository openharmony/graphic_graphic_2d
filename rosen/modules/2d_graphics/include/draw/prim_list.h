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

#ifndef PRIM_LIST_H
#define PRIM_LIST_H

#include <memory>
#include <utility>
#include <vector>

#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class PrimListImpl;

class DRAWING_API PrimList {
public:
    PrimList();
    ~PrimList();

    PrimList(const PrimList&) = delete;
    PrimList& operator=(const PrimList&) = delete;

    /**
     * @brief Updates the alpha factor for all primitives.
     * @param alphaFactor The alpha factor value to apply, range from 0.0 to 1.0.
     */
    void UpdateAlphaFactor(float alphaFactor);

    /**
     * @brief Gets the total count of primitives in the list.
     * @return The total number of primitives.
     */
    size_t GetPrimCount() const;

    /**
     * @brief Gets the count of primitives for each type.
     * @return A vector containing the type name and count of each primitive type.
     */
    std::vector<std::pair<const char*, size_t>> GetPrimCountPerType() const;

private:
    std::unique_ptr<PrimListImpl> impl_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // PRIM_LIST_H
