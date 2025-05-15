/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H

#if (defined(ROSEN_OHOS) && defined(RS_ENABLE_GL))
#include <GLES/gl.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#endif

#include "common/rs_common_def.h"

#include "recording/draw_cmd.h"
#include "recording/recording_canvas.h"
#include "render/rs_image.h"

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "surface_buffer.h"
#include "surface_type.h"
#include "sync_fence.h"
#include "external_window.h"
#endif
#ifdef RS_ENABLE_VK
#include "backend/native_buffer_utils.h"
#endif

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
struct DrawingSurfaceBufferInfo {
    DrawingSurfaceBufferInfo() = default;
    DrawingSurfaceBufferInfo(const sptr<SurfaceBuffer>& surfaceBuffer, int offSetX, int offSetY, int width, int height,
        pid_t pid = {}, uint64_t uid = {}, sptr<SyncFence> acquireFence = nullptr,
        GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
        Drawing::Rect srcRect = {})
        : surfaceBuffer_(surfaceBuffer), srcRect_(srcRect),
          dstRect_(Drawing::Rect { offSetX, offSetY, offSetX + width, offSetY + height }), pid_(pid), uid_(uid),
          acquireFence_(acquireFence), transform_(transform)
    {}
    sptr<SurfaceBuffer> surfaceBuffer_ = nullptr;
    Drawing::Rect srcRect_;
    Drawing::Rect dstRect_;
    pid_t pid_ = {};
    uint64_t uid_ = {};
    sptr<SyncFence> acquireFence_ = nullptr;
    GraphicTransformType transform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
};
#endif

class RSB_EXPORT RSExtendImageObject : public Drawing::ExtendImageObject {
public:
    RSExtendImageObject() = default;
    RSExtendImageObject(const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Data>& data,
        const Drawing::AdaptiveImageInfo& imageInfo);
    RSExtendImageObject(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::AdaptiveImageInfo& imageInfo);
    ~RSExtendImageObject() override;
    void Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::SamplingOptions& sampling, bool isBackground = false) override;
    bool Marshalling(Parcel &parcel) const;
    static RSExtendImageObject *Unmarshalling(Parcel &parcel);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
    bool GetDrawingImageFromSurfaceBuffer(Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer);
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    bool MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer *surfaceBuffer,
        const std::shared_ptr<Drawing::ColorSpace>& colorSpace = nullptr);
    bool GetRsImageCache(Drawing::Canvas& canvas, const std::shared_ptr<Media::PixelMap>& pixelMap,
        SurfaceBuffer *surfaceBuffer, const std::shared_ptr<Drawing::ColorSpace>& colorSpace = nullptr);
#endif
    void SetNodeId(NodeId id) override;
    NodeId GetNodeId() const override;
    void SetPaint(Drawing::Paint paint) override;
    void Purge() override;
    bool IsValid() override;
protected:
    std::shared_ptr<RSImage> rsImage_ = nullptr;
private:
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void PreProcessPixelMap(Drawing::Canvas& canvas, const std::shared_ptr<Media::PixelMap>& pixelMap,
        const Drawing::SamplingOptions& sampling);
#ifdef RS_ENABLE_GL
    mutable EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    mutable GLuint texId_ = 0;
#endif
    mutable OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
    mutable pid_t tid_ = 0;
#endif
#ifdef RS_ENABLE_VK
    mutable Drawing::BackendTexture backendTexture_ = {};
    mutable NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper_ = nullptr;
#endif
    std::shared_ptr<Drawing::Image> image_;
    Drawing::AdaptiveImageInfo imageInfo_;
    NodeId nodeId_ = INVALID_NODEID;
};

class RSB_EXPORT RSExtendImageBaseObj : public Drawing::ExtendImageBaseObj {
public:
    RSExtendImageBaseObj() = default;
    RSExtendImageBaseObj(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
        const Drawing::Rect& dst);
    ~RSExtendImageBaseObj() override = default;
    void Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect, const Drawing::SamplingOptions& sampling,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) override;
    bool Marshalling(Parcel &parcel) const;
    static RSExtendImageBaseObj *Unmarshalling(Parcel &parcel);
    void SetNodeId(NodeId id) override;
    void Purge() override;
