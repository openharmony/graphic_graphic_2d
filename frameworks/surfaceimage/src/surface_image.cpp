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

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#include "surface_image.h"

#include "securec.h"
#include "sandbox_utils.h"
#include "surface_utils.h"

#include <cinttypes>
#include <atomic>
#include "metadata_helper.h"
#include <sync_fence.h>
#include <unistd.h>
#include <window.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

namespace OHOS {
namespace {
// Get a uniqueID in a process
static int GetProcessUniqueId()
{
    static std::atomic<int> counter { 0 };
    return counter.fetch_add(1, std::memory_order_relaxed);
}
}

SurfaceImage::SurfaceImage(uint32_t textureId, uint32_t textureTarget)
    : ConsumerSurface("SurfaceImage-" + std::to_string(GetRealPid()) + "-" + std::to_string(GetProcessUniqueId())),
      textureId_(textureId),
      textureTarget_(textureTarget),
      updateSurfaceImage_(false),
      eglDisplay_(EGL_NO_DISPLAY),
      eglContext_(EGL_NO_CONTEXT),
      currentSurfaceImage_(UINT_MAX),
      currentSurfaceBuffer_(nullptr),
      currentTimeStamp_(0)
{
    InitSurfaceImage();
}

SurfaceImage::SurfaceImage()
    : ConsumerSurface("SurfaceImageConsumer-" + std::to_string(GetRealPid()) +
    "-" + std::to_string(GetProcessUniqueId())),
      currentSurfaceBuffer_(nullptr),
      currentTimeStamp_(0)
{
    InitSurfaceImage();
}

SurfaceImage::~SurfaceImage()
{
    for (auto it = imageCacheSeqs_.begin(); it != imageCacheSeqs_.end(); it++) {
        DestroyEGLImage(it->second.eglImage_);
        DestroyEGLSync(it->second.eglSync_);
    }
}

void SurfaceImage::InitSurfaceImage()
{
    std::string name = "SurfaceImage-" + std::to_string(GetRealPid()) + "-" + std::to_string(GetProcessUniqueId());
    auto ret = ConsumerSurface::Init();
    uniqueId_ = GetUniqueId();
    BLOGI("InitSurfaceImage Init ret: %{public}d, uniqueId: %{public}" PRIu64 ".", ret, uniqueId_);
    surfaceImageName_ = name;
}

void SurfaceImage::UpdateBasicInfo(const sptr<SurfaceBuffer>& buffer, int64_t timestamp)
{
    currentSurfaceBuffer_ = buffer;
    currentTimeStamp_ = timestamp;
    preBufferProperties_ = bufferProperties_;
    bufferProperties_ = {
        GetBufferCropRegion(buffer),
        ConsumerSurface::GetTransform(),
        buffer->GetWidth(),
        buffer->GetHeight()
    };
}

void SurfaceImage::UpdateSurfaceInfo(sptr<SurfaceBuffer> buffer, const sptr<SyncFence> &acquireFence,
                                     int64_t timestamp, const Rect& damage)
{
    // release old buffer
    int releaseFence = -1;
    auto iter = imageCacheSeqs_.find(currentSurfaceImage_);
    if (iter != imageCacheSeqs_.end() && iter->second.eglSync_ != EGL_NO_SYNC_KHR) {
        releaseFence = eglDupNativeFenceFDANDROID(eglDisplay_, iter->second.eglSync_);
    }
    // There is no need to close this fd, because in function ReleaseBuffer it will be closed.
    ReleaseBuffer(currentSurfaceBuffer_, releaseFence);

    currentSurfaceImage_ = buffer->GetSeqNum();
    UpdateBasicInfo(buffer, timestamp);

    auto utils = SurfaceUtils::GetInstance();
    utils->ComputeTransformMatrix(currentTransformMatrix_, TRANSFORM_MATRIX_ELE_COUNT,
        currentSurfaceBuffer_, bufferProperties_.transformType, damage);
    utils->ComputeTransformMatrixV2(currentTransformMatrixV2_, TRANSFORM_MATRIX_ELE_COUNT,
        currentSurfaceBuffer_, bufferProperties_.transformType, damage);
    if (preBufferProperties_ != bufferProperties_) {
        utils->ComputeBufferMatrix(currentBufferMatrix_, TRANSFORM_MATRIX_ELE_COUNT,
            currentSurfaceBuffer_, bufferProperties_.transformType, bufferProperties_.crop);
    }

    // wait on this acquireFence.
    if (acquireFence != nullptr) {
        acquireFence->Wait(-1);
    }
}

Rect SurfaceImage::GetBufferCropRegion(const sptr<OHOS::SurfaceBuffer>& buffer)
{
    BufferHandleMetaRegion cropRegion{0, 0, 0, 0};

    if (MetadataHelper::GetCropRectMetadata(buffer, cropRegion) == GSERROR_OK) {
        BLOGD("GetCropRectMetadata success,"
            "left: %{public}d, top: %{public}d, width: %{public}d, height: %{public}d",
            cropRegion.left, cropRegion.top, cropRegion.width, cropRegion.height);
        return {cropRegion.left, cropRegion.top, cropRegion.width, cropRegion.height};
    }

    return {0, 0, buffer->GetWidth(), buffer->GetHeight()};
}

SurfaceError SurfaceImage::UpdateSurfaceImage()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);

    // validate egl state
    SurfaceError ret = ValidateEglState();
    if (ret != SURFACE_ERROR_OK) {
        return ret;
    }

    // acquire buffer
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    int64_t timestamp = 0;
    Rect damage = {0, 0, 0, 0};
    if (!dropFrameMode_) {
        ret = AcquireBuffer(buffer, acquireFence, timestamp, damage);
        if (ret != SURFACE_ERROR_OK) {
            return ret;
        }
    } else {
        AcquireBufferReturnValue returnValue;
        ret = AcquireBuffer(returnValue, INT64_MAX, true);
        if (ret != SURFACE_ERROR_OK) {
            BLOGE("AcquireBuffer falied: %{public}d, uniqueId: %{public}" PRIu64, ret, uniqueId_);
            return ret;
        }
        buffer = returnValue.buffer;
        acquireFence = returnValue.fence;
        timestamp = returnValue.timestamp;
        if (returnValue.damages.size() != 0) {
            damage = returnValue.damages.at(0);
        }
    }

    ret = UpdateEGLImageAndTexture(buffer);
    if (ret != SURFACE_ERROR_OK) {
        ReleaseBuffer(buffer, -1);
        return ret;
    }

    UpdateSurfaceInfo(buffer, acquireFence, timestamp, damage);
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::AttachContext(uint32_t textureId)
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    // validate egl state
    SurfaceError ret = ValidateEglState();
    if (ret != SURFACE_ERROR_OK) {
        return ret;
    }

    textureId_ = textureId;
    auto iter = imageCacheSeqs_.find(currentSurfaceImage_);
    if (iter != imageCacheSeqs_.end()) {
        glBindTexture(textureTarget_, textureId);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            BLOGE("glBindTexture failed, textureTarget:%{public}d, textureId_:%{public}d, error:%{public}d,"
                "uniqueId: %{public}" PRIu64 ".", textureTarget_, textureId_, error, uniqueId_);
            return SURFACE_ERROR_EGL_API_FAILED;
        }
        glEGLImageTargetTexture2DOES(textureTarget_, static_cast<GLeglImageOES>(iter->second.eglImage_));
        error = glGetError();
        if (error != GL_NO_ERROR) {
            BLOGE("glEGLImageTargetTexture2DOES failed, textureTarget:%{public}d, error:%{public}d"
                "uniqueId: %{public}" PRIu64 ".", textureTarget_, error, uniqueId_);
            return SURFACE_ERROR_EGL_API_FAILED;
        }
    }

    // If there is no EGLImage, we cannot simply return an error.
    // Developers can call OH_NativeImage_UpdateSurfaceImage later to achieve their purpose.
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::DetachContext()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    // validate egl state
    SurfaceError ret = ValidateEglState();
    if (ret != SURFACE_ERROR_OK) {
        return ret;
    }

    textureId_ = 0;
    glBindTexture(textureTarget_, 0);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        BLOGE("glBindTexture failed, textureTarget:%{public}d, textureId:%{public}d, error:%{public}d"
            "uniqueId: %{public}" PRIu64 ".", textureTarget_, textureId_, error, uniqueId_);
        return SURFACE_ERROR_EGL_API_FAILED;
    }
    return SURFACE_ERROR_OK;
}

