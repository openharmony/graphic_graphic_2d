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

#include "createnodeandsurface_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "platform/drawing/rs_surface.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {

// Global RSRenderServiceClient instance, initialized in LLVMFuzzerInitialize
std::shared_ptr<RSRenderServiceClient> g_renderServiceClient = nullptr;

namespace {
const uint8_t DO_CREATE_NODE_AND_SURFACE = 0;
const uint8_t TARGET_SIZE = 1;
constexpr uint32_t STR_LEN = 10;

void DoCreateNodeAndSurface(FuzzedDataProvider& fdp)
{
    RSSurfaceRenderNodeConfig config;
    config.id = fdp.ConsumeIntegral<uint64_t>();
    config.name = fdp.ConsumeRandomLengthString(STR_LEN);
    config.nodeType = static_cast<RSSurfaceNodeType>(fdp.ConsumeIntegral<uint32_t>());
    config.isTextureExportNode = fdp.ConsumeBool();
    config.isSync = fdp.ConsumeBool();
    config.surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint32_t>());
    config.bundleName = fdp.ConsumeRandomLengthString(STR_LEN);

    bool unobscured = fdp.ConsumeBool();

    g_renderServiceClient->CreateNodeAndSurface(config, unobscured);
}

} // anonymous namespace
} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSRenderServiceClient using std::make_shared (consistent with business code and TDD)
    OHOS::Rosen::g_renderServiceClient = std::make_shared<OHOS::Rosen::RSRenderServiceClient>();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_renderServiceClient == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE_NODE_AND_SURFACE:
            OHOS::Rosen::DoCreateNodeAndSurface(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
