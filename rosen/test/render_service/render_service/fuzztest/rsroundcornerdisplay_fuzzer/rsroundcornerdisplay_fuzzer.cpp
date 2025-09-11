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
#include "rsroundcornerdisplay_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_round_corner_config.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/rs_processor_factory.h"
#include "securec.h"

#include "common/rs_singleton.h"
#include "rs_rcd_surface_render_node.h"

namespace OHOS {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;
} // namespace

// must greater than 0
constexpr uint8_t STATUS_FUZZ_BOUNDS = 3;
using RCDLayerType = Rosen::RoundCornerDisplayManager::RCDLayerType;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool RSRoundCornerDisplayLoadCfgFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    auto& rcdCfg = OHOS::Rosen::RSSingleton<OHOS::Rosen::rs_rcd::RCDConfig>::GetInstance();
    std::string randomConfig(reinterpret_cast<const char*>(data), size);
    rcdCfg.Load(randomConfig);
    rcdCfg.IsDataLoaded();
    return true;
}

bool RSRoundCornerDisplayGetLcdModelFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    auto& rcdCfg = OHOS::Rosen::RSSingleton<OHOS::Rosen::rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string(OHOS::Rosen::rs_rcd::PATH_CONFIG_FILE));
    rcdCfg.GetLcdModel(std::string(reinterpret_cast<const char*>(data), size));
    return true;
}

bool RSRoundCornerDisplayGetRogFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    auto w = GetData<int>();
    auto h = GetData<int>();

    auto& rcdCfg = OHOS::Rosen::RSSingleton<OHOS::Rosen::rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string(OHOS::Rosen::rs_rcd::PATH_CONFIG_FILE));
    auto Lcd = rcdCfg.GetLcdModel(OHOS::Rosen::rs_rcd::ATTR_DEFAULT);

    if (Lcd != nullptr) {
        Lcd->GetRog(w, h);
    }

    return true;
}

bool RSRoundCornerDisplayPrintParseRogFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    auto& rcdCfg = OHOS::Rosen::RSSingleton<OHOS::Rosen::rs_rcd::RCDConfig>::GetInstance();
    rcdCfg.Load(std::string(OHOS::Rosen::rs_rcd::PATH_CONFIG_FILE));

    OHOS::Rosen::rs_rcd::ROGSetting rog;
    rog.width = GetData<int>();
    rog.height = GetData<int>();

    auto keya = std::string(reinterpret_cast<const char*>(data), size / 2);
    auto keyb = std::string(reinterpret_cast<const char*>(data), size / 3);

    OHOS::Rosen::rs_rcd::RogPortrait rp {};
    OHOS::Rosen::rs_rcd::RogLandscape rl {};
    rog.portraitMap[keya] = rp;
    rog.landscapeMap[keyb] = rl;

    OHOS::Rosen::rs_rcd::RCDConfig::PrintParseRog(nullptr);
    OHOS::Rosen::rs_rcd::RCDConfig::PrintParseRog(&rog);
    return true;
}

bool RSRoundCornerDisplayAddRoundCornerDisplayFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    manager.AddRoundCornerDisplay(id);

    manager.RemoveRCDResource(id);

    return true;
}

bool RSRoundCornerDisplayAddLayerFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    std::string layerName(reinterpret_cast<const char*>(data), size);

    auto rawLayerType = GetData<uint32_t>();
    auto layerType = static_cast<RCDLayerType>(rawLayerType);

    manager.AddLayer(layerName, id, layerType);

    manager.RemoveRCDResource(id);
    return true;
}

bool RSRoundCornerDisplayRemoveRCDResourceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

    Rosen::NodeId id = GetData<Rosen::NodeId>();

    manager.RemoveRCDResource(id);

    return true;
}

