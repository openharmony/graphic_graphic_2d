/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "text_style.h"

using namespace rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
class OH_Drawing_PlaceHolderTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_PlaceHolderTest001
 * @tc.desc: test for creating PlaceHolder
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_PlaceHolderTest, OH_Drawing_PlaceHolderTest001, TestSize.Level1)
{
    double width = 10.0;
    double height = 5.0;
    PlaceholderAlignment holderAlignment(PlaceholderAlignment::BASELINE);
    TextBaseline textBaseLine(TextBaseline::ALPHABETIC);
    double baseLineOffset = 0.0;
    PlaceholderRun holder(width, height, holderAlignment, textBaseLine, baseLineOffset);
    EXPECT_EQ(holder.width_, width);
    EXPECT_EQ(holder.height_, height);
    EXPECT_EQ(holder.placeholderalignment_, holderAlignment);
    EXPECT_EQ(holder.textbaseline_, textBaseLine);
    EXPECT_EQ(holder.baselineOffset_, baseLineOffset);
}
}