/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "egl_data_impl.h"

#include "buffer_log.h"
#include <display_gralloc.h>
#include <drm_fourcc.h>

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "EglDataImpl" };
constexpr int32_t GENERAL_ATTRIBS = 3;
constexpr int32_t PLANE_ATTRIBS = 5;
constexpr int32_t ENTRIES_PER_ATTRIB = 2;
constexpr int32_t MAX_BUFFER_PLANES = 4;
} // namespace

EglDataImpl::EglDataImpl()
{
    BLOGD("ctor");
    sEglManager_ = EglManager::GetInstance();
    if (sEglManager_ == nullptr) {
        BLOGE("EglManager::GetInstance Failed.");
    }
}

EglDataImpl::~EglDataImpl()
{
    BLOGD("dtor");
    if (glFbo_) {
        glDeleteFramebuffers(1, &glFbo_);
    }

    if (glTexture_) {
        glDeleteTextures(1, &glTexture_);
    }

    if (eglImage_ && sEglManager_) {
        sEglManager_->EglDestroyImage(eglImage_);
    }
    sEglManager_ = nullptr;
}

GLuint EglDataImpl::GetFrameBufferObj() const
{
    return glFbo_;
}

namespace {
bool PixelFormatToDrmFormat(int32_t pixelFormat, uint32_t &drmFormat)
{
    constexpr struct {
        int32_t pixelFormat;
        uint32_t drmFormat;
    } formatTable[] = {
        { PIXEL_FMT_RGB_565, DRM_FORMAT_RGB565 },
        { PIXEL_FMT_RGBX_4444, DRM_FORMAT_RGBX4444 },
        { PIXEL_FMT_RGBA_4444, DRM_FORMAT_RGBA4444 },
        { PIXEL_FMT_RGBX_5551, DRM_FORMAT_RGBX5551 },
        { PIXEL_FMT_RGBA_5551, DRM_FORMAT_RGBA5551 },
        { PIXEL_FMT_RGBX_8888, DRM_FORMAT_RGBX8888 },
        { PIXEL_FMT_RGBA_8888, DRM_FORMAT_ABGR8888 },
        { PIXEL_FMT_RGB_888, DRM_FORMAT_RGB888 },
        { PIXEL_FMT_BGR_565, DRM_FORMAT_BGR565 },
        { PIXEL_FMT_BGRX_4444, DRM_FORMAT_BGRX4444 },
        { PIXEL_FMT_BGRA_4444, DRM_FORMAT_BGRA4444 },
        { PIXEL_FMT_BGRX_5551, DRM_FORMAT_BGRX5551 },
        { PIXEL_FMT_BGRA_5551, DRM_FORMAT_BGRA5551 },
        { PIXEL_FMT_BGRX_8888, DRM_FORMAT_BGRX8888 },
        { PIXEL_FMT_BGRA_8888, DRM_FORMAT_BGRA8888 },
        { PIXEL_FMT_YUV_422_I, DRM_FORMAT_YUV422 },
        { PIXEL_FMT_YUYV_422_PKG, DRM_FORMAT_YUYV },
        { PIXEL_FMT_UYVY_422_PKG, DRM_FORMAT_UYVY },
        { PIXEL_FMT_YVYU_422_PKG, DRM_FORMAT_YVYU },
        { PIXEL_FMT_VYUY_422_PKG, DRM_FORMAT_VYUY },
    };

    for (const auto &fmt : formatTable) {
        if (fmt.pixelFormat == pixelFormat) {
            drmFormat = fmt.drmFormat;
            return true;
        }
    }
    return false;
}
} // namespace

SurfaceError EglDataImpl::CreateEglData(const sptr<SurfaceBufferImpl> &buffer)
{
    EGLint attribs[(GENERAL_ATTRIBS + PLANE_ATTRIBS * MAX_BUFFER_PLANES) * ENTRIES_PER_ATTRIB + 1];
    unsigned int index = 0;

    BufferHandle *handle = buffer->GetBufferHandle();
    if (!handle) {
        BLOGE("Failed to GetBufferHandle");
        return SURFACE_ERROR_ERROR;
    }

    uint32_t drmFormat;
    if (PixelFormatToDrmFormat(handle->format, drmFormat) == false) {
        BLOGE("PixelFormatToDrmFormat failed");
        return SURFACE_ERROR_ERROR;
    }
    attribs[index++] = EGL_WIDTH;
    attribs[index++] = handle->width;
    attribs[index++] = EGL_HEIGHT;
    attribs[index++] = handle->height;
    attribs[index++] = EGL_LINUX_DRM_FOURCC_EXT;
    attribs[index++] = drmFormat;

    attribs[index++] = EGL_DMA_BUF_PLANE0_FD_EXT;
    attribs[index++] = handle->fd;
    attribs[index++] = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
    attribs[index++] = 0;
    attribs[index++] = EGL_DMA_BUF_PLANE0_PITCH_EXT;
    attribs[index++] = handle->stride;
    
    attribs[index] = EGL_NONE;
    eglImage_ = sEglManager_->EglCreateImage(EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, NULL, attribs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        BLOGE("##createImage failed.");
        return SURFACE_ERROR_ERROR;
    }
    sEglManager_->EglMakeCurrent();

    glGenTextures(1, &glTexture_);
    glBindTexture(GL_TEXTURE_2D, glTexture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    sEglManager_->EglImageTargetTexture2D(GL_TEXTURE_2D, eglImage_);

    glGenFramebuffers(1, &glFbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, glFbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glTexture_, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        BLOGE("FBO creation failed");
        return SURFACE_ERROR_ERROR;
    }
    return SURFACE_ERROR_OK;
}

} // namespace OHOS
