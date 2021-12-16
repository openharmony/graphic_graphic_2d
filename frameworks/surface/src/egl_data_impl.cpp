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

#include <display_gralloc.h>
#include <drm_fourcc.h>

#include "buffer_log.h"
#include "egl_manager.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "EglDataImpl" };
} // namespace

EglDataImpl::EglDataImpl()
{
    BLOGD("ctor");
    eglManager_ = EglManager::GetInstance();
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

    if (eglImage_) {
        EglManager::GetInstance()->EglDestroyImage(eglImage_);
    }
}

GLuint EglDataImpl::GetFrameBufferObj() const
{
    return glFbo_;
}

GLuint EglDataImpl::GetTexture() const
{
    return glTexture_;
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

GSError EglDataImpl::CreateEglData(const sptr<SurfaceBuffer> &buffer)
{
    if (!EglManager::GetInstance()->IsInit()) {
        EglManager::GetInstance()->Init();
    }

    BufferHandle *handle = buffer->GetBufferHandle();
    if (!handle) {
        BLOGE("Failed to GetBufferHandle");
        return GSERROR_INTERNEL;
    }

    uint32_t drmFormat;
    if (PixelFormatToDrmFormat(handle->format, drmFormat) == false) {
        BLOGE("PixelFormatToDrmFormat failed");
        return GSERROR_INTERNEL;
    }

    EGLint attribs[] = {
        EGL_WIDTH, handle->width,
        EGL_HEIGHT, handle->height,
        EGL_LINUX_DRM_FOURCC_EXT, drmFormat,
        EGL_DMA_BUF_PLANE0_FD_EXT, handle->fd,
        EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
        EGL_DMA_BUF_PLANE0_PITCH_EXT, handle->stride,
        EGL_NONE,
    };
    eglImage_ = EglManager::GetInstance()->EglCreateImage(EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, NULL, attribs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        BLOGE("##createImage failed.");
        return GSERROR_INTERNEL;
    }
    EglManager::GetInstance()->EglMakeCurrent();

    glGenTextures(1, &glTexture_);
    glBindTexture(GL_TEXTURE_2D, glTexture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    EglManager::GetInstance()->EglImageTargetTexture2D(GL_TEXTURE_2D, eglImage_);

    glGenFramebuffers(1, &glFbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, glFbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glTexture_, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        BLOGE("FBO creation failed");
        return GSERROR_INTERNEL;
    }
    return GSERROR_OK;
}
} // namespace OHOS
