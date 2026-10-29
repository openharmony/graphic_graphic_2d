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

#include "RsSurfaceNode5_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "ui/rs_surface_node.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"
#include "common/rs_occlusion_region.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t SURFACE_NAME_MAX_LENGTH = 64;
constexpr uint8_t SHADOW_MODIFIER_MAX_COUNT = 5;
constexpr uint8_t SHADOW_PROPERTY_TYPE_RANGE = 3;
constexpr uint8_t DAMAGE_RECT_MAX_COUNT = 4;
// Keep the input-to-case mapping identical with the original switch implementation.
constexpr uint8_t FUZZ_CASE_RANGE = 82;
using FuzzFunc = void (*)(FuzzedDataProvider&);

void FlushCommands()
{
    auto proxy = RSTransactionProxy::GetInstance();
    if (proxy != nullptr) {
        proxy->FlushImplicitTransaction();
    }
}

RSSurfaceNodeConfig GetRSSurfaceNodeConfig(FuzzedDataProvider& fdp)
{
    constexpr int len = 10;
    std::string name = fdp.ConsumeRandomLengthString(len);
    bool isTextureExportNode = fdp.ConsumeBool();
    SurfaceId surfaceId = fdp.ConsumeIntegral<uint64_t>();
    bool isSync = fdp.ConsumeBool();
    SurfaceWindowType surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint8_t>());
    RSSurfaceNodeConfig config = { name, nullptr, isTextureExportNode,
        surfaceId, isSync, surfaceWindowType, nullptr };
    return config;
}

void DoSendDateToRender(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = fdp.ConsumeRandomLengthString(SURFACE_NAME_MAX_LENGTH);
    config.isTextureExportNode = fdp.ConsumeBool();
    config.surfaceId = fdp.ConsumeIntegral<uint64_t>();
    config.isSync = fdp.ConsumeBool();
    config.surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint8_t>());
    auto surfaceNode = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (surfaceNode == nullptr) {
        return;
    }
    RSSurfaceNodeType type = static_cast<RSSurfaceNodeType>(fdp.ConsumeIntegral<uint8_t>());
    surfaceNode->SendDataToRender(config, type, fdp.ConsumeBool(), fdp.ConsumeBool());
    FlushCommands();
}

void DoCreateSurfaceNode(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = fdp.ConsumeRandomLengthString(SURFACE_NAME_MAX_LENGTH);
    config.isTextureExportNode = fdp.ConsumeBool();
    config.surfaceId = fdp.ConsumeIntegral<uint64_t>();
    config.isSync = fdp.ConsumeBool();
    config.surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint8_t>());
    auto node = RSSurfaceNode::CreateSurfaceNode(config, fdp.ConsumeBool());
    (void)node;
    FlushCommands();
}

void DoSetTextureExport(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->SetIsTextureExportNode(fdp.ConsumeBool());
        FlushCommands();
    }
}

void DoResetContextAlpha(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->ResetContextAlpha();
        FlushCommands();
    }
}

void DoCreateNode(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        (void)node;
        FlushCommands();
    }
}

void DoCreateNodeAndSurface(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        RSSurfaceRenderNodeConfig renderConfig;
        renderConfig.id = fdp.ConsumeIntegral<uint64_t>();
        renderConfig.name = fdp.ConsumeRandomLengthString(SURFACE_NAME_MAX_LENGTH);
        renderConfig.nodeType = static_cast<RSSurfaceNodeType>(fdp.ConsumeIntegral<uint8_t>());
        renderConfig.isTextureExportNode = fdp.ConsumeBool();
        renderConfig.isSync = fdp.ConsumeBool();
        renderConfig.surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint8_t>());
        SurfaceId sid = fdp.ConsumeIntegral<uint64_t>();
        bool unobscured = fdp.ConsumeBool();
        node->CreateNodeAndSurface(renderConfig, sid, unobscured);
        FlushCommands();
    }
}

void DoNeedForcedSendToRemote(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        (void)node->NeedForcedSendToRemote();
        FlushCommands();
    }
}

void DoCreateSurfaceExt(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        (void)node;
        FlushCommands();
    }
}

void DoSetWindowId(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->SetWindowId(fdp.ConsumeIntegral<uint32_t>());
        FlushCommands();
    }
}

void DoInitShadowModifiers(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        std::set<ShadowPropertyType> types;
        size_t count = fdp.ConsumeIntegral<uint8_t>() % SHADOW_MODIFIER_MAX_COUNT;
        for (size_t i = 0; i < count; i++) {
            types.insert(static_cast<ShadowPropertyType>(fdp.ConsumeIntegral<uint8_t>() %
                SHADOW_PROPERTY_TYPE_RANGE));
        }
        node->InitShadowModifiers(node, types);
        FlushCommands();
    }
}

void DoCreateShadowSurfaceNode(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        std::set<ShadowPropertyType> types;
        size_t count = fdp.ConsumeIntegral<uint8_t>() % SHADOW_MODIFIER_MAX_COUNT;
        for (size_t i = 0; i < count; i++) {
            types.insert(static_cast<ShadowPropertyType>(fdp.ConsumeIntegral<uint8_t>() %
                SHADOW_PROPERTY_TYPE_RANGE));
        }
        (void)node->CreateShadowSurfaceNode(types);
        FlushCommands();
    }
}

void DoSetBufferTransform(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        auto transform = static_cast<GraphicTransformType>(fdp.ConsumeIntegral<uint8_t>());
        node->SetBufferTransform(transform);
        FlushCommands();
    }
}

void DoSetBuffer(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        sptr<SurfaceBuffer> buffer = nullptr;
        node->SetBuffer(buffer, UniqueFd(0), {});
        FlushCommands();
    }
}

void DoCleanBuffer(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->CleanBuffer(fdp.ConsumeBool());
        FlushCommands();
    }
}

void DoSetDesiredPresentTime(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->SetDesiredPresentTime(fdp.ConsumeIntegral<int64_t>());
        FlushCommands();
    }
}

void DoSetDelegateDstRect(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->SetDelegateDstRect(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
            fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
        FlushCommands();
    }
}

void DoSetDelegateSrcRect(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->SetDelegateSrcRect(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
            fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
        FlushCommands();
    }
}

void DoSetDelegateMode(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        node->SetDelegateMode(fdp.ConsumeBool());
        FlushCommands();
    }
}

void DoDumpSubClass(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        std::string out;
        node->DumpSubClass(out);
        FlushCommands();
    }
}

void DoCreateRenderThreadNode(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        auto type = static_cast<RSSurfaceNodeType>(fdp.ConsumeIntegral<uint8_t>());
        node->CreateRenderThreadNode(type, fdp.ConsumeBool());
        FlushCommands();
    }
}

void DoSetDamageRegion(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfig(fdp);
    auto node = std::make_shared<RSSurfaceNode>(config, fdp.ConsumeBool());
    if (node) {
        std::vector<Rect> rects;
        size_t count = fdp.ConsumeIntegral<uint8_t>() % DAMAGE_RECT_MAX_COUNT;
        for (size_t i = 0; i < count; i++) {
            rects.push_back(Rect{fdp.ConsumeIntegral<int32_t>(), fdp.ConsumeIntegral<int32_t>(),
                fdp.ConsumeIntegral<int32_t>(), fdp.ConsumeIntegral<int32_t>()});
        }
        node->SetDamageRegion(rects);
        FlushCommands();
    }
}

// Keep the order consistent with the original switch-case mapping.
const FuzzFunc FUZZ_FUNC_TABLE[] = {
    DoSendDateToRender,
    DoCreateSurfaceNode,
    DoSetTextureExport,
    DoResetContextAlpha,
    DoCreateNode,
    DoCreateNodeAndSurface,
    DoNeedForcedSendToRemote,
    DoCreateSurfaceExt,
    DoSetWindowId,
    DoInitShadowModifiers,
    DoCreateShadowSurfaceNode,
    DoSetBufferTransform,
    DoSetBuffer,
    DoCleanBuffer,
    DoSetDesiredPresentTime,
    DoSetDelegateDstRect,
    DoSetDelegateSrcRect,
    DoSetDelegateMode,
    DoDumpSubClass,
    DoCreateRenderThreadNode,
    DoSetDamageRegion,
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
    uint8_t index = choice % OHOS::Rosen::FUZZ_CASE_RANGE;
    if (index < OHOS::Rosen::FUZZ_FUNC_TABLE_SIZE) {
        OHOS::Rosen::FUZZ_FUNC_TABLE[index](fdp);
    }
    return 0;
}
