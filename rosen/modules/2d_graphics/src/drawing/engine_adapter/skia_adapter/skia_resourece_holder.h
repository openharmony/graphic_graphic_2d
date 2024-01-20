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

#ifndef SKIA_RESOURCE_HOLDER_H
#define SKIA_RESOURCE_HOLDER_H
#include <unordered_map>

#include "impl_interface/resource_holder_impl.h"
#include "skia_image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaResourceHolder : public ResourceHolderImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaResourceHolder() noexcept = default;
    ~SkiaResourceHolder() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void HoldResource(const std::shared_ptr<Image>& img) override
    {
        const sk_sp<SkImage> skImage = img->GetImpl<SkiaImage>()->GetImage();
        if (!skImage) {
            return;
        }
        uint32_t id = skImage->uniqueID();
        if (skImages_.find(id) != skImages_.end()) {
            return;
        }
        skImages_.emplace(id, skImage);
    }

    void ReleaseResource() override
    {
        auto iter = skImages_.begin();
        while (iter != skImages_.end()) {
            if (iter->second->unique()) {
                auto tmp = iter++;
                skImages_.erase(tmp);
            } else {
                ++iter;
            }
        }
    }

    bool IsEmpty() override
    {
        return skImages_.empty();
    }

private:
    std::unordered_map<uint32_t, const sk_sp<SkImage>> skImages_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_RESOURCE_HOLDER_H