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

#include <display_type.h>
#include <surface.h>
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

    void Run(std::vector<std::string> &runArgs);

private:
    uint32_t freq_ = 30;
    uint32_t extraLayerWidth_ = 100;
    uint32_t extraLayerHeight_ = 100;
    uint32_t displayWidth_ = 360;
    uint32_t displayHeight_ = 720;
    uint32_t currentModeIndex_ = 0;
    std::vector<DisplayModeInfo> displayModeInfos_;
    std::vector<std::shared_ptr<HdiOutput>> outputs_;
    bool dump_ = false;
    bool ready_ = false;
    bool initDeviceFinished_ = false;
    bool deviceConnected_ = false;
    HdiBackend* backend_ = nullptr;
    std::unique_ptr<HdiScreen> screen_ = nullptr;
    std::shared_ptr<HdiOutput> output_ = nullptr;
    std::vector<std::unique_ptr<LayerContext>> drawLayers;

    void Init();
    void DrawFrameBufferData(void *image, uint32_t width, uint32_t height);
    void Draw();
    void DrawSurface();
    void Sync(int64_t, void *data);
    void CreatePhysicalScreen();
    void DoPrepareCompleted(sptr<Surface> &surface, const struct PrepareCompleteParam &param);
    void CreateBaseSurface(uint32_t index);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected);

    static void OnScreenPlug(std::shared_ptr<HdiOutput> &output, bool connected, void* data);
    static void OnPrepareCompleted(OHOS::sptr<Surface> &surface, const struct PrepareCompleteParam &param, void* data);
};
} // namespace Rosen
} // namespace OHOS

#endif // HELLO_COMPOSER_H