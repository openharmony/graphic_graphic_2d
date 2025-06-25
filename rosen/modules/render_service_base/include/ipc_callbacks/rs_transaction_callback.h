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

#ifndef ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_H

#include <vector>
#include <iremote_broker.h>

#include "common/rs_common_def.h"
#include "graphic_common.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {

struct OnCompletedRet {
    uint64_t queueId;
    sptr<SyncFence> releaseFence;
    uint32_t bufferSeqNum;
    bool isActiveGame;
};

struct OnDropBuffersRet {
    uint64_t queueId;
    std::vector<uint32_t> bufferSeqNums;
};

struct OnSurfaceDumpRet {
    uint64_t queueId;
    std::string dumpString;
};

struct OnSetDefaultWidthAndHeightRet {
    uint64_t queueId;
    int32_t width;
    int32_t height;
};

class RSITransactionCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.TransactionCallback");

    RSITransactionCallback() = default;
    virtual ~RSITransactionCallback() noexcept = default;

    virtual GSError OnCompleted(const OnCompletedRet& ret) = 0;
    virtual GSError GoBackground(uint64_t queueId) = 0;
    virtual GSError OnDropBuffers(const OnDropBuffersRet& ret) = 0;
    virtual GSError SetDefaultWidthAndHeight(const OnSetDefaultWidthAndHeightRet& ret) = 0;
    virtual GSError OnSurfaceDump(OnSurfaceDumpRet& ret) = 0;
};

using OnCompletedCallback = std::function<GSError(const OnCompletedRet& ret)>;
using GoBackgroundCallback = std::function<GSError(uint64_t queueId)>;
using OnDropBuffersCallback = std::function<GSError(const OnDropBuffersRet& ret)>;
using SetDefaultWidthAndHeightCallback = std::function<GSError(const OnSetDefaultWidthAndHeightRet& ret)>;
using OnSurfaceDumpCallback = std::function<GSError(const OnSurfaceDumpRet& ret)>;

struct TransactionCallbackFuncs {
    OnCompletedCallback onCompleted;
    GoBackgroundCallback goBackground;
    OnDropBuffersCallback onDropBuffers;
    SetDefaultWidthAndHeightCallback setDefaultWidthAndHeight;
    OnSurfaceDumpCallback onSurfaceDump;
};

class RSB_EXPORT RSDefaultTransactionCallback : public RSITransactionCallback {
public:
    RSDefaultTransactionCallback(TransactionCallbackFuncs funcs);
    ~RSDefaultTransactionCallback() noexcept override = default;

    GSError OnCompleted(const OnCompletedRet& ret) override;
    GSError GoBackground(uint64_t queueId) override;
    GSError OnDropBuffers(const OnDropBuffersRet& ret) override;
    GSError SetDefaultWidthAndHeight(const OnSetDefaultWidthAndHeightRet& ret) override;
    GSError OnSurfaceDump(OnSurfaceDumpRet& ret) override;
    sptr<IRemoteObject> AsObject() override;
private:
    std::function<GSError(const OnCompletedRet& ret)> onCompletedCallback_;
    std::function<GSError(uint64_t queueId)> goBackgroundCallback_;
    std::function<GSError(const OnDropBuffersRet& ret)> onDropBuffersCallback_;
    std::function<GSError(const OnSetDefaultWidthAndHeightRet& ret)> setDefaultWidthAndHeightCallback_;
    std::function<GSError(const OnSurfaceDumpRet& ret)> onSurfaceDumpCallback_;
};
} // namespace Rosen
} // namesapce OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_H