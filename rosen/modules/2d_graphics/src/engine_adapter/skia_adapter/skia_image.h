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

#ifndef SKIAIMAGE_H
#define SKIAIMAGE_H

#include "include/core/SkImage.h"

#include "impl_interface/image_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImage : public ImageImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaImage() noexcept;
    virtual ~SkiaImage() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    void* BuildFromBitmap(const Bitmap& bitmap) override;
    int GetWidth() override;
    int GetHeight() override;
    const sk_sp<SkImage> GetImage() const;

private:
    sk_sp<SkImage> skiaImage_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
