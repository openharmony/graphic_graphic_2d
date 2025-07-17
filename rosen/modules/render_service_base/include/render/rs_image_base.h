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

#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_IMAGE_BASE_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_IMAGE_BASE_H

#include <cstdint>
#include <mutex>
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include <memory>
#include "draw/canvas.h"
#include "memory/rs_dfx_string.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "transaction/rs_marshalling_helper.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "external_window.h"
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
namespace NativeBufferUtils {
class VulkanCleanupHelper;
}
#endif
class RSB_EXPORT RSImageBase {
public:
#ifdef ROSEN_OHOS
    /* This class is used to avoid Unmap is being called after ReMap and before pixelMap is used. */
    class PixelMapUseCountGuard {
    public:
        PixelMapUseCountGuard(std::shared_ptr<Media::PixelMap> pixelMap, bool purgeable);
        ~PixelMapUseCountGuard();
    private:
        std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
        bool purgeable_ = false;
    };
#endif
    RSImageBase() = default;
    virtual ~RSImageBase();

    virtual void DrawImage(Drawing::Canvas& canvas, const Drawing::SamplingOptions& samplingOptions,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    virtual void DrawImageNine(Drawing::Canvas& canvas, const Drawing::RectI& center, const Drawing::Rect& dst,
        Drawing::FilterMode filterMode = Drawing::FilterMode::NEAREST);
    virtual void DrawImageLattice(Drawing::Canvas& canvas, const Drawing::Lattice& lattice, const Drawing::Rect& dst,
        Drawing::FilterMode filterMode = Drawing::FilterMode::NEAREST);
    void SetImage(const std::shared_ptr<Drawing::Image> image);
    void SetCompressData(const std::shared_ptr<Drawing::Data> CompressData);
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
    void MarkPurgeable();
    bool IsPurgeable() const;
    std::shared_ptr<Media::PixelMap> GetPixelMap() const;
    void DumpPicture(DfxString& info) const;
    uint64_t GetUniqueId() const;
#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSImageBase* Unmarshalling(Parcel& parcel);
#endif

    void ConvertPixelMapToDrawingImage(bool parallelUpload = false);

    /*
     * This function is used to reduce memory usage by unmap the memory of the pixelMap_.
     * Only the pixelMap_ with one RefCount and one UseCount can be purged.
    */
    void Purge();
    void DePurge();

protected:
    void GenUniqueId(uint32_t id);
    void UploadGpu(Drawing::Canvas& canvas);
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void ProcessYUVImage(std::shared_ptr<Drawing::GPUContext> gpuContext);
    bool SetCompressedDataForASTC();
#if defined(RS_ENABLE_VK)
    void BindPixelMapToDrawingImage(Drawing::Canvas& canvas);
    std::shared_ptr<Drawing::Image> MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer);
#endif
#endif
    static bool UnmarshallingDrawingImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useDrawingImage,
        std::shared_ptr<Drawing::Image>& img, std::shared_ptr<Media::PixelMap>& pixelMap, void*& imagepixelAddr);
    static void IncreaseCacheRefCount(uint64_t uniqueId,
            bool useSkImage = true, std::shared_ptr<Media::PixelMap> pixelMap = nullptr);

    mutable std::mutex mutex_;
    std::shared_ptr<Drawing::Image> image_;
    void* imagePixelAddr_ = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap_;
    // used for astc render
    std::shared_ptr<Drawing::Data> compressData_;

    RectF srcRect_;
    RectF dstRect_;
    Drawing::Rect src_;
    Drawing::Rect dst_;
    Drawing::Rect lastRect_;
    bool isDrawn_ = false;
    uint64_t uniqueId_ = 0;
    bool renderServiceImage_ = false;
    bool isYUVImage_ = false;

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    mutable OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
    mutable pid_t tid_ = 0;
    mutable Drawing::BackendTexture backendTexture_ = {};
    mutable NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper_ = nullptr;
#endif

    enum class CanPurgeFlag : int8_t {
        UNINITED = -1,
        DISABLED = 0,
        ENABLED = 1,
    };
    CanPurgeFlag canPurgeShareMemFlag_ = CanPurgeFlag::UNINITED;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_IMAGE_BASE_H
