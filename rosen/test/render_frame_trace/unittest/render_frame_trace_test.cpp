/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "render_frame_trace.h"
#include "render_frame_trace_impl.h"

namespace OHOS {
namespace Rosen {
using namespace testing;
using namespace testing::ext;
using namespace FRAME_TRACE;

class RenderFrameTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderFrameTraceTest::SetUpTestCase() {}
void RenderFrameTraceTest::TearDownTestCase() {}
void RenderFrameTraceTest::SetUp() {}
void RenderFrameTraceTest::TearDown() {}

/**
 * @tc.name: RenderFrameTraceIsOpen
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceTest, RenderFrameTraceIsOpen, TestSize.Level1)
{
    RenderFrameTrace::implInstance_ = &RenderFrameTrace::GetInstance();
    RenderFrameTrace::implInstance_->RenderFrameTraceClose();
    bool ret = RenderFrameTrace::implInstance_->RenderFrameTraceIsOpen();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RenderFrameTraceOpen
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceTest, Open, TestSize.Level1)
{
    RenderFrameTrace::implInstance_ = &RenderFrameTrace::GetInstance();
    RenderFrameTrace::implInstance_->RenderFrameTraceClose();
    RenderFrameTrace::implInstance_->RenderFrameTraceOpen();
    RenderFrameTrace::implInstance_->RenderFrameTraceClose();
    bool ret = RenderFrameTrace::implInstance_->RenderFrameTraceIsOpen();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RenderFrameTraceClose
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceTest, Close, TestSize.Level1)
{
    RenderFrameTrace::implInstance_ = &RenderFrameTrace::GetInstance();
    RenderFrameTrace::implInstance_->RenderFrameTraceClose();
    bool ret = RenderFrameTrace::implInstance_->RenderFrameTraceIsOpen();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RenderStartAndEndFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceTest, ui, TestSize.Level1)
{
    const std::string traceTag = "ui";
    RenderFrameTrace::implInstance_ = &RenderFrameTrace::GetInstance();
    EXPECT_NE(RenderFrameTrace::implInstance_, nullptr);
    RenderFrameTrace::implInstance_->RenderStartFrameTrace(traceTag);
    RenderFrameTrace::implInstance_->RenderEndFrameTrace(traceTag);
}

/**
 * @tc.name: RenderStartAndEndFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceTest, renderthread, TestSize.Level1)
{
    const std::string traceTag = "renderthread";
    RenderFrameTrace::implInstance_ = &RenderFrameTrace::GetInstance();
    EXPECT_NE(RenderFrameTrace::implInstance_, nullptr);
    RenderFrameTrace::implInstance_->RenderStartFrameTrace(traceTag);
    RenderFrameTrace::implInstance_->RenderEndFrameTrace(traceTag);
}

/**
 * @tc.name: RenderStartAndEndFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceTest, renderservice, TestSize.Level1)
{
    const std::string traceTag = "renderservice";
    RenderFrameTrace::implInstance_ = &RenderFrameTrace::GetInstance();
    EXPECT_NE(RenderFrameTrace::implInstance_, nullptr);
    RenderFrameTrace::implInstance_->RenderStartFrameTrace(traceTag);
    RenderFrameTrace::implInstance_->RenderEndFrameTrace(traceTag);
}
} // namespace Rosen
} // namespace OHOS
