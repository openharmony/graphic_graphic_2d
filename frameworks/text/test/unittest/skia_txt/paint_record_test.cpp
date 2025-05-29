/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "paint_record.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::SPText;

namespace txt {
class PaintRecordTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<PaintRecord> paintRecord_ = nullptr;
};

void PaintRecordTest::SetUpTestCase()
{
    paintRecord_ = std::make_shared<PaintRecord>();
    if (!paintRecord_) {
        std::cout << "PaintRecordTest::SetUpTestCase error paintRecord_ is nullptr" << std::endl;
    }
}

void PaintRecordTest::TearDownTestCase()
{
}

/*
 * @tc.name: PaintRecordTest001
 * @tc.desc: test for SetColor
 * @tc.type: FUNC
 */
HWTEST_F(PaintRecordTest, PaintRecordTest001, TestSize.Level1)
{
    EXPECT_EQ(paintRecord_ != nullptr, true);
    RSBrush brush;
    RSPen pen;
    PaintRecord paintRecord(brush, pen);
    PaintRecord::RSColor color(0, 0, 0, 255); // 255 just fot test
    paintRecord.SetColor(color);
    EXPECT_EQ(paintRecord.color.GetAlpha() != 0, true);
}

/*
 * @tc.name: PaintRecordTest002
 * @tc.desc: test for SetColor
 * @tc.type: FUNC
 */
HWTEST_F(PaintRecordTest, PaintRecordTest002, TestSize.Level1)
{
    EXPECT_EQ(paintRecord_ != nullptr, true);
    RSBrush rsBrush;
    RSPen rsPen;
    std::optional<RSBrush> brush(rsBrush);
    std::optional<RSPen> pen(rsPen);
    PaintRecord paintRecord(brush, pen);
    SkColor color = 255; // 255 just fot test
    paintRecord.SetColor(color);
    EXPECT_EQ(paintRecord.color.GetBlue() != 0, true);
}

/*
 * @tc.name: PaintRecordTest003
 * @tc.desc: test for Equal
 * @tc.type: FUNC
 */
HWTEST_F(PaintRecordTest, PaintRecordTest003, TestSize.Level1)
{
    EXPECT_EQ(paintRecord_ != nullptr, true);
    RSBrush brush;
    RSPen pen;
    PaintRecord paintRecord(brush, pen);
    EXPECT_EQ(paintRecord == (*paintRecord_), false);
}

/*
 * @tc.name: PaintRecordTest004
 * @tc.desc: test for Unequal
 * @tc.type: FUNC
 */
HWTEST_F(PaintRecordTest, PaintRecordTest004, TestSize.Level1)
{
    EXPECT_EQ(paintRecord_ != nullptr, true);
    RSBrush brush;
    RSPen pen;
    PaintRecord paintRecord(brush, pen);
    EXPECT_EQ(paintRecord != (*paintRecord_), true);
}
} // namespace txt