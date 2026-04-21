/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_hdr_collection_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "info_collection/rs_hdr_collection.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<RsHdrCollection> g_rshdrcollection = nullptr;

namespace {
constexpr uint8_t DO_METHOD_HANDLE_HDR_STATE = 0;
constexpr uint8_t TARGET_SIZE = 1;

void DoHandleHdrState(FuzzedDataProvider& fdp)
{
    bool isHdrOn = fdp.ConsumeBool();
    g_rshdrcollection->HandleHdrState(isHdrOn);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_rshdrcollection = OHOS::Rosen::RsHdrCollection::GetInstance();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rshdrcollection == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_METHOD_HANDLE_HDR_STATE:
            OHOS::Rosen::DoHandleHdrState(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
