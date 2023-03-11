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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_BASE_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_BASE_H

#include <cstdint>
#include <mutex>
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "include/core/SkCanvas.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RSB_EXPORT RSImageBase {
public:
    RSImageBase() = default;
    virtual ~RSImageBase();

    virtual void DrawImage(SkCanvas& canvas, const SkPaint& paint);
    void SetImage(const sk_sp<SkImage> image);
    void SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap);
    void SetSrcRect(const RectF& dstRect);
    void SetDstRect(const RectF& dstRect);
#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSImageBase* Unmarshalling(Parcel& parcel);
#endif

protected:
    void ConvertPixelMapToSkImage();
    void GenUniqueId(uint32_t id);
    static bool UnmarshallingSkImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useSkImage,
        sk_sp<SkImage>& img, std::shared_ptr<Media::PixelMap>& pixelMap);
    static void IncreaseCacheRefCount(uint64_t uniqueId,
            bool useSkImage = true, std::shared_ptr<Media::PixelMap> pixelMap = nullptr);

    mutable std::mutex mutex_;
    sk_sp<SkImage> image_;
    std::shared_ptr<Media::PixelMap> pixelMap_;

    RectF srcRect_;
    RectF dstRect_;
    uint64_t uniqueId_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_BASE_H
