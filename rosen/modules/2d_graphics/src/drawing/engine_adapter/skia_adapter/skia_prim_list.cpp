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

#include "skia_prim_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaPrimList::SkiaPrimList() noexcept {}

void SkiaPrimList::UpdateAlphaFactor(float alphaFactor)
{
    LOGD("SkiaPrimList does not support UpdateAlphaFactor.");
}

size_t SkiaPrimList::GetPrimCount() const
{
    LOGD("SkiaPrimList does not support GetPrimCount.");
    return 0;
}

std::vector<std::pair<const char*, size_t>> SkiaPrimList::GetPrimCountPerType() const
{
    LOGD("SkiaPrimList does not support GetPrimCountPerType.");
    return {};
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
