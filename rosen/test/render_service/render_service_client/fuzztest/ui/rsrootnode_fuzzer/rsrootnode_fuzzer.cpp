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

#include "rsrootnode_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_CREATE = 0;
const uint8_t DO_GET_TYPE = 1;
const uint8_t DO_ATTACH_SURFACE = 2;
const uint8_t DO_ENABLE_RENDER = 3;
const uint8_t DO_BOUNDS_CHANGED = 4;
const uint8_t TARGET_SIZE = 5;
}

void DoCreate(FuzzedDataProvider& fdp)
{
    RSRootNode::Create(fdp.ConsumeBool());
}

void DoGetType(FuzzedDataProvider& fdp)
{
    auto node = std::make_shared<RSRootNode>(fdp.ConsumeBool());
    node->GetType();
}

void DoAttachRSSurfaceNode(FuzzedDataProvider& fdp)
{
    auto node = std::make_shared<RSRootNode>(fdp.ConsumeBool());
    Rosen::RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    node->AttachRSSurfaceNode(surfaceNode);
}

void DoSetEnableRender(FuzzedDataProvider& fdp)
{
    auto node = std::make_shared<RSRootNode>(fdp.ConsumeBool());
    node->SetEnableRender(fdp.ConsumeBool());
}

void DoOnBoundsSizeChanged(FuzzedDataProvider& fdp)
{
    auto node = std::make_shared<RSRootNode>(fdp.ConsumeBool());
    node->OnBoundsSizeChanged();
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(fdp);
            break;
        case OHOS::Rosen::DO_GET_TYPE:
            OHOS::Rosen::DoGetType(fdp);
            break;
        case OHOS::Rosen::DO_ATTACH_SURFACE:
            OHOS::Rosen::DoAttachRSSurfaceNode(fdp);
            break;
        case OHOS::Rosen::DO_ENABLE_RENDER:
            OHOS::Rosen::DoSetEnableRender(fdp);
            break;
        case OHOS::Rosen::DO_BOUNDS_CHANGED:
            OHOS::Rosen::DoOnBoundsSizeChanged(fdp);
            break;
        default:
            break;
    }
    return 0;
}
