/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HELLO_COMPOSER_H
#define HELLO_COMPOSER_H

#include <surface.h>
#include <event_handler.h>
#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "hdi_output.h"
#include "hdi_screen.h"
#include "layer_context.h"

namespace OHOS {
namespace Rosen {
class HelloComposer {
public:
    HelloComposer() = default;
    virtual ~HelloComposer() = default;

    void Init();
    void Run(const std::vector<std::string> &runArgs);

private:
    uint32_t freq_ = 30;
    uint32_t currScreenId_ = 0;
    // Draw color with PIXEL_FMT_YCBCR_420_SP format in LAYER_EXTRA layer, if YUVFormat_ is true
    // Draw color with PIXEL_FMT_RGBA_8888 format in LAYER_EXTRA layer, if YUVFormat_ is false
    bool yuvFormat_ = false;
    bool dump_ = false;
    bool ready_ = false;
    bool initDeviceFinished_ = false;
    bool deviceConnected_ = false;
    bool postHotPlugEvent_ = false;
    bool testClient_ = false;
    bool testLayerRotate_ = false;
    bool testLayerColor_ = false;
    HdiBackend* backend_ = nullptr;
    std::vector<std::unique_ptr<HdiScreen>> screens_;
    std::shared_ptr<HdiOutput> curOutput_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainThreadHandler_;

    /* map: key is screenId */
    std::unordered_map<uint32_t, uint32_t> displayWidthsMap_;
    std::unordered_map<uint32_t, uint32_t> displayHeightsMap_;
    std::unordered_map<uint32_t, std::shared_ptr<HdiOutput>> outputMap_;
    std::unordered_map<uint32_t, std::unique_ptr<HdiScreen>> screensMap_;
    std::unordered_map<uint32_t, std::vector<std::unique_ptr<LayerContext>>> drawLayersMap_;

    void InitLayers(uint32_t screenId);
    void RequestSync();
    void CreateLayers();
    void CreateShowLayers();
    void Draw();
    void Sync(int64_t, void *data);
    void DoPrepareCompleted(sptr<Surface> surface, const struct PrepareCompleteParam &param);
    void OnHotPlug(std::shared_ptr<HdiOutput> &output, bool connected);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected);
    void ParseArgs(const std::vector<std::string> &runArgs);
    void SetRunArgs(const std::unique_ptr<LayerContext> &drawLayer) const;
    void RemoveOffScreenData(uint32_t offScreenId);
    uint32_t CreatePhysicalScreen();

    static void OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data);
    static void OnPrepareCompleted(
        sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data);
};
} // namespace Rosen
} // namespace OHOS

#endif // HELLO_COMPOSER_H