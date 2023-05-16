/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include "include/core/SkSurface.h"
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#ifdef IOS_PLATFORM
#include "render_context_egl_defines.h"
#else
#include "EGL/egl.h"
#endif
namespace OHOS {
namespace Rosen {
class RenderContext {
public:
    virtual ~RenderContext() = default;
    virtual void InitializeEglContext() = 0;
    virtual sk_sp<SkSurface> AcquireSurface(int width, int height) = 0;
    virtual sk_sp<SkSurface> GetSurface() const = 0;
    virtual bool SetUpGrContext() = 0;
    virtual void MakeCurrent(EGLSurface surface = nullptr, EGLContext context = EGL_NO_CONTEXT) = 0;
    virtual void RenderFrame() = 0;
#if defined(NEW_SKIA)
    virtual GrDirectContext* GetGrContext() const = 0;
#else
    virtual GrContext* GetGrContext() const = 0;
#endif
    virtual void SetCacheDir(const std::string& filePath) {}
    virtual EGLContext GetEGLContext() const { return nullptr; }
    virtual EGLSurface CreateEGLSurface(EGLNativeWindowType) = 0;
    virtual void SwapBuffers(EGLSurface surface = nullptr) const = 0;
    virtual EGLint QueryEglBufferAge() { return 0;}
    virtual void ClearRedundantResources() {}
    virtual void SetUniRenderMode(bool isUni) {}
#ifdef RS_ENABLE_GL
    virtual std::string GetShaderCacheSize() const
    {
        return {};
    }

    virtual std::string CleanAllShaderCache() const
    {
        return {};
    }
#endif
};

class RenderContextFactory {
public:
    static RenderContextFactory& GetInstance();

    ~RenderContextFactory()
    {
        if (context_ != nullptr) {
            delete context_;
        }
        context_ = nullptr;
    }

    RenderContext* CreateEngine();
    RenderContext* CreateNewEngine();

private:
    RenderContextFactory() : context_(nullptr) {}
    RenderContextFactory(const RenderContextFactory&) = delete;
    RenderContextFactory& operator=(const RenderContextFactory&) = delete;

    RenderContext* context_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif
