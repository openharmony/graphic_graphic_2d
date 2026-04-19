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
#include "placeholder_run.h"
#include "text_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace SPText {

class PlaceholderRunTest : public testing::Test {};

/**
 * @tc.name: PlaceholderRun001
 * @tc.desc: Test PlaceholderRun default and parameterized constructors
 * @tc.type: FUNC
 */
HWTEST_F(PlaceholderRunTest, PlaceholderRun001, TestSize.Level0)
{
    // Default constructor - zero values
    PlaceholderRun run1;
    EXPECT_EQ(run1.width, 0.0);
    EXPECT_EQ(run1.height, 0.0);
    EXPECT_EQ(run1.alignment, PlaceholderAlignment::BASELINE);
    EXPECT_EQ(run1.baseline, TextBaseline::ALPHABETIC);
    EXPECT_EQ(run1.baselineOffset, 0.0);

    // Parameterized constructor - BASELINE alignment
    PlaceholderRun run2(100.0, 50.0, PlaceholderAlignment::BASELINE, TextBaseline::IDEOGRAPHIC, 10.0);
    EXPECT_EQ(run2.width, 100.0);
    EXPECT_EQ(run2.height, 50.0);
    EXPECT_EQ(run2.alignment, PlaceholderAlignment::BASELINE);
    EXPECT_EQ(run2.baseline, TextBaseline::IDEOGRAPHIC);
    EXPECT_EQ(run2.baselineOffset, 10.0);

    // Parameterized constructor - TOP alignment
    PlaceholderRun run3(50.0, 50.0, PlaceholderAlignment::TOP, TextBaseline::ALPHABETIC, 0.0);
    EXPECT_EQ(run3.width, 50.0);
    EXPECT_EQ(run3.height, 50.0);
    EXPECT_EQ(run3.alignment, PlaceholderAlignment::TOP);

    // Parameterized constructor - ABOVE_BASELINE alignment
    PlaceholderRun run4(75.0, 75.0, PlaceholderAlignment::ABOVE_BASELINE, TextBaseline::ALPHABETIC, 0.0);
    EXPECT_EQ(run4.alignment, PlaceholderAlignment::ABOVE_BASELINE);

    // Parameterized constructor - BELOW_BASELINE alignment
    PlaceholderRun run5(60.0, 40.0, PlaceholderAlignment::BELOW_BASELINE, TextBaseline::ALPHABETIC, 0.0);
    EXPECT_EQ(run5.alignment, PlaceholderAlignment::BELOW_BASELINE);

    // Parameterized constructor - MIDDLE alignment
    PlaceholderRun run6(80.0, 80.0, PlaceholderAlignment::MIDDLE, TextBaseline::ALPHABETIC, 15.0);
    EXPECT_EQ(run6.alignment, PlaceholderAlignment::MIDDLE);
    EXPECT_EQ(run6.baselineOffset, 15.0);
}

} // namespace SPText
} // namespace Rosen
} // namespace OHOS
