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

#include <filesystem>
#include <gtest/gtest.h>

#include "frame_collector.h"
#include "frame_saver.h"

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
 * @tc.name: FrameSaverTest
 * @tc.desc: Test FrameSaverTest.FrameSaver::FrameSaver()
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FrameSaverTest, FrameSaverTest1, TestSize.Level1)
{
    FrameSaver* frameSaver = new FrameSaver();
    ASSERT_FALSE(frameSaver->ofs_.is_open());
    delete frameSaver;
    frameSaver = nullptr;
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.GenerateTimeBars
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FrameSaverTest, FrameSaverTest2, TestSize.Level1)
{
    FrameSaver* frameSaver = new FrameSaver();
    frameSaver->SaveFrameEvent(FrameEventType::DrawStart, 6);
    ASSERT_FALSE(frameSaver->ofs_.is_open());
    delete frameSaver;
    frameSaver = nullptr;
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.GenerateTimeBars
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FrameSaverTest, FrameSaverTest3, TestSize.Level1)
{
    FrameSaver* frameSaver = new FrameSaver();
    frameSaver->ofs_.open("/data/frame_render/8888.log", frameSaver->ofs_.out | frameSaver->ofs_.app);
    ASSERT_TRUE(frameSaver->ofs_.is_open());
    frameSaver->SaveFrameEvent(FrameEventType::DrawStart, 123456789);
    std::ifstream ifs("/data/frame_render/8888.log");
    ASSERT_TRUE(ifs.is_open());
    std::string line;
    std::getline(ifs, line);
    ASSERT_EQ(line, "DrawStart 123456789");
    std::filesystem::remove_all(saveDirectory);
    ifs.close();
    delete frameSaver;
    frameSaver = nullptr;
}

} // namespace Rosen
} // namespace OHOS