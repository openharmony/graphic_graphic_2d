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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_RESULT_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_RESULT_H

#include <functional>

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "pipeline/rs_draw_cmd_list.h"

namespace OHOS {
namespace Rosen {
class RSRenderResult final {
public:
    RSRenderResult(const RectI& r, std::shared_ptr<Drawing::DrawCmdList> dcl) : dirty_(r), drawCmdList_(dcl) {}
    ~RSRenderResult() = default;

    RectI GetDirtyRegion() const
    {
        return dirty_;
    }
    std::shared_ptr<Drawing::DrawCmdList> GetDrawCmdList() const
    {
        return drawCmdList_;
    }

private:
    RectI dirty_;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList_;
};

using RenderFinishedCallback = std::function<void(const RSRenderResult&)>;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_RESULT_H