protected:
    std::shared_ptr<RSImageBase> rsImage_ = nullptr;
};

class RSB_EXPORT RSExtendImageNineObject : public Drawing::ExtendImageNineObject {
public:
    RSExtendImageNineObject() = default;
    RSExtendImageNineObject(const std::shared_ptr<Media::PixelMap>& pixelMap);
    ~RSExtendImageNineObject() override = default;
    void Playback(Drawing::Canvas& canvas, const Drawing::RectI& center, const Drawing::Rect& dst,
        Drawing::FilterMode filterMode) override;
    bool Marshalling(Parcel &parcel) const;
    static RSExtendImageNineObject *Unmarshalling(Parcel &parcel);
    void SetNodeId(NodeId id) override;
    void Purge() override;
protected:
    std::shared_ptr<RSImageBase> rsImage_ = nullptr;
};

class RSB_EXPORT RSExtendImageLatticeObject : public Drawing::ExtendImageLatticeObject {
public:
    RSExtendImageLatticeObject() = default;
    RSExtendImageLatticeObject(const std::shared_ptr<Media::PixelMap>& pixelMap);
    ~RSExtendImageLatticeObject() override = default;
    void Playback(Drawing::Canvas& canvas, const Drawing::Lattice& lattice, const Drawing::Rect& dst,
        Drawing::FilterMode filterMode) override;
    bool Marshalling(Parcel &parcel) const;
    static RSExtendImageLatticeObject *Unmarshalling(Parcel &parcel);
    void SetNodeId(NodeId id) override;
    void Purge() override;
protected:
    std::shared_ptr<RSImageBase> rsImage_ = nullptr;
};

class RSB_EXPORT RSExtendDrawFuncObj : public Drawing::ExtendDrawFuncObj {
public:
    explicit RSExtendDrawFuncObj(Drawing::RecordingCanvas::DrawFunc&& drawFunc) : drawFunc_(std::move(drawFunc)) {}
    ~RSExtendDrawFuncObj() override = default;
    void Playback(Drawing::Canvas* canvas, const Drawing::Rect* rect) override;
    bool Marshalling(Parcel& parcel) const;
    static RSExtendDrawFuncObj* Unmarshalling(Parcel& parcel);

private:
    Drawing::RecordingCanvas::DrawFunc drawFunc_;
};

namespace Drawing {
class RSB_EXPORT DrawImageWithParmOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::IMAGE_WITH_PARM_OPITEM), objectHandle(objectHandle), sampling(sampling),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        PaintHandle paintHandle;
    };
    DrawImageWithParmOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawImageWithParmOpItem(const std::shared_ptr<Image>& image, const std::shared_ptr<Data>& data,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling, const Paint& paint);
    ~DrawImageWithParmOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    void SetNodeId(NodeId id) override;
    void Dump(std::string& out) const override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

class RSB_EXPORT DrawPixelMapWithParmOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIXELMAP_WITH_PARM_OPITEM), objectHandle(objectHandle), sampling(sampling),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        PaintHandle paintHandle;
    };
    DrawPixelMapWithParmOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawPixelMapWithParmOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling, const Paint& paint);
    ~DrawPixelMapWithParmOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    void SetNodeId(NodeId id) override;
    virtual void DumpItems(std::string& out) const override;
    void Purge() override
    {
        if (objectHandle_) {
            objectHandle_->Purge();
        }
    }
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

