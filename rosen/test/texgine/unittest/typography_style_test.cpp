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

#include "typography_style.h"

#include <gtest/gtest.h>
#include <unicode/ubidi.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TypographyStyleTest : public testing::Test {
};

/**
 * @tc.name: GetEquivalentAlign1
 * @tc.desc: Verify the TypographyStyle
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(TypographyStyleTest, GetEquivalentAlign1, TestSize.Level1)
{
    std::shared_ptr<TypographyStyle> tpStyle = std::make_shared<TypographyStyle>();
    tpStyle->direction = TextDirection::LTR;
    tpStyle->align = TextAlign::LEFT;
    ASSERT_EQ(tpStyle->GetEquivalentAlign(), TextAlign::LEFT);
}

/**
 * @tc.name: GetEquivalentAlign2
 * @tc.desc: Verify the TypographyStyle
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(TypographyStyleTest, GetEquivalentAlign2, TestSize.Level1)
{
    std::shared_ptr<TypographyStyle> tpStyle = std::make_shared<TypographyStyle>();
    tpStyle->direction = TextDirection::RTL;
    tpStyle->align = TextAlign::RIGHT;
    ASSERT_EQ(tpStyle->GetEquivalentAlign(), TextAlign::RIGHT);
}

/**
 * @tc.name: GetEquivalentAlign3
 * @tc.desc: Verify the TypographyStyle
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(TypographyStyleTest, GetEquivalentAlign3, TestSize.Level1)
{
    std::shared_ptr<TypographyStyle> tpStyle = std::make_shared<TypographyStyle>();
    tpStyle->direction = TextDirection::LTR;
    tpStyle->align = TextAlign::END;
    ASSERT_EQ(tpStyle->GetEquivalentAlign(), TextAlign::RIGHT);
}

/**
 * @tc.name: GetEquivalentAlign4
 * @tc.desc: Verify the TypographyStyle
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(TypographyStyleTest, GetEquivalentAlign4, TestSize.Level1)
{
    std::shared_ptr<TypographyStyle> tpStyle = std::make_shared<TypographyStyle>();
    tpStyle->direction = TextDirection::RTL;
    tpStyle->align = TextAlign::END;
    ASSERT_EQ(tpStyle->GetEquivalentAlign(), TextAlign::LEFT);
}

/**
 * @tc.name: GetEquivalentAlign5
 * @tc.desc: Verify the TypographyStyle
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(TypographyStyleTest, GetEquivalentAlign5, TestSize.Level1)
{
    std::shared_ptr<TypographyStyle> tpStyle = std::make_shared<TypographyStyle>();
    tpStyle->direction = TextDirection::LTR;
    tpStyle->align = TextAlign::JUSTIFY;
    ASSERT_EQ(tpStyle->GetEquivalentAlign(), TextAlign::JUSTIFY);
}

/**
 * @tc.name: GetEquivalentAlign6
 * @tc.desc: Verify the TypographyStyle
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(TypographyStyleTest, GetEquivalentAlign6, TestSize.Level1)
{
    std::shared_ptr<TypographyStyle> tpStyle = std::make_shared<TypographyStyle>();
    tpStyle->direction = TextDirection::RTL;
    tpStyle->align = TextAlign::END;
    ASSERT_EQ(tpStyle->GetEquivalentAlign(), TextAlign::LEFT);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
