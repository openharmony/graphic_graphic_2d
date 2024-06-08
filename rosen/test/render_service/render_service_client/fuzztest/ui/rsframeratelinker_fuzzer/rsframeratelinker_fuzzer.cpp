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

#include "rsframeratelinker_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_frame_rate_linker.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    return true;
}

bool DoGetId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    frameRateLinker->GetId();
    return true;
}

bool DoIsUniRenderEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    frameRateLinker->IsUniRenderEnabled();
    return true;
}

bool DoUpdateFrameRateRange(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    FrameRateRange initialRange = GetData<FrameRateRange>();
    frameRateLinker->UpdateFrameRateRange(initialRange);
    return true;
}

bool DoUpdateFrameRateRangeImme(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    FrameRateRange initialRange = GetData<FrameRateRange>();
    frameRateLinker->UpdateFrameRateRangeImme(initialRange);
    return true;
}

bool DoSetEnable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    bool enabled = GetData<bool>();
    frameRateLinker->SetEnable(enabled);
    return true;
}

bool DoIsEnable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    frameRateLinker->IsEnable();
    return true;
}

bool DoInitUniRenderEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    frameRateLinker->InitUniRenderEnabled();
    return true;
}

bool DoGenerateId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    auto frameRateLinker = RSFrameRateLinker::Create();
    frameRateLinker->GenerateId();
    return true;
}
} // namespace Rosen
} // namespace OHOS


/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoGetId(data, size);
    OHOS::Rosen::DoIsUniRenderEnabled(data, size);
    OHOS::Rosen::DoUpdateFrameRateRange(data, size);
    OHOS::Rosen::DoUpdateFrameRateRangeImme(data, size);
    OHOS::Rosen::DoSetEnable(data, size);
    OHOS::Rosen::DoIsEnable(data, size);
    OHOS::Rosen::DoInitUniRenderEnabled(data, size);
    OHOS::Rosen::DoGenerateId(data, size);
    return 0;
}
