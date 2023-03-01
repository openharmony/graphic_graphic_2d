/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "buffer_utils.h"

#include <fcntl.h>
#include <unistd.h>

#include "buffer_log.h"
#include "surface_buffer_impl.h"

namespace OHOS {
void ReadFileDescriptor(MessageParcel &parcel, int32_t &fd)
{
    fd = parcel.ReadInt32();
    if (fd < 0) {
        return;
    }

    fd = parcel.ReadFileDescriptor();
}

void WriteFileDescriptor(MessageParcel &parcel, int32_t fd)
{
    if (fd >= 0 && fcntl(fd, F_GETFL) == -1 && errno == EBADF) {
        fd = -1;
    }

    parcel.WriteInt32(fd);

    if (fd < 0) {
        return;
    }

    parcel.WriteFileDescriptor(fd);
    close(fd);
}

void ReadRequestConfig(MessageParcel &parcel, BufferRequestConfig &config)
{
    config.width = parcel.ReadInt32();
    config.height = parcel.ReadInt32();
    config.strideAlignment = parcel.ReadInt32();
    config.format = parcel.ReadInt32();
    config.usage = parcel.ReadUint64();
    config.timeout = parcel.ReadInt32();
    config.colorGamut = static_cast<GraphicColorGamut>(parcel.ReadInt32());
    config.transform = static_cast<GraphicTransformType>(parcel.ReadInt32());
}

void WriteRequestConfig(MessageParcel &parcel, BufferRequestConfig const & config)
{
    parcel.WriteInt32(config.width);
    parcel.WriteInt32(config.height);
    parcel.WriteInt32(config.strideAlignment);
    parcel.WriteInt32(config.format);
    parcel.WriteUint64(config.usage);
    parcel.WriteInt32(config.timeout);
    parcel.WriteInt32(static_cast<int32_t>(config.colorGamut));
    parcel.WriteInt32(static_cast<int32_t>(config.transform));
}

void ReadFlushConfig(MessageParcel &parcel, BufferFlushConfig &config)
{
    config.damage.x = parcel.ReadInt32();
    config.damage.y = parcel.ReadInt32();
    config.damage.w = parcel.ReadInt32();
    config.damage.h = parcel.ReadInt32();
    config.timestamp = parcel.ReadInt64();
}

void WriteFlushConfig(MessageParcel &parcel, BufferFlushConfig const & config)
{
    parcel.WriteInt32(config.damage.x);
    parcel.WriteInt32(config.damage.y);
    parcel.WriteInt32(config.damage.w);
    parcel.WriteInt32(config.damage.h);
    parcel.WriteInt64(config.timestamp);
}

void ReadSurfaceBufferImpl(MessageParcel &parcel,
                           uint32_t &sequence, sptr<SurfaceBuffer>& buffer)
{
    sequence = parcel.ReadUint32();
    if (parcel.ReadBool()) {
        buffer = new SurfaceBufferImpl(sequence);
        buffer->ReadFromMessageParcel(parcel);
    }
}

void WriteSurfaceBufferImpl(MessageParcel &parcel,
    uint32_t sequence, const sptr<SurfaceBuffer> &buffer)
{
    parcel.WriteUint32(sequence);
    parcel.WriteBool(buffer != nullptr);
    if (buffer == nullptr) {
        return;
    }
    buffer->WriteToMessageParcel(parcel);
}

void ReadVerifyAllocInfo(MessageParcel &parcel, std::vector<BufferVerifyAllocInfo> &infos)
{
    uint32_t size = parcel.ReadUint32();
    infos.clear();
    BufferVerifyAllocInfo info;
    for (uint32_t index = 0; index < size; index++) {
        info.width = parcel.ReadUint32();
        info.height = parcel.ReadUint32();
        info.usage = parcel.ReadUint64();
        info.format = static_cast<GraphicPixelFormat>(parcel.ReadInt32());
        infos.push_back(info);
    }
}

void WriteVerifyAllocInfo(MessageParcel &parcel, const std::vector<BufferVerifyAllocInfo> &infos)
{
    parcel.WriteUint32(infos.size());
    for (const auto &info : infos) {
        parcel.WriteUint32(info.width);
        parcel.WriteUint32(info.height);
        parcel.WriteUint64(info.usage);
        parcel.WriteInt32(info.format);
    }
}

void ReadHDRMetaData(MessageParcel &parcel, std::vector<GraphicHDRMetaData> &metaData)
{
    uint32_t size = parcel.ReadUint32();
    metaData.clear();
    GraphicHDRMetaData data;
    for (uint32_t index = 0; index < size; index++) {
        data.key = static_cast<GraphicHDRMetadataKey>(parcel.ReadUint32());
        data.value = parcel.ReadFloat();
        metaData.push_back(data);
    }
}

void WriteHDRMetaData(MessageParcel &parcel, const std::vector<GraphicHDRMetaData> &metaData)
{
    parcel.WriteUint32(metaData.size());
    for (const auto &data : metaData) {
        parcel.WriteUint32(static_cast<uint32_t>(data.key));
        parcel.WriteFloat(data.value);
    }
}

void ReadHDRMetaDataSet(MessageParcel &parcel, std::vector<uint8_t> &metaData)
{
    uint32_t size = parcel.ReadUint32();
    metaData.clear();
    for (uint32_t index = 0; index < size; index++) {
        uint8_t data = parcel.ReadUint8();
        metaData.push_back(data);
    }
}

void WriteHDRMetaDataSet(MessageParcel &parcel, const std::vector<uint8_t> &metaData)
{
    parcel.WriteUint32(metaData.size());
    for (const auto &data : metaData) {
        parcel.WriteUint8(data);
    }
}

void ReadExtDataHandle(MessageParcel &parcel, sptr<SurfaceTunnelHandle> &handle)
{
    if (handle == nullptr) {
        BLOGE("ReadExtDataHandle failed, handle is null");
        return;
    }
    uint32_t reserveInts = parcel.ReadUint32();
    GraphicExtDataHandle *tunnelHandle = AllocExtDataHandle(reserveInts);
    if (tunnelHandle == nullptr) {
        BLOGE("AllocExtDataHandle failed");
        return;
    }
    ReadFileDescriptor(parcel, tunnelHandle->fd);
    for (uint32_t index = 0; index < reserveInts; index++) {
        tunnelHandle->reserve[index] = parcel.ReadInt32();
    }
    if (handle->SetHandle(tunnelHandle) != GSERROR_OK) {
        BLOGE("SetHandle failed");
        return;
    }
    FreeExtDataHandle(tunnelHandle);
}

void WriteExtDataHandle(MessageParcel &parcel, const GraphicExtDataHandle *handle)
{
    if (handle == nullptr) {
        BLOGE("WriteExtDataHandle failed, handle is null");
        return;
    }
    parcel.WriteUint32(handle->reserveInts);
    WriteFileDescriptor(parcel, handle->fd);
    for (uint32_t index = 0; index < handle->reserveInts; index++) {
        parcel.WriteInt32(handle->reserve[index]);
    }
}
} // namespace OHOS
