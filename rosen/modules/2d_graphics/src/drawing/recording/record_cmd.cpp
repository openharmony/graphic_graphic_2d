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

#include "recording/record_cmd.h"
#include "recording/draw_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

RecordCmd::RecordCmd(const std::shared_ptr<DrawCmdList>& cmdList, const Rect& bounds)
    : drawCmdList_(cmdList), cullRect_(bounds) { }

void RecordCmd::Playback(Canvas* canvas)
{
    if (drawCmdList_ == nullptr || canvas == nullptr) {
        LOGE("RecordCmd::Playback failed, drawCmdList_ or canvas is nullptr");
        return;
    }
    drawCmdList_->Playback(*canvas);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
