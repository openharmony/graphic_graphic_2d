/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PIXEL_MAP_FILTER_H
#define RENDER_SERVICE_BASE_PIXEL_MAP_FILTER_H
#include "effect/image_filter.h"
#include "pixel_map.h"
#include "utils/rect.h"
#include "utils/extend_object.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPixelMapFilter : public Drawing::ExtendObject {
public:
    RSPixelMapFilter() : Drawing::ExtendObject(ExtendObjectType::IMAGE_FILTER) {}
    RSPixelMapFilter(std::shared_ptr<Media::PixelMap> pixelMap, Drawing::Rect srcRect, Drawing::Rect dstRect);
    ~RSPixelMapFilter() override = default;
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel) override;
#endif
    std::shared_ptr<void> GenerateBaseObject() override;
private:
    std::shared_ptr<Media::PixelMap> pixelMap_;
    Drawing::Rect srcRect_;
    Drawing::Rect dstRect_;
    std::shared_ptr<void> baseObject_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIXEL_MAP_FILTER_H