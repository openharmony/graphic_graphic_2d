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
#include <memory>
#include "draw/canvas.h"
#include "memory/rs_dfx_string.h"
#include "pipeline/rs_paint_filter_canvas.h"
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

    virtual void DrawImage(Drawing::Canvas& canvas, const Drawing::SamplingOptions& samplingOptions);
    void SetImage(const std::shared_ptr<Drawing::Image> image);
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void SetDmaImage(const std::shared_ptr<Drawing::Image> image);
    void MarkYUVImage();
#endif
    void SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap);
    void SetSrcRect(const RectF& dstRect);
    void SetDstRect(const RectF& dstRect);
    void SetImagePixelAddr(void* addr);
    void UpdateNodeIdToPicture(NodeId nodeId);
    void MarkRenderServiceImage();
    std::shared_ptr<Media::PixelMap> GetPixelMap() const;
    void DumpPicture(DfxString& info) const;
#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSImageBase* Unmarshalling(Parcel& parcel);
#endif

    void ConvertPixelMapToDrawingImage(bool parallelUpload = false);

protected:
    void GenUniqueId(uint32_t id);
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void ProcessYUVImage(std::shared_ptr<Drawing::GPUContext> gpuContext);
#endif
    static bool UnmarshallingDrawingImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useDrawingImage,
        std::shared_ptr<Drawing::Image>& img, std::shared_ptr<Media::PixelMap>& pixelMap, void*& imagepixelAddr);
    static void IncreaseCacheRefCount(uint64_t uniqueId,
            bool useSkImage = true, std::shared_ptr<Media::PixelMap> pixelMap = nullptr);

    mutable std::mutex mutex_;
    std::shared_ptr<Drawing::Image> image_;
    void* imagePixelAddr_ = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap_;

    RectF srcRect_;
    RectF dstRect_;
    Drawing::Rect src_;
    Drawing::Rect dst_;
    Drawing::Rect lastRect_;
    bool isDrawn_ = false;
    uint64_t uniqueId_ = 0;
    bool renderServiceImage_ = false;
    bool isYUVImage_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_BASE_H
