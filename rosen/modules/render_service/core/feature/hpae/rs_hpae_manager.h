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

#include <cinttypes>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "hpae_base/rs_hpae_base_types.h"
#include "hpae_base/rs_hpae_data.h"
#include "common/rs_common_def.h"
#include "feature/hpae/rs_hpae_buffer.h"

namespace OHOS {
namesapce Rosen {
class RSRenderNode;

enum class HpaeFrameState {
    HPAE_STATE_IDLE = 0,
    HPAE_STATE_ACTIVE,
    HPAE_STATE_DEACTIVE,
    HPAE_STATE_WORKING,
    HPAE_STATE_SWITCH_BLUR,
};

/*1.该类在prepare阶段收集模糊视效节点信息
  2、根据流程化的需要，及时启动如AAE硬件的初始化
  3.启动和维护需要的线程
  4、AAE 模糊需要的输入输出缓存
  5.资源的释放*/
class RSHpaeManager {
public:
    static RSHpaeManager& GetInstance();

    // Functions @Prepare()
    void InitHpaeBlurResource();
    void OnUniRenderStart(); // happens before prepare
    void OnSync(bool isHdrOn);
    void RegisterHpaeCallback(RSRenderNode& node, uint32_t phyWidth, uint32_t phyWeight);

    // Function @Process
    bool HasHpaeBlurNode() const;

    void InitToBuffers();
    void SetUpHpaeSurface(GraphicPixelFormat pixelFormat, GraphicColorGamut colorSpace, bool isHebc);

    bool IsFirstFrame();

    bool GetInTargetSurface() const;
    void CheckInTargetSurface(const std::string& nodeName);
    void SetInTargetSurface(bool status);

private:
    RSHpaeManager();
    ~RSHpaeManager();

    bool IsHpaeBlurNode(RSRenderNode& node, uint32_t phyWidth, uint32_t phyWeight);

    void SetUpSurfaceIn(const BufferRequestConfig& bufferConfig, bool isHebc);
    void SetUpSurfaceOut(const BufferRequestConfig& bufferConfig, bool isHebc);

    void WaitPreviousReleaseAll();

private:
    HpaeStatus stagingHpaeStatus_;
    HpaeStatus hpaeStatus_;
    HpaeFrameState hpaeFrameState_; // for Process()

    uint32_t hpaeBufferWidth_ = 0;
    uint32_t hpaeBufferHeight = 0;

    std::shared_ptr<DrawableV2::RSHpaeBuffer> hpaeBufferIn_;
    std::shared_ptr<DrawableV2::RSHpaeBuffer> hpaeBufferOut_;

    std::vector<HpaeBufferInfo> inBufferVec_;
    std::vector<HpaeBufferInfo> outBufferVec_;

    // store RSRenderFrame to avoid flush at destruction
    std::unique_ptr<RSRenderFrame> layerFrameIn_[HPAE_BUFFER_SIZE];
    std::unique_ptr<RSRenderFrame> layerFrameoUT_[HPAE_BUFFER_SIZE];

    // buffer queue
    uint32_t curIndex_ = 0;

    bool inTargetSurface_ = false;

    std::mutex releaseAllMutex_;
    bool releaseAllDone_ = true;
    std::condition_variable releaseAllCv_;

    BufferRequestConfig prevBufferConfig_;
    bool prevIsHebc_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // HPAE_RS_HPAE_MANAGER_H