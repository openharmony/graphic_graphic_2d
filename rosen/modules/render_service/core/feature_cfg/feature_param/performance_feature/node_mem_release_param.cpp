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

#include "node_mem_release_param.h"

namespace OHOS::Rosen {

bool NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled()
{
    return isCanvasDrawingNodeDMAMemEnabled_;
}

bool NodeMemReleaseParam::IsRsRenderNodeGCMemReleaseEnabled()
{
    return isRsRenderNodeGCMemReleaseEnabled_;
}


void NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(bool isEnable)
{
    isCanvasDrawingNodeDMAMemEnabled_ = isEnable;
}

void NodeMemReleaseParam::SetRsRenderNodeGCMemReleaseEnabled(bool isEnable)
{
    isRsRenderNodeGCMemReleaseEnabled_ = isEnable;
}

bool NodeMemReleaseParam::IsCanvasDrawingNodeBufferEnabled()
{
    return isCanvasDrawingNodeBufferEnabled_;
}
 
void NodeMemReleaseParam::SetCanvasDrawingNodeBufferEnabled(bool isEnable)
{
    isCanvasDrawingNodeBufferEnabled_ = isEnable;
}
 
void NodeMemReleaseParam::AddToCanvasBufferBlacklist(std::string processName, std::string value)
{
    if (canvasBufferBlacklist_ == nullptr) {
        canvasBufferBlacklist_ = std::make_shared<std::map<std::string, std::string>>();
    }
    canvasBufferBlacklist_->emplace(processName, value);
}
 
bool NodeMemReleaseParam::IsCanvasBufferEnabled(const std::string& processName)
{
    if (canvasBufferBlacklist_ == nullptr || canvasBufferBlacklist_->empty()) {
        return true;
    }
    auto it = canvasBufferBlacklist_->find(processName);
    if (it == canvasBufferBlacklist_->end()) {
        return true;
    }
    return it->second != "1";
}
} // namespace OHOS::Rosen