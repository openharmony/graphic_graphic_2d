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

#ifndef PICTUREIMPL_H
#define PICTUREIMPL_H

#include "base_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PictureImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    PictureImpl() noexcept {}
    ~PictureImpl() override {}
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
