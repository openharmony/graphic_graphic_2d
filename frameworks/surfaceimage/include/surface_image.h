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
#include "native_window.h"

namespace OHOS {
struct ImageCacheSeq {
    ImageCacheSeq() : eglImage_(EGL_NO_IMAGE_KHR), eglSync_(EGL_NO_SYNC_KHR) {}
    EGLImageKHR eglImage_;
    EGLSyncKHR eglSync_;
};

static constexpr uint32_t TRANSFORM_MATRIX_ELE_COUNT = 16;
typedef void (*OnBufferAvailableListener)(void *context);

class SurfaceImage : public ConsumerSurface {
public:
    SurfaceImage(uint32_t textureId, uint32_t textureTarget = GL_TEXTURE_EXTERNAL_OES);
    SurfaceImage();
    virtual ~SurfaceImage();

    void InitSurfaceImage();

    std::string GetSurfaceImageName() const
    {
        return surfaceImageName_;
    }

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
    SurfaceError GetBufferMatrix(float matrix[16]);
    SurfaceError SetOnBufferAvailableListener(void *context, OnBufferAvailableListener listener);
    SurfaceError UnsetOnBufferAvailableListener();
    OnBufferAvailableListener listener_ = nullptr;
    void *context_ = nullptr;

    SurfaceError AcquireNativeWindowBuffer(OHNativeWindowBuffer** nativeWindowBuffer, int32_t* fenceFd);
    SurfaceError ReleaseNativeWindowBuffer(OHNativeWindowBuffer* nativeWindowBuffer, int32_t fenceFd);
    
    SurfaceError SetDefaultUsage(uint64_t usage);
    SurfaceError SetDefaultSize(int32_t width, int32_t height);
    SurfaceError SetDropBufferSwitch(bool isOpen);
private:
    void UpdateBasicInfo(const sptr<SurfaceBuffer>& buffer, int64_t timestamp);
    Rect GetBufferCropRegion(const sptr<OHOS::SurfaceBuffer>& buffer);
    SurfaceError ValidateEglState();
    EGLImageKHR CreateEGLImage(EGLDisplay disp, const sptr<SurfaceBuffer>& buffer);
    SurfaceError UpdateEGLImageAndTexture(const sptr<SurfaceBuffer>& buffer);
    void UpdateSurfaceInfo(sptr<SurfaceBuffer> buffer, const sptr<SyncFence> &acquireFence,
                           int64_t timestamp, const Rect& damage);
    void CheckImageCacheNeedClean(uint32_t seqNum);
    void DestroyEGLImage(EGLImageKHR &eglImage);
    void DestroyEGLSync(EGLSyncKHR &eglSync);
    void NewBufferDestroyEGLImage(bool isNewBuffer, uint32_t seqNum);
    void DestroyEGLImageBySeq(uint32_t seqNum);

    uint32_t textureId_ = 0;
    uint32_t textureTarget_ = GL_TEXTURE_EXTERNAL_OES;
    std::string surfaceImageName_;

    std::mutex opMutex_;
    std::atomic<bool> updateSurfaceImage_;

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    std::map<uint32_t, ImageCacheSeq> imageCacheSeqs_;
    uint32_t currentSurfaceImage_ = UINT_MAX;
    sptr<SurfaceBuffer> currentSurfaceBuffer_;
    int64_t currentTimeStamp_;
    float currentTransformMatrix_[TRANSFORM_MATRIX_ELE_COUNT] = {0.0};
    float currentTransformMatrixV2_[TRANSFORM_MATRIX_ELE_COUNT] = {0.0};
    float currentBufferMatrix_[TRANSFORM_MATRIX_ELE_COUNT] = {0.0};
    uint64_t uniqueId_ = 0;
    bool dropFrameMode_ = false;

    /**
     * @brief Represents the properties of a graphics buffer frame
     *
     * This structure encapsulates essential information about a graphics buffer frame,
     * including its effective region, dimensions, and transformation settings. It is used
     * to track and compare buffer states for efficient graphics rendering and processing.
     */
    struct BufferProperties {
        /**
         * @brief Defines the valid/effective region within the buffer
         * Represents the actual usable area of the buffer that contains valid content,
         * which may be smaller than the full buffer dimensions
         */
        Rect crop = {0, 0, 0, 0};

        /**
         * @brief Rotation/transformation type applied to the buffer
         * Specifies how the buffer content should be transformed during rendering
         */
        GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;

        /**
         * @brief Width of the buffer in pixels
         * Represents the horizontal dimension of the graphics buffer
         */
        uint32_t bufferWidth = 0;

        /**
         * @brief Height of the buffer in pixels
         * Represents the vertical dimension of the graphics buffer
         */
        uint32_t bufferHeight = 0;

        /**
         * @brief Equality comparison operator
         * @param other The BufferProperties object to compare with
         * @return true if all properties match, false otherwise
         */
        bool operator==(const BufferProperties& other) const
        {
            return crop == other.crop &&
                transformType == other.transformType &&
                bufferWidth == other.bufferWidth &&
                bufferHeight == other.bufferHeight;
        }

        /**
         * @brief Inequality comparison operator
         * @param other The BufferProperties object to compare with
         * @return true if any property differs, false if all match
         */
        bool operator!=(const BufferProperties& other) const
        {
            return !(*this == other);
        }
    };
    BufferProperties bufferProperties_;
    BufferProperties preBufferProperties_;
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
