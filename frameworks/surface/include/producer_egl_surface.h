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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_PRODUCER_EGL_SURFACE_H
#define FRAMEWORKS_SURFACE_INCLUDE_PRODUCER_EGL_SURFACE_H

#include <map>
#include <string>

#include "egl_manager.h"
#include <egl_surface.h>
#include <ibuffer_producer.h>
#include "surface_buffer_impl.h"

namespace OHOS {
class ProducerEglSurface : public EglRenderSurface {
public:
    ProducerEglSurface(sptr<IBufferProducer>& producer);
    virtual ~ProducerEglSurface();

    SurfaceError InitContext(EGLContext context = EGL_NO_CONTEXT) override;
    EGLDisplay GetEglDisplay() const override;
    EGLContext GetEglContext() const override;
    EGLSurface GetEglSurface() const override;
    GLuint GetEglFbo() const override;
    SurfaceError SwapBuffers(const Rect &damage) override;
    int32_t GetDefaultWidth() override;
    int32_t GetDefaultHeight() override;

private:
    bool IsRemote();
    SurfaceError RequestBufferProc();
    SurfaceError RequestBuffer(sptr<SurfaceBuffer> &buffer, int32_t &fence, BufferRequestConfig &config);

    SurfaceError FlushBufferProc(const Rect &damage);
    SurfaceError FlushBuffer(sptr<SurfaceBuffer> &buffer, int32_t fence, BufferFlushConfig &config);
    SurfaceError AddEglData(sptr<SurfaceBuffer> &buffer);
    SurfaceError CreateEglFenceFd(int32_t &fd);
    SurfaceError WaitForReleaseFence(int32_t fd);

    std::map<int32_t, sptr<SurfaceBufferImpl>> bufferProducerCache_;
    sptr<IBufferProducer> producer_ = nullptr;
    std::string name_ = "not init";
    bool initFlag_ = false;
    sptr<EglManager> sEglManager_ = nullptr;
    sptr<SurfaceBuffer> currentBuffer_ = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_PRODUCER_EGL_SURFACE_H
