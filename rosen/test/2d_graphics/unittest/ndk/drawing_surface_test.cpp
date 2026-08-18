/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"
#include "window.h"
#include "core/ui/rs_surface_node.h"

#include "drawing_canvas.h"
#include "drawing_error_code.h"
#include "drawing_gpu_context.h"
#include "drawing_surface.h"
#include "drawing_surface_utils.h"
#ifdef RS_ENABLE_VK
#include "drawing_gpu_context_manager.h"
#include "drawing_surface_utils.cpp"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

constexpr static int32_t WIDTH = 720;
constexpr static int32_t HEIGHT = 1280;

class NativeDrawingSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
protected:
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLConfig eglConfig_ = EGL_NO_CONFIG_KHR;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    OH_Drawing_GpuContext* gpuContext_ = nullptr;
    OH_Drawing_Surface* surface_ = nullptr;
    OH_Drawing_Canvas* canvas_ = nullptr;
    NativeWindow *window_ = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
};

void NativeDrawingSurfaceTest::SetUpTestCase() {}
void NativeDrawingSurfaceTest::TearDownTestCase() {}
void NativeDrawingSurfaceTest::SetUp()
{
    eglDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EXPECT_NE(eglDisplay_, EGL_NO_DISPLAY);

    EGLint eglMajVers;
    EGLint eglMinVers;
    EGLBoolean ret = eglInitialize(eglDisplay_, &eglMajVers, &eglMinVers);
    EXPECT_EQ(ret, EGL_TRUE);

    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };
    ret = eglChooseConfig(eglDisplay_, configAttribs, &eglConfig_, 1, &count);
    EXPECT_EQ(ret, EGL_TRUE);
    EXPECT_GE(count, 1);

    const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    eglContext_ = eglCreateContext(eglDisplay_, eglConfig_, EGL_NO_CONTEXT, contextAttribs);
    EXPECT_NE(eglContext_, EGL_NO_CONTEXT);

    EGLint attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    eglSurface_ = eglCreatePbufferSurface(eglDisplay_, eglConfig_, attribs);
    EXPECT_NE(eglSurface_, EGL_NO_SURFACE);

    ret = eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_);
    EXPECT_EQ(ret, EGL_TRUE);

    RSSurfaceNodeConfig config;
    surfaceNode = RSSurfaceNode::Create(config);
    EXPECT_NE(surfaceNode, nullptr);
    sptr<OHOS::Surface> surf = surfaceNode->GetSurface();
    window_ = CreateNativeWindowFromSurface(&surf);
    EXPECT_NE(window_, nullptr);

    NativeWindowHandleOpt(window_, SET_USAGE, BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);
    NativeWindowHandleOpt(window_, SET_BUFFER_GEOMETRY, WIDTH, HEIGHT);
    NativeWindowHandleOpt(window_, SET_COLOR_GAMUT, GRAPHIC_COLOR_GAMUT_SRGB);
}

void NativeDrawingSurfaceTest::TearDown()
{
    EGLBoolean ret = eglDestroySurface(eglDisplay_, eglSurface_);
    EXPECT_EQ(ret, EGL_TRUE);

    ret = eglDestroyContext(eglDisplay_, eglContext_);
    EXPECT_EQ(ret, EGL_TRUE);

    ret = eglTerminate(eglDisplay_);
    EXPECT_EQ(ret, EGL_TRUE);

    eglSurface_ = EGL_NO_SURFACE;
    eglContext_ = EGL_NO_CONTEXT;
    eglDisplay_ = EGL_NO_DISPLAY;
}

