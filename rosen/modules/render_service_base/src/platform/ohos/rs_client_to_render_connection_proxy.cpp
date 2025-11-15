/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_client_to_render_connection_proxy.h"

#include <algorithm>
#include <cstdint>
#include <message_option.h>
#include <message_parcel.h>
#include <vector>
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_hrp_service.h"
#include "transaction/rs_marshalling_helper.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr size_t ASHMEM_SIZE_THRESHOLD = 200 * 1024; // cannot > 500K in TF_ASYNC mode
static constexpr int MAX_RETRY_COUNT = 20;
static constexpr int RETRY_WAIT_TIME_US = 1000; // wait 1ms before retry SendRequest
static constexpr int MAX_SECURITY_EXEMPTION_LIST_NUMBER = 1024; // securityExemptionList size not exceed 1024
static constexpr uint32_t EDID_DATA_MAX_SIZE = 64 * 1024;
static constexpr int MAX_VOTER_SIZE = 100; // SetWindowExpectedRefreshRate map size not exceed 100
static constexpr int ZERO = 0; // empty map size
}

RSClientToRenderConnectionProxy::RSClientToRenderConnectionProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIClientToRenderConnection>(impl)
{
}


ErrCode RSClientToRenderConnectionProxy::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    return ERR_OK;
}

int32_t RSClientToRenderConnectionProxy::SendRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (!Remote()) {
        return static_cast<int32_t>(RSInterfaceErrorCode::NULLPTR_ERROR);
    }
    return Remote()->SendRequest(code, data, reply, option);
}

ErrCode RSClientToRenderConnectionProxy::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteInterfaceToken err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(info.pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteInt32 pid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(info.uid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteInt32 uid err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteString bundleName err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(info.abilityName)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteString abilityName err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(info.focusNodeId)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: WriteUint64 focusNodeId err.");
        repCode = WRITE_PARCEL_ERR;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetFocusAppInfo: Send Request err.");
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    repCode = reply.ReadInt32();
    return ERR_OK;
}


void RSClientToRenderConnectionProxy::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect, RSSurfaceCapturePermissions /* permissions */)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s write callback failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureBlurParam(blurParam, data)) {
        ROSEN_LOGE("%{public}s write blurParam failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureAreaRect(specifiedAreaRect, data)) {
        ROSEN_LOGE("%{public}s write specifiedAreaRect failed", __func__);
        return;
    }
    
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
RSClientToRenderConnectionProxy::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig, RSSurfaceCapturePermissions)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return pixelMapIdPairVector;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return pixelMapIdPairVector;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_SOLO);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return pixelMapIdPairVector;
    }
    if (!RSMarshallingHelper::Unmarshalling(reply, pixelMapIdPairVector)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::TakeSurfaceCaptureSoloNode Unmarshalling failed");
    }
    return pixelMapIdPairVector;
}


void RSClientToRenderConnectionProxy::TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s write callback failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}


ErrCode RSClientToRenderConnectionProxy::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isFreeze)) {
        ROSEN_LOGE("%{public}s write isFreeze failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (isFreeze) {
        if (callback == nullptr) {
            ROSEN_LOGE("%{public}s callback == nullptr", __func__);
            return ERR_INVALID_VALUE;
        }
        if (!data.WriteRemoteObject(callback->AsObject())) {
            ROSEN_LOGE("%{public}s write callback failed", __func__);
            return ERR_INVALID_VALUE;
        }
        if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
            ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
            return ERR_INVALID_VALUE;
        }
        if (!WriteSurfaceCaptureBlurParam(blurParam, data)) {
            ROSEN_LOGE("%{public}s write blurParam failed", __func__);
            return ERR_INVALID_VALUE;
        }
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::TakeSurfaceCaptureWithAllWindows(NodeId id,
    sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock, RSSurfaceCapturePermissions /*permissions*/)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s GetDescriptor err", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(checkDrmAndSurfaceLock)) {
        ROSEN_LOGE("%{public}s write checkDrmAndSurfaceLock failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s write callback failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::FreezeScreen(NodeId id, bool isFreeze)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s GetDescriptor err", __func__);
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isFreeze)) {
        ROSEN_LOGE("%{public}s write isFreeze failed", __func__);
        return ERR_INVALID_VALUE;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FREEZE_SCREEN);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToRenderConnectionProxy::TakeUICaptureInRange(
    NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr", __func__);
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s write id failed", __func__);
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s write callback failed", __func__);
        return;
    }
    if (!WriteSurfaceCaptureConfig(captureConfig, data)) {
        ROSEN_LOGE("%{public}s write captureConfig failed", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s SendRequest() error[%{public}d]", __func__, err);
        return;
    }
}

bool RSClientToRenderConnectionProxy::WriteSurfaceCaptureConfig(
    const RSSurfaceCaptureConfig& captureConfig, MessageParcel& data)
{
    if (!data.WriteFloat(captureConfig.scaleX) || !data.WriteFloat(captureConfig.scaleY) ||
        !data.WriteBool(captureConfig.useDma) || !data.WriteBool(captureConfig.useCurWindow) ||
        !data.WriteUint8(static_cast<uint8_t>(captureConfig.captureType)) || !data.WriteBool(captureConfig.isSync) ||
        !data.WriteBool(captureConfig.isHdrCapture) ||
        !data.WriteBool(captureConfig.needF16WindowCaptureForScRGB) ||
        !data.WriteFloat(captureConfig.mainScreenRect.left_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.top_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.right_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.bottom_) ||
        !data.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId) ||
        !data.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.left_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.top_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.right_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.bottom_) ||
        !data.WriteUInt64Vector(captureConfig.blackList) ||
        !data.WriteUint32(captureConfig.backGroundColor)) {
        ROSEN_LOGE("WriteSurfaceCaptureConfig: WriteSurfaceCaptureConfig captureConfig err.");
        return false;
    }
    return true;
}

