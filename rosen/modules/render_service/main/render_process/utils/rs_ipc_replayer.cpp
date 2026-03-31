/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_ipc_replayer.h"

#include "rs_render_pipeline_agent.h"

#undef LOG_TAG
#define LOG_TAG "RSIpcReplayer"

namespace OHOS {
namespace Rosen {
void RSIpcReplayer::ReplayIpcData(
    const sptr<RSRenderPipelineAgent>& renderPipelineAgent, const std::shared_ptr<IpcReplayTypeToDataMap>& replayData)
{
    for (const auto& [type, dataVec] : *replayData) {
        for (const auto& data : dataVec) {
            switch (type) {
                case RSIpcReplayType::SET_WATERMARK: {
                    auto setWatermarkData = std::static_pointer_cast<SetWatermarkReplayData>(data);
                    if (!setWatermarkData) {
                        RS_LOGE("%{public}s: replay SetWatermark failed, type:%{public}u, pid:%{public}d", __func__,
                            type, data->GetPid());
                        return;
                    }
                    renderPipelineAgent->SetWatermark(setWatermarkData->pid_, setWatermarkData->name_,
                        setWatermarkData->watermark_, setWatermarkData->success_);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}
} // namespace Rosen
} // namespace OHOS
