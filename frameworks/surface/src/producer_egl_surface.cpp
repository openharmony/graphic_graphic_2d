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

#include <mutex>

#include <scoped_bytrace.h>

#include "buffer_log.h"
#include "buffer_manager.h"
#include "egl_data_impl.h"
#include "buffer_extra_data_impl.h"
#include "sync_fence.h"

namespace OHOS {
ProducerEglSurface::ProducerEglSurface(sptr<IBufferProducer>& producer)
{
    producer_ = producer;
    width_ = producer_->GetDefaultWidth();
    height_ = producer_->GetDefaultHeight();
    auto sret = producer_->GetName(name_);
    if (sret != GSERROR_OK) {
        BLOGNE("GetName failed, %{public}s", GSErrorStr(sret).c_str());
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

    if (currentBuffer_ != nullptr) {
        const sptr<BufferExtraData>& bedataimpl = currentBuffer_->GetExtraData();
        producer_->CancelBuffer(currentBuffer_->GetSeqNum(), bedataimpl);
        currentBuffer_ = nullptr;
    }
    sEglManager_ = nullptr;
    producer_ = nullptr;
}

GSError ProducerEglSurface::RequestBuffer(sptr<SurfaceBuffer> &buffer,
    int32_t& fence, BufferRequestConfig &config)
{
    IBufferProducer::RequestBufferReturnValue retval;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;
    retval.fence = SyncFence::INVALID_FENCE;
    GSError ret = producer_->RequestBuffer(config, bedataimpl, retval);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Producer report %{public}s", GSErrorStr(ret).c_str());
        return ret;
    }

    // add cache
    if (retval.buffer != nullptr && IsRemote()) {
        ret = BufferManager::GetInstance()->Map(retval.buffer);
        if (ret != GSERROR_OK) {
            BLOGN_FAILURE_ID(retval.sequence, "Map failed");
        } else {
            BLOGN_SUCCESS_ID(retval.sequence, "Map");
        }
    }

    if (retval.buffer != nullptr) {
        bufferProducerCache_[retval.sequence] = retval.buffer;
    } else {
        retval.buffer = bufferProducerCache_[retval.sequence];
    }
    buffer = retval.buffer;

    ret = BufferManager::GetInstance()->InvalidateCache(buffer);
    if (ret != GSERROR_OK) {
        BLOGNW("Warning [%{public}d], InvalidateCache failed", retval.sequence);
    }

    if (buffer != nullptr) {
        buffer->SetExtraData(bedataimpl);
    }

    for (auto it = retval.deletingBuffers.begin(); it != retval.deletingBuffers.end(); it++) {
        if (IsRemote() && bufferProducerCache_[*it]->GetVirAddr() != nullptr) {
            bufferProducerCache_[*it]->SetEglData(nullptr);
            BufferManager::GetInstance()->Unmap(bufferProducerCache_[*it]);
        }
        bufferProducerCache_.erase(*it);
    }

    fence = retval.fence->Get();
    return GSERROR_OK;
}

GSError ProducerEglSurface::FlushBuffer(sptr<SurfaceBuffer> &buffer,
    int32_t fence, BufferFlushConfig &config)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    sptr<SyncFence> syncFence = new SyncFence(fence);
    const sptr<BufferExtraData>& bedataimpl = buffer->GetExtraData();
    return producer_->FlushBuffer(buffer->GetSeqNum(), bedataimpl, syncFence, config);
}

GSError ProducerEglSurface::InitContext(EGLContext context)
{
    ScopedBytrace func(__func__);

    sEglManager_ = EglManager::GetInstance();
    if (sEglManager_ == nullptr) {
        BLOGNE("EglManager::GetInstance Failed.");
        return GSERROR_INTERNEL;
    }

    if (sEglManager_->Init(context) != GSERROR_OK) {
        BLOGNE("EglManager init failed.");
        return GSERROR_INTERNEL;
    }

    if (initFlag_) {
        return GSERROR_OK;
    }

    if (RequestBufferProc() != GSERROR_OK) {
        BLOGNE("RequestBufferProc failed.");
        return GSERROR_INTERNEL;
    }

    initFlag_ = true;
    return GSERROR_OK;
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
        return currentBuffer_->GetEglData()->GetFrameBufferObj();
    }

    BLOGNE("ProducerEglSurface is not init.");
    return 0;
}

GSError ProducerEglSurface::SwapBuffers()
{
    ScopedBytrace func(__func__);
    if (!initFlag_) {
        BLOGNE("ProducerEglSurface is not init.");
        return GSERROR_INTERNEL;
    }

    if (FlushBufferProc() != GSERROR_OK) {
        BLOGNE("FlushBufferProc failed.");
    }

    if (RequestBufferProc() != GSERROR_OK) {
        BLOGNE("RequestBufferProc failed.");
        return GSERROR_INTERNEL;
    }

    return GSERROR_OK;
}

