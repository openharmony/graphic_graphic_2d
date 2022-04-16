/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DRAWING_SAMPLE_H
#define DRAWING_SAMPLE_H

#include <iostream>
#include <string>

#include <display_type.h>
#include <event_handler.h>
#include <surface.h>

#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "hdi_output.h"
#include "hdi_screen.h"
#include "layer_context.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawingSample {
public:
    using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;
    DrawingSample() = default;
    virtual ~DrawingSample() = default;

    void Init();
    void Run();

    inline DrawingSample& SetDraw(std::map<std::string, std::vector<TestFunc>> testFuncMap)
    {
        drawFuncMap_.swap(testFuncMap);
        return *this;
    }
private:
    uint32_t freq_ = 30;
    uint32_t currScreenId_ = 0;
    // key is screenId
    std::unordered_map<uint32_t, uint32_t> displayWidthsMap_;
    std::unordered_map<uint32_t, uint32_t> displayHeightsMap_;
    uint32_t currentModeIndex_ = 0;
    std::vector<DisplayModeInfo> displayModeInfos_;
    // key is screenId
    std::unordered_map<uint32_t, std::shared_ptr<HdiOutput>> outputMap_;
    bool ready_ = false;
    bool initDeviceFinished_ = false;
    bool deviceConnected_ = false;
    bool postHotPlugEvent_ = false;
    HdiBackend* backend_ = nullptr;
    std::vector<std::unique_ptr<HdiScreen>> screens_;
    std::shared_ptr<HdiOutput> curOutput_;
    // key is screenId
    std::unordered_map<uint32_t, std::unique_ptr<LayerContext>> drawLayersMap_;
    std::map<std::string, std::vector<TestFunc>> drawFuncMap_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainThreadHandler_;

    void InitLayers(uint32_t screenId);
    void RequestSync();
    void CreateShowLayers();
    void Draw();
    void Sync(int64_t, void* data);
    void DoPrepareCompleted(sptr<Surface> surface, const struct PrepareCompleteParam& param);
    void OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput>& output, bool connected);
    uint32_t CreatePhysicalScreen();

    static void OnScreenPlug(std::shared_ptr<HdiOutput>& output, bool connected, void* data);
    static void OnPrepareCompleted(
        std::shared_ptr<RSSurfaceOhos> &rsSurface, const struct PrepareCompleteParam& param, void* data);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // DRAWING_SAMPLE_H