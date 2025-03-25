/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "customized/random_draw_cmd_list.h"

#include "customized/random_drawop.h"
#include "random/random_engine.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<Drawing::DrawCmdList> RandomDrawCmdList::GetRandomDrawCmdList()
{
    uint32_t length = RandomEngine::GetRandomMiddleVectorLength();
    static std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    for (int i = 0; i < length; i++) {
        std::shared_ptr<Drawing::DrawOpItem> op = Drawing::RandomDrawOp::GetRandomDrawOp();
        drawCmdList->AddDrawOp(std::move(op));
    }
    return drawCmdList;
}
} // namespace Rosen
} // namespace OHOS
