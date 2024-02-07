/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gles_render_backend.h"

#include "egl_manager.h"
#include "drawing_utils.h"
#include "skia_adapter/skia_surface.h"
#include "surface_ohos_gl.h"
#include "iostream"

namespace OHOS {
namespace Rosen {
GLESRenderBackend::GLESRenderBackend() noexcept
{
    eglManager_ = new EGLManager();
}

GLESRenderBackend::~GLESRenderBackend()
{
    if (eglManager_) {
        delete eglManager_;
        eglManager_ = nullptr;
    }
}

void GLESRenderBackend::InitDrawContext()
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not InitDrawContext");
        return;
    }
    eglManager_->Init();
}

bool GLESRenderBackend::SetUpGrContext()
{
    if (grContext_ != nullptr) {
        LOGD("grContext has already created!!");
        return true;
    }

    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (glInterface.get() == nullptr) {
        LOGE("SetUpGrContext failed to make native interface");
        return false;
    }

    GrContextOptions options;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    // fix svg antialiasing bug
    options.fGpuPathRenderers &= ~GpuPathRenderers::kAtlas;
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    sk_sp<GrDirectContext> grContext(GrDirectContext::MakeGL(std::move(glInterface), options));
    if (grContext == nullptr) {
        LOGE("SetUpGrContext grContext is null");
        return false;
    }
#else
    sk_sp<GrContext> grContext(GrContext::MakeGL(std::move(glInterface), options));
    if (grContext == nullptr) {
        LOGE("SetUpGrContext grContext is null");
        return false;
    }
#endif
    grContext_ = std::move(grContext);
    LOGI("SetUpGrContext done");
    return true;
}

bool GLESRenderBackend::SetUpDrContext()
{
    if (drGPUContext_ != nullptr) {
        LOGD("Drawing GPUContext has already created!!");
        return true;
    }

    Drawing::GPUContextOptions options;

    auto drGPUContext = std::make_shared<Drawing::GPUContext>();
    if (!drGPUContext->BuildFromGL(options)) {
        LOGE("SetUpGrContext drGPUContext is null");
        return false;
    }
    drGPUContext_ = std::move(drGPUContext);
    return true;
}

void GLESRenderBackend::MakeCurrent()
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not MakeCurrent");
        return;
    }
    eglManager_->MakeCurrent();
}

void GLESRenderBackend::SwapBuffers()
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not SwapBuffers");
        return;
    }
    eglManager_->SwapBuffers();
}

void* GLESRenderBackend::CreateSurface(void* window)
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not CreateSurface");
        return nullptr;
    }
    return static_cast<void*>(eglManager_->CreateSurface((EGLNativeWindowType)window));
}

void GLESRenderBackend::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not SetDamageRegion");
        return;
    }
    eglManager_->SetDamageRegion(left, top, width, height);
}

void GLESRenderBackend::Destroy()
{
    grContext_ = nullptr;
    skSurface_ = nullptr;
}

void GLESRenderBackend::RenderFrame()
{
    // flush commands
    if (pSkSurface_->getCanvas() != nullptr) {
        LOGD("RenderFrame: Canvas");
        std::cout << "GLESRenderBackend::RenderFrame flushing" << std::endl;
        pSkSurface_->getCanvas()->flush();
    } else {
        LOGW("canvas is nullptr!!!");
    }
}

SkCanvas* GLESRenderBackend::AcquireSkCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (!SetUpGrContext()) {
        LOGE("GrContext is not ready!!!");
        return nullptr;
    }

    SurfaceFrameOhosGl* framePtr = static_cast<SurfaceFrameOhosGl*>(frame.get());

    if (GetGrContext() == nullptr) {
        LOGE("GrContext is not ready!!!");
        return nullptr;
    }

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    int32_t width = framePtr->GetWidth();
    int32_t height = framePtr->GetHeight();
    const int stencilBufferSize = 8;

    GrBackendRenderTarget backendRenderTarget(width, height, 0, stencilBufferSize, framebufferInfo);
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    SkSurfaceProps surfaceProps(0, kRGB_H_SkPixelGeometry);
#else
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
#endif
    skSurface_ = SkSurface::MakeFromBackendRenderTarget(
        GetGrContext(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, &surfaceProps);
    if (skSurface_ == nullptr) {
        LOGW("skSurface is nullptr");
        return nullptr;
    }
    pSkSurface_ = skSurface_.get();
    LOGI("CreateCanvas successfully!!!");

    return skSurface_->getCanvas();
}

Drawing::Canvas* GLESRenderBackend::AcquireDrCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (!SetUpDrContext()) {
        LOGE("GrContext is not ready!!!");
        return nullptr;
    }

    SurfaceFrameOhosGl* framePtr = static_cast<SurfaceFrameOhosGl*>(frame.get());

    int32_t width = framePtr->GetWidth();
    int32_t height = framePtr->GetHeight();

    std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr;

    colorSpace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB,
        Drawing::CMSMatrixType::DCIP3);

    struct Drawing::FrameBuffer bufferInfo;
    bufferInfo.width = width;
    bufferInfo.height = height;
    bufferInfo.FBOID = 0;
    bufferInfo.Format = GL_RGBA8;
    bufferInfo.colorType = Drawing::COLORTYPE_RGBA_8888;
    bufferInfo.gpuContext = drGPUContext_;
    bufferInfo.colorSpace = colorSpace;

    drSurface_ = std::make_shared<Drawing::Surface>();
    if (!drSurface_->Bind(bufferInfo)) {
        LOGW("surface_ is nullptr");
        drSurface_ = nullptr;
        return nullptr;
    }

    LOGD("CreateCanvas successfully!!!");
    pSkSurface_ = drSurface_->GetImpl<Drawing::SkiaSurface>()->GetSkSurface().get();
    return drSurface_->GetCanvas().get();
}
}
}