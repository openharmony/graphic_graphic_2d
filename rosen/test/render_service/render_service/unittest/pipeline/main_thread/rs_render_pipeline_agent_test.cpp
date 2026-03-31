/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "rs_render_pipeline.h"
#include "rs_render_pipeline_agent.h"
#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderPipelineAgentTest : public testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: SetGlobalDarkColorMode_NullPipeline_ReturnInvalidValue
 * @tc.desc: Verify SetGlobalDarkColorMode returns ERR_INVALID_VALUE when pipeline is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, SetGlobalDarkColorMode_NullPipeline_ReturnInvalidValue, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    ErrCode ret = agent->SetGlobalDarkColorMode(true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetGlobalDarkColorMode_ValidPipeline_ReturnOk
 * @tc.desc: Verify SetGlobalDarkColorMode returns ERR_OK when pipeline exists.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, SetGlobalDarkColorMode_ValidPipeline_ReturnOk, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    ErrCode retDark = agent->SetGlobalDarkColorMode(true);
    ErrCode retLight = agent->SetGlobalDarkColorMode(false);
    EXPECT_EQ(retDark, ERR_OK);
    EXPECT_EQ(retLight, ERR_OK);
}

/**
 * @tc.name: DropFrameByPid_NullPipeline_ReturnInvalidValue
 * @tc.desc: Verify DropFrameByPid returns ERR_INVALID_VALUE when pipeline is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, DropFrameByPid_NullPipeline_ReturnInvalidValue, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::vector<int32_t> pidList = { 1001, 1002 };
    constexpr int32_t DROP_FRAME_LEVEL = 1;
    ErrCode ret = agent->DropFrameByPid(pidList, DROP_FRAME_LEVEL);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: DropFrameByPid_ValidPipeline_ReturnOk
 * @tc.desc: Verify DropFrameByPid returns ERR_OK when pipeline exists.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, DropFrameByPid_ValidPipeline_ReturnOk, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    RSMainThread* mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    renderPipeline->mainThread_ = mainThread;

    std::vector<int32_t> pidList = { 1001, 1002 };
    constexpr int32_t DROP_FRAME_LEVEL = 1;

    ErrCode ret = agent->DropFrameByPid(pidList, DROP_FRAME_LEVEL);
    EXPECT_EQ(ret, ERR_OK);
}
} // namespace OHOS::Rosen
