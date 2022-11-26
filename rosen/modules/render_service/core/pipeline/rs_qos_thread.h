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

#ifndef RS_QOS_THREAD
#define RS_QOS_THREAD

#include <cstdint>
#include "vsync_distributor.h"

namespace OHOS::Rosen {
class RSQosThread {
public:
    RSQosThread() {};
    ~RSQosThread() {};

    static sptr<VSyncDistributor> appVSyncDistributor_;

    static RSQosThread* GetInstance();
    static void ThreadStart();
    static void ThreadStop();
    static void ResetQosPid();
    void OnRSVisibilityChangeCB(std::map<uint32_t, bool>& pidVisMap);

    void SetQosCal(bool qosCal)
    {
        qosCal_ = qosCal;
    }

    bool GetQosCal() const
    {
        return qosCal_;
    }

private:
    static const int MAX_RATE = 1;
    static const int MIN_RATE = INT_MAX;
    static std::once_flag flag_;
    static RSQosThread* instance_;

    bool qosCal_ = false;

    static void Init();

    static void GetQosVSyncRateInfos(std::vector<std::pair<uint32_t, int>>& appsRateVec);
    static void SetQosVSyncRate(uint32_t pid, int32_t rate);
};
} // namespace OHOS::Rosen

#endif // RS_QOS_THREAD