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

#include "surface_buffer_impl.h"

namespace OHOS {
void ReadFence(MessageParcel &parcel, int32_t &fence)
{
    fence = parcel.ReadInt32();
    if (fence < 0) {
        return;
    }

    fence = parcel.ReadFileDescriptor();
}

void WriteFence(MessageParcel &parcel, int32_t fence)
{
    if (fence >= 0 && fcntl(fence, F_GETFL) == -1 && errno == EBADF) {
        fence = -1;
    }

    parcel.WriteInt32(fence);

    if (fence < 0) {
        return;
    }

    parcel.WriteFileDescriptor(fence);
    close(fence);
}

void ReadRequestConfig(MessageParcel &parcel, BufferRequestConfig &config)
{
    config.width = parcel.ReadInt32();
    config.height = parcel.ReadInt32();
    config.strideAlignment = parcel.ReadInt32();
    config.format = parcel.ReadInt32();
    config.usage = parcel.ReadInt32();
    config.timeout = parcel.ReadInt32();
    config.colorGamut = static_cast<ColorGamut>(parcel.ReadInt32());
    config.transform = static_cast<TransformType>(parcel.ReadInt32());
}

void WriteRequestConfig(MessageParcel &parcel, BufferRequestConfig const & config)
{
    parcel.WriteInt32(config.width);
    parcel.WriteInt32(config.height);
    parcel.WriteInt32(config.strideAlignment);
    parcel.WriteInt32(config.format);
    parcel.WriteInt32(config.usage);
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
                           int32_t &sequence, sptr<SurfaceBuffer>& buffer)
{
    sequence = parcel.ReadInt32();
    if (parcel.ReadBool()) {
        sptr<SurfaceBuffer> bufferImpl = new SurfaceBufferImpl(sequence);
        auto handle = ReadBufferHandle(parcel);
        bufferImpl->SetBufferHandle(handle);
        buffer = bufferImpl;
    }
}

void WriteSurfaceBufferImpl(MessageParcel &parcel,
    int32_t sequence, const sptr<SurfaceBuffer> &buffer)
{
    parcel.WriteInt32(sequence);
    parcel.WriteBool(buffer != nullptr);
    if (buffer == nullptr) {
        return;
    }
    buffer->WriteToMessageParcel(parcel);
}

void ReadVerifyAllocInfo(MessageParcel &parcel, std::vector<VerifyAllocInfo> &infos)
{
    uint32_t size = parcel.ReadUint32();
    infos.clear();
    VerifyAllocInfo info;
    for (uint32_t index = 0; index < size; index++) {
        info.width = parcel.ReadUint32();
        info.height = parcel.ReadUint32();
        info.usage = parcel.ReadUint64();
        info.format = static_cast<PixelFormat>(parcel.ReadInt32());
        infos.push_back(info);
    }
}

void WriteVerifyAllocInfo(MessageParcel &parcel, const std::vector<VerifyAllocInfo> &infos)
{
    parcel.WriteUint32(infos.size());
    for (const auto &info : infos) {
        parcel.WriteUint32(info.width);
        parcel.WriteUint32(info.height);
        parcel.WriteUint64(info.usage);
        parcel.WriteInt32(info.format);
    }
}
} // namespace OHOS
