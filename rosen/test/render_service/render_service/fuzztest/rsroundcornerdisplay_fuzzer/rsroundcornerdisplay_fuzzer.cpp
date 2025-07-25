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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

    Rosen::NodeId id = GetData<Rosen::NodeId>();

    manager.RemoveRCDResource(id);

    return true;
}

bool RSRoundCornerDisplayManagerAPIsTest(
    const uint8_t* data, size_t size, Rosen::NodeId lid, RCDLayerType lType, std::string lName, bool isRCDSet = false)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

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

    manager.RegisterRcdMsg();
    manager.UpdateDisplayParameter(id, left, top, width, height);
    manager.UpdateNotchStatus(id, status);
    manager.UpdateOrientationStatus(id, rotation);
    manager.UpdateHardwareResourcePrepared(id, prepared);
    manager.RefreshFlagAndUpdateResource(id);
    manager.GetHardwareInfo(id);
    manager.PrepareHardwareInfo(id);
    manager.GetHardwareInfo(id);
    manager.IsNotchNeedUpdate(id, notchStatus);
    manager.RunHardwareTask(id, task);
    manager.HandleRoundCornerDirtyRect(id, dirtyRect, layerType);
    manager.GetRcdEnable();

    manager.GetLayerPair(layerName);
    manager.CheckLayerIsRCD(layerName);

    std::vector<std::pair<Rosen::NodeId, RCDLayerType>> renderTargetNodeInfoList;
    renderTargetNodeInfoList.emplace_back(id, RCDLayerType::INVALID);
    renderTargetNodeInfoList.emplace_back(id, RCDLayerType::TOP);
    renderTargetNodeInfoList.emplace_back(id, RCDLayerType::BOTTOM);

    auto baseCanvas = std::make_shared<Rosen::Drawing::Canvas>();
    auto canvas = std::make_shared<Rosen::RSPaintFilterCanvas>(baseCanvas.get(), 1.0f);
    manager.DrawRoundCorner(renderTargetNodeInfoList, canvas.get());

    manager.RemoveRCDResource(id);

    return true;
}

bool RSRoundCornerDisplayManagerAPIsFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // rcd stub
    Rosen::NodeId randomId = GetData<Rosen::NodeId>();
    std::string layerName(reinterpret_cast<const char*>(data), size);

    auto rawLayerType = GetData<uint32_t>();
    rawLayerType = rawLayerType % (static_cast<uint32_t>(RCDLayerType::BOTTOM) + 1);
    auto layerType = static_cast<RCDLayerType>(rawLayerType);

    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

    // test manager APIs without add rcd
    RSRoundCornerDisplayManagerAPIsTest(data, size, randomId, layerType, layerName);

    // test manager APIs with add rcd
    manager.AddRoundCornerDisplay(randomId);
    manager.AddLayer(layerName, randomId, layerType);
    RSRoundCornerDisplayManagerAPIsTest(data, size, randomId, layerType, layerName, true);

    return true;
}

bool RSRcdSurfaceRenderNodeSetRcdBufferWidthFuzzTest(const uint8_t* data, size_t size)
{
    int dataNum = 2;
    if (data == nullptr || size < sizeof(Rosen::NodeId) + sizeof(uint32_t) * dataNum) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    Rosen::NodeId nodeId = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(nodeId, type);
    uint32_t width = GetData<uint32_t>();
    node->SetRcdBufferWidth(width);

    return true;
}

bool RSRcdSurfaceRenderNodeFuzzTest01(const uint8_t* data, size_t size)
{
    if (data == nullptr || sizeof(Rosen::NodeId) + size < sizeof(uint32_t)) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    return true;
}

bool RSRcdSurfaceRenderNodeBuffSetGetFuzzTest(const uint8_t* data, size_t size)
{
    int dataNum = 8;
    if (data == nullptr || size < sizeof(Rosen::NodeId) + sizeof(uint32_t) * dataNum) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

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

    g_data = data;
    g_size = size;
    g_pos = 0;

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
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
    if (data == nullptr || size < sizeof(Rosen::NodeId) + sizeof(uint32_t) * dataNum) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);
    uint32_t rcdBufferWidth = GetData<uint32_t>();
    uint32_t rcdBufferHeight = GetData<uint32_t>();
    uint32_t rcdBufferSize = GetData<uint32_t>();
    node->SetRcdBufferWidth(rcdBufferWidth);
    node->SetRcdBufferHeight(rcdBufferHeight);
    node->SetRcdBufferSize(rcdBufferSize);
    return true;
}

bool RSRcdSurfaceIsTypeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || sizeof(Rosen::NodeId) + size < sizeof(uint32_t)) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

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
    if (data == nullptr || sizeof(Rosen::NodeId) + size < sizeof(uint32_t)) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    Rosen::NodeId id = GetData<Rosen::NodeId>();
    Rosen::RCDSurfaceType type = static_cast<Rosen::RCDSurfaceType>(GetData<uint32_t>());
    auto node = Rosen::RSRcdSurfaceRenderNode::Create(id, type);

    node->Reset();

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
    RSRoundCornerDisplayManagerAPIsFuzzTest(data, size);

    RSRcdSurfaceRenderNodeSetRcdBufferWidthFuzzTest(data, size);
    RSRcdSurfaceRenderNodeFuzzTest01(data, size);
    RSRcdSurfaceRenderNodeBuffSetGetFuzzTest(data, size);
    RSRcdSurfaceRenderNodePrepareHardwareFuzzTest(data, size);
    RSRcdSurfaceRenderNodeSetHardwareFuzzTest(data, size);
    RSRcdSurfaceIsTypeFuzzTest(data, size);
    RSRcdSurfacePrintResetFuzzTest(data, size);

    return true;
}

} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::RSRoundCornerDisplayFuzzTest(data, size);
    return 0;
}