#ifdef RS_ENABLE_VK
class RSB_EXPORT DrawHybridPixelMapOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            const PaintHandle& paintHandle, uint32_t entryId, bool isRenderForeground)
            : OpItem(DrawOpItem::HYBRID_RENDER_PIXELMAP_OPITEM), objectHandle(objectHandle), sampling(sampling),
              paintHandle(paintHandle), entryId(entryId), isRenderForeground(isRenderForeground) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        PaintHandle paintHandle;
        uint32_t entryId;
        bool isRenderForeground;
    };
    DrawHybridPixelMapOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawHybridPixelMapOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling, const Paint& paint);
    ~DrawHybridPixelMapOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    void SetNodeId(NodeId id) override;
    virtual void DumpItems(std::string& out) const override;
    void Purge() override
    {
        if (objectHandle_) {
            objectHandle_->Purge();
        }
    }
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
    bool isRenderForeground_ = false;
    sptr<SyncFence> fence_ = nullptr;
};
#endif

class RSB_EXPORT DrawPixelMapRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            SrcRectConstraint constraint, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIXELMAP_RECT_OPITEM), objectHandle(objectHandle), sampling(sampling),
              constraint(constraint), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        SrcRectConstraint constraint;
        PaintHandle paintHandle;
    };
    DrawPixelMapRectOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawPixelMapRectOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap, const Rect& src, const Rect& dst,
        const SamplingOptions& sampling, SrcRectConstraint constraint, const Paint& paint);
    ~DrawPixelMapRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    void SetNodeId(NodeId id) override;
    virtual void DumpItems(std::string& out) const override;
    void Purge() override
    {
        if (objectHandle_) {
            objectHandle_->Purge();
        }
    }
private:
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
    std::shared_ptr<ExtendImageBaseObj> objectHandle_;
};

class RSB_EXPORT DrawFuncOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(uint32_t funcObjectId)
            : OpItem(DrawOpItem::DRAW_FUNC_OPITEM), funcObjectId(funcObjectId) {}
        ~ConstructorHandle() override = default;
        uint32_t funcObjectId;
    };
    DrawFuncOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawFuncOpItem(RecordingCanvas::DrawFunc&& drawFunc);
    ~DrawFuncOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<ExtendDrawFuncObj> objectHandle_;
};

class DrawPixelMapNineOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const Drawing::RectI& center,
            const Drawing::Rect& dst, Drawing::FilterMode filterMode, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIXELMAP_NINE_OPITEM), objectHandle(objectHandle), center(center),
              dst(dst), filterMode(filterMode), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        Drawing::RectI center;
        Drawing::Rect dst;
        FilterMode filterMode;
        PaintHandle paintHandle;
    };
    DrawPixelMapNineOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawPixelMapNineOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::RectI& center,
        const Drawing::Rect& dst, Drawing::FilterMode filterMode, const Paint& paint);
    ~DrawPixelMapNineOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    void SetNodeId(NodeId id) override;
    virtual void DumpItems(std::string& out) const override;
    void Purge() override
    {
        if (objectHandle_) {
            objectHandle_->Purge();
        }
    }
private:
    Drawing::RectI center_;
    Drawing::Rect dst_;
    FilterMode filterMode_;
    std::shared_ptr<ExtendImageNineObject> objectHandle_;
};

class DrawPixelMapLatticeOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const LatticeHandle& latticeHandle,
            const Drawing::Rect& dst, Drawing::FilterMode filterMode, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIXELMAP_LATTICE_OPITEM), objectHandle(objectHandle), latticeHandle(latticeHandle),
              dst(dst), filterMode(filterMode), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        LatticeHandle latticeHandle;
        Drawing::Rect dst;
        FilterMode filterMode;
        PaintHandle paintHandle;
    };
    DrawPixelMapLatticeOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawPixelMapLatticeOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Lattice& lattice,
        const Drawing::Rect& dst, Drawing::FilterMode filterMode, const Paint& paint);
    ~DrawPixelMapLatticeOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    void SetNodeId(NodeId id) override;
    virtual void DumpItems(std::string& out) const override;
    void Purge() override
    {
        if (objectHandle_) {
            objectHandle_->Purge();
        }
    }
