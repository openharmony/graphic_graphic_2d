/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "drawing_path.h"
#include "drawing_matrix.h"
#include "drawing_rect.h"
#include "drawing_round_rect.h"
#include "draw/path.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPathTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPathTest::SetUpTestCase() {}
void NativeDrawingPathTest::TearDownTestCase() {}
void NativeDrawingPathTest::SetUp() {}
void NativeDrawingPathTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPathTest_pathCreate001
 * @tc.desc: test for create drawing_path.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathCreate001, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_EQ(path == nullptr, false);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathTest_pathMoveTo002
 * @tc.desc: test for PathMoveTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathMoveTo002, TestSize.Level1)
{
    OH_Drawing_Path* path1 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path1, 20, 20);
    OH_Drawing_PathMoveTo(path1, -1, 21.5);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path1)->GetBounds().GetWidth(), 21.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path1)->GetBounds().GetHeight(), 1.5));
    OH_Drawing_PathDestroy(path1);
}

/*
 * @tc.name: NativeDrawingPathTest_pathLineTo003
 * @tc.desc: test for PathLineTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathLineTo003, TestSize.Level1)
{
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path2, 50, 40);
    OH_Drawing_PathLineTo(path2, -50, 10.2);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetWidth(), 100.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetHeight(), 40.0));
    OH_Drawing_PathDestroy(path2);
}

/*
 * @tc.name: NativeDrawingPathTest_pathReset004
 * @tc.desc: test for PathReset func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathReset004, TestSize.Level1)
{
    OH_Drawing_Path* path3 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path3, 20, 20);
    OH_Drawing_PathReset(path3);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path3)->GetBounds().GetWidth(), 0.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path3)->GetBounds().GetHeight(), 0.0));
    OH_Drawing_PathDestroy(path3);
}

/*
 * @tc.name: NativeDrawingPathTest_pathArcTo005
 * @tc.desc: test for PathArcTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathArcTo005, TestSize.Level1)
{
    OH_Drawing_Path* path4 = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(path4, 10, 10, 20, 0, 0, 90);
    OH_Drawing_PathArcTo(path4, -10, 10, 10, -10, 0, 90);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path4)->GetBounds().GetWidth(), 0.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path4)->GetBounds().GetHeight(), 0.0));
    OH_Drawing_PathDestroy(path4);
}

/*
 * @tc.name: NativeDrawingPathTest_pathQuadTo006
 * @tc.desc: test for PathQuadTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathQuadTo006, TestSize.Level1)
{
    OH_Drawing_Path* path5 = OH_Drawing_PathCreate();
    OH_Drawing_PathQuadTo(path5, 0, 0, 30, 30);
    OH_Drawing_PathQuadTo(path5, -20.5f, -20.5f, 30, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path5)->GetBounds().GetWidth(), 50.5));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path5)->GetBounds().GetHeight(), 50.5));
    OH_Drawing_PathDestroy(path5);
}

/*
 * @tc.name: NativeDrawingPathTest_pathCubicTo007
 * @tc.desc: test for PathCubicTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathCubicTo007, TestSize.Level1)
{
    OH_Drawing_Path* path6 = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path6, 30, 40, 60, 0, 50, 20);
    OH_Drawing_PathCubicTo(path6, -30, 40, 60, -30.6f, 50, -20);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path6)->GetBounds().GetWidth(), 90.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path6)->GetBounds().GetHeight(), 70.6));
    OH_Drawing_PathDestroy(path6);
}

/*
 * @tc.name: NativeDrawingPathTest_pathClose008
 * @tc.desc: test for PathClose func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathClose008, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path, 50, 40);
    OH_Drawing_PathClose(path);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 50.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 40.0));
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathTest_pathCopy009
 * @tc.desc: test for PathCopy func.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathCopy009, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // line point x = 50, y = 40
    OH_Drawing_PathLineTo(path, 50, 40);
    OH_Drawing_PathClose(path);
    EXPECT_EQ(OH_Drawing_PathCopy(nullptr), nullptr);
    OH_Drawing_Path* pathCopy = OH_Drawing_PathCopy(path);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(pathCopy)->GetBounds().GetWidth(), 50.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(pathCopy)->GetBounds().GetHeight(), 40.0));
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(pathCopy);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddRect010
 * @tc.desc: test for PathAddRect func.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddRect010, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // rect left[50], top[50],right[250], bottom[250]
    OH_Drawing_PathAddRect(nullptr, 50, 50, 250, 250, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_PathAddRect(path, 50, 50, 250, 250, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 200.0));
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddRoundRect011
 * @tc.desc: test for PathAddRoundRect func.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddRoundRect011, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // rect left[50], top[50],right[250], bottom[250]
    OH_Drawing_PathAddRect(nullptr, 50, 50, 250, 250, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(50, 50, 250, 250);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 20, 20);
    OH_Drawing_PathAddRoundRect(path, roundRect, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 200.0));
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddArc012
 * @tc.desc: test for PathAddArc func.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddArc012, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // rect left[50], top[50],right[250], bottom[250]
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(50, 50, 250, 250);
    OH_Drawing_PathAddArc(nullptr, rect, 0, 180);
    OH_Drawing_PathAddArc(path, nullptr, 0, 180);
    OH_Drawing_PathAddArc(path, rect, 0, 180);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 100.0));
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingPathTest_pathContains013
 * @tc.desc: test for PathContains func.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathContains013, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // rect left[50], top[50],right[250], bottom[250]
    OH_Drawing_PathAddRect(path, 50, 50, 250, 250, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_PathContains(nullptr, 0, 0);
    bool ret = OH_Drawing_PathContains(path, 0, 0);
    EXPECT_EQ(ret, false);
    ret = OH_Drawing_PathContains(path, 60, 60);
    EXPECT_EQ(ret, true);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathTest_pathTransform014
 * @tc.desc: test for PathTransform func.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathTransform014, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // rect left[50], top[50],right[250], bottom[250]
    OH_Drawing_PathAddRect(path, 50, 50, 250, 250, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateTranslation(1, 1);
    OH_Drawing_PathTransform(nullptr, matrix);
    OH_Drawing_PathTransform(path, nullptr);

    bool ret = OH_Drawing_PathContains(path, 50, 50);
    EXPECT_EQ(ret, true);
    OH_Drawing_PathTransform(path, matrix);
    ret = OH_Drawing_PathContains(path, 50, 50);
    EXPECT_EQ(ret, false);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathTest_pathSetFilltype015
 * @tc.desc: test for PathSetFillType func.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathSetFilltype015, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathSetFillType(nullptr, PATH_FILL_TYPE_WINDING);
    OH_Drawing_PathSetFillType(path, PATH_FILL_TYPE_WINDING);
    // line point x = 50, y = 40
    OH_Drawing_PathLineTo(path, 50, 40);
    OH_Drawing_PathClose(path);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 50.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 40.0));
    float ret = OH_Drawing_PathGetLength(path, true);
    EXPECT_TRUE(IsScalarAlmostEqual(ret, 128.062485)); // 128.062485 is length of path
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathTest_pathConicTo016
 * @tc.desc: test for PathConicTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathConicTo016, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathConicTo(path, 0, 0, 30, 30, 1);
    OH_Drawing_PathConicTo(path, -20.5f, -20.5f, 30, 0, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 50.5));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 50.5));
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddRectWithInitialCorner017
 * @tc.desc: test for PathAddRectWithInitialCorner func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddRectWithInitialCorner017, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0.0f, 0.0f, 200.0f, 300.0f);
    OH_Drawing_PathAddRectWithInitialCorner(path, rect, PATH_DIRECTION_CW, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 300.0));
    OH_Drawing_PathClose(path);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddPathWithMode018
 * @tc.desc: test for PathAddPathWithMode func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddPathWithMode018, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0.0f, 0.0f, 200.0f, 300.0f);
    OH_Drawing_PathAddRectWithInitialCorner(path, rect, PATH_DIRECTION_CW, 0);
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathAddPathWithMode(path2, path, PATH_ADD_MODE_APPEND);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetHeight(), 300.0));
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path2);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddPathWithOffsetAndMode019
 * @tc.desc: test for PathAddPathWithOffsetAndMode func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddPathWithOffsetAndMode019, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0.0f, 0.0f, 200.0f, 300.0f);
    OH_Drawing_PathAddRectWithInitialCorner(path, rect, PATH_DIRECTION_CW, 0);
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathAddPathWithOffsetAndMode(path2, path, 0, 0, PATH_ADD_MODE_APPEND);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetHeight(), 300.0));
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path2);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddPathWithMatrixAndMode020
 * @tc.desc: test for PathAddPathWithMatrixAndMode func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddPathWithMatrixAndMode020, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0.0f, 0.0f, 200.0f, 300.0f);
    OH_Drawing_PathAddRectWithInitialCorner(path, rect, PATH_DIRECTION_CW, 0);
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();

    OH_Drawing_PathAddPathWithMatrixAndMode(path2, path, matrix, PATH_ADD_MODE_APPEND);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path2)->GetBounds().GetHeight(), 300.0));

    OH_Drawing_Path* pathRect = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(pathRect, 0.0f, 0.0f, 200.0f, 300.0f, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        5, 4, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_PathAddPath(pathRect, pathRect, nullptr);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(pathRect)->GetBounds().GetWidth(), 200.0));
    OH_Drawing_PathAddPath(pathRect, pathRect, matrix);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(pathRect)->GetBounds().GetWidth(), 2200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(pathRect)->GetBounds().GetHeight(), 600.0));
    OH_Drawing_PathAddPath(nullptr, pathRect, matrix);
    OH_Drawing_PathAddPath(pathRect, nullptr, matrix);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path2);
    OH_Drawing_PathDestroy(pathRect);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingPathTest_pathOffset021
 * @tc.desc: test for PathOffset func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathOffset021, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathOffset(path, path2, 50, 40);
    OH_Drawing_PathReset(path);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 0.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 0.0));
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(path2);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddOvalWithInitialPoint022
 * @tc.desc: test for PathAddOvalWithInitialPoint func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddOvalWithInitialPoint022, TestSize.Level1)
{
    OH_Drawing_Path* path9 = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 100, 500, 400);
    OH_Drawing_PathAddOvalWithInitialPoint(path9, rect, 10, PATH_DIRECTION_CW);
    OH_Drawing_PathClose(path9);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path9)->GetBounds().GetWidth(), 500.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path9)->GetBounds().GetHeight(), 300.0));
    OH_Drawing_PathDestroy(path9);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingPathTest_pathRMoveTo023
 * @tc.desc: test for PathRMoveTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathRMoveTo023, TestSize.Level1)
{
    OH_Drawing_Path* path10 = OH_Drawing_PathCreate();
    OH_Drawing_PathRMoveTo(path10, 100, 100);
    OH_Drawing_PathLineTo(path10, 300, 300);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path10)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path10)->GetBounds().GetHeight(), 200.0));
    OH_Drawing_PathDestroy(path10);
}

/*
 * @tc.name: NativeDrawingPathTest_pathRLineTo024
 * @tc.desc: test for PathRLineTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathRLineTo024, TestSize.Level1)
{
    OH_Drawing_Path* path11 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path11, 100, 100);
    OH_Drawing_PathRLineTo(path11, 300, 300);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path11)->GetBounds().GetWidth(), 300.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path11)->GetBounds().GetHeight(), 300.0));
    OH_Drawing_PathDestroy(path11);
}

/*
 * @tc.name: NativeDrawingPathTest_pathRQuadTo025
 * @tc.desc: test for PathRQuadTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathRQuadTo025, TestSize.Level1)
{
    OH_Drawing_Path* path12 = OH_Drawing_PathCreate();
    OH_Drawing_PathQuadTo(path12, 0, 0, 30, 30);
    OH_Drawing_PathRQuadTo(path12, 100, 100, 100, 300);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path12)->GetBounds().GetWidth(), 130.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path12)->GetBounds().GetHeight(), 330.0));
    OH_Drawing_PathDestroy(path12);
}

/*
 * @tc.name: NativeDrawingPathTest_pathRConicTo026
 * @tc.desc: test for PathRConicTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathRConicTo026, TestSize.Level1)
{
    OH_Drawing_Path* path13 = OH_Drawing_PathCreate();
    OH_Drawing_PathRConicTo(path13, 100, 100, 100, 300, 5);
    OH_Drawing_PathRConicTo(path13, 100, 100, 100, 300, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path13)->GetBounds().GetWidth(), 200.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path13)->GetBounds().GetHeight(), 600.0));
    OH_Drawing_PathDestroy(path13);
}

/*
 * @tc.name: NativeDrawingPathTest_pathRCubicTo027
 * @tc.desc: test for PathRCubicTo func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathRCubicTo027, TestSize.Level1)
{
    OH_Drawing_Path* path14 = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path14, 30, 40, 60, 0, 50, 20);
    OH_Drawing_PathRCubicTo(path14, 30, 40, 60, 0, 50, 20);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path14)->GetBounds().GetWidth(), 110.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path14)->GetBounds().GetHeight(), 60.0));
    OH_Drawing_PathDestroy(path14);
}

/*
 * @tc.name: NativeDrawingPathTest_pathTransformWithPerspectiveClip028
 * @tc.desc: test for PathTransformWithPerspectiveClip func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathTransformWithPerspectiveClip028, TestSize.Level1)
{
    OH_Drawing_Path* path15 = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path15, 100, 500, 500, 100, PATH_DIRECTION_CW);
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(100, 100);
    OH_Drawing_PathTransformWithPerspectiveClip(path15, matrix, path15, true);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path15)->GetBounds().GetWidth(), 400.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path15)->GetBounds().GetHeight(), 400.0));
    OH_Drawing_PathDestroy(path15);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingPathTest_pathAddPath029
 * @tc.desc: test for PathAddPath func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_pathAddPath029, TestSize.Level1)
{
    OH_Drawing_Path* path16 = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path16, 100, 500, 500, 100, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_PathAddPath(path16, path16, matrix);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path16)->GetBounds().GetWidth(), 400.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path16)->GetBounds().GetHeight(), 1000.0));
    OH_Drawing_PathDestroy(path16);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingPathTest_PathAddOval030
 * @tc.desc: test for PathAddOval func.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathTest, NativeDrawingPathTest_PathAddOval030, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 100, 500, 400);
    OH_Drawing_PathAddOval(path, rect, PATH_DIRECTION_CW);
    OH_Drawing_PathClose(path);
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetWidth(), 500.0));
    EXPECT_TRUE(IsScalarAlmostEqual(reinterpret_cast<Path*>(path)->GetBounds().GetHeight(), 300.0));
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