bool RSRoundCornerAPIsTest(
    const uint8_t* data, size_t size, Rosen::NodeId lid, RCDLayerType lType, std::string lName, bool isRCDSet = false)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    Rosen::NodeId id {};
    RCDLayerType layerType {};
    std::string layerName {};

    if (isRCDSet) {
        id = lid;
        layerType = lType;
        layerName = lName;
    } else {
        id = GetData<Rosen::NodeId>();
        layerName = std::string(reinterpret_cast<const char*>(data), size);

        auto rawLayerType = GetData<uint32_t>();
        rawLayerType = rawLayerType % (static_cast<uint32_t>(RCDLayerType::BOTTOM) + 1);
        layerType = static_cast<RCDLayerType>(rawLayerType);
    }

    auto& displayManager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();
    auto& renderManager = Rosen::RSRcdRenderManager::GetInstance();

    uint32_t left = GetData<uint32_t>();
    uint32_t top = GetData<uint32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();

    int status = static_cast<int>(GetData<uint8_t>() % STATUS_FUZZ_BOUNDS) - 1;

    auto rawRotation = GetData<uint32_t>();
    rawRotation = rawRotation % (static_cast<uint32_t>(Rosen::ScreenRotation::INVALID_SCREEN_ROTATION) + 1);
    auto rotation = static_cast<Rosen::ScreenRotation>(rawRotation);

    auto rawPrepared = GetData<uint8_t>();
    auto rawNotchStatus = GetData<uint8_t>();

    bool prepared = (rawPrepared % 2) != 0;
    bool notchStatus = (rawNotchStatus % 2) != 0;

    std::function<void()> task = []() { std::string stub; };
    Rosen::RectI dirtyRect;

    // test display manager APIs
    displayManager.UpdateDisplayParameter(id, left, top, width, height);
    displayManager.UpdateNotchStatus(id, status);
    displayManager.UpdateOrientationStatus(id, rotation);
    displayManager.UpdateHardwareResourcePrepared(id, prepared);
    displayManager.GetHardwareInfo(id, prepared);
    displayManager.IsNotchNeedUpdate(id, notchStatus);
    displayManager.RunHardwareTask(id, task);
    displayManager.HandleRoundCornerDirtyRect(id, dirtyRect, layerType);
    displayManager.GetRcdEnable();

    displayManager.GetLayerPair(layerName);
    displayManager.CheckLayerIsRCD(layerName);

    std::vector<std::pair<Rosen::NodeId, RCDLayerType>> renderTargetNodeInfoList;
    renderTargetNodeInfoList.emplace_back(id, RCDLayerType::INVALID);
    renderTargetNodeInfoList.emplace_back(id, RCDLayerType::TOP);
    renderTargetNodeInfoList.emplace_back(id, RCDLayerType::BOTTOM);

    auto baseCanvas = std::make_shared<Rosen::Drawing::Canvas>();
    auto canvas = std::make_shared<Rosen::RSPaintFilterCanvas>(baseCanvas.get(), 1.0f);
    displayManager.DrawRoundCorner(renderTargetNodeInfoList, canvas.get());

    // test render manager APIs
    Rosen::RSContext context;
    auto topLayer = std::make_shared<Rosen::rs_rcd::RoundCornerLayer>();
    auto bottomLayer = std::make_shared<Rosen::rs_rcd::RoundCornerLayer>();
    auto uniProcessor = Rosen::RSProcessorFactory::CreateProcessor(Rosen::CompositeType::UNI_RENDER_COMPOSITE);

    topLayer->offsetX = GetData<int>();
    topLayer->offsetY = GetData<int>();
    topLayer->bufferSize = GetData<int>();
    topLayer->cldWidth = GetData<int>();
    topLayer->cldHeight = GetData<int>();

    bottomLayer->offsetX = GetData<int>();
    bottomLayer->offsetY = GetData<int>();
    bottomLayer->bufferSize = GetData<int>();
    bottomLayer->cldWidth = GetData<int>();
    bottomLayer->cldHeight = GetData<int>();

    Rosen::RcdProcessInfo info {};

    // uniProcessor is nullptr
    renderManager.IsRcdProcessInfoValid(info);
    info.uniProcessor = uniProcessor;

    // layer is nullptr
    renderManager.IsRcdProcessInfoValid(info);
    info.topLayer = topLayer;
    info.bottomLayer = bottomLayer;

    renderManager.GetRcdRenderEnabled();
    Rosen::RSRcdRenderManager::InitInstance();
    renderManager.GetRcdRenderEnabled();

    renderManager.GetTopSurfaceNode(id);
    renderManager.GetBottomSurfaceNode(id);
    renderManager.DoProcessRenderTask(id, info);
    renderManager.DoProcessRenderMainThreadTask(id, info);
    renderManager.CheckRenderTargetNode(context);
    renderManager.IsRcdProcessInfoValid(info);

    // clear
    displayManager.RemoveRCDResource(id);
    renderManager.RemoveRcdResource(id);
    renderManager.topSurfaceNodeMap_.clear();
    renderManager.bottomSurfaceNodeMap_.clear();
    context.nodeMap.renderNodeMap_.clear();

    return true;
}

bool RSRoundCornerAPIsFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    // rcd stub
    Rosen::NodeId randomId = GetData<Rosen::NodeId>();
    std::string layerName(reinterpret_cast<const char*>(data), size);

    auto rawLayerType = GetData<uint32_t>();
    rawLayerType = rawLayerType % (static_cast<uint32_t>(RCDLayerType::BOTTOM) + 1);
    auto layerType = static_cast<RCDLayerType>(rawLayerType);

    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

    // test manager APIs without add rcd
    RSRoundCornerAPIsTest(data, size, randomId, layerType, layerName);

    // test manager APIs with add rcd
    manager.AddRoundCornerDisplay(randomId);
    manager.AddLayer(layerName, randomId, layerType);
    RSRoundCornerAPIsTest(data, size, randomId, layerType, layerName, true);

    return true;
}

bool RSRcdSurfaceRenderNodeSetRcdBufferWidthFuzzTest(const uint8_t* data, size_t size)
{
    int dataNum = 2;
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t) * dataNum)) {
        return false;
    }

    Rosen::NodeId nodeId = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(nodeId, type);
    uint32_t width = GetData<uint32_t>();
    node->SetRcdBufferWidth(width);

    return true;
}

bool RSRcdSurfaceRenderNodeFuzzTest01(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    return true;
}

bool RSRcdSurfaceRenderNodeCreateSurfaceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    Rosen::RCDSurfaceType surfaceType = type;
    Rosen::RCDSurfaceType surfaceType2 = (surfaceType == Rosen::RCDSurfaceType::TOP) ?
        Rosen::RCDSurfaceType::BOTTOM : Rosen::RCDSurfaceType::TOP;

    auto node1 = Rosen::RSRcdSurfaceRenderNode::Create(id, surfaceType);
    auto node2 = Rosen::RSRcdSurfaceRenderNode::Create(id, surfaceType2);
    node1->SetRenderTargetId(id);
    sptr<IBufferConsumerListener> listener = nullptr;

    node1->CreateSurface(listener);
    node2->CreateSurface(listener);

    return true;
}

bool RSRcdSurfaceRenderNodeBuffSetGetFuzzTest(const uint8_t* data, size_t size)
{
    int dataNum = 8;
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t) * dataNum)) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());

    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    uint32_t targetRectX = GetData<uint32_t>();
    uint32_t targetRectY = GetData<uint32_t>();
    uint32_t targetRectWidth = GetData<uint32_t>();
    uint32_t targetRectHeight = GetData<uint32_t>();
    node->displayRect_.SetAll(targetRectX, targetRectY, targetRectWidth, targetRectHeight);

    uint32_t rcdBufferWidth = GetData<uint32_t>();
    uint32_t rcdBufferHeight = GetData<uint32_t>();
    uint32_t rcdBufferSize = GetData<uint32_t>();
    node->SetRcdBufferWidth(rcdBufferWidth);
    node->SetRcdBufferHeight(rcdBufferHeight);
    node->SetRcdBufferSize(rcdBufferSize);
    node->GetHardenBufferRequestConfig();
    return true;
}

