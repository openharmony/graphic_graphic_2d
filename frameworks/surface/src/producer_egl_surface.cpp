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

#include "producer_egl_surface.h"

#include "buffer_log.h"
#include "buffer_manager.h"
#include "egl_data_impl.h"

#define CHECK_NULLPTR(ptr)                  \
    if (ptr == nullptr) {                   \
        BLOGNE("new failed.");              \
        return SURFACE_ERROR_NOMEM;         \
    }

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "ProducerEglSurface" };
}

ProducerEglSurface::ProducerEglSurface(sptr<IBufferProducer>& producer)
{
    producer_ = producer;
    auto sret = producer_->GetName(name_);
    if (sret != SURFACE_ERROR_OK) {
        BLOGNE("GetName failed, %{public}s", SurfaceErrorStr(sret).c_str());
    }
    BLOGND("ctor");
}

ProducerEglSurface::~ProducerEglSurface()
{
    BLOGND("dtor");
    initFlag_ = false;
    if (IsRemote()) {
        for (auto it = bufferProducerCache_.begin(); it != bufferProducerCache_.end(); it++) {
            if (it->second->GetVirAddr() != nullptr) {
                BufferManager::GetInstance()->Unmap(it->second);
                it->second->SetEglData(nullptr);
            }
        }
    }

    sEglManager_ = nullptr;
    currentBuffer_ = nullptr;
    producer_ = nullptr;
}

SurfaceError ProducerEglSurface::RequestBuffer(sptr<SurfaceBuffer> &buffer,
    int32_t& fence, BufferRequestConfig &config)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferExtraDataImpl bedataimpl;
    retval.fence = EGL_NO_NATIVE_FENCE_FD_ANDROID;
    SurfaceError ret = producer_->RequestBuffer(config, bedataimpl, retval);
    if (ret != SURFACE_ERROR_OK) {
        BLOGN_FAILURE("Producer report %{public}s", SurfaceErrorStr(ret).c_str());
        return ret;
    }

    // add cache
    if (retval.buffer != nullptr && IsRemote()) {
        sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
        ret = BufferManager::GetInstance()->Map(bufferImpl);
        if (ret != SURFACE_ERROR_OK) {
            BLOGN_FAILURE_ID(retval.sequence, "Map failed");
        } else {
            BLOGN_SUCCESS_ID(retval.sequence, "Map");
        }
    }

    if (retval.buffer != nullptr) {
        bufferProducerCache_[retval.sequence] = SurfaceBufferImpl::FromBase(retval.buffer);
    } else {
        retval.buffer = bufferProducerCache_[retval.sequence];
    }
    buffer = retval.buffer;

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = BufferManager::GetInstance()->InvalidateCache(bufferImpl);
    if (ret != SURFACE_ERROR_OK) {
        BLOGNW("Warning [%{public}d], InvalidateCache failed", retval.sequence);
    }

    if (bufferImpl != nullptr) {
        bufferImpl->SetExtraData(bedataimpl);
    }

    for (auto it = retval.deletingBuffers.begin(); it != retval.deletingBuffers.end(); it++) {
        if (IsRemote() && bufferProducerCache_[*it]->GetVirAddr() != nullptr) {
            bufferProducerCache_[*it]->SetEglData(nullptr);
            BufferManager::GetInstance()->Unmap(bufferProducerCache_[*it]);
        }
        bufferProducerCache_.erase(*it);
    }

    fence = retval.fence;
    return SURFACE_ERROR_OK;
}

