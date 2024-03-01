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

#ifndef RS_UPLOAD_TEXTURE_THREAD_H
#define RS_UPLOAD_TEXTURE_THREAD_H

#include <condition_variable>
#include <vector>
#include <mutex>

#include "event_handler.h"
#include "common/rs_macros.h"
#include <include/gpu/GrDirectContext.h>
#include "src/gpu/GrSurfaceProxy.h"
#ifdef RS_ENABLE_GL
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "include/core/SkSurface.h"
#endif
#include "draw/canvas.h"
#include "draw/surface.h"
#include "draw/canvas.h"
#include "image/gpu_context.h"
#include "image/image.h"
#include "platform/common/rs_log.h"
#include "render_context/render_context.h"

namespace OHOS::Rosen {

void UploadTextureWithDrawing(bool paraUpload, const std::shared_ptr<Drawing::Image>& imageUp,
    const std::shared_ptr<Media::PixelMap>& pixelMapUp, uint64_t uniqueId);

// Resource Collector
class ResourceCollection : public GrDirectContext::ResourceCollector {
public:
    void collectSurfaceProxy(sk_sp<GrSurfaceProxy>& surface) override
    {
        std::unique_lock<std::mutex> lock(mutex_);
        grSurfaceProxyVec_.push_back(surface);
    }

    void SwapCollection(std::vector<sk_sp<GrSurfaceProxy>>& dst, uint32_t overSize)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (grSurfaceProxyVec_.size() > overSize) {
            RS_LOGI("RSUploadResourceThread SwapCollection");
            std::swap(grSurfaceProxyVec_, dst);
        }
    }

private:
    std::mutex mutex_;
    std::vector<sk_sp<GrSurfaceProxy>> grSurfaceProxyVec_;
};

class RSB_EXPORT RSUploadResourceThread final {
public:
    static RSUploadResourceThread& Instance();
    // Task Related
    void PostTask(const std::function<void()>& task);
    void PostSyncTask(const std::function<void()>& task);
    void PostTask(const std::function<void()>& task, const std::string& name);
    void RemoveTask(const std::string& name);
    void InitRenderContext(RenderContext* context);

    void OnRenderEnd();
    void OnProcessBegin();
    bool TaskIsValid(int64_t count);
    inline int64_t GetFrameCount() const {
        return frameCount_.load();
    }
    inline bool IsEnable() const {
        return uploadProperity_ && isTargetPlatform_;
    }
    inline bool ImageSupportParallelUpload(int w, int h) {
        return (w < IMG_WIDTH_MAX) && (h < IMG_HEIGHT_MAX);
    }

    std::shared_ptr<Drawing::GPUContext> GetShareGrContext() const;
    void ReleaseNotUsedPinnedViews();
private:
    RSUploadResourceThread();
    ~RSUploadResourceThread() = default;
    RSUploadResourceThread(const RSUploadResourceThread&) = delete;
    RSUploadResourceThread(const RSUploadResourceThread&&)= delete;
    RSUploadResourceThread& operator=(const RSUploadResourceThread&) = delete;
    RSUploadResourceThread& operator=(const RSUploadResourceThread&&) = delete;

    void WaitUntilRenderEnd();
    void CreateShareEglContext();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;

    std::condition_variable uploadTaskCond_;
    std::mutex uploadTaskMutex_;
    bool enableTime_ = false;
    std::atomic<int64_t> frameCount_{0};
    bool uploadProperity_ = true;
    bool isTargetPlatform_ = false;
    static constexpr int CLEAN_VIEW_COUNT = 10;
    static constexpr int IMG_WIDTH_MAX = 300;
    static constexpr int IMG_HEIGHT_MAX = 300;
    RenderContext* renderContext_ = nullptr;
#ifdef RS_ENABLE_GL
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#endif
    std::mutex proxyViewMutex_;
    std::shared_ptr<Drawing::GPUContext> CreateShareGrContext();
    std::shared_ptr<Drawing::GPUContext> grContext_ = nullptr;
    sk_sp<GrDirectContext> skContext_ = nullptr;
    ResourceCollection resCollector_;
};
}
#endif // RS_UPLOAD_TEXTURE_THREAD_H
