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

#ifndef ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASIC_BASE
#define ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASIC_BASE

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "../canvas_render_context_base.h"
#include "webgl_context_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class WebGLRenderingContextBasicBase : public OHOS::Ace::CanvasRenderContextBase {
public:
    static WebGLRenderingContextBasicBase *instance;

    WebGLRenderingContextBasicBase() {};

    virtual ~WebGLRenderingContextBasicBase() {};

    static WebGLRenderingContextBasicBase *GetContext(std::string id);

    void SetEglWindow(void *window);

    virtual void Create(void *context);

    virtual void Init();

    virtual void Attach(uint64_t textureId);

    virtual void Update();

    virtual void Detach();

    virtual void SetBitMapPtr(char *bitMapPtr, int bitMapWidth, int bitMapHeight);

    virtual uint64_t CreateTexture();

    void SetTexture(uint64_t id);

    virtual void SetUpdateCallback(std::function<void()>);

public:
    GLuint frameBufferId = 0;
    GLuint textureId = 0;
    GLuint imgId = 0;
    void drawImg();
    GLuint compileShader(GLenum type, const char *sources);
    int useProgram();
    GLuint program = 0;
    GLuint vertShader = 0;
    GLuint fragShader = 0;
    GLint uniformSampler = 0;
    EGLSurface mEGLSurface = nullptr;
    NativeWindow *mEglWindow = nullptr;
    WebGLContextAttributes *webGlContextAttributes = nullptr;
    char *mBitMapPtr = nullptr;
    int mBitMapWidth = 0;
    int mBitMapHeight = 0;
    std::function<void()> mUpdateCallback;
};
} // namespace Rosen
} // namespace OHOS

OHOS::Rosen::WebGLRenderingContextBasicBase *GetWebGLInstance(std::string id);

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASIC_BASE
