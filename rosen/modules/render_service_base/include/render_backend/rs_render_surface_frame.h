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

#ifndef RS_RENDER_SURFACE_FRAME_H
#define RS_RENDER_SURFACE_FRAME_H

#include <EGL/egl.h>
#include <surface.h>

#include "common/rs_rect.h"
#include "include/core/SkSurface.h"

#ifdef RS_ENABLE_VK
#include <vulkan_window.h>
#endif

namespace OHOS {
namespace Rosen {
enum PlatformName {
    OHOS,
    ANDROID,
    IOS,
    WINDOWS,
    DARWIN,
};

enum RenderType {
    RASTER,
    GLES,
    VULKAN,
};

typedef struct EGLState {
    EGLState()
    {
        eglContext = EGL_NO_CONTEXT;
        eglDisplay = EGL_NO_DISPLAY;
        eglSurface = EGL_NO_SURFACE;
    }
    EGLContext eglContext;
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;
} EGLState;

#ifdef RS_ENABLE_VK
typedef struct VulkanState {
    std::shared_ptr<vulkan::VulkanWindow> vulkanWindow;
} VulkanState;
#endif

typedef struct FrameConfig {
    FrameConfig()
    {
        width = 0;
        height = 0;
        uiTimestamp = 0;
        bpp = 0;
        releaseFence = -1;
        pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
        bufferUsage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
        useAFBC = false;
        colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
        requestConfig = {
            .width = 0x100,
            .height = 0x100,
            .strideAlignment = 0x8,
            .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
            .timeout = 0,
        };
        flushConfig = {
            .damage = {
                .x = 0,
                .y = 0,
                .w = 0x100,
                .h = 0x100,
            },
        };
        skSurface = nullptr;
        buffer = nullptr;
    }
    int32_t width;
    int32_t height;
    uint64_t uiTimestamp;
    int32_t bpp;
    int32_t releaseFence;
    int32_t pixelFormat;
    uint64_t bufferUsage;
    bool useAFBC;
    GraphicColorGamut colorSpace;
    std::vector<RectI> damageRects;
    std::unique_ptr<uint32_t[]> addr;
    BufferRequestConfig requestConfig;
    BufferFlushConfig flushConfig;
    sptr<SurfaceBuffer> buffer;
    sk_sp<SkSurface> skSurface;
} FrameConfig;

typedef struct SurfaceConfig {
    sptr<Surface> producer;
    struct NativeWindow* nativeWindow;
} SurfaceConfig;

typedef struct RSRenderSurfaceFrame {
    std::shared_ptr<SurfaceConfig> surfaceConfig;
    std::shared_ptr<FrameConfig> frameConfig;
    std::shared_ptr<EGLState> eglState;
#if defined(RS_ENABLE_VK)
    std::shared_ptr<VulkanState> vulkanState;
#endif
} RSRenderSurfaceFrame;
}
}
#endif