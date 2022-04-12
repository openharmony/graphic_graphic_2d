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

#ifndef RS_EGL_IMAGE_MANAGER_H
#define RS_EGL_IMAGE_MANAGER_H
#ifdef RS_ENABLE_GL
#include <map>
#include <mutex>
#include <surface.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "GLES/glext.h"
#include "GLES3/gl32.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {
struct ImageCacheSeq {
    ImageCacheSeq() : eglImage_(EGL_NO_IMAGE_KHR), eglSync_(EGL_NO_SYNC_KHR) {}
    EGLImageKHR eglImage_;
    EGLSyncKHR eglSync_;
    uint32_t textureId_ = 0;
    ~ImageCacheSeq();
};

class RSEglImageManager {
public:
    explicit RSEglImageManager(EGLDisplay display);
    ~RSEglImageManager() {};
    uint32_t MapEglImageFromSurfaceBuffer(sptr<OHOS::SurfaceBuffer>& buffer);
    void UnMapEglImageFromSurfaceBuffer(int32_t seqNum);

    EGLImageKHR CreateEGLImage(const sptr<OHOS::SurfaceBuffer>& buffer);
    bool WaitReleaseEGLSync(int32_t seqNum);
private:
    std::mutex opMutex_;
    std::map<int32_t, ImageCacheSeq> imageCacheSeqs_;
    EGLDisplay eglDisplay_;
    int32_t currentImageSeqs_ = -1;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_ENABLE_GL

#endif