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

#include "utils/resource_holder.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ResourceHolder::ResourceHolder() : impl_(ImplFactory::CreateResourceHolderImpl()) {}

void ResourceHolder::HoldResource(const std::shared_ptr<Drawing::Image>& img)
{
    impl_->HoldResource(img);
}

void ResourceHolder::ReleaseResource()
{
    impl_->ReleaseResource();
}

bool ResourceHolder::IsEmpty()
{
    return impl_->IsEmpty();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS