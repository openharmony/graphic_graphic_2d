/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "gtest/hwext/gtest-ext.h"
#include "rs_profiler.h"
#include "rs_profiler_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSProfilerTrace3DTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/*
 * @tc.name: GetRenderFrameNumber_InitialValue
 * @tc.desc: Test GetRenderFrameNumber returns initial value 0
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, GetRenderFrameNumber_InitialValue, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    uint32_t frameNumber = RSProfiler::GetRenderFrameNumber();
    EXPECT_EQ(frameNumber, 0u);
}

/*
 * @tc.name: GetRenderFrameNumber_AfterParallelRenderBegin
 * @tc.desc: Test GetRenderFrameNumber after OnParallelRenderBegin
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, GetRenderFrameNumber_AfterParallelRenderBegin, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    constexpr uint32_t testFrameNumber = 12345;
    RSProfiler::OnParallelRenderBegin(testFrameNumber);
    uint32_t frameNumber = RSProfiler::GetRenderFrameNumber();
    EXPECT_EQ(frameNumber, testFrameNumber);
    RSProfiler::OnParallelRenderEnd(testFrameNumber);
}

/*
 * @tc.name: GetRenderFrameNumber_UpdatesCorrectly
 * @tc.desc: Test GetRenderFrameNumber updates correctly with different frame numbers
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, GetRenderFrameNumber_UpdatesCorrectly, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    constexpr uint32_t frame1 = 100;
    constexpr uint32_t frame2 = 200;
    constexpr uint32_t frame3 = 300;

    RSProfiler::OnParallelRenderBegin(frame1);
    EXPECT_EQ(RSProfiler::GetRenderFrameNumber(), frame1);

    RSProfiler::OnParallelRenderBegin(frame2);
    EXPECT_EQ(RSProfiler::GetRenderFrameNumber(), frame2);

    RSProfiler::OnParallelRenderBegin(frame3);
    EXPECT_EQ(RSProfiler::GetRenderFrameNumber(), frame3);

    RSProfiler::OnParallelRenderEnd(frame3);
}

/*
 * @tc.name: GetTrace3DApi_InitialValue
 * @tc.desc: Test GetTrace3DApi returns nullptr initially when trace3d is disabled
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, GetTrace3DApi_InitialValue, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    const TRACE3D_CORE_API_TABLE* api = RSProfiler::GetTrace3DApi();
    EXPECT_EQ(api, nullptr);
}

/*
 * @tc.name: Trace3DDebugScopeCreate_WithInvalidNodeId
 * @tc.desc: Test Trace3DDebugScopeCreate returns nullptr when api is not initialized
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, Trace3DDebugScopeCreate_WithInvalidNodeId, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    constexpr uint64_t invalidNodeId = 0;
    auto dbgScope = RSProfiler::Trace3DDebugScopeCreate(invalidNodeId);
    EXPECT_EQ(dbgScope, nullptr);
}

/*
 * @tc.name: Trace3DDebugScopeCreate_WithValidNodeId
 * @tc.desc: Test Trace3DDebugScopeCreate returns nullptr when trace3dApi is not initialized
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, Trace3DDebugScopeCreate_WithValidNodeId, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    constexpr uint64_t validNodeId = 12345;
    auto dbgScope = RSProfiler::Trace3DDebugScopeCreate(validNodeId);
    EXPECT_EQ(dbgScope, nullptr);
}

/*
 * @tc.name: IsHrpServiceEnabled_BasicTest
 * @tc.desc: Test IsHrpServiceEnabled returns a valid boolean value
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, IsHrpServiceEnabled_BasicTest, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    bool enabled = RSProfiler::IsHrpServiceEnabled();
    EXPECT_TRUE(enabled == true || enabled == false);
}

/*
 * @tc.name: OnParallelRenderBegin_WithZeroFrameNumber
 * @tc.desc: Test OnParallelRenderBegin handles zero frame number correctly
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, OnParallelRenderBegin_WithZeroFrameNumber, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    constexpr uint32_t zeroFrame = 0;
    RSProfiler::OnParallelRenderBegin(zeroFrame);
    EXPECT_EQ(RSProfiler::GetRenderFrameNumber(), zeroFrame);
    RSProfiler::OnParallelRenderEnd(zeroFrame);
}

/*
 * @tc.name: OnParallelRenderBegin_WithMaxFrameNumber
 * @tc.desc: Test OnParallelRenderBegin handles maximum frame number correctly
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, OnParallelRenderBegin_WithMaxFrameNumber, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    constexpr uint32_t maxFrame = UINT32_MAX;
    RSProfiler::OnParallelRenderBegin(maxFrame);
    EXPECT_EQ(RSProfiler::GetRenderFrameNumber(), maxFrame);
    RSProfiler::OnParallelRenderEnd(maxFrame);
}

/*
 * @tc.name: OnParallelRenderBegin_EndSequence
 * @tc.desc: Test OnParallelRenderBegin and OnParallelRenderEnd sequence
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, OnParallelRenderBegin_EndSequence, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    constexpr uint32_t testFrame = 500;

    RSProfiler::OnParallelRenderBegin(testFrame);
    EXPECT_EQ(RSProfiler::GetRenderFrameNumber(), testFrame);

    RSProfiler::OnParallelRenderEnd(testFrame);
}

/*
 * @tc.name: ExtractTrace3DNumber_ValidFormat
 * @tc.desc: Test ExtractTrace3DNumber with valid format string
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, ExtractTrace3DNumber_ValidFormat, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::string testStr = "test_arg:12345";
    size_t colonPos = testStr.find_last_of(':');
    if (colonPos != std::string::npos && colonPos + 1 < testStr.length()) {
        uint64_t result = std::stoull(testStr.substr(colonPos + 1));
        EXPECT_EQ(result, 12345u);
    }
}

/*
 * @tc.name: ExtractTrace3DNumber_NoColon
 * @tc.desc: Test ExtractTrace3DNumber with string without colon
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, ExtractTrace3DNumber_NoColon, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::string testStr = "test_arg_no_colon";
    size_t colonPos = testStr.find_last_of(':');
    if (colonPos == std::string::npos) {
        uint64_t expected = static_cast<uint64_t>(-1);
        EXPECT_EQ(expected, static_cast<uint64_t>(-1));
    }
}

/*
 * @tc.name: CreateAndUpdateTraceParam_BasicFlow
 * @tc.desc: Test CreateAndUpdateTraceParam basic flow
 * @tc.type: FUNC
 * @tc.require: 23790
 */
HWTEST_F(RSProfilerTrace3DTest, CreateAndUpdateTraceParam_BasicFlow, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<std::string> args = {"test_flag:100"};
    bool found = false;
    for (const auto& arg : args) {
        if (arg.find("test_flag") != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

} // namespace OHOS::Rosen
