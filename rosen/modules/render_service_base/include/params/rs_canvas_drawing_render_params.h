/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_CANVAS_DRAWING_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_CANVAS_DRAWING_RENDER_PARAMS_H

#include "params/rs_render_params.h"

namespace OHOS::Rosen {
class RSCanvasDrawingRenderParams : public RSRenderParams {
public:
    explicit RSCanvasDrawingRenderParams(NodeId id);
    ~RSCanvasDrawingRenderParams() override = default;
    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    bool IsNeedProcess() const override
    {
        return isNeedProcess_;
    }
    void SetNeedProcess(bool isNeedProcess)
    {
        isNeedProcess_ = isNeedProcess;
        needSync_ = true;
    }
private:
    std::atomic<bool> isNeedProcess_ = false;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_CANVAS_DRAWING_RENDER_PARAMS_H
