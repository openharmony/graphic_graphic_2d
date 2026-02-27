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

#include "avcodecVideo_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_AVCODEC_VIDEO_START = 0;
const uint8_t DO_AVCODEC_VIDEO_STOP = 1;
const uint8_t DO_AVCODEC_VIDEO_GET = 2;
const uint8_t TARGET_SIZE = 3;
constexpr uint32_t STR_LEN = 10;

void DoAvcodecVideoStart(FuzzedDataProvider& fdp)
{
    std::vector<uint64_t> uniqueIdList;
    std::vector<std::string> surfaceNameList;
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < listSize; i++) {
        uniqueIdList.push_back(fdp.ConsumeIntegral<uint64_t>());
        surfaceNameList.push_back(fdp.ConsumeRandomLengthString(STR_LEN));
    }
    uint32_t fps = fdp.ConsumeIntegral<uint32_t>();
    uint64_t reportTime = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
}

void DoAvcodecVideoStop(FuzzedDataProvider& fdp)
{
    std::vector<uint64_t> uniqueIdList;
    std::vector<std::string> surfaceNameList;
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < listSize; i++) {
        uniqueIdList.push_back(fdp.ConsumeIntegral<uint64_t>());
        surfaceNameList.push_back(fdp.ConsumeRandomLengthString(STR_LEN));
    }
    uint32_t fps = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
}

void DoAvcodecVideoGet(FuzzedDataProvider& fdp)
{
    uint64_t uniqueId = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->AvcodecVideoGet(uniqueId);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_AVCODEC_VIDEO_START:
            OHOS::Rosen::DoAvcodecVideoStart(fdp);
            break;
        case OHOS::Rosen::DO_AVCODEC_VIDEO_STOP:
            OHOS::Rosen::DoAvcodecVideoStop(fdp);
            break;
        case OHOS::Rosen::DO_AVCODEC_VIDEO_GET:
            OHOS::Rosen::DoAvcodecVideoGet(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
