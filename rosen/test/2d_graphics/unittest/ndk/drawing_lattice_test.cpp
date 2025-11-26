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
#include "drawing_lattice.h"
#include "drawing_rect.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingLatticeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingLatticeTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingLatticeTest::TearDownTestCase() {}
void NativeDrawingLatticeTest::SetUp() {}
void NativeDrawingLatticeTest::TearDown() {}

/*
 * @tc.name: NativeDrawingLatticeTest_LatticeCreate001
 * @tc.desc: test for lattice create.
 * @tc.type: FUNC
 * @tc.require: 20652
 */
HWTEST_F(NativeDrawingLatticeTest, NativeDrawingLatticeTest_LatticeCreate001, TestSize.Level1)
{
    OH_Drawing_Lattice* lattice = nullptr;
    int xDivs1[2] = {10, 20};
    int xDivs2[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int yDivs1[2] = {10, 20};
    int yDivs2[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0.0f, 0.0f, 100.0f, 100.0f);
    OH_Drawing_LatticeRectType rectTypes1[9] = {OH_Drawing_LatticeRectType::DEFAULT};
    OH_Drawing_LatticeRectType rectTypes2[9] = {static_cast<OH_Drawing_LatticeRectType>(5)};
    uint32_t colors[9] = {0xffff0000};
    EXPECT_EQ(OH_Drawing_LatticeCreate(nullptr, yDivs1, 2, 2, rect, rectTypes1, 9, colors, 9, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, nullptr, 2, 2, rect, rectTypes1, 9, colors, 9, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs2, yDivs1, 10, 2, rect, rectTypes1, 9, colors, 9, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs2, 2, 10, rect, rectTypes1, 9, colors, 9, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, rect, rectTypes1, 9, colors, 9, nullptr),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, rect, nullptr, 9, colors, 9, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, rect, rectTypes1, 9, nullptr, 9, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, rect, rectTypes1, 5, colors, 9, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, rect, rectTypes1, 9, colors, 5, &lattice),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, rect, rectTypes2, 9, colors, 9, &lattice),
        OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);

    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, rect, rectTypes1, 9, colors, 9, &lattice),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_LatticeDestroy(nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_LatticeDestroy(lattice), OH_DRAWING_SUCCESS);

    EXPECT_EQ(OH_Drawing_LatticeCreate(xDivs1, yDivs1, 2, 2, nullptr, rectTypes1, 9, colors, 9, &lattice),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_LatticeDestroy(lattice), OH_DRAWING_SUCCESS);
    OH_Drawing_RectDestroy(rect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
