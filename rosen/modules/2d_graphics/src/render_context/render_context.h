/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <memory>
#include <mutex>
#include "common/rs_rect.h"

#ifdef ROSEN_IOS
#include "render_context_egl_defines.h"
#else
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"
#endif

#include "draw/surface.h"
#include "memory_handler.h"
#include "shader_cache.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {
class RenderContext {
public:
    static std::shared_ptr<RenderContext> Create();
    virtual ~RenderContext() = default;

    virtual bool Init() = 0;
    virtual bool AbandonContext() = 0;
    virtual std::string GetShaderCacheSize() const = 0;
    virtual std::string CleanAllShaderCache() const = 0;
    virtual bool SetUpGpuContext(std::shared_ptr<Drawing::GPUContext> drawingContext = nullptr) = 0;

    static std::shared_ptr<Drawing::ColorSpace> ConvertColorGamutToColorSpace(GraphicColorGamut colorGamut);

    // EGL special function
    virtual std::shared_ptr<Drawing::Surface> AcquireSurface(int width, int height) { return nullptr; }
    virtual void RenderFrame() { return; }
    virtual void DamageFrame(const std::vector<RectI> &rects) { return; }
    virtual void ClearRedundantResources() { return; }
    virtual void CreateShareContext() { return; }
    virtual void DestroyShareContext() { return; }
    virtual int32_t QueryEglBufferAge() { return 0; }

    void SetUniRenderMode(bool isUni)
    {
        isUniRender_ = isUni;
    }

    void SetCacheDir(const std::string& filePath)
    {
        cacheDir_ = filePath;
    }

    void SetDrGPUContext(std::shared_ptr<Drawing::GPUContext> drawingContext)
    {
        drGPUContext_ = drawingContext;
    }

    Drawing::GPUContext* GetDrGPUContext() const
    {
        return drGPUContext_.get();
    }

    std::shared_ptr<Drawing::GPUContext> GetSharedDrGPUContext() const
    {
        return drGPUContext_;
    }

    std::shared_ptr<Drawing::Surface> GetSurface() const
    {
        return surface_;
    }

    void SetColorSpace(GraphicColorGamut colorSpace)
    {
        colorSpace_ = colorSpace;
    }

    GraphicColorGamut GetColorSpace() const
    {
        return colorSpace_;
    }

#ifndef ROSEN_CROSS_PLATFORM
    void SetPixelFormat(int32_t pixelFormat)
    {
        pixelFormat_ = pixelFormat;
    }

    int32_t GetPixelFormat() const
    {
        return pixelFormat_;
    }
#endif

protected:
    RenderContext() = default;

    std::shared_ptr<Drawing::GPUContext> drGPUContext_ = nullptr;
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    std::mutex shareContextMutex_;
    bool isUniRender_ = false;
    bool isUniRenderMode_ = false;
    std::string cacheDir_ = "";
    std::shared_ptr<MemoryHandler> mHandler_ = nullptr;
    int32_t pixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
};
} // namespace Rosen
} // namespace OHOS
#endif