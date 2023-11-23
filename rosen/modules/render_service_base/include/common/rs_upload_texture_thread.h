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

#include "event_handler.h"
#include "common/rs_macros.h"
#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "include/core/SkSurface.h"
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#endif
#endif
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

#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
    sk_sp<GrDirectContext> GetShareGrContext() const;
    void CleanGrResource();
#endif
#endif
private:
    RSUploadTextureThread();
    ~RSUploadTextureThread() = default;
    RSUploadTextureThread(const RSUploadTextureThread&);
    RSUploadTextureThread(const RSUploadTextureThread&&);
    RSUploadTextureThread& operator=(const RSUploadTextureThread&);
    RSUploadTextureThread& operator=(const RSUploadTextureThread&&);

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
    sk_sp<GrDirectContext> CreateShareGrContext();
    void CreateShareEglContext();
    RenderContext* renderContext_ = nullptr;
    sk_sp<GrDirectContext> grContext_ = nullptr;
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#endif
#endif
};
}
#endif // RS_UPLOAD_TEXTURE_THREAD_H
