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
#include <securec.h>

#include "pipeline/rs_draw_cmd.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSDrawCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawCmdTest::SetUpTestCase() {}
void RSDrawCmdTest::TearDownTestCase() {}
void RSDrawCmdTest::SetUp() {}
void RSDrawCmdTest::TearDown() {}

/**
 * @tc.name: RectOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, RectOpItem001, TestSize.Level1)
{
    SkRect rect;
    SkPaint paint;
    auto op = std::make_unique<RectOpItem>(rect, paint);
    Parcel parcel;
    char* buffer = static_cast<char *>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) & (RectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: RoundRectOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, RoundRectOpItem001, TestSize.Level1)
{
    SkRRect rrect;
    SkPaint paint;
    auto op = std::make_unique<RoundRectOpItem>(rrect, paint);
    Parcel parcel;
    char* buffer = static_cast<char *>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) & (RoundRectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: ImageWithParmOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, ImageWithParmOpItem001, TestSize.Level1)
{
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    SkPaint paint;
    auto op = std::make_unique<ImageWithParmOpItem>(rsImage, paint);
    Parcel parcel;
    char* buffer = static_cast<char *>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) & (ImageWithParmOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DRRectOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, DRRectOpItem001, TestSize.Level1)
{
    SkRRect outer;
    SkRRect inner;
    SkPaint paint;
    auto op = std::make_unique<DRRectOpItem>(outer, inner, paint);
    Parcel parcel;
    char* buffer = static_cast<char *>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) & (ImageWithParmOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}
} // namespace Rosen
} // namespace OHOS
