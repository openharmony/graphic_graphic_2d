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

#ifndef HPAE_RS_HPAE_MANAGER_H
#define HPAE_RS_HPAE_MANAGER_H

#if defined(ROSEN_OHOS)

#include <cinttypes>
#include <memory>
#include <mutex>
#include <atomic>

#include "feature/hpae/rs_hpae_buffer.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_base_types.h"

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSRenderNode;
class RSScreenRenderNode;

enum class HpaeFrameState {
    IDLE = 0,
    ACTIVE,
    DEACTIVE,
    WORKING,
    SWITCH_BLUR,
    CHANGE_CONFIG,
    ALLOC_BUF,
    WAITING,
};

// Use in Prepare to collect info for HPAE blur
class RSHpaeManager {
public:
    static RSHpaeManager& GetInstance();

    // Functions @Prepare()
    void InitHpaeBlurResource();
    void OnUniRenderStart(); // happens before prepare
    void OnSync(bool isHdrOn);
    void RegisterHpaeCallback(RSRenderNode& node, std::shared_ptr<RSScreenRenderNode> screenNode);

    // Functions @Process
    bool HasHpaeBlurNode() const;

    void CheckHpaeBlur(bool isHdrOn, GraphicPixelFormate pixelFormat, GraphicColorGamut colorSpace, bool isHebc);

    void InitIoBuffers();
    void SetUpHpaeSurface(GraphicPixelFormat pixelFormat, GraphicColorGamut colorSpace, bool isHebc);

    bool IsFirstFrame();

private:
    RSHpaeManager();
    ~RSHpaeManager();

    void ReleaseIoBuffers();

    bool IsHpaeBlurNode(RSRenderNode& node, uint32_t phyWidth, uint32_t phyHeight);

    void AllocBuffer(const BufferRequestConfig& bufferConfig, bool isHebc);
    void SetUpSurfaceIn(const BufferRequestConfig& bufferConfig, bool isHebc);
    void SetUpSurfaceOut(const BufferRequestConfig& bufferConfig, bool isHebc);

    void UpdateHpaeState();
    void HandleHpaeStateChange();
    bool UpdateBufferIfNeed(const BufferRequestConfig& bufferConfig, bool isHebc);

    void OnActive();
    void OnDeActive();

private:
    HpaeStatus stagingHpaeStatus_;
    HpaeStatus hpaeStatus_;
    HpaeFrameState hpaeFrameState_ = HpaeFrameState::IDLE; // for Process()

    std::shared_ptr<DrawableV2::RSHpaeBuffer> hpaeBufferIn_;
    std::shared_ptr<DrawableV2::RSHpaeBuffer> hpaeBufferOut_;

    std::mutex releaseIoBuffersMutex_;
    std::vector<HpaeBufferInfo> inBufferVec_;
    std::vector<HpaeBufferInfo> outBufferVec_;

    // store RSRenderFrame to avoid flush at destruction
    std::unique_ptr<RSRenderFrame> layerFrameIn_[HPAE_BUFFER_SIZE];
    std::unique_ptr<RSRenderFrame> layerFrameOut_[HPAE_BUFFER_SIZE];

    // buffer queue
    uint32_t curIndex_ = 0;

    std::atomic_bool releaseAllDone_{true};
    std::atomic_bool hpaeBufferAllocating_{false};
    std::atomic_bool hpaeBufferNeedInit_{true};

    BufferRequestConfig prevBufferConfig_ {};
    bool prevIsHebc_ = true;
};
} // namespace Rosen
} // namespace OHOS

#endif // (ROSEN_OHOS)
#endif // HPAE_RS_HPAE_MANAGER_H