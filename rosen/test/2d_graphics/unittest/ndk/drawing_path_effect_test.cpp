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

#include "gtest/gtest.h"
#include "drawing_path_effect.h"
#include "drawing_pen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPathEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPathEffectTest::SetUpTestCase() {}
void NativeDrawingPathEffectTest::TearDownTestCase() {}
void NativeDrawingPathEffectTest::SetUp() {}
void NativeDrawingPathEffectTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPathEffectTest_PathEffect001
 * @tc.desc: test for PathEffect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_PathEffect001, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float intervals[] = {1, 1, 1};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 3, 0.0);
    OH_Drawing_PenSetPathEffect(nullptr, pathEffect);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS