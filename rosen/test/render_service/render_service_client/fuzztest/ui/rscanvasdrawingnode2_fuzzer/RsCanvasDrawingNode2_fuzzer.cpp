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

#include "RsCanvasDrawingNode2_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "ui/rs_canvas_drawing_node.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
namespace {
using FuzzFunc = void (*)(FuzzedDataProvider&);

void FlushCommands()
{
    auto proxy = RSTransactionProxy::GetInstance();
    if (proxy != nullptr) {
        proxy->FlushImplicitTransaction();
    }
}

void DoOnCreate(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        FlushCommands();
    }
}

void DoCreateRenderNode(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        node->CreateRenderNode();
        FlushCommands();
    }
}

void DoResetSurface(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        int w = fdp.ConsumeIntegral<int>();
        int h = fdp.ConsumeIntegral<int>();
        node->ResetSurface(w, h);
        FlushCommands();
    }
}

void DoResetSurfaceForClientRender(FuzzedDataProvider& fdp)
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        int w = fdp.ConsumeIntegral<int>();
        int h = fdp.ConsumeIntegral<int>();
        node->ResetSurfaceForClientRender(w, h);
        FlushCommands();
    }
#else
    (void)fdp;
#endif
}

void DoGenerateResetSurfaceIndex(FuzzedDataProvider& fdp)
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    (void)fdp;
    uint32_t idx = RSCanvasDrawingNode::GenerateResetSurfaceIndex();
    (void)idx;
#else
    (void)fdp;
#endif
}

void DoPreAllocateDMABuffer(FuzzedDataProvider& fdp)
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        int w = fdp.ConsumeIntegral<int>();
        int h = fdp.ConsumeIntegral<int>();
        uint32_t resetSurfaceIndex = RSCanvasDrawingNode::GenerateResetSurfaceIndex();
        RSCanvasDrawingNode::PreAllocateDMABuffer(node, node->GetId(), w, h, resetSurfaceIndex);
        FlushCommands();
    }
#else
    (void)fdp;
#endif
}

void DoCheckNodeAndSurfaceBufferState(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        int w = fdp.ConsumeIntegral<int>();
        int h = fdp.ConsumeIntegral<int>();
        auto list = std::make_shared<Drawing::DrawCmdList>(w, h);
        float fLeft = fdp.ConsumeFloatingPoint<float>();
        float fTop = fdp.ConsumeFloatingPoint<float>();
        float fRight = fdp.ConsumeFloatingPoint<float>();
        float fBottom = fdp.ConsumeFloatingPoint<float>();
        Drawing::Rect rect { fLeft, fTop, fRight, fBottom };
        node->GetBitmap(*(new Drawing::Bitmap()), list, &rect);
        FlushCommands();
    }
}

void DoOnSurfaceBufferChanged(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // OnSurfaceBufferChanged is a callback, called internally
}

void DoOnFinishRecording(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // OnFinishRecording is a callback, called internally
}

void DoRenderInClient(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        int w = fdp.ConsumeIntegral<int>();
        int h = fdp.ConsumeIntegral<int>();
        auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(w, h);
        node->RenderInClient(drawCmdList);
        FlushCommands();
    }
}

void DoRegisterNodeMap(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // RegisterNodeMap requires node in tree context
}

void DoSetIsOnTheTree(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        bool onTheTree = fdp.ConsumeBool();
        node->SetIsOnTheTree(onTheTree);
        FlushCommands();
    }
}

void DoSetNodeState(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    auto node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    if (node) {
        auto state = static_cast<RSNodeState>(fdp.ConsumeIntegral<uint8_t>());
        node->SetNodeState(state);
        FlushCommands();
    }
}

void DoIsSkipContentModifierDraw(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // IsSkipContentModifierDraw returns bool, no params
}

void DoSetSkipContentModifierDraw(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // SetSkipContentModifierDraw takes a bool param
}

// Keep the order consistent with the original switch-case mapping.
const FuzzFunc FUZZ_FUNC_TABLE[] = {
    DoOnCreate,
    DoCreateRenderNode,
    DoResetSurface,
    DoResetSurfaceForClientRender,
    DoGenerateResetSurfaceIndex,
    DoPreAllocateDMABuffer,
    DoCheckNodeAndSurfaceBufferState,
    DoOnSurfaceBufferChanged,
    DoOnFinishRecording,
    DoRenderInClient,
    DoRegisterNodeMap,
    DoSetIsOnTheTree,
    DoSetNodeState,
    DoIsSkipContentModifierDraw,
    DoSetSkipContentModifierDraw,
};
constexpr size_t FUZZ_FUNC_TABLE_SIZE = sizeof(FUZZ_FUNC_TABLE) / sizeof(FUZZ_FUNC_TABLE[0]);
} // namespace
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>();
    OHOS::Rosen::FUZZ_FUNC_TABLE[choice % OHOS::Rosen::FUZZ_FUNC_TABLE_SIZE](fdp);
    return 0;
}