SurfaceError ProducerEglSurface::FlushBuffer(sptr<SurfaceBuffer> &buffer,
    int32_t fence, BufferFlushConfig &config)
{
    if (buffer == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    auto bufferImpl = SurfaceBufferImpl::FromBase(buffer);
    BufferExtraDataImpl bedataimpl;
    bufferImpl->GetExtraData(bedataimpl);
    return producer_->FlushBuffer(bufferImpl->GetSeqNum(), bedataimpl, fence, config);
}

SurfaceError ProducerEglSurface::InitContext(EGLContext context)
{
    sEglManager_ = EglManager::GetInstance();
    if (sEglManager_ == nullptr) {
        BLOGNE("EglManager::GetInstance Failed.");
        return SURFACE_ERROR_INIT;
    }

    if (sEglManager_->Init(context) != SURFACE_ERROR_OK) {
        BLOGNE("EglManager init failed.");
        return SURFACE_ERROR_INIT;
    }

    if (RequestBufferProc() != SURFACE_ERROR_OK) {
        BLOGNE("RequestBufferProc failed.");
        return SURFACE_ERROR_INIT;
    }    

    initFlag_ = true;
    return SURFACE_ERROR_OK;
}

EGLDisplay ProducerEglSurface::GetEglDisplay() const
{
    if (initFlag_) {
        return sEglManager_->GetEGLDisplay();
    }
    BLOGNE("ProducerEglSurface is not init.");
    return EGL_NO_DISPLAY;
}

EGLContext ProducerEglSurface::GetEglContext() const
{
    if (initFlag_) {
        return sEglManager_->GetEGLContext();
    }
    BLOGNE("ProducerEglSurface is not init.");
    return EGL_NO_CONTEXT;
}

EGLSurface ProducerEglSurface::GetEglSurface() const
{
    return EGL_NO_SURFACE;
}

GLuint ProducerEglSurface::GetEglFbo() const
{
    if (initFlag_ && currentBuffer_ != nullptr) {
        auto bufferImpl = SurfaceBufferImpl::FromBase(currentBuffer_);
        return bufferImpl->GetEglData()->GetFrameBufferObj();
    }

    BLOGNE("ProducerEglSurface is not init.");
    return 0;
}

SurfaceError ProducerEglSurface::SwapBuffers(const Rect &damage)
{
    if (!initFlag_) {
        BLOGNE("ProducerEglSurface is not init.");
        return SURFACE_ERROR_INIT;
    }

    if (FlushBufferProc(damage) != SURFACE_ERROR_OK) {
        BLOGNE("FlushBufferProc failed.");
    }

    if (RequestBufferProc() != SURFACE_ERROR_OK) {
        BLOGNE("RequestBufferProc failed.");
        return SURFACE_ERROR_ERROR;
    }

    return SURFACE_ERROR_OK;
}

SurfaceError ProducerEglSurface::WaitForReleaseFence(int32_t fd)
{
    SurfaceError ret = SURFACE_ERROR_OK;
    if (fd != EGL_NO_NATIVE_FENCE_FD_ANDROID) {
        BLOGNI("releaseFence %{public}d.", fd);
        EGLint attribList[] = {
            EGL_SYNC_NATIVE_FENCE_FD_ANDROID, fd,
            EGL_NONE,
        };

        EGLSyncKHR sync = sEglManager_->EglCreateSync(EGL_SYNC_NATIVE_FENCE_ANDROID, attribList);
        if (sync == EGL_NO_SYNC_KHR) {
            BLOGNE("EglCreateSync failed.");
            return SURFACE_ERROR_ERROR;
        }

        if (sEglManager_->EglWaitSync(sync, 0) != EGL_TRUE) {
            BLOGNE("EglWaitSync failed.");
            ret = SURFACE_ERROR_ERROR;
        }

        if (sEglManager_->EglDestroySync(sync) != EGL_TRUE) {
            BLOGNE("EglDestroySync failed.");
            ret = SURFACE_ERROR_ERROR;
        }
    }
    return ret;
}

SurfaceError ProducerEglSurface::RequestBufferProc()
{
    int32_t releaseFence;
    BufferRequestConfig rconfig = {
        .width = producer_->GetDefaultWidth(),
        .height = producer_->GetDefaultHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = producer_->GetDefaultUsage(),
        .timeout = 0,
    };

    currentBuffer_ = nullptr;
    if (RequestBuffer(currentBuffer_, releaseFence, rconfig) != SURFACE_ERROR_OK) {
        BLOGNE("RequestBuffer failed.");
        return SURFACE_ERROR_ERROR;
    }

    if (AddEglData(currentBuffer_) != SURFACE_ERROR_OK) {
        BLOGNE("AddEglData failed.");
        return SURFACE_ERROR_ERROR;
    }

    if (WaitForReleaseFence(releaseFence) != SURFACE_ERROR_OK) {
        BLOGNE("WaitForReleaseFence failed.");
        return SURFACE_ERROR_ERROR;
    }

    return SURFACE_ERROR_OK;
}

SurfaceError ProducerEglSurface::CreateEglFenceFd(int32_t &fd)
{
    EGLSyncKHR sync = sEglManager_->EglCreateSync(EGL_SYNC_NATIVE_FENCE_ANDROID, nullptr);
    if (sync == EGL_NO_SYNC_KHR) {
        BLOGNE("EglCreateSync failed.");
        return SURFACE_ERROR_ERROR;
    }

    glFlush();

    fd = sEglManager_->EglDupNativeFenceFd(sync);
    if (sEglManager_->EglDestroySync(sync) != EGL_TRUE) {
        BLOGNE("EglDestroySync failed.");
    }

    if (fd == EGL_NO_NATIVE_FENCE_FD_ANDROID) {
        BLOGNE("EglDupNativeFenceFd failed."); 
        return SURFACE_ERROR_ERROR;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError ProducerEglSurface::FlushBufferProc(const Rect &damage)
{
    int32_t fd = EGL_NO_NATIVE_FENCE_FD_ANDROID;
    if (currentBuffer_ == nullptr) {
        BLOGNE("currentBuffer_ is nullptr.");
        return SURFACE_ERROR_ERROR;
    }

    if (CreateEglFenceFd(fd) != SURFACE_ERROR_OK) {
        BLOGNE("CreateEglFenceFd failed.");
        // return SURFACE_ERROR_ERROR;
    }
    BLOGNE("flush fence fd %{public}d.", fd);

    BufferFlushConfig fconfig = {
        .damage = {
            .x = damage.x,
            .y = damage.y,
            .w = damage.w,
            .h = damage.h,
        },
    };

    if (FlushBuffer(currentBuffer_, fd, fconfig) != SURFACE_ERROR_OK) {
        BLOGNE("FlushBuffer failed.");
        return SURFACE_ERROR_ERROR;
    }

    return SURFACE_ERROR_OK;
}

bool ProducerEglSurface::IsRemote()
{
    return producer_->AsObject()->IsProxyObject();
}

SurfaceError ProducerEglSurface::AddEglData(sptr<SurfaceBuffer> &buffer)
{
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);
    sptr<EglData> sEglData = bufferImpl->GetEglData();
    if (sEglData != nullptr) {
        // sEglManager_->EglMakeCurrent();
        glBindFramebuffer(GL_FRAMEBUFFER, sEglData->GetFrameBufferObj());
        BLOGI("bufferImpl is reused return.");
        return SURFACE_ERROR_OK;
    }

    sptr<EglDataImpl> sEglDataImpl = new EglDataImpl();
    CHECK_NULLPTR(sEglDataImpl);
    auto sret = sEglDataImpl->CreateEglData(bufferImpl);
    if (sret == SURFACE_ERROR_OK) {
        bufferImpl->SetEglData(sEglDataImpl);
        BLOGI("bufferImpl FBO=%{public}d.", sEglDataImpl->GetFrameBufferObj());
    }
    return sret;
}

int32_t ProducerEglSurface::GetDefaultWidth()
{
    return producer_->GetDefaultWidth();
}

int32_t ProducerEglSurface::GetDefaultHeight()
{
    return producer_->GetDefaultHeight();
}

} // namespace OHOS
