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
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#else
#include <memory>
#include "draw/canvas.h"
#endif
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

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    virtual void DrawImage(SkCanvas& canvas, const SkSamplingOptions& samplingOptions, const SkPaint& paint);
#else
    virtual void DrawImage(SkCanvas& canvas, const SkPaint& paint);
#endif
    void SetImage(const sk_sp<SkImage> image);
#else
    virtual void DrawImage(Drawing::Canvas& canvas, const Drawing::Brush& brush);
    void SetImage(const std::shared_ptr<Drawing::Image> image);
#endif
    void SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap);
    void SetSrcRect(const RectF& dstRect);
    void SetDstRect(const RectF& dstRect);
    void SetImagePixelAddr(void* addr);
    void UpdateNodeIdToPicture(NodeId nodeId);
#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSImageBase* Unmarshalling(Parcel& parcel);
#endif

protected:
#ifndef USE_ROSEN_DRAWING
    void ConvertPixelMapToSkImage();
#else
    void ConvertPixelMapToDrawingImage();
#endif
    void GenUniqueId(uint32_t id);
#ifndef USE_ROSEN_DRAWING
    static bool UnmarshallingSkImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useSkImage,
        sk_sp<SkImage>& img, std::shared_ptr<Media::PixelMap>& pixelMap, void*& imagepixelAddr);
#else
    static bool UnmarshallingDrawingImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useDrawingImage,
        std::shared_ptr<Drawing::Image>& img, std::shared_ptr<Media::PixelMap>& pixelMap, void*& imagepixelAddr);
#endif
    static void IncreaseCacheRefCount(uint64_t uniqueId,
            bool useSkImage = true, std::shared_ptr<Media::PixelMap> pixelMap = nullptr);

    mutable std::mutex mutex_;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> image_;
#else
    std::shared_ptr<Drawing::Image> image_;
#endif
    void* imagePixelAddr_ = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap_;

    RectF srcRect_;
    RectF dstRect_;
    uint64_t uniqueId_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_BASE_H
