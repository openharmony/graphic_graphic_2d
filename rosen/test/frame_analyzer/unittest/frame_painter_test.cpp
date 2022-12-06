/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "frame_collector.h"
#include "frame_painter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FramePainterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FramePainterTest::SetUpTestCase() {}
void FramePainterTest::TearDownTestCase() {}
void FramePainterTest::SetUp() {}
void FramePainterTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(FramePainterTest, Create001, TestSize.Level1)
{
    FramePainter* painter_ptr = new FramePainter(FrameCollector::GetInstance());
    ASSERT_TRUE(painter_ptr != nullptr);
}
} // namespace Rosen
} // namespace OHOS
