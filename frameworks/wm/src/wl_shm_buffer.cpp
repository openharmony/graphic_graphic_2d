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

#include "wl_shm_buffer.h"

#include <map>
#include <sys/mman.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMWlSHMBuffer" };
} // namespace

WlSHMBuffer::WlSHMBuffer(struct wl_buffer *buffer) : WlBuffer(buffer)
{
}

WlSHMBuffer::~WlSHMBuffer()
{
    if (mmapPtr != nullptr && mmapSize > 0) {
        munmap(mmapPtr, mmapSize);
    }
}

void WlSHMBuffer::SetMmap(void *ptr, uint32_t size)
{
    mmapPtr = ptr;
    mmapSize = size;
}
} // namespace OHOS
