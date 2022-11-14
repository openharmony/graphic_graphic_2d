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

#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>

#include "pipeline/rs_qos_thread.h"
#include "platform/common/rs_innovation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSQosThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static void* CreateQosService();
    static void QosTreadStart();
    static void QosTreadStop();
    static void QosSetBoundaryRate(int minRate, int maxRate);
    static void QosRegisteFuncCB(std::function<void(uint32_t, int)> setRate,
        std::function<void(std::vector<std::pair<uint32_t, int> >&)> requestCount);
    static void QosOnRSVisibilityChangeCB(std::map<uint32_t, bool>& pidVisMap);
    static void QosOnRSResetPid();

    static void FakeSo();
};

void RSQosThreadTest::SetUpTestCase() {}
void RSQosThreadTest::TearDownTestCase()
{
    RSQosThread::GetInstance()->SetQosCal(false);
}
void RSQosThreadTest::SetUp() {}
void RSQosThreadTest::TearDown() {}

void* RSQosThreadTest::CreateQosService()
{
    return nullptr;
}
void RSQosThreadTest::QosTreadStart() {}
void RSQosThreadTest::QosTreadStop() {}
void RSQosThreadTest::QosSetBoundaryRate(int minRate, int maxRate)
{
    (void)minRate;
    (void)maxRate;
}

void RSQosThreadTest::QosRegisteFuncCB(std::function<void(uint32_t, int)> setRate,
    std::function<void(std::vector<std::pair<uint32_t, int> >&)> requestCount)
{
    (void)setRate;
    (void)requestCount;
}

void RSQosThreadTest::QosOnRSVisibilityChangeCB(std::map<uint32_t, bool>& pidVisMap)
{
    (void)pidVisMap;
}
void RSQosThreadTest::QosOnRSResetPid() {}

void RSQosThreadTest::FakeSo()
{
    RSInnovation::_s_createRSQosService = (void*)CreateQosService;
    RSInnovation::_s_qosThreadStart = (void*)QosTreadStart;
    RSInnovation::_s_qosThreadStop = (void*)QosTreadStop;
    RSInnovation::_s_qosSetBoundaryRate = (void*)QosSetBoundaryRate;
    RSInnovation::_s_qosOnRSVisibilityChangeCB = (void*)QosOnRSVisibilityChangeCB;
    RSInnovation::_s_qosRegisteFuncCB = (void*)QosRegisteFuncCB;
    RSInnovation::_s_qosOnRSResetPid = (void*)QosOnRSResetPid;
    RSInnovation::_s_qosVsyncFuncLoaded = true;
}

HWTEST_F(RSQosThreadTest, QosThreadStartAndStop, TestSize.Level1)
{
    RSQosThread::ThreadStart();
    RSQosThread::ThreadStop();
}

HWTEST_F(RSQosThreadTest,  QosOnRSVisibilityChangeCB, TestSize.Level1)
{
    std::map<uint32_t, bool> pidVisMap;

    // qosCal not setted
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
}

HWTEST_F(RSQosThreadTest, QosFakeSo, TestSize.Level1)
{
    RSInnovation::OpenInnovationSo();
    ASSERT_EQ(RSInnovation::_s_qosVsyncFuncLoaded, false);

    FakeSo();
    ASSERT_EQ(RSInnovation::_s_qosVsyncFuncLoaded, true);

    RSQosThread::GetInstance();

    RSQosThread::ThreadStart();
    RSQosThread::ThreadStop();

    std::map<uint32_t, bool> pidVisMap;
    RSQosThread::GetInstance()->SetQosCal(true);
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
}

} // OHOS::Rosen
