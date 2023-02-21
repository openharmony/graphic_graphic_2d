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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_UTILS_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_UTILS_H

#include <errno.h>
#include <message_parcel.h>
#include <surface_type.h>
#include <surface_tunnel_handle.h>

#include "surface_buffer.h"

namespace OHOS {
void ReadFileDescriptor(MessageParcel &parcel, int32_t &fd);
void WriteFileDescriptor(MessageParcel &parcel, int32_t fd);

void ReadRequestConfig(MessageParcel &parcel, BufferRequestConfig &config);
void WriteRequestConfig(MessageParcel &parcel, const BufferRequestConfig  &config);

void ReadFlushConfig(MessageParcel &parcel, BufferFlushConfig &config);
void WriteFlushConfig(MessageParcel &parcel, const BufferFlushConfig &config);

void ReadSurfaceBufferImpl(MessageParcel &parcel, uint32_t &sequence, sptr<SurfaceBuffer> &buffer);
void WriteSurfaceBufferImpl(MessageParcel &parcel, uint32_t sequence, const sptr<SurfaceBuffer> &buffer);

void ReadVerifyAllocInfo(MessageParcel &parcel, std::vector<BufferVerifyAllocInfo> &infos);
void WriteVerifyAllocInfo(MessageParcel &parcel, const std::vector<BufferVerifyAllocInfo> &infos);

void ReadHDRMetaData(MessageParcel &parcel, std::vector<GraphicHDRMetaData> &metaData);
void WriteHDRMetaData(MessageParcel &parcel, const std::vector<GraphicHDRMetaData> &metaData);

void ReadHDRMetaDataSet(MessageParcel &parcel, std::vector<uint8_t> &metaData);
void WriteHDRMetaDataSet(MessageParcel &parcel, const std::vector<uint8_t> &metaData);

void ReadExtDataHandle(MessageParcel &parcel, sptr<SurfaceTunnelHandle> &handle);
void WriteExtDataHandle(MessageParcel &parcel, const GraphicExtDataHandle *handle);
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_UTILS_H
