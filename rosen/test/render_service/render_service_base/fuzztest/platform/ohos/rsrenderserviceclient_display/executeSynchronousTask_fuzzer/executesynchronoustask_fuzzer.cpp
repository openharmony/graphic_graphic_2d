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

#include "executesynchronoustask_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "command/rs_command.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {

// Global RSRenderServiceClient instance, initialized in LLVMFuzzerInitialize
std::shared_ptr<RSRenderServiceClient> g_renderServiceClient = nullptr;

namespace {
const uint8_t DO_EXECUTE_SYNCHRONOUS_TASK = 0;
const uint8_t TARGET_SIZE = 1;

// Test implementation of RSSyncTask for fuzzing
class TestRSSyncTask : public RSSyncTask {
public:
    using RSSyncTask::RSSyncTask;

    bool CheckHeader(Parcel& parcel) const override
    {
        return true;
    }

    bool ReadFromParcel(Parcel& parcel) override
    {
        return true;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }

    void Process(RSContext& context) override {}
};

void DoExecuteSynchronousTask(FuzzedDataProvider& fdp)
{
    uint64_t timeoutNS = fdp.ConsumeIntegral<uint64_t>();
    auto task = std::make_shared<TestRSSyncTask>(timeoutNS);
    g_renderServiceClient->ExecuteSynchronousTask(task);
}

} // anonymous namespace
} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSRenderServiceClient using std::make_shared (consistent with business code)
    OHOS::Rosen::g_renderServiceClient = std::make_shared<OHOS::Rosen::RSRenderServiceClient>();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::g_renderServiceClient || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_EXECUTE_SYNCHRONOUS_TASK:
            OHOS::Rosen::DoExecuteSynchronousTask(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