bool RSRcdSurfaceRenderNodePrepareHardwareFuzzTest(const uint8_t* data, size_t size)
{
    int targetDataNum = 4;
    int layerDataNum = 4;
    size_t requiredSize = sizeof(Rosen::NodeId) + sizeof(uint32_t) * targetDataNum + sizeof(int) * layerDataNum;
    if (data == nullptr || size < requiredSize) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    node->SetRenderTargetId(id);
    uint32_t targetRectX = GetData<uint32_t>();
    uint32_t targetRectY = GetData<uint32_t>();
    uint32_t targetRectWidth = GetData<uint32_t>();
    uint32_t targetRectHeight = GetData<uint32_t>();
    node->displayRect_.SetAll(targetRectX, targetRectY, targetRectWidth, targetRectHeight);

    auto layer = std::make_shared<Rosen::rs_rcd::RoundCornerLayer>();
    layer->bufferSize = GetData<int>();
    layer->cldWidth = GetData<int>();
    layer->cldHeight = GetData<int>();
    layer->offsetX = GetData<int>();
    layer->binFileName = "test.bin";
    layer->curBitmap = new Rosen::Drawing::Bitmap();

    Rosen::Drawing::ImageInfo info = Rosen::Drawing::ImageInfo::MakeN32Premul(4, 4);

    auto surface = Rosen::Drawing::Surface::MakeRasterN32Premul(
        info.GetWidth(), info.GetHeight());
    auto canvas = surface->GetCanvas();

    canvas->DrawColor(0xFFFF0000);
    node->PrepareHardwareResourceBuffer(layer);
    return true;
}

bool RSRcdSurfaceRenderNodeSetHardwareFuzzTest(const uint8_t* data, size_t size)
{
    int dataNum = 4;
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t) * dataNum)) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    uint32_t rcdBufferWidth = GetData<uint32_t>();
    uint32_t rcdBufferHeight = GetData<uint32_t>();
    uint32_t rcdBufferSize = GetData<uint32_t>();
    node->SetRcdBufferWidth(rcdBufferWidth);
    node->SetRcdBufferHeight(rcdBufferHeight);
    node->SetRcdBufferSize(rcdBufferSize);
    node->SetHardwareResourceToBuffer();
    return true;
}

bool RSRcdSurfaceIsTypeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    node->IsTopSurface();
    node->IsBottomSurface();
    node->IsInvalidSurface();

    return true;
}

bool RSRcdSurfacePrintResetFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);

    node->Reset();
    node->PrintRcdNodeInfo();

    return true;
}

bool RSRcdSurfaceIsInvalidSurfaceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) +  sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);

    node->IsInvalidSurface();

    return true;
}

bool RSRcdSurfaceRenderNodeIsSurfaceCreatedFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    Rosen::RCDSurfaceType surfaceType = type;

    auto node1 = Rosen::RSRcdSurfaceRenderNode::Create(id, surfaceType);
    node1->SetRenderTargetId(id);
    sptr<IBufferConsumerListener> listener = nullptr;

    bool isCreatedBefore = node1->IsSurfaceCreated();
    if (isCreatedBefore != false) {
        return false;
    }

    node1->CreateSurface(listener);
    bool isCreatedAfter = node1->IsSurfaceCreated();
    if (isCreatedAfter != true) {
        return false;
    }

    node1->Reset();
    bool isCreatedAfterReset = node1->IsSurfaceCreated();
    if (isCreatedAfterReset != false) {
        return false;
    }

    return true;
}

bool RSRcdSurfaceRenderNodeGetRSSurfaceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    Rosen::RCDSurfaceType surfaceType = type;

    auto node1 = Rosen::RSRcdSurfaceRenderNode::Create(id, surfaceType);
    node1->SetRenderTargetId(id);
    sptr<IBufferConsumerListener> listener = nullptr;

    std::shared_ptr<Rosen::RSSurface> surfaceBefore = node1->GetRSSurface();
    if (surfaceBefore != nullptr) {
        return false;
    }

    node1->CreateSurface(listener);
    std::shared_ptr<Rosen::RSSurface> surfaceAfter = node1->GetRSSurface();
    if (surfaceAfter == nullptr) {
        return false;
    }

    node1->Reset();
    std::shared_ptr<Rosen::RSSurface> surfaceAfterReset = node1->GetRSSurface();
    if (surfaceAfterReset != nullptr) {
        return false;
    }

    return true;
}

