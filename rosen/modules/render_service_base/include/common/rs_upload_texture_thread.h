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
#include <mutex>
#include <vector>
#include "common/rs_macros.h"
#include "event_handler.h"
#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#endif

namespace OHOS::Rosen {
class RenderContext;

class RSB_EXPORT RSUploadTextureThread final {
public:
    static RSUploadTextureThread& Instance();
    void PostTask(const std::function<void()>& task);
    void PostSyncTask(const std::function<void()>& task);
    void PostTask(const std::function<void()>& task, const std::string& name);
    void RemoveTask(const std::string& name);
    void InitRenderContext(RenderContext* context);

    void OnRenderEnd();
    void OnProcessBegin();
    int64_t GetFrameCount() const;
    bool TaskIsValid(int64_t count);
    bool IsEnable() const;
    bool ImageSupportParallelUpload(int w, int h);

#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#endif
private:
    RSUploadTextureThread();
    ~RSUploadTextureThread() = default;
    RSUploadTextureThread(const RSUploadTextureThread&);
    RSUploadTextureThread(const RSUploadTextureThread&&);
    RSUploadTextureThread& operator=(const RSUploadTextureThread&);
    RSUploadTextureThread& operator=(const RSUploadTextureThread&&);

    void WaitUntilRenderEnd();

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
#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#endif
};
}
#endif // RS_UPLOAD_TEXTURE_THREAD_H
