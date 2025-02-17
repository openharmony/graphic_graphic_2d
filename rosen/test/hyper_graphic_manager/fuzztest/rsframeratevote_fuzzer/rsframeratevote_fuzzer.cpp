/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rsframeratevote_fuzzer.h"

#include "rs_frame_rate_vote.h"
#include "buffer_extra_data.h"
#include "buffer_extra_data_impl.h"
#include "surface_buffer.h"
#include "surface_buffer_impl.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
const uint32_t usleepTime = 1500 * 1000;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoVideoFrameRateVote(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    OHSurfaceSource type = GetData<OHSurfaceSource>();
    double rate = GetData<double>();
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    sptr<BufferExtraData> extraData = new BufferExtraDataImpl();
    extraData->ExtraSet("VIDEO_RATE", rate);
    buffer->SetExtraData(extraData);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(id, type, buffer);
    usleep(usleepTime);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoVideoFrameRateVote(data, size);
    return 0;
}
