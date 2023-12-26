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

#ifndef DRAWING_SURFACE_IMPL_H
#define DRAWING_SURFACE_IMPL_H

#include "base_impl.h"
#include "utils/rect.h"
#ifdef RS_ENABLE_VK
#include "vulkan/vulkan.h"
#endif
namespace OHOS {
namespace Rosen {
namespace Drawing {
class BackendTexture;
class Bitmap;
class Canvas;
class Image;
class Surface;
struct FlushInfo;
#ifdef ACE_ENABLE_GPU
struct FrameBuffer;
class ImageInfo;
class GPUContext;
#endif
enum class BackendAccess;

class SurfaceImpl : public BaseImpl {
public:
    SurfaceImpl() {};
    ~SurfaceImpl() override {};

    virtual bool Bind(const Bitmap& bitmap) = 0;
#ifdef ACE_ENABLE_GPU
    virtual bool Bind(const Image& image) = 0;
    virtual bool Bind(const FrameBuffer& frameBuffer) = 0;
#endif
    virtual std::shared_ptr<Canvas> GetCanvas() const = 0;
    virtual std::shared_ptr<Image> GetImageSnapshot() const = 0;
    virtual std::shared_ptr<Image> GetImageSnapshot(const RectI& bounds) const = 0;
    virtual std::shared_ptr<Surface> MakeSurface(int width, int height) const = 0;
    virtual BackendTexture GetBackendTexture(BackendAccess access) const = 0;
    virtual void FlushAndSubmit(bool syncCpu) = 0;
    virtual void Flush(FlushInfo *drawingflushInfo = nullptr) = 0;
#ifdef RS_ENABLE_VK
    virtual void Wait(int32_t time, const VkSemaphore& semaphore) = 0;
    virtual void SetDrawingArea(const std::vector<RectI>& rects) = 0;
    virtual void ClearDrawingArea() = 0;
#endif
    virtual int Width() const = 0;
    virtual int Height() const = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // DRAWING_SURFACE_IMPL_H
