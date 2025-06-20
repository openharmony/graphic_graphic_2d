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

#include "ipc_callbacks/rs_transaction_callback.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSDefaultTransactionCallback::RSDefaultTransactionCallback(TransactionCallbackFuncs funcs)
    :onCompletedCallback_(funcs.onCompleted),
    goBackgroundCallback_(funcs.goBackground),
    onDropBuffersCallback_(funcs.onDropBuffers),
    setDefaultWidthAndHeightCallback_(funcs.setDefaultWidthAndHeight),
    onSurfaceDumpCallback_(funcs.onSurfaceDump)
{}

GSError RSDefaultTransactionCallback::OnCompleted(const OnCompletedRet& ret)
{
    if (onCompletedCallback_ == nullptr) {
        ROSEN_LOGE("RSDefaultTransactionCallback::OnCompleted onCompletedCallback_ is nullptr");
        return GSERROR_INTERNAL;
    }
    return std::invoke(onCompletedCallback_, ret);
}

GSError RSDefaultTransactionCallback::GoBackground(uint64_t queueId)
{
    if (goBackgroundCallback_ == nullptr) {
        ROSEN_LOGE("RSDefaultTransactionCallback::GoBackground goBackgroundCallback_ is nullptr");
        return GSERROR_INTERNAL;
    }
    return std::invoke(goBackgroundCallback_, queueId);
}

GSError RSDefaultTransactionCallback::OnDropBuffers(const OnDropBuffersRet& ret)
{
    if (onDropBuffersCallback_ == nullptr) {
        ROSEN_LOGE("RSDefaultTransactionCallback::OnDropBuffers onDropBuffersCallback_ is nullptr");
        return GSERROR_INTERNAL;
    }
    return std::invoke(onDropBuffersCallback_, ret);
}

GSError RSDefaultTransactionCallback::SetDefaultWidthAndHeight(const OnSetDefaultWidthAndHeightRet& ret)
{
    if (setDefaultWidthAndHeightCallback_ == nullptr) {
        ROSEN_LOGE("RSDefaultTransactionCallback::SetDefaultWidthAndHeight setDefaultWidthAndHeightCallback_ is null");
        return GSERROR_INTERNAL;
    }
    return std::invoke(setDefaultWidthAndHeightCallback_, ret);
}

GSError RSDefaultTransactionCallback::OnSurfaceDump(OnSurfaceDumpRet& ret)
{
    if (onSurfaceDumpCallback_ == nullptr) {
        ROSEN_LOGE("RSDefaultTransactionCallback::OnSurfaceDump onSurfaceDumpCallback_ is nullptr");
        return GSERROR_INTERNAL;
    }
    return std::invoke(onSurfaceDumpCallback_, ret);
}
sptr<IRemoteObject> RSDefaultTransactionCallback::AsObject()
{
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS