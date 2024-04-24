/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <test_header.h>

#include "frame_saver.h"
#include "frame_collector.h"
#include "frame_painter.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FrameSaverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FrameSaverTest::SetUpTestCase() {}
void FrameSaverTest::TearDownTestCase() {}
void FrameSaverTest::SetUp() {}
void FrameSaverTest::TearDown() {}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.GenerateTimeBars
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FrameSaverTest, FrameSaverTest, TestSize.Level1)
{
    FrameSaver* frameSaver = new FrameSaver();
    delete frameSaver;
    ASSERT_FALSE(frameSaver->ofs_.is_open());
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.GenerateTimeBars
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FrameSaverTest, SaveFrameEventTest1, TestSize.Level1)
{
    FrameSaver* frameSaver = new FrameSaver();
    frameSaver->SaveFrameEvent(FrameEventType::DrawStart, 6);
    delete frameSaver;
    ASSERT_FALSE(frameSaver->ofs_.is_open());
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.GenerateTimeBars
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FrameSaverTest, SaveFrameEventTest2, TestSize.Level1)
{
    FrameSaver* frameSaver = new FrameSaver();
    frameSaver->ofs_.open("/data/frame_render/8888.log");
    ASSERT_TRUE(frameSaver->ofs_.is_open());
    frameSaver->SaveFrameEvent(FrameEventType::DrawStart, 6);
    delete frameSaver;
    ASSERT_FALSE(frameSaver->ofs_.is_open());
}

} // namespace Rosen
} // namespace OHOS