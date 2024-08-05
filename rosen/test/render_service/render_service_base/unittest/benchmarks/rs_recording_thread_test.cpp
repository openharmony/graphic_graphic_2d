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
 
#ifdef ENABLE_RECORDING_DCL
#include "gtest/gtest.h"
#include "benchmarks/rs_recording_thread.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRecordingThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRecordingThreadTest::SetUpTestCase() {}
void RSRecordingThreadTest::TearDownTestCase() {}
void RSRecordingThreadTest::SetUp() {}
void RSRecordingThreadTest::TearDown() {}

/**
 * @tc.name: CheckAndRecording001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRecordingThreadTest, CheckAndRecording001, TestSize.Level1)
{
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    RSRecordingThread rsRecordingThread(renderContext);
    bool ret = rsRecordingThread.CheckAndRecording();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetCurDumpFrame001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRecordingThreadTest, GetCurDumpFrame001, TestSize.Level1)
{
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    RSRecordingThread rsRecordingThread(renderContext);
    bool ret = rsRecordingThread.GetCurDumpFrame();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RecordingToFile001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRecordingThreadTest, RecordingToFile001, TestSize.Level1)
{
    int w = 150;
    int h = 300;
    auto list = std::make_shared<Drawing::DrawCmdList>(w, h);
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    RSRecordingThread rsRecordingThread(renderContext);
    rsRecordingThread.RecordingToFile(list);
}
} // namespace OHOS::Rosen
#endif // ENABLE_RECORDING_DCL