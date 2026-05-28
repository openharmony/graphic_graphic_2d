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

#include "rscanvasnodecommand_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "command/rs_canvas_node_command.h"

namespace OHOS {
namespace Rosen {
bool DoCanvasNodeCommand(FuzzedDataProvider& fdp)
{
    RSContext context;
    NodeId id = fdp.ConsumeIntegral<NodeId>();
    bool isTextureExportNode = fdp.ConsumeBool();
    bool hdrPresent = fdp.ConsumeBool();
    uint32_t colorGamut = fdp.ConsumeIntegral<uint32_t>();
    uint16_t modifierType = fdp.ConsumeIntegral<uint16_t>();

    RSCanvasNodeCommandHelper::Create(context, id, isTextureExportNode);
    RSCanvasNodeCommandHelper::SetHDRPresent(context, id, hdrPresent);
    RSCanvasNodeCommandHelper::SetColorGamut(context, id, colorGamut);
    RSCanvasNodeCommandHelper::ClearRecording(context, id);
    Drawing::DrawCmdListPtr drawCmdList = nullptr;
    RSCanvasNodeCommandHelper::UpdateRecording(context, id, drawCmdList, modifierType);
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::DoCanvasNodeCommand(fdp);
    return 0;
}
