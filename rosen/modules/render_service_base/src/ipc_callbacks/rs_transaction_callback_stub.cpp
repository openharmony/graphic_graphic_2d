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

#include "rs_transaction_callback_stub.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSTransactionCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSITransactionCallback::GetDescriptor()) {
        ROSEN_LOGE("int RSTransactionCallbackStub: token error");
        return ERR_INVALID_STATE;
    }
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_COMPLETED): {
            OnCompletedRet onCompletedRet;
            bool readRet = data.ReadUint64(onCompletedRet.queueId);
            readRet = readRet && data.ReadUint32(onCompletedRet.bufferSeqNum);
            onCompletedRet.releaseFence = SyncFence::ReadFromMessageParcel(data);
            onCompletedRet.isActiveGame = data.ReadBool();
            readRet = readRet && (onCompletedRet.releaseFence != nullptr);
            if (!readRet) {
                RS_LOGE("RSTransactionCallbackStub::ON_COMPLETED read remote data failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = OnCompleted(onCompletedRet);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSTransactionCallbackStub::ON_COMPLETED Write reply failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::GO_BACKGROUND): {
            uint64_t queueId;
            bool readRet = data.ReadUint64(queueId);
            if (!readRet) {
                RS_LOGE("RSTransactionCallbackStub::GO_BACKGROUND read remote data failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = GoBackground(queueId);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSTransactionCallbackStub::GO_BACKGROUND Write reply failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_DROPBUFFERS): {
            OnDropBuffersRet onDropBuffersRet;
            bool readRet = data.ReadUint64(onDropBuffersRet.queueId);
            uint32_t num = 0;
            readRet = readRet && data.ReadUint32(num);
            if (!readRet) {
                RS_LOGE("RSTransactionCallbackStub::ON_DROPBUFFERS read remote data failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            for (uint32_t i = 0; i < num; ++i) {
                uint32_t bufferSequence;
                if (!data.ReadUint32(bufferSequence)) {
                    RS_LOGE("RSTransactionCallbackStub::ON_DROPBUFFERS read buffer sequence failed!");
                    ret = ERR_INVALID_DATA;
                    break;
                }
                onDropBuffersRet.bufferSeqNums.push_back(bufferSequence);
            }
            int32_t status = OnDropBuffers(onDropBuffersRet);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSTransactionCallbackStub::ON_DROPBUFFERS Write reply failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::SET_DEFAULT_WIDTH_AND_HEIGHT): {
            OnSetDefaultWidthAndHeightRet onSetDefaultWidthAndHeightRet;
            if (!data.ReadUint64(onSetDefaultWidthAndHeightRet.queueId) ||
                !data.ReadInt32(onSetDefaultWidthAndHeightRet.width) ||
                !data.ReadInt32(onSetDefaultWidthAndHeightRet.height)) {
                RS_LOGE("RSTransactionCallbackStub::SET_DEFAULT_WIDTH_AND_HEIGHT read remote data failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetDefaultWidthAndHeight(onSetDefaultWidthAndHeightRet);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSTransactionCallbackStub::SET_DEFAULT_WIDTH_AND_HEIGHT Write reply failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_SURFACE_DUMP): {
            OnSurfaceDumpRet onSurfaceDumpRet;
            if (!data.ReadUint64(onSurfaceDumpRet.queueId)) {
                RS_LOGE("RSTransactionCallbackStub::ON_SURFACE_DUMP read remote data failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = OnSurfaceDump(onSurfaceDumpRet);
            if (!reply.WriteInt32(status) || !reply.WriteString(onSurfaceDumpRet.dumpString)) {
                RS_LOGE("RSTransactionCallbackStub::ON_SURFACE_DUMP Write reply failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        default: {
            RS_LOGE("RSTransactionCallbackStub: unhandled enumeration value");
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}
} // namespace Rosen
} // namespace OHOS
    