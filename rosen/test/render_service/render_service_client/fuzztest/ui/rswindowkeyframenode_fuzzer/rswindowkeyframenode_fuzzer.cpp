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

#include "rswindowkeyframenode_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "feature/window_keyframe/rs_window_keyframe_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_SET_FREEZE = 0;
const uint8_t DO_SET_LINKED_NODE_ID = 1;
const uint8_t DO_READ_FROM_PARCEL_AND_WRITE_TO_PARCEL = 2;
const uint8_t TARGET_SIZE = 3;
RSWindowKeyFrameNode::SharedPtr g_keyframeNode = nullptr;
} // namespace

bool DoSetFreeze(FuzzedDataProvider& fdp)
{
    if (!OHOS::Rosen::g_keyframeNode) {
        return false;
    }
    bool freeze = fdp.ConsumeBool();
    g_keyframeNode->SetFreeze(freeze);
    return true;
}

bool DoSetLinkedNodeId(FuzzedDataProvider& fdp)
{
    if (!OHOS::Rosen::g_keyframeNode) {
        return false;
    }
    NodeId id = fdp.ConsumeIntegral<uint64_t>();
    g_keyframeNode->SetLinkedNodeId(id);
    return true;
}

bool DoReadFromParcelAndWriteToParcel(FuzzedDataProvider& fdp)
{
    if (!OHOS::Rosen::g_keyframeNode) {
        return false;
    }
    Parcel parcel1;
    g_keyframeNode->WriteToParcel(parcel1);
    parcel1.RewindRead(0);
    auto node1 = g_keyframeNode->ReadFromParcel(parcel1);
    (void)node1;

    Parcel parcel2;
    parcel2.RewindRead(0);
    auto node2 = g_keyframeNode->ReadFromParcel(parcel2);
    (void)node2;

    uint64_t linkedNodeId = fdp.ConsumeIntegral<uint64_t>();
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    bool ret = parcel2.WriteUint64(id) && parcel2.WriteBool(fdp.ConsumeBool()) &&
        parcel2.WriteUint64(linkedNodeId);
    if (!ret) {
        return false;
    }
    parcel2.RewindRead(0);
    auto node3 = g_keyframeNode->ReadFromParcel(parcel2);
    (void)node3;
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return -1;
    }
    FuzzedDataProvider fdp(data, size);
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto rsUIContext = std::make_shared<OHOS::Rosen::RSUIContext>();
    OHOS::Rosen::g_keyframeNode = OHOS::Rosen::RSWindowKeyFrameNode::Create(isRenderServiceNode,
        isTextureExportNode, rsUIContext);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_FREEZE:
            OHOS::Rosen::DoSetFreeze(fdp);
            break;
        case OHOS::Rosen::DO_SET_LINKED_NODE_ID:
            OHOS::Rosen::DoSetLinkedNodeId(fdp);
            break;
        case OHOS::Rosen::DO_READ_FROM_PARCEL_AND_WRITE_TO_PARCEL:
            OHOS::Rosen::DoReadFromParcelAndWriteToParcel(fdp);
            break;
        default:
            break;
    }
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseNodeMemory();
    return 0;
}