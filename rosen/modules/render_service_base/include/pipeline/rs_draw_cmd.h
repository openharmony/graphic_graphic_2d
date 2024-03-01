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

#ifdef ROSEN_OHOS
#include <GLES/gl.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#endif

#include "recording/adaptive_image_helper.h"
#include "recording/draw_cmd.h"
#include "recording/recording_canvas.h"
#include "render/rs_image.h"

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "surface_buffer.h"
#include "external_window.h"
#endif
#ifdef RS_ENABLE_VK
#include "../../src/platform/ohos/backend/native_buffer_utils.h"
#endif

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
struct DrawingSurfaceBufferInfo {
    DrawingSurfaceBufferInfo() = default;
    DrawingSurfaceBufferInfo(
        const sptr<SurfaceBuffer>& surfaceBuffer, int offSetX, int offSetY, int width, int height)
        : surfaceBuffer_(surfaceBuffer), offSetX_(offSetX), offSetY_(offSetY), width_(width), height_(height)
    {}
    sptr<SurfaceBuffer> surfaceBuffer_ = nullptr;
    int offSetX_ = 0;
    int offSetY_ = 0;
    int width_ = 0;
    int height_ = 0;
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
    bool MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer *surfaceBuffer);
#endif
    void SetNodeId(NodeId id) override;
protected:
    std::shared_ptr<RSImage> rsImage_;
private:
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
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
};

class RSB_EXPORT RSExtendImageBaseObj : public Drawing::ExtendImageBaseObj {
public:
    RSExtendImageBaseObj() = default;
    RSExtendImageBaseObj(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
        const Drawing::Rect& dst);
    ~RSExtendImageBaseObj() override = default;
    void Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::SamplingOptions& sampling) override;
    bool Marshalling(Parcel &parcel) const;
    static RSExtendImageBaseObj *Unmarshalling(Parcel &parcel);
    void SetNodeId(NodeId id) override;
protected:
    std::shared_ptr<RSImageBase> rsImage_;
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
class DrawImageWithParmOpItem : public DrawWithPaintOpItem {
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
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

class DrawPixelMapWithParmOpItem : public DrawWithPaintOpItem {
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
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

class DrawPixelMapRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIXELMAP_RECT_OPITEM), objectHandle(objectHandle), sampling(sampling),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        PaintHandle paintHandle;
    };
    DrawPixelMapRectOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawPixelMapRectOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap, const Rect& src, const Rect& dst,
        const SamplingOptions& sampling, const Paint& paint);
    ~DrawPixelMapRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
    void SetNodeId(NodeId id) override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageBaseObj> objectHandle_;
};

class DrawFuncOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle)
            : OpItem(DrawOpItem::DRAW_FUNC_OPITEM), objectHandle(objectHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
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

#ifdef ROSEN_OHOS
class DrawSurfaceBufferOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(uint32_t surfaceBufferId, int offSetX, int offSetY, int width, int height,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::SURFACEBUFFER_OPITEM), surfaceBufferId(surfaceBufferId),
            surfaceBufferInfo(nullptr, offSetX, offSetY, width, height), paintHandle(paintHandle) {}
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
private:
    mutable DrawingSurfaceBufferInfo surfaceBufferInfo_;
    void Clear();
    void Draw(Canvas* canvas);
    void DrawWithVulkan(Canvas* canvas);
    void DrawWithGles(Canvas* canvas);
    bool CreateEglTextureId();

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
#endif
#ifdef RS_ENABLE_VK
    mutable std::shared_ptr<Image> image_ = nullptr;
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
