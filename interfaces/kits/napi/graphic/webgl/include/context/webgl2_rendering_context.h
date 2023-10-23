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

#ifndef WEBGL2_RENDERING_CONTEXT_H
#define WEBGL2_RENDERING_CONTEXT_H

#include "webgl_rendering_context_base.h"
#include "webgl2_rendering_context_impl.h"
#include "webgl2_rendering_context_base.h"
#include "webgl2_rendering_context_overloads.h"
#include "napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
class WebGL2RenderingContext
    : public WebGLRenderingContextBasicBase, public WebGLRenderingContextBase, WebGL2RenderingContextBase,
    public WebGL2RenderingContextOverloads, public NExporter {
public:
    inline static const std::string className = "WebGL2RenderingContext";

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    WebGL2RenderingContext(napi_env env, napi_value exports)
        : WebGLRenderingContextBasicBase(),
        NExporter(env, exports), contextImpl_(WEBGL_2_0, this) {}; // 2 is WebGL2

    explicit WebGL2RenderingContext() : WebGLRenderingContextBasicBase(), contextImpl_(WEBGL_2_0, this) {};
    virtual ~WebGL2RenderingContext();

    Impl::WebGL2RenderingContextImpl &GetWebGL2RenderingContextImpl()
    {
        return contextImpl_;
    }

    Impl::WebGLRenderingContextBaseImpl &GetWebGLRenderingContextImpl()
    {
        return contextImpl_;
    }

    void Init() override
    {
        WebGLRenderingContextBasicBase::Init();
        contextImpl_.Init();
    }
private:
    Impl::WebGL2RenderingContextImpl contextImpl_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL2_RENDERING_CONTEXT_H
