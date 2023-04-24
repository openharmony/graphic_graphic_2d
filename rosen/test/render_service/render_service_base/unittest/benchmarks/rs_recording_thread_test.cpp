/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "gtest/gtest.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderThreadTest::SetUpTestCase() {}
void RSRenderThreadTest::TearDownTestCase() {}
void RSRenderThreadTest::SetUp() {}
void RSRenderThreadTest::TearDown() {}

/**
 * @tc.name: CheckAndRecording001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, CheckAndRecording001, TestSize.Level1)
{
    bool ret = RSRecordingThread::Instance().CheckAndRecording();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: FinishRecordingOneFrame001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, FinishRecordingOneFrame001, TestSize.Level1)
{
    RSRecordingThread::Instance().FinishRecordingOneFrame();
}

/**
 * @tc.name: RecordingToFile001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, RecordingToFile001, TestSize.Level1)
{
    int w = 150;
    int h = 300;
    auto list = make_shared<DrawCmdList>(w, h);
    RSRecordingThread::Instance().RecordingToFile(list);
}
} // namespace OHOS::Rosen