/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstdio>
#include "gtest/gtest.h"

#include "drawing_error_code.h"
#include "drawing_path.h"
#include "drawing_path_iterator.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPathIteratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPathIteratorTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingPathIteratorTest::TearDownTestCase() {}
void NativeDrawingPathIteratorTest::SetUp() {}
void NativeDrawingPathIteratorTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPathIteratorTest_001
 * @tc.desc: test for path iterator.
 * @tc.type: FUNC
 * @tc.require: 20650
 */
HWTEST_F(NativeDrawingPathIteratorTest, NativeDrawingPathIteratorTest_AbnormalParameter001, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, 5, 5);
    OH_Drawing_PathQuadTo(path, 10, 50, 100, 100);
    OH_Drawing_PathConicTo(path, 150, 200, 250, 350, 0.5f);
    OH_Drawing_PathCubicTo(path, 350, 450, 450, 550, 550, 650);
    OH_Drawing_PathClose(path);

    OH_Drawing_PathIterator* pathIterator = nullptr;
    EXPECT_EQ(OH_Drawing_PathIteratorCreate(nullptr, &pathIterator), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorCreate(path, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorCreate(path, &pathIterator), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(pathIterator != nullptr);

    bool hasNext = false;
    OH_Drawing_PathIteratorVerb verb = OH_Drawing_PathIteratorVerb::MOVE;
    OH_Drawing_Point2D point2d[28] = {{0, 0}};
    EXPECT_EQ(OH_Drawing_PathIteratorPeek(nullptr, &verb), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorPeek(pathIterator, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorHasNext(nullptr, &hasNext), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorHasNext(pathIterator, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorNext(nullptr, point2d, 28, 0, &verb), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorNext(pathIterator, nullptr, 0, 0, &verb), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorNext(pathIterator, point2d, 28, 0, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorNext(pathIterator, point2d, 28, 26, &verb),
        OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);

    EXPECT_EQ(OH_Drawing_PathIteratorDestroy(nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_PathIteratorDestroy(pathIterator), OH_DRAWING_SUCCESS);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathIteratorTest_pathIterator001
 * @tc.desc: test for path iterator.
 * @tc.type: FUNC
 * @tc.require: 20650
 */
HWTEST_F(NativeDrawingPathIteratorTest, NativeDrawingPathIteratorTest_pathIterator001, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, 5, 5);
    OH_Drawing_PathQuadTo(path, 10, 50, 100, 100);
    OH_Drawing_PathConicTo(path, 150, 200, 250, 350, 0.5f);
    OH_Drawing_PathCubicTo(path, 350, 450, 450, 550, 550, 650);
    OH_Drawing_PathClose(path);
    OH_Drawing_PathIterator* pathIterator = nullptr;
    EXPECT_EQ(OH_Drawing_PathIteratorCreate(path, &pathIterator), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(pathIterator != nullptr);

    bool hasNext = false;
    OH_Drawing_PathIteratorVerb verb = OH_Drawing_PathIteratorVerb::MOVE;
    const int verbSize = 7;
    const int offsetSize = 4;
    const int pointSize = verbSize * offsetSize;
    OH_Drawing_Point2D point2d[pointSize] = {{0, 0}};

    for (int verbNumber = 0; verbNumber < verbSize; ++verbNumber) {
        EXPECT_EQ(OH_Drawing_PathIteratorPeek(pathIterator, &verb), OH_DRAWING_SUCCESS);
        EXPECT_EQ(verb, static_cast<OH_Drawing_PathIteratorVerb>(verbNumber));
        EXPECT_EQ(OH_Drawing_PathIteratorHasNext(pathIterator, &hasNext), OH_DRAWING_SUCCESS);
        EXPECT_EQ(hasNext, verb == OH_Drawing_PathIteratorVerb::DONE ? false : true);
        EXPECT_EQ(OH_Drawing_PathIteratorNext(pathIterator, point2d, pointSize, verbNumber * offsetSize, &verb),
            OH_DRAWING_SUCCESS);
        EXPECT_EQ(verb, static_cast<OH_Drawing_PathIteratorVerb>(verbNumber));
    }

    EXPECT_EQ(OH_Drawing_PathIteratorNext(pathIterator, point2d, pointSize, 6 * offsetSize, &verb),
            OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PathIteratorPeek(pathIterator, &verb), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PathIteratorHasNext(pathIterator, &hasNext), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PathIteratorHasNext(pathIterator, &hasNext), OH_DRAWING_SUCCESS);

    EXPECT_EQ(OH_Drawing_PathIteratorDestroy(pathIterator), OH_DRAWING_SUCCESS);
    OH_Drawing_PathDestroy(path);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
