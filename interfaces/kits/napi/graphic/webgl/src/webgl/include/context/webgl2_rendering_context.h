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

#ifndef ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT
#define ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT

#include "webgl2_rendering_context_base.h"
#include "webgl2_rendering_context_overloads.h"
#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
class WebGL2RenderingContext : public WebGL2RenderingContextBase,
    public WebGL2RenderingContextOverloads, public NExporter {
public:
    inline static const std::string className = "WebGL2RenderingContext";

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    WebGL2RenderingContext(napi_env env, napi_value exports) : NExporter(env, exports) {};

    explicit WebGL2RenderingContext() {};

    virtual ~WebGL2RenderingContext();
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT
