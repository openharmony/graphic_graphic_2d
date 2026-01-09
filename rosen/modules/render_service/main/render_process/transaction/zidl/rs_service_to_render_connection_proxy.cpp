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

#include "rs_service_to_render_connection_proxy.h"

#include <algorithm>
#include <cstdint>
#include <message_option.h>
#include <message_parcel.h>
#include <vector>
#include "buffer_utils.h"
#include "rs_trace.h"

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_marshalling_helper.h"

#undef LOG_TAG
#define LOG_TAG "RSServiceToRenderConnectionProxy"

namespace OHOS {
namespace Rosen {
RSServiceToRenderConnectionProxy::RSServiceToRenderConnectionProxy(const sptr<IRemoteObject>& impl) :
    IRemoteProxy<RSIServiceToRenderConnection>(impl) {}

ErrCode RSServiceToRenderConnectionProxy::SetForceRefresh(const std::string& nodeIdStr, bool isForceRefresh)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetForceRefresh: write token err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(nodeIdStr)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetForceRefresh: write nodeIdStr val err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(isForceRefresh)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetForceRefresh: write isForceRefresh val err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_FORCE_REFRESH);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetForceRefresh: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::CreatePixelMapFromSurface(sptr<Surface> surface, const Rect& srcRect,
    std::shared_ptr<Media::PixelMap>& pixelMap)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (surface == nullptr) {
        return ERR_INVALID_VALUE;
    }

    auto producer = surface->GetProducer();
    if (producer == nullptr) {
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteInterfaceToken RSIRenderServiceConnection::GetDescriptor() err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(producer->AsObject())) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteRemoteObject producer->AsObject() err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(srcRect.x) || !data.WriteInt32(srcRect.y) ||
        !data.WriteInt32(srcRect.w) || !data.WriteInt32(srcRect.h)) {
        ROSEN_LOGE("CreatePixelMapFromSurface: WriteInt32 srcRect err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::CreatePixelMapFromSurface: Send Request err.");
        return ERR_INVALID_VALUE;
    }

    if (reply.ReadBool()) {
        pixelMap.reset(Media::PixelMap::Unmarshalling(reply));
    } else {
        ROSEN_LOGE("CreatePixelMapFromSurface: ReadBool err.");
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::SetLayerTop(const std::string& nodeIdStr, bool isTop)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSClientToServiceConnectionProxy::SetLayerTop: write token err.");
        return ERR_INVALID_VALUE;
    }
    if (data.WriteString(nodeIdStr) && data.WriteBool(isTop)) {
        uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_LAYER_TOP);
        int32_t err = Remote()->SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            ROSEN_LOGE("RSClientToServiceConnectionProxy::SetLayerTop: Send Request err.");
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("GetTotalAppMemSize: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadFloat(cpuMemSize) || !reply.ReadFloat(gpuMemSize)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::GetTotalAppMemSize Read MemSize failed");
        return READ_PARCEL_ERR;
    }
    return ERR_OK;
}

void RSServiceToRenderConnectionProxy::ReportDataBaseRs(
    MessageParcel& data, MessageParcel& reply, MessageOption& option, DataBaseRs info)
{
    if (!data.WriteInt32(info.appPid)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt32 info.appPid err.");
        return;
    }
    if (!data.WriteInt32(info.eventType)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt32 info.eventType err.");
        return;
    }
    if (!data.WriteInt32(info.versionCode)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt32 info.versionCode err.");
        return;
    }
    if (!data.WriteInt64(info.uniqueId)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt64 info.uniqueId err.");
        return;
    }
    if (!data.WriteInt64(info.inputTime)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt64 info.inputTime err.");
        return;
    }
    if (!data.WriteInt64(info.beginVsyncTime) || !data.WriteInt64(info.endVsyncTime)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteInt64 info.beginVsyncTime OR info.endVsyncTime err.");
        return;
    }
    if (!data.WriteBool(info.isDisplayAnimator)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteBool info.isDisplayAnimator err.");
        return;
    }
    if (!data.WriteString(info.sceneId)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.sceneId err.");
        return;
    }
    if (!data.WriteString(info.versionName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.versionName err.");
        return;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.bundleName err.");
        return;
    }
    if (!data.WriteString(info.processName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.processName err.");
        return;
    }
    if (!data.WriteString(info.abilityName)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.abilityName err.");
        return;
    }
    if (!data.WriteString(info.pageUrl)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.pageUrl err.");
        return;
    }
    if (!data.WriteString(info.sourceType)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.sourceType err.");
        return;
    }
    if (!data.WriteString(info.note)) {
        ROSEN_LOGE("ReportDataBaseRs: WriteString info.note err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
}

void RSServiceToRenderConnectionProxy::WriteAppInfo(
    MessageParcel& data, MessageParcel& reply, MessageOption& option, AppInfo info)
{
    if (!data.WriteInt64(info.startTime)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt64 info.startTime err.");
        return;
    }
    if (!data.WriteInt64(info.endTime)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt64 info.endTime err.");
        return;
    }
    if (!data.WriteInt32(info.pid)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt32 info.pid err.");
        return;
    }
    if (!data.WriteString(info.versionName)) {
        ROSEN_LOGE("WriteAppInfo: WriteString info.versionName err.");
        return;
    }
    if (!data.WriteInt32(info.versionCode)) {
        ROSEN_LOGE("WriteAppInfo: WriteInt32 info.versionCode err.");
        return;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("WriteAppInfo: WriteString info.bundleName err.");
        return;
    }
    if (!data.WriteString(info.processName)) {
        ROSEN_LOGE("WriteAppInfo: WriteString info.processName err.");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
}

ErrCode RSServiceToRenderConnectionProxy::SetDiscardJankFrames(bool discardJankFrames)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(discardJankFrames)) {
        ROSEN_LOGE("%{public}s: WriteBool discardJankFrames err.", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_DISCARD_JANK_FRAME);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::ReportJankStats()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportJankStats: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_JANK_STATS);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportJankStats: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::ReportEventResponse(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventResponse: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportEventResponse: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::ReportEventComplete(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventComplete: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportEventComplete: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::ReportEventJankFrame(DataBaseRs info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportEventJankFrame: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    ReportDataBaseRs(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportEventJankFrame: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::ReportRsSceneJankStart(AppInfo info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportRsSceneJankStart: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    WriteAppInfo(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportRsSceneJankStart: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSServiceToRenderConnectionProxy::RegisterUIExtensionCallback(pid_t pid, uint64_t userId,
    sptr<RSIUIExtensionCallback> callback, bool unobscured)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterUIExtensionCallback: callback is nullptr.");
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterUIExtensionCallback: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    if (data.WriteInt32(pid) && data.WriteUint64(userId) && data.WriteRemoteObject(callback->AsObject()) && data.WriteBool(unobscured)) {
        uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK);
        int32_t err = Remote()->SendRequest(code, data, reply, option);
        if (err != NO_ERROR) {
            return RS_CONNECTION_ERROR;
        }
        int32_t result{0};
        if (!reply.ReadInt32(result)) {
            ROSEN_LOGE("RSClientToRenderConnectionProxy::RegisterUIExtensionCallback Read result failed");
            return READ_PARCEL_ERR;
        }
        return result;
    } else {
        ROSEN_LOGE("RegisterUIExtensionCallback: WriteUint64[userId] OR WriteRemoteObject[callback] err.");
        return RS_CONNECTION_ERROR;
    }
}

ErrCode RSServiceToRenderConnectionProxy::ReportRsSceneJankEnd(AppInfo info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("ReportRsSceneJankEnd: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }
    WriteAppInfo(data, reply, option, info);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportRsSceneJankEnd: Send Request err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoStart: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUInt64Vector(uniqueIdList)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteUInt64Vector uniqueIdList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteStringVector(surfaceNameList)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteStringVector surfaceNameList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(fps)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteUint32 fps err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(reportTime)) {
        ROSEN_LOGE("AvcodecVideoStart: WriteUint64 reportTime err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_START);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::AvcodecVideoStart: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::AvcodecVideoStart Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSServiceToRenderConnectionProxy::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("AvcodecVideoStop: WriteInterfaceToken GetDescriptor err.");
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUInt64Vector(uniqueIdList)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteUInt64Vector uniqueIdList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteStringVector(surfaceNameList)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteStringVector surfaceNameList err.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(fps)) {
        ROSEN_LOGE("AvcodecVideoStop: WriteUint32 fps err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::AvcodecVideoStop: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::AvcodecVideoStop Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSServiceToRenderConnectionProxy::GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("GetMemoryGraphics: WriteInterfaceToken GetDescriptor err.");
        return ERR_INVALID_VALUE;
    }

    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHICS);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }

    uint64_t count{0};
    if (!reply.ReadUint64(count)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::GetMemoryGraphics Read count failed");
        return ERR_INVALID_VALUE;
    }
    size_t readableSize = reply.GetReadableBytes();
    size_t len = static_cast<size_t>(count);
    if (len > readableSize || len > memoryGraphics.max_size()) {
        RS_LOGE("RSServiceToRenderConnectionProxy GetMemoryGraphics Failed to read vector, size:%{public}zu,"
            " readableSize:%{public}zu", len, readableSize);
        return ERR_INVALID_VALUE;
    }
    memoryGraphics.resize(count);
    for (uint64_t index = 0; index < count; index++) {
        sptr<MemoryGraphic> item = reply.ReadParcelable<MemoryGraphic>();
        if (item == nullptr) {
            continue;
        } else {
            memoryGraphics[index] = *item;
        }
    }
    return ERR_OK;
}

int32_t RSServiceToRenderConnectionProxy::RegisterOcclusionChangeCallback(
    pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s: callback is nullptr.", __func__);
        return INVALID_ARGUMENTS;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s WriteInterfaceToken GetDescriptor err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteInt32(static_cast<int32_t>(pid))) {
        ROSEN_LOGE("%{public}s WriteInt32 failed.", __func__);
        return INVALID_ARGUMENTS;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s WriteRemoteObject callback->AsObject() err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
            RSIServiceToRenderConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    return SUCCESS;
}

int32_t RSServiceToRenderConnectionProxy::SetBrightnessInfoChangeCallback(pid_t pid,
    sptr<RSIBrightnessInfoChangeCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s WriteInterfaceToken GetDescriptor err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteInt32(static_cast<int32_t>(pid))) {
        ROSEN_LOGE("%{public}s WriteInt32 failed.", __func__);
        return INVALID_ARGUMENTS;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s WriteRemoteObject callback->AsObject() err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    return SUCCESS;
}

int32_t RSServiceToRenderConnectionProxy::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback is nullptr.", __func__);
        return INVALID_ARGUMENTS;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s WriteInterfaceToken GetDescriptor err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s WriteUint64 id err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("%{public}s WriteRemoteObject callback->AsObject() err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteFloatVector(partitionPoints)) {
        ROSEN_LOGE("%{public}s WriteFloatVector partitionPoints err.", __func__);
        return WRITE_PARCEL_ERR;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("%{public}s Read result failed", __func__);
        return READ_PARCEL_ERR;
    }
    return result;
}

int32_t RSServiceToRenderConnectionProxy::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s WriteInterfaceToken GetDescriptor err.", __func__);
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s WriteUint64 id err.", __func__);
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("%{public}s Read result failed", __func__);
        return READ_PARCEL_ERR;
    }
    return result;
}

ErrCode RSServiceToRenderConnectionProxy::GetPixelMapByProcessId(
    std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(pid)) {
        ROSEN_LOGE("%{public}s: WriteUint64 pid err.", __func__);
        repCode = INVALID_ARGUMENTS;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err", __func__);
        repCode = RS_CONNECTION_ERROR;
        return ERR_INVALID_VALUE;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("%{public}s Read result failed", __func__);
        return ERR_INVALID_VALUE;
    }
    if (result == SUCCESS) {
        pixelMapInfoVector.clear();
        if (!RSMarshallingHelper::Unmarshalling(reply, pixelMapInfoVector)) {
            ROSEN_LOGE("%{public}s: Unmarshalling failed", __func__);
        }
    } else {
        ROSEN_LOGE("%{public}s: Invalid reply", __func__);
    }
    return ERR_OK;
}

void RSServiceToRenderConnectionProxy::ShowWatermark(const std::shared_ptr<Media::PixelMap>& watermarkImg, bool isShow)
{
    if (watermarkImg == nullptr) {
        ROSEN_LOGE("%{public}s: watermarkImg is nullptr.", __func__);
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return;
    }
    if (!data.WriteParcelable(watermarkImg.get())) {
        ROSEN_LOGE("%{public}s: WriteParcelable watermarkImg.get() err.", __func__);
        return;
    }
    if (!data.WriteBool(isShow)) {
        ROSEN_LOGE("%{public}s: WriteBool isShow err.", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        return;
    }
}

ErrCode RSServiceToRenderConnectionProxy::GetSurfaceRootNodeId(NodeId& windowNodeId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint64(windowNodeId)) {
        ROSEN_LOGE("%{public}s: windowNodeId:%{public}" PRIu64 "err.", __func__, windowNodeId);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SURFACE_ROOT_NODE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadUint64(windowNodeId)) {
        ROSEN_LOGE("%{public}s: ReadWindowNodeId err.", __func__);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSServiceToRenderConnectionProxy::SetVmaCacheStatus(bool flag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(flag)) {
        ROSEN_LOGE("SetVmaCacheStatus: WriteBool flag err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSClientToRenderConnectionProxy::SetVmaCacheStatus %d: Send Request err.", flag);
        return;
    }
}

ErrCode RSServiceToRenderConnectionProxy::SetWatermark(pid_t callingPid, const std::string& name,
    std::shared_ptr<Media::PixelMap> watermark, bool& success)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(callingPid)) {
        ROSEN_LOGE("%{public}s: WriteUint64 pid err.", __func__);
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(name)) {
        ROSEN_LOGE("%{public}s: WriteString name err.", __func__);
        success = false;
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(watermark.get())) {
        ROSEN_LOGE("%{public}s: WriteParcelable watermark.get() err.", __func__);
        success = false;
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        success = false;
        return ERR_INVALID_VALUE;
    }
    success = true;
    return ERR_OK;
}

void RSServiceToRenderConnectionProxy::DoDump(std::unordered_set<std::u16string>& argSets, sptr<RSIDumpCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    std::vector<std::u16string> args(argSets.begin(), argSets.end());
    if (!data.WriteString16Vector(args)) {
        ROSEN_LOGE("%{public}s: WriteString16Vector failed", __func__);
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        RS_LOGE("%{public}s: WriteRemoteObject failed", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::DFX_DUMP);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRquest failed, err is %{public}d", __func__, err);
        return;
    }
}

void RSServiceToRenderConnectionProxy::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return;
    }
    if (listSize != packageList.size()) {
        ROSEN_LOGE("%{public}s: input size doesn't match", __func__);
        return;
    }
    if (!data.WriteUint32(listSize)) {
        ROSEN_LOGE("%{public}s: WriteUint32 listSize err.", __func__);
        return;
    }
    for (auto pkg : packageList) {
        if (!data.WriteString(pkg)) {
            ROSEN_LOGE("%{public}s: WriteString pkg err.", __func__);
            return;
        }
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: Send Request err.", __func__);
        return;
    }
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
ErrCode RSServiceToRenderConnectionProxy::SetOverlayDisplayMode(int32_t mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: Write InterfaceToken val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(mode)) {
        ROSEN_LOGE("%{public}s: Write Int32 val err.", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed. err:%{public}d.", __func__, err);
        return ERR_INVALID_VALUE;
    }
    ROSEN_LOGI("%{public}s: mode:%{public}d", __func__, mode);
    return ERR_OK;
}
#endif

void RSServiceToRenderConnectionProxy::ReportGameStateData(GameStateData info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportGameStateData failed to get descriptor");
        return;
    }
    if (!data.WriteInt32(info.pid)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportGameStateData WriteInt32 pid failed");
        return;
    }
    if (!data.WriteInt32(info.uid)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportGameStateData WriteInt32 uid failed");
        return;
    }
    if (!data.WriteInt32(info.state)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportGameStateData WriteInt32 state failed");
        return;
    }
    if (!data.WriteInt32(info.renderTid)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportGameStateData WriteInt32 renderTid failed");
        return;
    }
    if (!data.WriteString(info.bundleName)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::ReportGameStateData WriteString bundleName failed");
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_GAMESTATE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy sendrequest error : %{public}d", err);
        return;
    }
}

ErrCode RSServiceToRenderConnectionProxy::SetBehindWindowFilterEnabled(bool enabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetBehindWindowFilterEnabled WriteInterfaceToken failed.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(enabled)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetBehindWindowFilterEnabled WriteBool err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetBehindWindowFilterEnabled sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::GetBehindWindowFilterEnabled(bool& enabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::GetBehindWindowFilterEnabled WriteInterfaceToken failed.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::GetBehindWindowFilterEnabled sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(enabled)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::GetBehindWindowFilterEnabled ReadBool err.");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed.", __func__);
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(static_cast<int32_t>(pid))) {
        ROSEN_LOGE("%{public}s: WriteInt32 failed.", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return ERR_INVALID_VALUE;
    }
    sptr<MemoryGraphic> pMemoryGraphic(reply.ReadParcelable<MemoryGraphic>());
    if (pMemoryGraphic == nullptr) {
        return ERR_INVALID_VALUE;
    }
    memoryGraphic = *pMemoryGraphic;
    return ERR_OK;
}

bool RSServiceToRenderConnectionProxy::RegisterTypeface(uint64_t globalUniqueId,
    std::shared_ptr<Drawing::Typeface>& typeface)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken GetDescriptor err.", __func__);
        return false;
    }
    if (!typeface) {
        RS_LOGW("%{public}s: typeface is nullptr", __func__);
        return false;
    }
    uint32_t hash = typeface->GetHash();
    if (!data.WriteUint64(globalUniqueId)) {
        ROSEN_LOGE("%{public}s: WriteUint64 globalUniqueId err.", __func__);
        return false;
    }
    if (!data.WriteUint32(hash)) {
        ROSEN_LOGE("%{public}s: WriteUint32 hash err.", __func__);
        return false;
    }

    RSMarshallingHelper::Marshalling(data, typeface);

    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_TYPEFACE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGD("%{public}s: RegisterTypeface failed", __func__);
        return false;
    }
    bool result{false};
    if (!reply.ReadBool(result)) {
        ROSEN_LOGE("%{public}s: Read result failed", __func__);
        return READ_PARCEL_ERR;
    }
    return result;
}

bool RSServiceToRenderConnectionProxy::UnRegisterTypeface(uint64_t globalUniqueId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterTypeface: WriteInterfaceToken GetDescriptor err.");
        return false;
    }
    if (!data.WriteUint64(globalUniqueId)) {
        ROSEN_LOGE("UnRegisterTypeface: WriteUint64 globalUniqueId err.");
        return false;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_TYPEFACE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGD("RSServiceToRenderConnectionProxy::UnRegisterTypeface: send request failed");
        return false;
    }
    return true;
}

void RSServiceToRenderConnectionProxy::HgmForceUpdateTask(bool flag, const std::string& fromWhom)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed.", __func__);
        return;
    }
    if (!data.WriteBool(flag)) {
        ROSEN_LOGE("%{public}s: WriteBool err.", __func__);
        return;
    }
    if (!data.WriteString(fromWhom)) {
        ROSEN_LOGE("%{public}s: WriteString err.", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::HGM_FORCE_UPDATE_TASK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return;
    }
}

void RSServiceToRenderConnectionProxy::HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        RS_LOGE("%{public}s: WriteInterfaceToken failed.", __func__);
        return;
    }
    if (!data.WriteUint32(deviceId)) {
        RS_LOGE("%{public}s: WriteUint32 err.", __func__);
        return;
    }
    if (!data.WriteUint32(eventId)) {
        RS_LOGE("%{public}s: WriteUint32 err.", __func__);
        return;
    }
    if (!data.WriteInt32Vector(eventData)) {
        RS_LOGE("%{public}s: WriteInt32Vector err.", __func__);
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::HANDLE_HWC_EVENT);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return;
    }
}

int32_t RSServiceToRenderConnectionProxy::GetPidGpuMemoryInMB(pid_t pid, float& gpuMemInMB)
{
    MessageParcel data;
    MessageOption option;
    MessageParcel reply;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return RS_CONNECTION_ERROR;
    }
    if (!data.WriteInt32(pid)) {
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return err;
    }
    if (!reply.ReadFloat(gpuMemInMB)) {
        return READ_PARCEL_ERR;
    }

    return err;
}

ErrCode RSServiceToRenderConnectionProxy::RepaintEverything()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed.", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPAINT_EVERYTHING);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return ERR_INVALID_VALUE;
    }
    auto replyMessage = reply.ReadInt32();
    return replyMessage;
}

ErrCode RSServiceToRenderConnectionProxy::SetColorFollow(const std::string& nodeIdStr, bool isColorFollow)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetColorFollow: write token err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteString(nodeIdStr)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetColorFollow: write nodeIdStr err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteBool(isColorFollow)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetColorFollow: write isColorFollow err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_COLOR_FOLLOW);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::SetColorFollow: Send Request err.");
        return RS_CONNECTION_ERROR;
    }
    return ERR_OK;
}

void RSServiceToRenderConnectionProxy::SetFreeMultiWindowStatus(bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SetFreeMultiWindowStatus: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(enable)) {
        ROSEN_LOGE("SetFreeMultiWindowStatus: WriteBool err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("RSServiceToRenderConnectionProxy::SetFreeMultiWindowStatus: send request err.");
        return;
    }
}

int32_t RSServiceToRenderConnectionProxy::RegisterSelfDrawingNodeRectChangeCallback(
    pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
{
    if (!callback) {
        ROSEN_LOGE("%{public}s callback is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(remotePid)) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Write remotePid err.");
        return WRITE_PARCEL_ERR;
    }

    uint32_t size = constraint.pids.size();
    if (!data.WriteUint32(size)) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Write size err.");
        return WRITE_PARCEL_ERR;
    }
    for (int32_t pid : constraint.pids) {
        if (!data.WriteInt32(pid)) {
            ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Write pid err.");
            return WRITE_PARCEL_ERR;
        }
    }

    if (!data.WriteInt32(constraint.range.lowLimit.width) || !data.WriteInt32(constraint.range.lowLimit.height) ||
        !data.WriteInt32(constraint.range.highLimit.width) || !data.WriteInt32(constraint.range.highLimit.height)) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Write rectRange err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: WriteRemoteObject callback->AsObject() err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code =
        static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RegisterSelfDrawingNodeRectChangeCallback: Send request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t RSServiceToRenderConnectionProxy::UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("UnRegisterSelfDrawingNodeRectChangeCallback: WriteInterfaceToken GetDescriptor err.");
        return WRITE_PARCEL_ERR;
    }
    if (!data.WriteInt32(remotePid)) {
        ROSEN_LOGE("UnRegisterSelfDrawingNodeRectChangeCallback: Write remotePid err.");
        return WRITE_PARCEL_ERR;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("UnRegisterSelfDrawingNodeRectChangeCallback: Send request err.");
        return RS_CONNECTION_ERROR;
    }
    int32_t result{0};
    if (!reply.ReadInt32(result)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::UnRegisterSelfDrawingNodeRectChangeCallback Read result failed");
        return READ_PARCEL_ERR;
    }
    return result;
}

uint32_t RSServiceToRenderConnectionProxy::GetRealtimeRefreshRate(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("GetRealtimeRefreshRate: WriteInterfaceToken GetDescriptor err.");
        return SUCCESS;
    }
    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("GetRealtimeRefreshRate: WriteUint64 id err.");
        return SUCCESS;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("GetRealtimeRefreshRate: sendrequest error : %{public}d", err);
        return SUCCESS;
    }
    uint32_t rate{0};
    if (!reply.ReadUint32(rate)) {
        ROSEN_LOGE("GetRealtimeRefreshRate: Read rate failed");
        return READ_PARCEL_ERR;
    }
    return rate;
}

void RSServiceToRenderConnectionProxy::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("SetShowRefreshRateEnabled: WriteInterfaceToken GetDescriptor err.");
        return;
    }
    if (!data.WriteBool(enabled) || !data.WriteInt32(type)) {
        ROSEN_LOGE("SetShowRefreshRateEnabled: WriteBool[enable] OR WriteInt32[type] err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SetShowRefreshRateEnabled: sendrequest error : %{public}d", err);
        return;
    }
}

ErrCode RSServiceToRenderConnectionProxy::GetShowRefreshRateEnabled(bool& enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("GetShowRefreshRateEnabled: failed to get descriptor");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("GetShowRefreshRateEnabled: sendrequest error : %{public}d", err);
        return ERR_INVALID_VALUE;
    }
    if (!reply.ReadBool(enable)) {
        ROSEN_LOGE("GetShowRefreshRateEnabled: Read enable failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

std::vector<ActiveDirtyRegionInfo> RSServiceToRenderConnectionProxy::GetActiveDirtyRegionInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    std::vector<ActiveDirtyRegionInfo> activeDirtyRegionInfos;
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return activeDirtyRegionInfos;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return activeDirtyRegionInfos;
    }
    int32_t activeDirtyRegionInfosSize{0};
    if (!reply.ReadInt32(activeDirtyRegionInfosSize)) {
        ROSEN_LOGE("%{public}s: Read activeDirtyRegionInfoSize failed", __func__);
        return activeDirtyRegionInfos;
    }
    while (activeDirtyRegionInfosSize--) {
        int64_t activeDirtyRegionArea{0};
        int32_t activeFramesNumber{0};
        int32_t pidOfBelongsApp{0};
        std::string windowName;
        if (!reply.ReadInt64(activeDirtyRegionArea) || !reply.ReadInt32(activeFramesNumber) ||
            !reply.ReadInt32(pidOfBelongsApp) || !reply.ReadString(windowName)) {
            ROSEN_LOGE("%{public}s: Read parcel failed", __func__);
            return activeDirtyRegionInfos;
        }
        activeDirtyRegionInfos.emplace_back(
            ActiveDirtyRegionInfo(activeDirtyRegionArea, activeFramesNumber, pidOfBelongsApp, windowName));
    }
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo RSServiceToRenderConnectionProxy::GetGlobalDirtyRegionInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    GlobalDirtyRegionInfo globalDirtyRegionInfo;
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return globalDirtyRegionInfo;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return globalDirtyRegionInfo;
    }
    int64_t globalDirtyRegionAreas{0};
    int32_t globalFramesNumber{0};
    int32_t skipProcessFramesNumber{0};
    int32_t mostSendingPidWhenDisplayNodeSkip{0};
    if (!reply.ReadInt64(globalDirtyRegionAreas) || !reply.ReadInt32(globalFramesNumber) ||
        !reply.ReadInt32(skipProcessFramesNumber) || !reply.ReadInt32(mostSendingPidWhenDisplayNodeSkip)) {
        ROSEN_LOGE("%{public}s: Read parcel failed", __func__);
        return globalDirtyRegionInfo;
    }
    return globalDirtyRegionInfo;
}

