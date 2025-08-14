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

class RenderFrameTraceImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderFrameTraceImplTest::SetUpTestCase() {}
void RenderFrameTraceImplTest::TearDownTestCase() {}
void RenderFrameTraceImplTest::SetUp() {}
void RenderFrameTraceImplTest::TearDown() {}

/**
 * @tc.name: AccessFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, AccessFrameTrace, TestSize.Level1)
{
    bool ret = RenderFrameTraceImpl::instance_->AccessFrameTrace();
    EXPECT_NE(ret, 0);
}

/**
 * @tc.name: RenderFrameTraceOpen
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, RenderFrameTraceOpen, TestSize.Level1)
{
    RenderFrameTraceImpl::instance_->RenderFrameTraceClose();
    RenderFrameTraceImpl::instance_->RenderFrameTraceOpen();
    RenderFrameTraceImpl::instance_->RenderFrameTraceClose();
    bool ret = RenderFrameTraceImpl::instance_->RenderFrameTraceIsOpen();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RenderFrameTraceIsOpen
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, RenderFrameTraceIsOpen, TestSize.Level1)
{
    RenderFrameTraceImpl::instance_->RenderFrameTraceClose();
    bool ret = RenderFrameTraceImpl::instance_->RenderFrameTraceIsOpen();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RenderFrameTraceClose
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, RenderFrameTraceClose, TestSize.Level1)
{
    RenderFrameTraceImpl::instance_->RenderFrameTraceClose();
    bool ret = RenderFrameTraceImpl::instance_->RenderFrameTraceIsOpen();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RenderStartAndEndFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, ui, TestSize.Level1)
{
    const std::string traceTag = "ui";
    bool ret = RenderFrameTraceImpl::instance_->AccessFrameTrace();
    EXPECT_EQ(ret, true);
    RenderFrameTraceImpl::instance_->RenderStartFrameTrace(traceTag);
    RenderFrameTraceImpl::instance_->RenderEndFrameTrace(traceTag);
}

/**
 * @tc.name: RenderStartAndEndFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, renderthread, TestSize.Level1)
{
    const std::string traceTag = "renderthread";
    bool ret = RenderFrameTraceImpl::instance_->AccessFrameTrace();
    EXPECT_EQ(ret, true);
    RenderFrameTraceImpl::instance_->RenderStartFrameTrace(traceTag);
    RenderFrameTraceImpl::instance_->RenderEndFrameTrace(traceTag);
}

/**
 * @tc.name: RenderStartAndEndFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, renderservice, TestSize.Level1)
{
    const std::string traceTag = "renderservice";
    bool ret = RenderFrameTraceImpl::instance_->AccessFrameTrace();
    EXPECT_EQ(ret, true);
    RenderFrameTraceImpl::instance_->RenderStartFrameTrace(traceTag);
    RenderFrameTraceImpl::instance_->RenderEndFrameTrace(traceTag);
}

/**
 * @tc.name: RenderStartAndEndFrameTrace
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RenderFrameTraceImplTest, test, TestSize.Level1)
{
    const std::string traceTag = "test";
    bool ret = RenderFrameTraceImpl::instance_->AccessFrameTrace();
    EXPECT_EQ(ret, true);
    RenderFrameTraceImpl::instance_->RenderStartFrameTrace(traceTag);
    RenderFrameTraceImpl::instance_->RenderEndFrameTrace(traceTag);
}
} // namespace Rosen
} // namespace OHOS