int64_t SurfaceImage::GetTimeStamp()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    return currentTimeStamp_;
}

SurfaceError SurfaceImage::GetTransformMatrix(float matrix[TRANSFORM_MATRIX_ELE_COUNT])
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    auto ret = memcpy_s(matrix, sizeof(float) * TRANSFORM_MATRIX_ELE_COUNT,
                        currentTransformMatrix_, sizeof(currentTransformMatrix_));
    if (ret != EOK) {
        BLOGE("memcpy_s failed ret: %{public}d, uniqueId: %{public}" PRIu64 ".", ret, uniqueId_);
        return SURFACE_ERROR_UNKOWN;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::GetTransformMatrixV2(float matrix[TRANSFORM_MATRIX_ELE_COUNT])
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    auto ret = memcpy_s(matrix, sizeof(float) * TRANSFORM_MATRIX_ELE_COUNT,
                        currentTransformMatrixV2_, sizeof(currentTransformMatrixV2_));
    if (ret != EOK) {
        BLOGE("memcpy_s failed ret: %{public}d, uniqueId: %{public}" PRIu64 ".", ret, uniqueId_);
        return SURFACE_ERROR_UNKOWN;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::GetBufferMatrix(float matrix[TRANSFORM_MATRIX_ELE_COUNT])
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    auto ret = memcpy_s(matrix, sizeof(float) * TRANSFORM_MATRIX_ELE_COUNT,
                        currentBufferMatrix_, sizeof(currentBufferMatrix_));
    if (ret != EOK) {
        BLOGE("memcpy_s failed ret: %{public}d, uniqueId: %{public}" PRIu64 ".", ret, uniqueId_);
        return SURFACE_ERROR_MEM_OPERATION_ERROR;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::ValidateEglState()
{
    EGLDisplay disp = eglGetCurrentDisplay();
    EGLContext context = eglGetCurrentContext();

    if ((eglDisplay_ != disp && eglDisplay_ != EGL_NO_DISPLAY) || (disp == EGL_NO_DISPLAY)) {
        BLOGE("EGLDisplay is invalid, errno : 0x%{public}x, uniqueId: %{public}" PRIu64 ".",
            eglGetError(), uniqueId_);
        return SURFACE_ERROR_EGL_STATE_UNKONW;
    }
    if ((eglContext_ != context && eglContext_ != EGL_NO_CONTEXT) || (context == EGL_NO_CONTEXT)) {
        BLOGE("EGLContext is invalid, errno : 0x%{public}x, uniqueId: %{public}" PRIu64 ".",
            eglGetError(), uniqueId_);
        return SURFACE_ERROR_EGL_STATE_UNKONW;
    }

    eglDisplay_ = disp;
    eglContext_ = context;
    return SURFACE_ERROR_OK;
}

EGLImageKHR SurfaceImage::CreateEGLImage(EGLDisplay disp, const sptr<SurfaceBuffer>& buffer)
{
    sptr<SurfaceBuffer> bufferImpl = buffer;
    NativeWindowBuffer* nBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&bufferImpl);
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    EGLImageKHR img = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nBuffer, attrs);
    if (img == EGL_NO_IMAGE_KHR) {
        EGLint error = eglGetError();
        BLOGE("failed, error %{public}d, uniqueId: %{public}" PRIu64 ".", error, uniqueId_);
        eglTerminate(disp);
    }
    DestroyNativeWindowBuffer(nBuffer);
    return img;
}

void SurfaceImage::CheckImageCacheNeedClean(uint32_t seqNum)
{
    for (auto it = imageCacheSeqs_.begin(); it != imageCacheSeqs_.end();) {
        bool result = true;
        if (seqNum == it->first) {
            it++;
            continue;
        }
        if (IsSurfaceBufferInCache(it->first, result) == SURFACE_ERROR_OK && !result) {
            DestroyEGLImage(it->second.eglImage_);
            DestroyEGLSync(it->second.eglSync_);
            it = imageCacheSeqs_.erase(it);
        } else {
            it++;
        }
    }
}

void SurfaceImage::DestroyEGLImage(EGLImageKHR &eglImage)
{
    if (eglImage != EGL_NO_IMAGE_KHR) {
        eglDestroyImageKHR(eglDisplay_, eglImage);
        eglImage = EGL_NO_IMAGE_KHR;
    }
}

void SurfaceImage::DestroyEGLSync(EGLSyncKHR &eglSync)
{
    if (eglSync != EGL_NO_SYNC_KHR) {
        eglDestroySyncKHR(eglDisplay_, eglSync);
        eglSync = EGL_NO_SYNC_KHR;
    }
}

void SurfaceImage::DestroyEGLImageBySeq(uint32_t seqNum)
{
    auto iter = imageCacheSeqs_.find(seqNum);
    if (iter != imageCacheSeqs_.end()) {
        DestroyEGLImage(iter->second.eglImage_);
        imageCacheSeqs_.erase(seqNum);
    }
}

void SurfaceImage::NewBufferDestroyEGLImage(bool isNewBuffer, uint32_t seqNum)
{
    if (isNewBuffer) {
        DestroyEGLImageBySeq(seqNum);
    }
}

SurfaceError SurfaceImage::UpdateEGLImageAndTexture(const sptr<SurfaceBuffer>& buffer)
{
    bool isNewBuffer = false;
    // private function, buffer is always valid.
    uint32_t seqNum = buffer->GetSeqNum();
    // If there was no eglImage binding to this buffer, we create a new one.
    if (imageCacheSeqs_.find(seqNum) == imageCacheSeqs_.end()) {
        isNewBuffer = true;
        EGLImageKHR eglImage = CreateEGLImage(eglDisplay_, buffer);
        if (eglImage == EGL_NO_IMAGE_KHR) {
            return SURFACE_ERROR_EGL_API_FAILED;
        }
        imageCacheSeqs_[seqNum].eglImage_ = eglImage;
    }

    auto &image = imageCacheSeqs_[seqNum];
    glBindTexture(textureTarget_, textureId_);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        NewBufferDestroyEGLImage(isNewBuffer, seqNum);
        BLOGE("glBindTexture failed, textureTarget:%{public}d, textureId_:%{public}d, error:%{public}d"
            "uniqueId: %{public}" PRIu64 ".", textureTarget_, textureId_, error, uniqueId_);
        return SURFACE_ERROR_EGL_API_FAILED;
    }
    glEGLImageTargetTexture2DOES(textureTarget_, static_cast<GLeglImageOES>(image.eglImage_));
    error = glGetError();
    if (error != GL_NO_ERROR) {
        NewBufferDestroyEGLImage(isNewBuffer, seqNum);
        BLOGE("glEGLImageTargetTexture2DOES failed, textureTarget:%{public}d, error:%{public}d"
            "uniqueId: %{public}" PRIu64 ".", textureTarget_, error, uniqueId_);
        return SURFACE_ERROR_EGL_API_FAILED;
    }

    // Create fence object for current image
    auto iter = imageCacheSeqs_.find(currentSurfaceImage_);
    if (iter != imageCacheSeqs_.end()) {
        auto &currentImage = iter->second;
        auto preSync = currentImage.eglSync_;
        if (preSync != EGL_NO_SYNC_KHR) {
            eglDestroySyncKHR(eglDisplay_, preSync);
        }
        currentImage.eglSync_ = eglCreateSyncKHR(eglDisplay_, EGL_SYNC_NATIVE_FENCE_ANDROID, nullptr);
        glFlush();
    }

    if (isNewBuffer) {
        CheckImageCacheNeedClean(seqNum);
    }
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::SetOnBufferAvailableListener(void *context, OnBufferAvailableListener listener)
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    if (listener == nullptr) {
        BLOGE("listener is nullptr, uniqueId: %{public}" PRIu64 ".", uniqueId_);
        return SURFACE_ERROR_INVALID_PARAM;
    }

    listener_ = listener;
    context_ = context;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::UnsetOnBufferAvailableListener()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    listener_ = nullptr;
    context_ = nullptr;
    return SURFACE_ERROR_OK;
}

