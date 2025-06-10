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

#ifndef RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_H
#define RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_H
#include <cstdint>
#include <mutex>
#include <deque>
#include <set>
#include <condition_variable>
#include "common/rs_macros.h"
#include "hpae_base/rs_hpae_hianimation_types.h"

namespace OHOS::Rosen {

class RSB_EXPORT HianimationManager final {
public:
    HianimationManager(const HianimationManager &) = delete;
    HianimationManager &operator==(const HianimationManager &) = delete;

    static HianimationManager &GetInstance();

    void OpenDeviceAsync();
    void AlgoInitAsync(uint32_t imgWidth, uint32_t imgHeight, float maxSigma, uint32_t format);
    void AlgoDeInitAsync();

    void OpenDevice();
    void CloseDevice();

    bool HianimationInputCheck(const struct BlurImgParam *ImgInfo, const struct HaeNoiseValue *noisePara);

    int32_t HianimationAlgoInit(uint32_t imgWidth, uint32_t imgHeight, float maxSigma, uint32_t format);

    int32_t HianimationAlgoDeInit();

    int32_t HianimationBuildTask(const struct HaeBlurBaseicAttr *basicInfo,
        const struct HaeBlurEffectAttr *effectInfo, uint32_t *outTaskId, void **outTaskPtr);

    void* HianimationCommitTask(void* taskFunc, uint32_t taskId, std::vector<void*> hpaeDependence);

    int32_t HianimationDestroyTask(uint32_t taskId);

    bool HianimationInvalid();

    int32_t HianimationDestroyTaskAndNotify(uint32_t taskId);

    bool WaitPreviousTask();

    bool WaitAllTaskDone();

    void WaitAlgoInit();

    void HianimationPerfTrack();

private:
    std::mutex mutex_; // mutex for public api

    HianimationManager();
    ~HianimationManager();

    void WaitHpaeDone();
    void NotifyHpaeDone();

    hianimation_algo_device_t *hianimationDevice_;
    void* libHandle_;

    std::mutex hpaePerfMutex_;
    bool hpaePerfDone_ = true;
    std::condidition_variable hpaePerfCv_;

    int openFailNum_ = 0;

    std::set<uint32_t> taskIdMap_;
    std::condition_variable taskAvaliableCv_;

    std::mutex algoInitMutex_;
    bool algoInitDone_ = true;
    std::condition_variable algoInitCv_;
};

} // OHOS::Rosen

#endif // RENDER_SERVICE_BASE_RS_HPAE_HIANIMATION_H
