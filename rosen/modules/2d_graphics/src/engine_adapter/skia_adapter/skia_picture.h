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

#ifndef SKIAPICTURE_H
#define SKIAPICTURE_H

#include "include/core/SkPicture.h"

#include "impl_interface/picture_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPicture : public PictureImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaPicture() noexcept;
    virtual ~SkiaPicture() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    const sk_sp<SkPicture> GetPicture() const;

private:
    sk_sp<SkPicture> skiaPicture_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif