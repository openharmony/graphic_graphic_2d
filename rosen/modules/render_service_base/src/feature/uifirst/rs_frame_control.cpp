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

#include "feature/uifirst/rs_frame_control.h"

namespace OHOS {
namespace Rosen {

RSFrameControlTool& RSFrameControlTool::Instance()
{
    static RSFrameControlTool instance;
    return instance;
}

void RSFrameControlTool::InsertNodeIdToAppWindowSet(NodeId id)
{
    refreshAppWindowSet_.insert(id);
}

bool RSFrameControlTool::SerchNodeIdFromAppWindowsSet(NodeId id)
{
    if (refreshAppWindowSet_.count(id) > 0) {
        refreshAppWindowSet_.clear();
        return true;
    }
    return false;
}

}
}