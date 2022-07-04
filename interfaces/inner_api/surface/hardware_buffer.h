/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NDK_INCLUDE_HARDWARE_BUFFER_H_
#define NDK_INCLUDE_HARDWARE_BUFFER_H_

#include "external_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

OHHardwareBuffer* HardwareBufferAlloc(const OHHardwareBufferConfig* config);

int32_t HardwareBufferReference(OHHardwareBuffer *buffer);

int32_t HardwareBufferUnreference(OHHardwareBuffer *buffer);

void GetHardwareBufferConfig(OHHardwareBuffer *buffer, OHHardwareBufferConfig*);

int32_t HardwareBufferMap(OHHardwareBuffer *buffer);

int32_t HardwareBufferUnmap(OHHardwareBuffer *buffer);

uint32_t HardwareBufferGetSeqNum(OHHardwareBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif