/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_processor.h"

#include "platform/common/rs_log.h"
#include "rs_main_thread.h"

namespace OHOS {
namespace Rosen {
bool RSProcessor::Init(ScreenId id, int32_t offsetX, int32_t offsetY, ScreenId mirroredId)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    mirroredId_ = mirroredId;
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSPhysicalScreenProcessor::Init: ScreenManager is nullptr");
        return false;
    }
    screenInfo_ = screenManager->QueryScreenInfo(id);

    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr) {
        renderEngine_ = mainThread->GetRenderEngine();
    }
    if (renderEngine_ == nullptr) {
        return false;
    }

    if (mirroredId_ != INVALID_SCREEN_ID) {
        auto mirroredScreenInfo = screenManager->QueryScreenInfo(mirroredId_);
        CalculateMirrorAdaptiveCoefficient(
            static_cast<float>(screenInfo_.width), static_cast<float>(screenInfo_.height),
            static_cast<float>(mirroredScreenInfo.width), static_cast<float>(mirroredScreenInfo.height)
        );
    }

    // set default render frame config
    renderFrameConfig_.width = static_cast<int32_t>(screenInfo_.width);
    renderFrameConfig_.height = static_cast<int32_t>(screenInfo_.height);
    renderFrameConfig_.strideAlignment = 0x8; // default stride is 8 Bytes.
    renderFrameConfig_.format = PIXEL_FMT_RGBA_8888;
    renderFrameConfig_.usage = HBM_USE_CPU_READ | HBM_USE_MEM_DMA | HBM_USE_MEM_FB;
    renderFrameConfig_.timeout = 0;

    return true;
}

void RSProcessor::CalculateMirrorAdaptiveCoefficient(float curWidth, float curHeight,
    float mirroredWidth, float mirroredHeight)
{
    if (std::fabs(mirroredWidth) < 1e-6 || std::fabs(mirroredHeight) < 1e-6) {
        RS_LOGE("RSSoftwareProcessor::Init mirroredScreen width or height is zero");
        return;
    }
    mirrorAdaptiveCoefficient_ = std::min(curWidth / mirroredWidth, curHeight / mirroredHeight);
}
} // namespace Rosen
} // namespace OHOS
