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
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "pipline/rs_qos_thread.h"
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

    static bool threadStarted;
    static bool visibleChangeCB;
    static void QosTreadStartTest();
    static void QosTreadStopTest();
    static void QosOnRSVisibilityChangeCB(std::map<uint32_t, bool>& pidVisMap);
};
bool RSQosThreadTest::threadStarted = false;
bool RSQosThreadTest::visibleChangeCB = false;

void RSQosThreadTest::SetUpTestCase() {}
void RSQosThreadTest::TearDownTestCase() {
    RSInnovation::_s_qosVsyncFuncLoaded = false;
    RSInnovation::_s_createRSQosService = nullptr;
    RSInnovation::_s_qosThreadStart = nullptr;
    RSInnovation::_s_qosThreadStop = nullptr;
    RSInnovation::_s_qosSetBoundaryRate = nullptr;
    RSInnovation::_s_qosOnRSVisibilityChangeCB = nullptr;
    RSInnovation::_s_qosRegisteFuncCB = nullptr;
    RSInnovation::_s_qosOnRSResetPid = nullptr;
    RSQosThread::GetInstance()->SetQosCal(false);
    RSInnovation::OpenInnovationSo();
}
void RSQosThreadTest::SetUp() {}
void RSQosThreadTest::TearDown() {}
void RSQosThreadTest::QosTreadStartTest() { threadStarted = true; }
void RSQosThreadTest::QosTreadStopTest() { threadStarted = false; }
void RSQosThreadTest::QosOnRSVisibilityChangeCB(std::map<uint32_t, bool>& pidVisMap) { visibleChangeCB = true; }

/*
 * @tc.name: QosThreadStartAndStop
 * @tc.desc: 
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSQosThreadTest, QosThreadStartAndStop, TestSize.Level1)
{
    // so not loaded 
    RSQosThread::GetInstance()->ThreadStart();
    ASSERT_TRUE(!threadStarted);
    RSQosThread::GetInstance()->ThreadStop();
    ASSERT_TRUE(!threadStarted);

    // so and func is loaded
    RSInnovation::_s_qosVsyncFuncLoaded = true;
    RSInnovation::_s_qosThreadStart = (void*)RSQosThreadTest::QosTreadStartTest();
    RSInnovation::_s_qosThreadStop = (void*)RSQosThreadTest::QosTreadStopTest();
    RSQosThread::GetInstance()->ThreadStart();
    ASSERT_TRUE(threadStarted);
    RSQosThread::GetInstance()->ThreadStop();
    ASSERT_TRUE(!threadStarted);
}

/*
 * @tc.name: QosOnRSVisibilityChangeCB
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSQosThreadTest, QosOnRSVisibilityChangeCB, TestSize.Level1)
{
    std::map<uint32_t, bool> pidVisMap;
    RSInnovation::_s_qosOnRSVisibilityChangeCB = (void*)QosOnRSVisibilityChangeCB;

    // qosCal not setted
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
    ASSERT_TRUE(!visibleChangeCB);

    // qosCal not is setted
    RSQosThread::GetInstance()->SetQosCal(true);
    bool qosCal = RSQosThread::GetInstance()->GetQosCal();
    ASSERT_EQ(qosCal, true);
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
    ASSERT_TRUE(visibleChangeCB);
}

} // OHOS::Rosen