bool RSRcdSurfaceRenderNodeClearBufferCacheFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    Rosen::RCDSurfaceType surfaceType = type;

    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, surfaceType);
    node->SetRenderTargetId(id);

    sptr<IBufferConsumerListener> listener = nullptr;
    node->ClearBufferCache();

    node->CreateSurface(listener);
    node->ClearBufferCache();

    node->Reset();
    node->ClearBufferCache();

    return true;
}

bool RSRcdSurfaceRenderNodeFillHardwareResourceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());

    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    if (node == nullptr) {
        return false;
    }

    Rosen::HardwareLayerInfo cldLayerInfo;
    cldLayerInfo.bufferSize = GetData<int>();
    cldLayerInfo.cldWidth = GetData<int>();
    cldLayerInfo.cldHeight = GetData<int>();
    std::string pathBin = GetData<std::string>();
    cldLayerInfo.pathBin = pathBin;

    int height = GetData<int>();
    int width = GetData<int>();

    bool result = node->FillHardwareResource(cldLayerInfo, height, width);

    return result;
}

bool RSRcdSurfaceRenderNodeDataLocalFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(Rosen::NodeId) + sizeof(uint32_t))) {
        return false;
    }

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    Rosen::RCDSurfaceType surfaceType = type;

    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, surfaceType);

    auto datax = node->GetFrameOffsetX();
    auto datay = node->GetFrameOffsetY();
    auto dataw = node->GetRcdBufferWidth();
    auto datah = node->GetRcdBufferHeight();

    float offset = 0;
    if (datax < offset || datay < offset || dataw < offset || datah < offset) {
        return false;
    }

    return true;
}

bool RSRoundCornerDisplayFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    RSRoundCornerDisplayLoadCfgFuzzTest(data, size);
    RSRoundCornerDisplayGetLcdModelFuzzTest(data, size);
    RSRoundCornerDisplayGetRogFuzzTest(data, size);
    RSRoundCornerDisplayPrintParseRogFuzzTest(data, size);

    RSRoundCornerDisplayAddRoundCornerDisplayFuzzTest(data, size);
    RSRoundCornerDisplayAddLayerFuzzTest(data, size);
    RSRoundCornerDisplayRemoveRCDResourceFuzzTest(data, size);
    RSRoundCornerAPIsFuzzTest(data, size);

    RSRcdSurfaceRenderNodeSetRcdBufferWidthFuzzTest(data, size);
    RSRcdSurfaceRenderNodeFuzzTest01(data, size);
    RSRcdSurfaceRenderNodeCreateSurfaceFuzzTest(data, size);
    RSRcdSurfaceRenderNodeBuffSetGetFuzzTest(data, size);
    RSRcdSurfaceRenderNodePrepareHardwareFuzzTest(data, size);
    RSRcdSurfaceRenderNodeSetHardwareFuzzTest(data, size);
    RSRcdSurfaceIsTypeFuzzTest(data, size);
    RSRcdSurfacePrintResetFuzzTest(data, size);

    RSRcdSurfaceIsInvalidSurfaceFuzzTest(data, size);
    RSRcdSurfaceRenderNodeIsSurfaceCreatedFuzzTest(data, size);
    RSRcdSurfaceRenderNodeGetRSSurfaceFuzzTest(data, size);
    RSRcdSurfaceRenderNodeClearBufferCacheFuzzTest(data, size);
    RSRcdSurfaceRenderNodeFillHardwareResourceFuzzTest(data, size);
    RSRcdSurfaceRenderNodeDataLocalFuzzTest(data, size);

    return true;
}

} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::g_data = data;
    OHOS::g_size = size;
    OHOS::g_pos = 0;

    /* Run your code on data */
    OHOS::RSRoundCornerDisplayFuzzTest(data, size);
    return 0;
}