/*
 * @tc.name: NativeDrawingSurfaceTest_CreateFromGpuContext
 * @tc.desc: test for CreateFromGpuContext.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_CreateFromGpuContext, TestSize.Level1)
{
    OH_Drawing_GpuContextOptions options {true};
    gpuContext_ = OH_Drawing_GpuContextCreateFromGL(options);
    EXPECT_NE(gpuContext_, nullptr);

    const int32_t width = 500;
    const int32_t height = 500;
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    surface_ = OH_Drawing_SurfaceCreateFromGpuContext(gpuContext_, true, imageInfo);
    EXPECT_NE(surface_, nullptr);
    OH_Drawing_SurfaceDestroy(surface_);

    surface_ = OH_Drawing_SurfaceCreateFromGpuContext(gpuContext_, false, imageInfo);
    EXPECT_NE(surface_, nullptr);
    OH_Drawing_SurfaceDestroy(surface_);

    surface_ = OH_Drawing_SurfaceCreateFromGpuContext(nullptr, false, imageInfo);
    EXPECT_EQ(surface_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_SurfaceDestroy(surface_);
    OH_Drawing_GpuContextDestroy(gpuContext_);
}

/*
 * @tc.name: NativeDrawingSurfaceTest_CreateOnScreen
 * @tc.desc: test for CreateOnScreen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_CreateOnScreen, TestSize.Level1)
{
    gpuContext_ = OH_Drawing_GpuContextCreate();
    EXPECT_NE(gpuContext_, nullptr);

    const int32_t width = 500;
    const int32_t height = 500;
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    surface_ = OH_Drawing_SurfaceCreateOnScreen(gpuContext_, imageInfo, window_);
    EXPECT_NE(surface_, nullptr);
    OH_Drawing_SurfaceDestroy(surface_);

    surface_ = OH_Drawing_SurfaceCreateOnScreen(nullptr, imageInfo, window_);
    EXPECT_EQ(surface_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    surface_ = OH_Drawing_SurfaceCreateOnScreen(gpuContext_, imageInfo, nullptr);
    EXPECT_EQ(surface_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    surface_ = OH_Drawing_SurfaceCreateOnScreen(nullptr, imageInfo, nullptr);
    EXPECT_EQ(surface_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_SurfaceDestroy(surface_);
    OH_Drawing_GpuContextDestroy(gpuContext_);
}

/*
 * @tc.name: NativeDrawingSurfaceTest_GetCanvas
 * @tc.desc: test for GetCanvas.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_GetCanvas, TestSize.Level1)
{
    OH_Drawing_GpuContextOptions options {true};
    gpuContext_ = OH_Drawing_GpuContextCreateFromGL(options);
    EXPECT_NE(gpuContext_, nullptr);

    const int32_t width = 500;
    const int32_t height = 500;
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    surface_ = OH_Drawing_SurfaceCreateFromGpuContext(gpuContext_, true, imageInfo);
    EXPECT_NE(surface_, nullptr);

    canvas_ = OH_Drawing_SurfaceGetCanvas(surface_);
    EXPECT_NE(canvas_, nullptr);

    canvas_ = OH_Drawing_SurfaceGetCanvas(nullptr);
    EXPECT_EQ(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_SurfaceDestroy(surface_);
    OH_Drawing_GpuContextDestroy(gpuContext_);
}

/*
 * @tc.name: NativeDrawingSurfaceTest_Flush
 * @tc.desc: test for Flush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_Flush, TestSize.Level1)
{
    gpuContext_ = OH_Drawing_GpuContextCreate();
    EXPECT_NE(gpuContext_, nullptr);

    const int32_t width = 500;
    const int32_t height = 500;
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    surface_ = OH_Drawing_SurfaceCreateOnScreen(gpuContext_, imageInfo, window_);
    EXPECT_NE(surface_, nullptr);
    OH_Drawing_SurfaceDestroy(surface_);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_SurfaceFlush(nullptr);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_GpuContextDestroy(gpuContext_);
}

/*
 * @tc.name: NativeDrawingSurfaceTest_Utils
 * @tc.desc: test for Utils.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_Utils, TestSize.Level1)
{
    const int32_t width = 500;
    const int32_t height = 500;
    ImageInfo imageInfo(width, height, COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE);
    std::shared_ptr<Drawing::Surface> surface = DrawingSurfaceUtils::CreateFromWindow(nullptr, imageInfo, window_);
    EXPECT_EQ(surface_, nullptr);
    surface = DrawingSurfaceUtils::CreateFromWindow(nullptr, imageInfo, nullptr);
    EXPECT_EQ(surface_, nullptr);
    bool ret = DrawingSurfaceUtils::FlushSurface(nullptr);
    EXPECT_EQ(ret, false);
    DrawingSurfaceUtils::RemoveSurface(surface.get());
}

/*
 * @tc.name: NativeDrawingSurfaceTest_CreateOnScreen002
 * @tc.desc: test the handling of the mismatch between the size of imageInfo and buffersize.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_CreateOnScreen002, TestSize.Level1)
{
    gpuContext_ = OH_Drawing_GpuContextCreate();
    EXPECT_NE(gpuContext_, nullptr);
    int fenceFd = -1;
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    OHNativeWindow *nativeWindow = reinterpret_cast<OHNativeWindow *>(window_);
    auto res = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    EXPECT_EQ(res, OHOS::GSERROR_OK);
    OH_NativeBuffer* nativeBuffer = nullptr;
    auto ret = OH_NativeBuffer_FromNativeWindowBuffer(nativeWindowBuffer, &nativeBuffer);
    EXPECT_EQ(ret, OHOS::GSERROR_OK);
    OH_NativeBuffer_Config config = {};
    OH_NativeBuffer_GetConfig(nativeBuffer, &config);
    const int32_t width = config.width + 100;
    const int32_t height = config.height + 100;
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    surface_ = OH_Drawing_SurfaceCreateOnScreen(gpuContext_, imageInfo, window_);
    EXPECT_NE(surface_, nullptr);
    OH_Drawing_SurfaceDestroy(surface_);
    OH_Drawing_GpuContextDestroy(gpuContext_);
}

#ifdef RS_ENABLE_VK
// Include the cpp directly to access file-scope static variables and functions

/**
 * @tc.name: NativeDrawingSurfaceTest_FlushVulkanSurfaceNotFound
 * @tc.desc: test FlushVulkanSurface when surface is not found in the map
 *           covers iter == end() branch (line 149)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_FlushVulkanSurfaceNotFound, TestSize.Level1)
{
#ifdef RS_ENABLE_VK
    if (!Drawing::SystemProperties::IsUseVulkan()) {
        GTEST_SKIP() << "Not using Vulkan";
    }
    auto surface = std::make_shared<Drawing::Surface>();
    bool ret = FlushVulkanSurface(surface.get());
    EXPECT_EQ(ret, false);
#else
    GTEST_SKIP() << "RS_ENABLE_VK not defined";
#endif
}

/**
 * @tc.name: NativeDrawingSurfaceTest_FlushVulkanSurfaceNullNativeSurface
 * @tc.desc: test FlushVulkanSurface when nativeSurface is nullptr
 *           covers nativeSurface == nullptr branch (line 156-158)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_FlushVulkanSurfaceNullNativeSurface, TestSize.Level1)
{
#ifdef RS_ENABLE_VK
    if (!Drawing::SystemProperties::IsUseVulkan()) {
        GTEST_SKIP() << "Not using Vulkan";
    }
    auto surface = std::make_shared<Drawing::Surface>();
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    DrawingSurfaceUtils::InsertSurface(surface, gpuContext.get());
    // InsertSurface inserts nativeSurface=nullptr → FlushAndSubmit + return true
    bool ret = FlushVulkanSurface(surface.get());
    EXPECT_EQ(ret, true);
    g_vulkanSurfaceMap_.erase(surface.get());
#else
    GTEST_SKIP() << "RS_ENABLE_VK not defined";
#endif
}

/**
 * @tc.name: NativeDrawingSurfaceTest_FlushVulkanSurfaceNullRenderContext
 * @tc.desc: test FlushVulkanSurface when renderContext is nullptr
 *           covers renderContext == nullptr branch (line 162-164)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_FlushVulkanSurfaceNullRenderContext, TestSize.Level1)
{
#ifdef RS_ENABLE_VK
    if (!Drawing::SystemProperties::IsUseVulkan()) {
        GTEST_SKIP() << "Not using Vulkan";
    }
    auto surface = std::make_shared<Drawing::Surface>();
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    // Insert a non-null NativeSurfaceInfo directly into g_vulkanSurfaceMap_
    auto nativeSurface = new NativeBufferUtils::NativeSurfaceInfo();
    g_vulkanSurfaceMap_.insert({surface.get(),
        std::make_tuple(surface, gpuContext.get(), nativeSurface)});

    auto& manager = DrawingGpuContextManager::GetInstance();
    auto origRenderContext = manager.renderContext_;
    manager.renderContext_ = nullptr;

    bool ret = FlushVulkanSurface(surface.get());
    EXPECT_EQ(ret, false);

    manager.renderContext_ = origRenderContext;
    g_vulkanSurfaceMap_.erase(surface.get());
    delete nativeSurface;
#else
    GTEST_SKIP() << "RS_ENABLE_VK not defined";
#endif
}

/**
 * @tc.name: NativeDrawingSurfaceTest_FlushVulkanSurfaceNullVkInterface
 * @tc.desc: test FlushVulkanSurface when vkInterface is nullptr
 *           covers vkInterface == nullptr branch (line 167-169)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_FlushVulkanSurfaceNullVkInterface, TestSize.Level1)
{
#ifdef RS_ENABLE_VK
    if (!Drawing::SystemProperties::IsUseVulkan()) {
        GTEST_SKIP() << "Not using Vulkan";
    }
    auto surface = std::make_shared<Drawing::Surface>();
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    auto nativeSurface = new NativeBufferUtils::NativeSurfaceInfo();
    g_vulkanSurfaceMap_.insert({surface.get(),
        std::make_tuple(surface, gpuContext.get(), nativeSurface)});

    auto& manager = DrawingGpuContextManager::GetInstance();
    auto origRenderContext = manager.renderContext_;
    // Set renderContext to one whose RsVulkanContext::Get returns invalid interface
    auto emptyCtx = std::make_shared<RenderContextGL>();
    manager.renderContext_ = emptyCtx;

    bool ret = FlushVulkanSurface(surface.get());
    // With invalid Vulkan, vkInterface may be nullptr → return false
    EXPECT_EQ(ret, false);

    manager.renderContext_ = origRenderContext;
    g_vulkanSurfaceMap_.erase(surface.get());
    delete nativeSurface;
#else
    GTEST_SKIP() << "RS_ENABLE_VK not defined";
#endif
}

/**
 * @tc.name: NativeDrawingSurfaceTest_CreateVulkanWindowSurfaceNullRenderContext
 * @tc.desc: test CreateVulkanWindowSurface when renderContext is nullptr
 *           covers renderContext == nullptr branch (line 108-110)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeDrawingSurfaceTest, NativeDrawingSurfaceTest_CreateVulkanWindowSurfaceNullRenderContext, TestSize.Level1)
{
#ifdef RS_ENABLE_VK
    if (!Drawing::SystemProperties::IsUseVulkan()) {
        GTEST_SKIP() << "Not using Vulkan";
    }
    auto& manager = DrawingGpuContextManager::GetInstance();
    auto origRenderContext = manager.renderContext_;
    manager.renderContext_ = nullptr;

    const int32_t width = 500;
    const int32_t height = 500;
    ImageInfo imageInfo(width, height, COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE);
    auto surface = CreateVulkanWindowSurface(nullptr, imageInfo, window_);
    EXPECT_EQ(surface, nullptr);

    manager.renderContext_ = origRenderContext;
#else
    GTEST_SKIP() << "RS_ENABLE_VK not defined";
#endif
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS