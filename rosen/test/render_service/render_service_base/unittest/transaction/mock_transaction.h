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

#ifndef TRANSACTION_MOCK__H
#define TRANSACTION_MOCK__H

#include <gmock/gmock.h>

#include "transaction/transaction.h"

namespace OHOS {
namespace Rosen {
namespace Mock {
class MockCallback : public RSITransactionCallback {
public:
    MockCallback() {};
    ~MockCallback() override {};
    MOCK_METHOD(GSError, GoBackground, (uint64_t queueId), (override));
    MOCK_METHOD(GSError, OnCompleted, (const OnCompletedRet& ret), (override));
    MOCK_METHOD(GSError, OnDropBuffers, (const OnDropBuffersRet& ret), (override));
    MOCK_METHOD(GSError, SetDefaultWidthAndHeight, (const OnSetDefaultWidthAndHeightRet& ret), (override));
    MOCK_METHOD(GSError, OnSurfaceDump, (OnSurfaceDumpRet& ret), (override));
    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

// Mock listener，用于捕获事件
class MockListener : public IBufferConsumerListener {
public:
    MockListener() {};
    ~MockListener() override {};
    MOCK_METHOD(void, OnBufferAvailable, (), (override));
    MOCK_METHOD(void, OnTunnelHandleChange, (), (override));
    MOCK_METHOD(void, OnGoBackground, (), ());
    MOCK_METHOD(void, OnCleanCache, (uint32_t *bufSeqNum), ());
    MOCK_METHOD(void, OnTransformChange, (), ());
};
}
} // Rosen
} // OHOS
#endif