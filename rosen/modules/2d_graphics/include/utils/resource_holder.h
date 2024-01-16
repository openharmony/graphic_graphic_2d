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

#ifndef RESOURCE_HOLDER_H
#define RESOURCE_HOLDER_H
#include "drawing/engine_adapter/impl_interface/resource_holder_impl.h"
#include "utils/drawing_macros.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API ResourceHolder final {
public:
    ResourceHolder();
    ~ResourceHolder() = default;
    void HoldResource(const std::shared_ptr<Drawing::Image>& img);
    void ReleaseResource();
    bool IsEmpty();
private:
    std::shared_ptr<ResourceHolderImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // RESOURCE_HOLDER_H