private:
    Drawing::Lattice lattice_;
    Drawing::Rect dst_;
    FilterMode filterMode_;
    std::shared_ptr<ExtendImageLatticeObject> objectHandle_;
};

#ifdef ROSEN_OHOS
struct RSB_EXPORT DrawSurfaceBufferFinishCbData {
    uint64_t uid;
    pid_t pid;
    uint32_t surfaceBufferId;
    NodeId rootNodeId = INVALID_NODEID;
    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    bool isRendered = false;
    bool isNeedTriggerCbDirectly = false;
};
 
struct RSB_EXPORT DrawSurfaceBufferAfterAcquireCbData {
    uint64_t uid;
    pid_t pid;
};

struct RSB_EXPORT DrawSurfaceBufferOpItemCb {
    std::function<void(const DrawSurfaceBufferFinishCbData&)> OnFinish;
    std::function<void(const DrawSurfaceBufferAfterAcquireCbData&)> OnAfterAcquireBuffer;
};

class RSB_EXPORT DrawSurfaceBufferOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(uint32_t surfaceBufferId, int offSetX, int offSetY, int width, int height, pid_t pid,
            uint64_t uid, GraphicTransformType transform, Drawing::Rect srcRect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::SURFACEBUFFER_OPITEM), surfaceBufferId(surfaceBufferId),
              surfaceBufferInfo(nullptr, offSetX, offSetY, width, height, pid, uid, nullptr, transform, srcRect),
              paintHandle(paintHandle)
        {}
        ~ConstructorHandle() override = default;
        uint32_t surfaceBufferId;
        DrawingSurfaceBufferInfo surfaceBufferInfo;
        PaintHandle paintHandle;
    };

    DrawSurfaceBufferOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawSurfaceBufferOpItem(const DrawingSurfaceBufferInfo& surfaceBufferInfo, const Paint& paint)
        : DrawWithPaintOpItem(paint, SURFACEBUFFER_OPITEM), surfaceBufferInfo_(surfaceBufferInfo) {}
    ~DrawSurfaceBufferOpItem();

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    virtual void DumpItems(std::string& out) const override;
    RSB_EXPORT static void RegisterSurfaceBufferCallback(DrawSurfaceBufferOpItemCb callbacks);
    RSB_EXPORT static void RegisterGetRootNodeIdFuncForRT(std::function<NodeId()> func);
    RSB_EXPORT static void SetIsUniRender(bool isUniRender);
private:
    void OnAfterAcquireBuffer();
    void OnBeforeDraw();
    void OnAfterDraw();
    void OnDestruct();
    void ReleaseBuffer();
    void Clear();
    void DealWithRotate(Canvas* canvas);
    void Draw(Canvas* canvas);
    void DrawWithVulkan(Canvas* canvas);
    void DrawWithGles(Canvas* canvas);
    bool CreateEglTextureId();
    bool IsNeedDrawDirectly() const;
    static Drawing::SamplingOptions CreateSamplingOptions(const Drawing::Matrix& matrix);
    static Drawing::BitmapFormat CreateBitmapFormat(int32_t bufferFormat);
#ifdef RS_ENABLE_GL
    static GLenum GetGLTextureFormatByBitmapFormat(Drawing::ColorType colorType);
#endif
    static GraphicTransformType MapGraphicTransformType(GraphicTransformType origin);
    static bool IsValidRemoteAddress(pid_t pid, uint64_t uid);
    mutable DrawingSurfaceBufferInfo surfaceBufferInfo_;
    NodeId rootNodeId_ = INVALID_NODEID;
    sptr<SyncFence> releaseFence_ = SyncFence::INVALID_FENCE;
    bool isRendered_ = false;
    bool isNeedRotateDstRect_ = false;
    bool isNeedDrawDirectly_ = false;

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
#endif
#ifdef RS_ENABLE_GL
    mutable EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    mutable GLuint texId_ = 0;
#endif
};
#endif
}
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H