SurfaceImageListener::~SurfaceImageListener()
{
    BLOGE("~SurfaceImageListener");
    surfaceImage_ = nullptr;
}

void SurfaceImageListener::OnBufferAvailable()
{
    BLOGD("enter OnBufferAvailable");
    auto surfaceImage = surfaceImage_.promote();
    if (surfaceImage == nullptr) {
        BLOGE("surfaceImage promote failed");
        return;
    }

    // check here maybe a messagequeue, flag instead now
    surfaceImage->OnUpdateBufferAvailableState(true);
    if (surfaceImage->listener_ != nullptr) {
        surfaceImage->listener_(surfaceImage->context_);
    }
}

SurfaceError SurfaceImage::AcquireNativeWindowBuffer(OHNativeWindowBuffer** nativeWindowBuffer, int32_t* fenceFd)
{
    if (nativeWindowBuffer == nullptr || fenceFd == nullptr) {
        return SURFACE_ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = AcquireBuffer(buffer, acquireFence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        BLOGD("AcquireBuffer failed: %{public}d, uniqueId: %{public}" PRIu64 ".", ret, uniqueId_);
        return ret;
    }

    UpdateBasicInfo(buffer, timestamp);

    auto utils = SurfaceUtils::GetInstance();
    utils->ComputeTransformMatrixV2(currentTransformMatrixV2_, TRANSFORM_MATRIX_ELE_COUNT,
        currentSurfaceBuffer_, bufferProperties_.transformType, damage);
    if (bufferProperties_ != preBufferProperties_) {
        utils->ComputeBufferMatrix(currentBufferMatrix_, TRANSFORM_MATRIX_ELE_COUNT,
            currentSurfaceBuffer_, bufferProperties_.transformType, bufferProperties_.crop);
    }

    *fenceFd = acquireFence->Dup();
    OHNativeWindowBuffer *nwBuffer = new(std::nothrow) OHNativeWindowBuffer();
    if (nwBuffer == nullptr) {
        return SURFACE_ERROR_NOMEM;
    }
    nwBuffer->sfbuffer = buffer;
    NativeObjectReference(nwBuffer);
    *nativeWindowBuffer = nwBuffer;

    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::ReleaseNativeWindowBuffer(OHNativeWindowBuffer* nativeWindowBuffer, int32_t fenceFd)
{
    if (nativeWindowBuffer == nullptr) {
        return SURFACE_ERROR_INVALID_PARAM;
    }
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    // There is no need to close this fd, because in function ReleaseBuffer it will be closed.
    SurfaceError ret = ReleaseBuffer(nativeWindowBuffer->sfbuffer, fenceFd);
    if (ret != SURFACE_ERROR_OK) {
        BLOGE("ReleaseBuffer failed: %{public}d, uniqueId: %{public}" PRIu64 ".", ret, uniqueId_);
        return ret;
    }
    NativeObjectUnreference(nativeWindowBuffer);
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::SetDefaultUsage(uint64_t usage)
{
    SurfaceError ret = ConsumerSurface::SetDefaultUsage(usage);
    if (ret != SURFACE_ERROR_OK) {
        BLOGE("SetDefaultUsage failed: %{public}d, uniqueId: %{public}" PRIu64 ", usage: %{public}" PRIu64 ".", ret,
            uniqueId_, usage);
    }
    return ret;
}

SurfaceError SurfaceImage::SetDefaultSize(int32_t width, int32_t height)
{
    SurfaceError ret = SetDefaultWidthAndHeight(width, height);
    if (ret != SURFACE_ERROR_OK) {
        BLOGE("SetDefaultWidthAndHeight failed: %{public}d, uniqueId: %{public}" PRIu64 ", width: %{public}d, "
            "height: %{public}d", ret, uniqueId_, width, height);
    }
    return ret;
}

SurfaceError SurfaceImage::SetDropBufferMode(bool enableDrop)
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    BLOGI("SetDropBufferMode switch: %{public}d", enableDrop);
    SurfaceError ret = ConsumerSurface::SetDropBufferMode(enableDrop);
    if (ret != SURFACE_ERROR_OK) {
        BLOGE("ConsumerSurface::SetDropBufferMode ret: %{public}d", ret);
        return ret;
    }
    dropFrameMode_ = enableDrop;
    return SURFACE_ERROR_OK;
}
} // namespace OHOS
