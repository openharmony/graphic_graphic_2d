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

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <optional>
#include <securec.h>


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
std::optional<std::reference_wrapper<FuzzedDataProvider>> g_fdp;
} // namespace

bool DoSetFreeze()
{
    // test
    if (!g_fdp || !OHOS::Rosen::g_keyframeNode) {
        return false;
    }
    bool freeze = g_fdp->get().ConsumeBool();
    g_keyframeNode->SetFreeze(freeze);
    return true;
}

bool DoSetLinkedNodeId()
{
    // test
    if (!g_fdp || !OHOS::Rosen::g_keyframeNode) {
        return false;
    }
    NodeId id = g_fdp->get().ConsumeIntegral<uint64_t>();
    g_keyframeNode->SetLinkedNodeId(id);
    return true;
}

bool DoReadFromParcelAndWriteToParcel()
{
    // test
    if (!g_fdp || !OHOS::Rosen::g_keyframeNode) {
        return false;
    }
    Parcel parcel1;
    Parcel parcel2;
    g_keyframeNode->WriteToParcel(parcel1);
    g_keyframeNode->ReadFromParcel(parcel1);

    g_keyframeNode->ReadFromParcel(parcel2);
    if (!RSNodeMap::MutableInstance().RegisterNode(g_keyframeNode)) {
        return false;
    }
    uint64_t linkedNodeId = g_fdp->get().ConsumeIntegral<uint64_t>();
    bool ret = parcel2.WriteUint64(g_keyframeNode->GetId()) && parcel2.WriteBool(g_fdp->get().ConsumeBool()) &&
        parcel2.WriteUint64(linkedNodeId);
    if (!ret) {
        return false;
    }
    g_keyframeNode->ReadFromParcel(parcel2);
    uint64_t id = g_fdp->get().ConsumeIntegral<uint64_t>();
    linkedNodeId = g_fdp->get().ConsumeIntegral<uint64_t>();
    ret = parcel2.WriteUint64(id) && parcel2.WriteBool(g_fdp->get().ConsumeBool()) &&
        parcel2.WriteUint64(linkedNodeId);
    if (!ret) {
        return false;
    }
    g_keyframeNode->ReadFromParcel(parcel2);
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
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::g_fdp = std::ref(fdp);
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto rsUIContext = std::make_shared<OHOS::Rosen::RSUIContext>();
    OHOS::Rosen::g_keyframeNode = OHOS::Rosen::RSWindowKeyFrameNode::Create(isRenderServiceNode,
        isTextureExportNode, rsUIContext);
    if (data == nullptr) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_FREEZE:
            OHOS::Rosen::DoSetFreeze();
            break;
        case OHOS::Rosen::DO_SET_LINKED_NODE_ID:
            OHOS::Rosen::DoSetLinkedNodeId();
            break;
        case OHOS::Rosen::DO_READ_FROM_PARCEL_AND_WRITE_TO_PARCEL:
            OHOS::Rosen::DoReadFromParcelAndWriteToParcel();
            break;
        default:
            OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseNodeMemory();
            return -1;
    }
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseNodeMemory();
    return 0;
}