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
#include "ui/rs_ext_node_operation.h"

namespace OHOS {
namespace Rosen {
RSExtNodeOperation& RSExtNodeOperation::GetInstance()
{
    static RSExtNodeOperation singleton;
    return singleton;
}

bool RSExtNodeOperation::CheckNeedToProcess(std::string xcomponentId)
{
    (void)xcomponentId;
    return false;
}

void RSExtNodeOperation::ProcessRSExtNode(std::string xcomponentId,
    uint64_t nodeId, float x, float y, std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    (void)xcomponentId;
    (void)nodeId;
    (void)x;
    (void)y;
    (void)surfaceNode;
}
}
}