bool RSClientToRenderConnectionProxy::WriteSurfaceCaptureBlurParam(
    const RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data)
{
    if (!data.WriteBool(blurParam.isNeedBlur) || !data.WriteFloat(blurParam.blurRadius)) {
        ROSEN_LOGE("WriteSurfaceCaptureBlurParam: WriteBool OR WriteFloat [blurParam] err.");
        return false;
    }
    return true;
}

bool RSClientToRenderConnectionProxy::WriteSurfaceCaptureAreaRect(
    const Drawing::Rect& specifiedAreaRect, MessageParcel& data)
{
    if (!data.WriteFloat(specifiedAreaRect.left_) || !data.WriteFloat(specifiedAreaRect.top_) ||
        !data.WriteFloat(specifiedAreaRect.right_) || !data.WriteFloat(specifiedAreaRect.bottom_)) {
        ROSEN_LOGE("WriteSurfaceCaptureAreaRect: WriteFloat specifiedAreaRect err.");
        return false;
    }
    return true;
}

ErrCode RSClientToRenderConnectionProxy::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(rsNodeId)) {
        ROSEN_LOGE("SetHwcNodeBounds write id failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteFloat(positionX) || !data.WriteFloat(positionY) || !data.WriteFloat(positionZ) ||
        !data.WriteFloat(positionW)) {
        ROSEN_LOGE("SetHwcNodeBounds write bound failed");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetHwcNodeBounds SendRequest() error[%{public}d]", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus, int32_t& resCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus WriteUint64 id err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus SendRequest error(%{public}d)", err);
        return RS_CONNECTION_ERROR;
    }
    if (!reply.ReadInt32(resCode)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus Read resCode failed");
        return READ_PARCEL_ERR;
    }
    if (resCode == SUCCESS) {
        uint32_t readHdrStatus{0};
        if (!reply.ReadUint32(readHdrStatus)) {
            ROSEN_LOGE("RSClientToRenderConnectionProxy::GetScreenHDRStatus Read HDR status failed");
            return READ_PARCEL_ERR;
        }
        hdrStatus = static_cast<HdrStatus>(readHdrStatus);
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::DropFrameByPid(const std::vector<int32_t> pidList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("DropFrameByPid: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    if (pidList.size() > MAX_DROP_FRAME_PID_LIST_SIZE || !data.WriteInt32Vector(pidList)) {
        ROSEN_LOGE("DropFrameByPid: WriteInt32Vector pidList err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::DropFrameByPid: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetAncoForceDoDirect(bool direct, bool& res)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SetAncoForceDoDirect: WriteInterfaceToken GetDescriptor err.");
        res = false;
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (data.WriteBool(direct)) {
        uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            res = false;
            return ERR_INVALID_VALUE;
        }
        res = reply.ReadBool();
        return ERR_OK;
    } else {
        ROSEN_LOGE("SetAncoForceDoDirect: WriteBool direct err.");
        res = false;
        return ERR_INVALID_VALUE;
    }
}

ErrCode RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, sptr<RSISurfaceBufferCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback callback == nullptr");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write Int32 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: write RemoteObject val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterSurfaceBufferCallback: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteInt32(pid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: write Int32 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(uid)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::UnregisterSurfaceBufferCallback: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSClientToRenderConnectionProxy::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetLayerTopForHWC: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (data.WriteUint64(nodeId) && data.WriteBool(isTop) && data.WriteUint32(zOrder)) {
        uint32_t code =
            static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER);
        int32_t err = SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSClientToRenderConnectionProxy::SetLayerTopForHWC: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

void RSClientToRenderConnectionProxy::RegisterTransactionDataCallback(uint64_t token,
    uint64_t timeStamp, sptr<RSITransactionDataCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback callback == nullptr.");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    static_assert(std::is_same_v<int32_t, pid_t>, "pid_t is not int32_t on this platform.");
    if (!data.WriteUint64(token)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write multi token val err.");
        return;
    }
    if (!data.WriteUint64(timeStamp)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write timeStamp val err.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: write Callback val err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    RS_LOGD("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterTransactionDataCallback: Send Request err.");
        return;
    }
}

ErrCode RSClientToRenderConnectionProxy::SetWindowContainer(NodeId nodeId, bool value)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: write token err.");
        return ERR_INVALID_VALUE;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(nodeId)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: write Uint64 val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(value)) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: write Bool val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetWindowContainer: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSClientToRenderConnectionProxy::ClearUifirstCache(NodeId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearUifirstCache: write token err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("ClearUifirstCache: WriteUint64 id err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_UIFIRST_CACHE);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::ClearUifirstCache sendrequest error : %{public}d", err);
        return;
    }
}

void RSClientToRenderConnectionProxy::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);

    if (!data.WriteUint64(id) || !data.WriteInt32(gravity)) {
        ROSEN_LOGE("%{public}s: write error.", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_FRAME_GRAVITY);
    int32_t err = SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
    }
}
} // namespace Rosen
} // namespace OHOS
