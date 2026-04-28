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

#include "rseffectnode_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "ui/rs_effect_node.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_CREATE = 0;
const uint8_t DO_SET_FREEZE = 1;
const uint8_t TARGET_SIZE = 2;
}

void DoCreate(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSEffectNode::SharedPtr effectNode = RSEffectNode::Create(isRenderServiceNode);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
}

void DoSetFreeze(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSEffectNode::SharedPtr effectNode = RSEffectNode::Create(isRenderServiceNode);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    bool isFreeze = fdp.ConsumeBool();
    effectNode->SetFreeze(isFreeze);
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(fdp);
            break;
        case OHOS::Rosen::DO_SET_FREEZE:
            OHOS::Rosen::DoSetFreeze(fdp);
            break;
        default:
            break;
    }
    return 0;
}
