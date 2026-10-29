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

#include "RsEffectNode2_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "ui/rs_effect_node.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_CREATE = 0;
const uint8_t DO_CREATE_RENDER_NODE = 1;
const uint8_t DO_SET_FREEZE = 2;
const uint8_t DO_REGISTER_NODE_MAP = 3;
const uint8_t DO_TARGET_SIZE = 4;

void FlushCommands()
{
    auto proxy = RSTransactionProxy::GetInstance();
    if (proxy != nullptr) {
        proxy->FlushImplicitTransaction();
    }
}

void DoCreate(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSEffectNode::Create(isRenderServiceNode, isTextureExportNode, nullptr);
    (void)node;
    FlushCommands();
}

void DoCreateRenderNode(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSEffectNode::Create(isRenderServiceNode, isTextureExportNode, nullptr);
    if (node) {
        node->CreateRenderNode();
        FlushCommands();
    }
}

void DoSetFreeze(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSEffectNode::Create(isRenderServiceNode, isTextureExportNode, nullptr);
    if (node) {
        bool isFreeze = fdp.ConsumeBool();
        bool isMarkedByUI = fdp.ConsumeBool();
        node->SetFreeze(isFreeze, isMarkedByUI);
        FlushCommands();
    }
}

void DoRegisterNodeMap(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // RegisterNodeMap requires RSUIContext which is set up in Create with non-null context
}
} // namespace
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>();
    switch (choice % OHOS::Rosen::DO_TARGET_SIZE) {
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(fdp);
            break;
        case OHOS::Rosen::DO_CREATE_RENDER_NODE:
            OHOS::Rosen::DoCreateRenderNode(fdp);
            break;
        case OHOS::Rosen::DO_SET_FREEZE:
            OHOS::Rosen::DoSetFreeze(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_NODE_MAP:
            OHOS::Rosen::DoRegisterNodeMap(fdp);
            break;
        default:
            break;
    }
    return 0;
}
