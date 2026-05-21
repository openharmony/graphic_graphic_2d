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

#include "draw/prim_list.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

PrimList::PrimList() : impl_(ImplFactory::CreatePrimListImpl()) {}

PrimList::~PrimList() {}

void PrimList::UpdateAlphaFactor(float alphaFactor)
{
    impl_->UpdateAlphaFactor(alphaFactor);
}

size_t PrimList::GetPrimCount() const
{
    return impl_->GetPrimCount();
}

std::vector<std::pair<const char*, size_t>> PrimList::GetPrimCountPerType() const
{
    return impl_->GetPrimCountPerType();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
