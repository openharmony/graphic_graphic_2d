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
#include "securec.h"

#include "common/rs_singleton.h"

namespace OHOS {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;
} // namespace

// must greater than 0
constexpr uint8_t STATUS_FUZZ_BOUNDS = 3;
constexpr Rosen::NodeId ID_FUZZ_BOUNDS = 10;

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

bool RSRoundCornerDisplayTest(const uint8_t* data, size_t size)
{
    // prepare data
    if (data == nullptr || size == 0) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    // test rcd_config
    std::string randomConfig(reinterpret_cast<const char*>(data), size);
    auto w = GetData<int>();
    auto h = GetData<int>();

    auto& rcdCfg = OHOS::Rosen::RSSingleton<OHOS::Rosen::rs_rcd::RCDConfig>::GetInstance();

    rcdCfg.Load(randomConfig);
    rcdCfg.GetLcdModel(std::string(reinterpret_cast<const char*>(data), size));

    rcdCfg.Load(std::string(OHOS::Rosen::rs_rcd::PATH_CONFIG_FILE));
    auto Lcd = rcdCfg.GetLcdModel(OHOS::Rosen::rs_rcd::ATTR_DEFAULT);
    rcdCfg.IsDataLoaded();

    if (Lcd != nullptr) {
        Lcd->GetRog(w, h);
    }

    OHOS::Rosen::rs_rcd::RCDConfig::PrintParseRog(nullptr);
    OHOS::Rosen::rs_rcd::ROGSetting rog;
    rog.width = GetData<int>();
    rog.height = GetData<int>();
    OHOS::Rosen::rs_rcd::RogPortrait rp;
    OHOS::Rosen::rs_rcd::RogLandscape rl;
    rog.portraitMap["a"] = rp;
    rog.landscapeMap["b"] = rl;
    OHOS::Rosen::rs_rcd::RCDConfig::PrintParseRog(&rog);

    // test rcd_display_manager
    auto& manager = Rosen::RSSingleton<Rosen::RoundCornerDisplayManager>::GetInstance();

    Rosen::NodeId id = GetData<Rosen::NodeId>() % ID_FUZZ_BOUNDS;
    Rosen::NodeId idWrong = GetData<Rosen::NodeId>() % ID_FUZZ_BOUNDS;
    uint32_t u32_1 = GetData<uint32_t>();
    uint32_t u32_2 = GetData<uint32_t>();
    uint32_t u32_3 = GetData<uint32_t>();
    uint32_t u32_4 = GetData<uint32_t>();

    int status = static_cast<int>(GetData<uint8_t>() % STATUS_FUZZ_BOUNDS) - 1;

    auto rawRotation = GetData<uint32_t>();
    rawRotation = rawRotation % (static_cast<uint32_t>(Rosen::ScreenRotation::INVALID_SCREEN_ROTATION) + 1);
    auto rotation = static_cast<Rosen::ScreenRotation>(rawRotation);

    auto rawPrepared = GetData<uint8_t>();
    auto rawNotchStatus = GetData<uint8_t>();

    bool prepared = (rawPrepared % 2) != 0;
    bool notchStatus = (rawNotchStatus % 2) != 0;

    using RCDLayerType = Rosen::RoundCornerDisplayManager::RCDLayerType;
    auto rawLayerType = GetData<uint32_t>();
    rawLayerType = rawLayerType % (static_cast<uint32_t>(RCDLayerType::BOTTOM) + 1);
    auto layerType = static_cast<RCDLayerType>(rawLayerType);

    std::function<void()> task = []() { std::string stub; };
    Rosen::RectI dirtyRect;

    std::string layerName1(reinterpret_cast<const char*>(data), size);
    std::string layerName2(reinterpret_cast<const char*>(data), size / 2);

    manager.RegisterRcdMsg();

    manager.AddRoundCornerDisplay(id);
    manager.AddLayer(layerName1, id, layerType);

    manager.UpdateDisplayParameter(id, u32_1, u32_2, u32_3, u32_4);
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

    manager.GetLayerPair(layerName1);
    manager.CheckLayerIsRCD(layerName1);
    manager.GetLayerPair(layerName2);
    manager.CheckLayerIsRCD(layerName2);

    std::vector<std::pair<Rosen::NodeId, RCDLayerType>> renderTargetNodeInfoList1;
    renderTargetNodeInfoList1.emplace_back(id, RCDLayerType::INVALID);
    renderTargetNodeInfoList1.emplace_back(id, RCDLayerType::TOP);
    renderTargetNodeInfoList1.emplace_back(id, RCDLayerType::BOTTOM);

    auto baseCanvas1 = std::make_shared<Rosen::Drawing::Canvas>();
    auto canvas1 = std::make_shared<Rosen::RSPaintFilterCanvas>(baseCanvas1.get(), 1.0f);
    manager.DrawRoundCorner(renderTargetNodeInfoList1, canvas1.get());

    manager.GetRcdEnable();

    manager.RemoveRCDResource(id);
    manager.RemoveRCDResource(id);

    // wrong ID
    manager.UpdateDisplayParameter(idWrong, u32_1, u32_2, u32_3, u32_4);
    manager.UpdateNotchStatus(idWrong, status);
    manager.UpdateOrientationStatus(idWrong, rotation);
    manager.UpdateHardwareResourcePrepared(idWrong, prepared);
    manager.RefreshFlagAndUpdateResource(idWrong);
    manager.GetHardwareInfo(idWrong);
    manager.PrepareHardwareInfo(idWrong);
    manager.IsNotchNeedUpdate(idWrong, notchStatus);
    manager.RunHardwareTask(idWrong, task);
    manager.HandleRoundCornerDirtyRect(idWrong, dirtyRect, layerType);

    manager.AddLayer(layerName1, idWrong, layerType);

    std::vector<std::pair<Rosen::NodeId, RCDLayerType>> renderTargetNodeInfoList2;
    renderTargetNodeInfoList2.emplace_back(idWrong, RCDLayerType::INVALID);
    renderTargetNodeInfoList2.emplace_back(idWrong, RCDLayerType::TOP);
    renderTargetNodeInfoList2.emplace_back(idWrong, RCDLayerType::BOTTOM);

    auto baseCanvas2 = std::make_shared<Rosen::Drawing::Canvas>();
    auto canvas2 = std::make_shared<Rosen::RSPaintFilterCanvas>(baseCanvas2.get(), 1.0f);
    manager.DrawRoundCorner(renderTargetNodeInfoList2, canvas2.get());

    manager.RemoveRCDResource(idWrong);

    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::RSRoundCornerDisplayTest(data, size);
    return 0;
}
