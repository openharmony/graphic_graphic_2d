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

#include "surface_buffer_impl.h"

namespace OHOS {
void ReadFence(MessageParcel& parcel, int32_t& fence);
void WriteFence(MessageParcel& parcel, int32_t fence);

void ReadRequestConfig(MessageParcel& parcel, BufferRequestConfig& config);
void WriteRequestConfig(MessageParcel& parcel, BufferRequestConfig const & config);

void ReadFlushConfig(MessageParcel& parcel, BufferFlushConfig& config);
void WriteFlushConfig(MessageParcel& parcel, BufferFlushConfig const & config);

void ReadSurfaceBufferImpl(MessageParcel& parcel,
    int32_t& sequence, sptr<SurfaceBufferImpl>& bufferImpl);
void WriteSurfaceBufferImpl(MessageParcel& parcel,
    int32_t sequence, sptr<SurfaceBufferImpl> const & bufferImpl);
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_UTILS_H