GSError ProducerEglSurface::SetWidthAndHeight(int32_t width, int32_t height)
{
    if (width <= 0 || height <= 0) {
        return SURFACE_ERROR_INVALID_PARAM;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    width_ = width;
    height_ = height;
    return GSERROR_OK;
}

GSError ProducerEglSurface::WaitForReleaseFence(int32_t fd)
{
    ScopedBytrace func(__func__);
    GSError ret = GSERROR_OK;
    if (fd != EGL_NO_NATIVE_FENCE_FD_ANDROID) {
        BLOGNI("releaseFence %{public}d.", fd);
        EGLint attribList[] = {
            EGL_SYNC_NATIVE_FENCE_FD_ANDROID, fd,
            EGL_NONE,
        };

        EGLSyncKHR sync = sEglManager_->EglCreateSync(EGL_SYNC_NATIVE_FENCE_ANDROID, attribList);
        if (sync == EGL_NO_SYNC_KHR) {
            BLOGNE("EglCreateSync failed.");
            return GSERROR_INTERNEL;
        }

        if (sEglManager_->EglWaitSync(sync, 0) != EGL_TRUE) {
            BLOGNE("EglWaitSync failed.");
            ret = GSERROR_INTERNEL;
        }

        if (sEglManager_->EglDestroySync(sync) != EGL_TRUE) {
            BLOGNE("EglDestroySync failed.");
            ret = GSERROR_INTERNEL;
        }
    }
    return ret;
}

GSError ProducerEglSurface::RequestBufferProc()
{
    ScopedBytrace func(__func__);
    int32_t releaseFence;
    {
        BufferRequestConfig rconfig;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            rconfig = {
                .width = width_,
                .height = height_,
                .strideAlignment = 0x8,
                .format = PIXEL_FMT_RGBA_8888,
                .usage = producer_->GetDefaultUsage(),
                .timeout = 0,
            };
        }

        currentBuffer_ = nullptr;
        if (RequestBuffer(currentBuffer_, releaseFence, rconfig) != GSERROR_OK) {
            BLOGNE("RequestBuffer failed.");
            return GSERROR_INTERNEL;
        }
    }

    if (AddEglData(currentBuffer_) != GSERROR_OK) {
        BLOGNE("AddEglData failed.");
        return GSERROR_INTERNEL;
    }

    if (WaitForReleaseFence(releaseFence) != GSERROR_OK) {
        BLOGNE("WaitForReleaseFence failed.");
        return GSERROR_INTERNEL;
    }

    return GSERROR_OK;
}

GSError ProducerEglSurface::CreateEglFenceFd(int32_t &fd)
{
    EGLSyncKHR sync = sEglManager_->EglCreateSync(EGL_SYNC_NATIVE_FENCE_ANDROID, nullptr);
    if (sync == EGL_NO_SYNC_KHR) {
        BLOGNE("EglCreateSync failed.");
        return GSERROR_INTERNEL;
    }

    glFlush();

    fd = sEglManager_->EglDupNativeFenceFd(sync);
    if (sEglManager_->EglDestroySync(sync) != EGL_TRUE) {
        BLOGNE("EglDestroySync failed.");
    }

    if (fd == EGL_NO_NATIVE_FENCE_FD_ANDROID) {
        BLOGNE("EglDupNativeFenceFd failed.");
        return GSERROR_INTERNEL;
    }
    return GSERROR_OK;
}

GSError ProducerEglSurface::FlushBufferProc()
{
    ScopedBytrace func(__func__);
    int32_t fd = EGL_NO_NATIVE_FENCE_FD_ANDROID;
    if (currentBuffer_ == nullptr) {
        BLOGNE("currentBuffer_ is nullptr.");
        return GSERROR_INTERNEL;
    }

    if (CreateEglFenceFd(fd) != GSERROR_OK) {
        BLOGNE("CreateEglFenceFd failed.");
    }
    BLOGNE("flush fence fd %{public}d.", fd);

    BufferFlushConfig fconfig = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = currentBuffer_->GetWidth(),
            .h = currentBuffer_->GetHeight(),
        },
    };
    if (FlushBuffer(currentBuffer_, fd, fconfig) != GSERROR_OK) {
        BLOGNE("FlushBuffer failed.");
        return GSERROR_INTERNEL;
    }

    return GSERROR_OK;
}

bool ProducerEglSurface::IsRemote()
{
    return producer_->AsObject()->IsProxyObject();
}

GSError ProducerEglSurface::AddEglData(sptr<SurfaceBuffer> &buffer)
{
    ScopedBytrace func(__func__);
    sptr<EglData> sEglData = buffer->GetEglData();
    if (sEglData != nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, sEglData->GetFrameBufferObj());
        BLOGI("buffer is reused return.");
        return GSERROR_OK;
    }

    sptr<EglDataImpl> sEglDataImpl = new EglDataImpl();
    if (sEglDataImpl == nullptr) {
        BLOGNE("new failed.");
        return GSERROR_NO_MEM;
    }
    auto sret = sEglDataImpl->CreateEglData(buffer);
    if (sret == GSERROR_OK) {
        buffer->SetEglData(sEglDataImpl);
        BLOGI("buffer FBO=%{public}d.", sEglDataImpl->GetFrameBufferObj());
    }
    return sret;
}
} // namespace OHOS