LayerComposeInfo RSServiceToRenderConnectionProxy::GetLayerComposeInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    LayerComposeInfo layerComposeInfo;
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return layerComposeInfo;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return layerComposeInfo;
    }
    int32_t uniformRenderFrameNumber{0};
    int32_t offlineComposeFrameNumber{0};
    int32_t redrawFrameNumber{0};
    if (!reply.ReadInt32(uniformRenderFrameNumber) || !reply.ReadInt32(offlineComposeFrameNumber) ||
        !reply.ReadInt32(redrawFrameNumber)) {
        ROSEN_LOGE("%{public}s: Read parcel failed", __func__);
        return layerComposeInfo;
    }
    return LayerComposeInfo(uniformRenderFrameNumber, offlineComposeFrameNumber, redrawFrameNumber);
}

HwcDisabledReasonInfos RSServiceToRenderConnectionProxy::GetHwcDisabledReasonInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    HwcDisabledReasonInfos hwcDisabledReasonInfos;
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed.", __func__);
        return hwcDisabledReasonInfos;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::
        GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
        return hwcDisabledReasonInfos;
    }
    int32_t size{0};
    if (!reply.ReadInt32(size)) {
        ROSEN_LOGE("%{public}s: Read size failed", __func__);
        return hwcDisabledReasonInfos;
    }
    size_t readableSize = reply.GetReadableBytes() / HWC_DISABLED_REASON_INFO_MINIMUM_SIZE;
    size_t len = static_cast<size_t>(size);
    if (len > readableSize || len > hwcDisabledReasonInfos.max_size()) {
        ROSEN_LOGE("%{public}s: Failed read vector, size:%{public}zu, readableSize:%{public}zu",
            __func__, len, readableSize);
        return hwcDisabledReasonInfos;
    }

    HwcDisabledReasonInfo hwcDisabledReasonInfo;
    while (size--) {
        for (int32_t pos = 0; pos < HwcDisabledReasons::DISABLED_REASON_LENGTH; pos++) {
            hwcDisabledReasonInfo.disabledReasonStatistics[pos] = reply.ReadInt32();
        }
        hwcDisabledReasonInfo.pidOfBelongsApp = reply.ReadInt32();
        hwcDisabledReasonInfo.nodeName = reply.ReadString();
        hwcDisabledReasonInfos.emplace_back(hwcDisabledReasonInfo);
    }
    return hwcDisabledReasonInfos;
}

