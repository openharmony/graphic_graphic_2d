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

#include "rslayercomposecollection_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "info_collection/rs_layer_compose_collection.h"

namespace OHOS {
namespace Rosen {

LayerComposeCollection* g_layercomposecollection = nullptr;

namespace {
constexpr uint8_t DO_METHOD_UPDATE_UNIFORM_OR_OFFLINE_COMPOSE = 0;
constexpr uint8_t DO_METHOD_UPDATE_REDRAW_FRAME_NUMBER = 1;
constexpr uint8_t DO_METHOD_UPDATE_DRAW_IMAGE_NUMBER = 2;
constexpr uint8_t DO_METHOD_GET_LAYER_COMPOSE_INFO = 3;
constexpr uint8_t DO_METHOD_RESET_LAYER_COMPOSE_INFO = 4;
constexpr uint8_t TARGET_SIZE = 5;

void DoUpdateUniformOrOfflineComposeFrameNumberForDFX(FuzzedDataProvider& fdp)
{
    size_t layerSize = fdp.ConsumeIntegral<size_t>();
    g_layercomposecollection->UpdateUniformOrOfflineComposeFrameNumberForDFX(layerSize);
}

void DoUpdateRedrawFrameNumberForDFX(FuzzedDataProvider& fdp)
{
    // LCOV_EXCL_START
    g_layercomposecollection->UpdateRedrawFrameNumberForDFX();
    // LCOV_EXCL_STOP
}

void DoUpdateDrawImageNumberForDFX(FuzzedDataProvider& fdp)
{
    // LCOV_EXCL_START
    g_layercomposecollection->UpdateDrawImageNumberForDFX();
    // LCOV_EXCL_STOP
}

void DoGetLayerComposeInfo(FuzzedDataProvider& fdp)
{
    // LCOV_EXCL_START
    g_layercomposecollection->GetLayerComposeInfo();
    // LCOV_EXCL_STOP
}

void DoResetLayerComposeInfo(FuzzedDataProvider& fdp)
{
    // LCOV_EXCL_START
    g_layercomposecollection->ResetLayerComposeInfo();
    // LCOV_EXCL_STOP
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_layercomposecollection = &OHOS::Rosen::LayerComposeCollection::GetInstance();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_layercomposecollection == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_METHOD_UPDATE_UNIFORM_OR_OFFLINE_COMPOSE:
            OHOS::Rosen::DoUpdateUniformOrOfflineComposeFrameNumberForDFX(fdp);
            break;
        case OHOS::Rosen::DO_METHOD_UPDATE_REDRAW_FRAME_NUMBER:
            OHOS::Rosen::DoUpdateRedrawFrameNumberForDFX(fdp);
            break;
        case OHOS::Rosen::DO_METHOD_UPDATE_DRAW_IMAGE_NUMBER:
            OHOS::Rosen::DoUpdateDrawImageNumberForDFX(fdp);
            break;
        case OHOS::Rosen::DO_METHOD_GET_LAYER_COMPOSE_INFO:
            OHOS::Rosen::DoGetLayerComposeInfo(fdp);
            break;
        case OHOS::Rosen::DO_METHOD_RESET_LAYER_COMPOSE_INFO:
            OHOS::Rosen::DoResetLayerComposeInfo(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
