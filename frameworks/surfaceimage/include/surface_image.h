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

#ifndef FRAMEWORKS_SURFACE_IMAGE_H
#define FRAMEWORKS_SURFACE_IMAGE_H

#include <atomic>
#include <mutex>
#include <array>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES3/gl32.h>
#include "buffer_log.h"
#include "consumer_surface.h"

namespace OHOS {
struct ImageCacheSeq {
    ImageCacheSeq() : eglImage_(EGL_NO_IMAGE_KHR), eglSync_(EGL_NO_SYNC_KHR) {}
    EGLImageKHR eglImage_;
    EGLSyncKHR eglSync_;
};

static constexpr int64_t TRANSFORM_MATRIX_ELE_COUNT = 16;
typedef void (*OnBufferAvailableListener)(void *context);

class SurfaceImage : public ConsumerSurface {
public:
    SurfaceImage(uint32_t textureId, uint32_t textureTarget = GL_TEXTURE_EXTERNAL_OES);
    virtual ~SurfaceImage();

    void InitSurfaceImage();

    std::string GetSurfaceImageName() const
    {
        return surfaceImageName_;
    }

    SurfaceError SetDefaultSize(int32_t width, int32_t height);

    SurfaceError UpdateSurfaceImage();
    int64_t GetTimeStamp();

    // update buffer available state, updateSurfaceImage_ and a private mutex
    void OnUpdateBufferAvailableState(bool updated)
    {
        updateSurfaceImage_ = updated;
    }

    bool GetBufferAvailableState()
    {
        return updateSurfaceImage_;
    }

    SurfaceError AttachContext(uint32_t textureId);
    SurfaceError DetachContext();

    SurfaceError GetTransformMatrix(float matrix[16]);
    SurfaceError GetTransformMatrixV2(float matrix[16]);
    SurfaceError SetOnBufferAvailableListener(void *context, OnBufferAvailableListener listener);
    SurfaceError UnsetOnBufferAvailableListener();
    OnBufferAvailableListener listener_ = nullptr;
    void *context_ = nullptr;

private:
    SurfaceError ValidateEglState();
    EGLImageKHR CreateEGLImage(EGLDisplay disp, const sptr<SurfaceBuffer>& buffer);
    SurfaceError UpdateEGLImageAndTexture(EGLDisplay disp, const sptr<SurfaceBuffer>& buffer);
    void UpdateSurfaceInfo(uint32_t seqNum, sptr<SurfaceBuffer> buffer, const sptr<SyncFence> &acquireFence,
                           int64_t timestamp, Rect damage);

    uint32_t textureId_;
    uint32_t textureTarget_;
    std::string surfaceImageName_;

    std::mutex opMutex_;
    std::atomic<bool> updateSurfaceImage_;

    EGLDisplay eglDisplay_;
    EGLContext eglContext_;
    std::map<uint32_t, ImageCacheSeq> imageCacheSeqs_;
    uint32_t currentSurfaceImage_;
    sptr<SurfaceBuffer> currentSurfaceBuffer_;
    int64_t currentTimeStamp_;
    Rect currentCrop_ = {};
    GraphicTransformType currentTransformType_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    float currentTransformMatrix_[TRANSFORM_MATRIX_ELE_COUNT] = {0.0};
    float currentTransformMatrixV2_[TRANSFORM_MATRIX_ELE_COUNT] = {0.0};
};

class SurfaceImageListener : public IBufferConsumerListener {
public:
    explicit SurfaceImageListener(const sptr<SurfaceImage> & surfaceImage) : surfaceImage_(surfaceImage)
    {
        BLOGI("SurfaceImageListener");
    };
    virtual ~SurfaceImageListener();

    virtual void OnBufferAvailable() override;

private:
    wptr<SurfaceImage> surfaceImage_;
};
} // namespace OHOS
#endif // FRAMEWORKS_SURFACE_IMAGE_H