ErrCode RSServiceToRenderConnectionProxy::GetHdrOnDuration(int64_t& hdrOnDuration)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed", __func__);
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_HDR_ON_DURATION);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        return ERR_INVALID_VALUE;
    }
    hdrOnDuration = reply.ReadInt64();
    return ERR_OK;
}

ErrCode RSServiceToRenderConnectionProxy::SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        RS_LOGE("SetOptimizeCanvasDirtyPidList: WriteInterfaceToken failed.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32Vector(pidList)) {
        RS_LOGE("SetOptimizeCanvasDirtyPidList: WriteInt32Vector err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code =
        static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("SetOptimizeCanvasDirtyPidList: SendRequest failed, err is %{public}d.", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

int32_t RSServiceToRenderConnectionProxy::NotifyScreenRefresh(ScreenId id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: WriteInterfaceToken failed.", __func__);
        return -1;
    }

    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("%{public}s: WriteUint64 failed.", __func__);
        return -1;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::NOTIFY_SCREEN_REFRESH);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d.", __func__, err);
    }
    return err;
}

ErrCode RSServiceToRenderConnectionProxy::SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        RS_LOGE("SetGpuCrcDirtyEnabledPidList: WriteInterfaceToken failed.");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32Vector(pidList)) {
        RS_LOGE("SetGpuCrcDirtyEnabledPidList: WriteInt32Vector err.");
        return ERR_INVALID_VALUE;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        RS_LOGE("SetGpuCrcDirtyEnabledPidList: SendRequest failed, err is %{public}d.", err);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void RSServiceToRenderConnectionProxy::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy failed to get descriptor");
        return;
    }
    if (!data.WriteBool(isCurtainScreenOn)) {
        ROSEN_LOGE("SetCurtainScreenUsingStatus: WriteBool isCurtainScreenOn err.");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy sendrequest error : %{public}d", err);
        return;
    }
}

void RSServiceToRenderConnectionProxy::OnScreenBacklightChanged(ScreenId screenId, uint32_t level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy failed to get descriptor");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::OnScreenBacklightChanged WriteUint64 screenId failed");
        return;
    }
    if (!data.WriteUint32(level)) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy::OnScreenBacklightChanged WriteUint32 level failed");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_BACKLIGHT_LEVEL);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy sendrequest failed, error is %{public}d", err);
    }
}

void RSServiceToRenderConnectionProxy::OnGlobalBlacklistChanged(const std::unordered_set<NodeId>& globalBlackList)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy failed to get descriptor");
        return;
    }
    if (!data.WriteUInt64Vector({globalBlackList.begin(), globalBlackList.end()})) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy:%{public}s WriteUInt64Vector globalBlackList err.", __func__);
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::ON_GLOBAL_BLACKLIST_CHANGED);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSServiceToRenderConnectionProxy sendrequest failed, error is %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS