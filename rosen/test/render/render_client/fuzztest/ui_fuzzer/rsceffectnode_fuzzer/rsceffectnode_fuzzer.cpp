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

#include "rsceffectnode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_effect_node.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSEffectNode::SharedPtr effectNode = RSEffectNode::Create(isRenderServiceNode);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    return true;
}

bool DoSetFreeze(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSEffectNode::SharedPtr effectNode = RSEffectNode::Create(isRenderServiceNode);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    bool isFreeze = GetData<bool>();
    effectNode->SetFreeze(isFreeze);
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoSetFreeze(data, size);
    return 0;
}

