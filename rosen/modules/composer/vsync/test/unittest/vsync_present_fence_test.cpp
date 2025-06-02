/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "vsync_sampler.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class VSyncPresentFenceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void Reset();

    static inline sptr<VSyncSampler> vsyncSampler = nullptr;
};

void VSyncPresentFenceTest::SetUpTestCase()
{
    vsyncSampler = CreateVSyncSampler();
}

void VSyncPresentFenceTest::Reset()
{
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->period_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->phase_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->referenceTime_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->error_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->firstSampleIndex_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->numSamples_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->modeUpdated_ = false;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_ = true;
}

namespace {
/*
* Function: AddPresentFenceTimeErrorTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test AddPresentFenceTime error_ oversize
 */
HWTEST_F(VSyncPresentFenceTest, AddPresentFenceTimeErrorTest, Function | MediumTest| Level3)
{
    Reset();
    vsyncSampler->StartSample(true);
    bool ret = true;
    for (int i = 1; i <= 10; i++) { // add 10 samples
        ret = vsyncSampler->AddSample(i * 16666667); // 16666667ns
    }
    ASSERT_EQ(ret, false);
    vsyncSampler->SetVsyncEnabledScreenId(0);
    for (int i = 1; i <= 10; i++) { // add 10 samples
        ret = vsyncSampler->AddPresentFenceTime(0, i * 16666667); // 16666667ns
    }
    ASSERT_EQ(ret, false);
    for (int i = 1; i <= 10; i++) { // add 10 samples
        ret = vsyncSampler->AddPresentFenceTime(0, i * 16666667 + 1000000); // 16666667ns, 1000000ns
    }
    ASSERT_EQ(ret, true);
    ret = vsyncSampler->AddPresentFenceTime(0, 1666666666); // 1666666666ns
    ASSERT_EQ(ret, true);
    ret = vsyncSampler->AddPresentFenceTime(0, 2666666666); // 2666666666ns
    ASSERT_EQ(ret, true);
    for (int i = 1; i <= 10; i++) { // add 10 samples
        ret = vsyncSampler->AddPresentFenceTime(0, i * 16666667 + 1000000); // 16666667ns, 1000000ns
    }
    ASSERT_EQ(ret, true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS