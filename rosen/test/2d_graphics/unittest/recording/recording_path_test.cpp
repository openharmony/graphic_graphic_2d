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

#include "gtest/gtest.h"

#include "recording/recording_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingPathTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingPathTest::SetUpTestCase() {}
void RecordingPathTest::TearDownTestCase() {}
void RecordingPathTest::SetUp() {}
void RecordingPathTest::TearDown() {}

/**
 * @tc.name: BuildFromSVGString001
 * @tc.desc: Test the playback of the BuildFromSVGString function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, BuildFromSVGString001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    std::string str;
    EXPECT_TRUE(recordingPath->BuildFromSVGString(str));
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: MoveTo001
 * @tc.desc: Test the playback of the MoveTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, MoveTo001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->MoveTo(5.0f, 4.5f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: LineTo001
 * @tc.desc: Test the playback of the LineTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, LineTo001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->LineTo(1.0f, 3.0f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: ArcTo001
 * @tc.desc: Test the playback of the ArcTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, ArcTo001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->ArcTo(1.0f, 3.0f, 2.2f, 2.3f, 0.0f, 5.0f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: ArcTo002
 * @tc.desc: Test the playback of the ArcTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, ArcTo002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->ArcTo(1.0f, 3.0f, 2.5f, PathDirection::CCW_DIRECTION, 1.0f, 3.0f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: ArcTo003
 * @tc.desc: Test the playback of the ArcTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, ArcTo003, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Point point1;
    Point point2;
    recordingPath->ArcTo(point1, point2, 2.5f, 2.4f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: CubicTo001
 * @tc.desc: Test the playback of the CubicTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, CubicTo001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->CubicTo(1.0f, 2.3f, 2.5f, 2.0f, 3.5f, 3.0f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: CubicTo002
 * @tc.desc: Test the playback of the CubicTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, CubicTo002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Point point1;
    Point point2;
    Point endPoint(2.3f, 1.5f);
    recordingPath->CubicTo(point1, point2, endPoint);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: QuadTo001
 * @tc.desc: Test the playback of the QuadTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, QuadTo001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Point point1(1.2f, 0.0f);
    Point endPoint(2.3f, 1.5f);
    recordingPath->QuadTo(point1, endPoint);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: QuadTo002
 * @tc.desc: Test the playback of the QuadTo function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, QuadTo002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->QuadTo(1.0f, 1.5f, 3.3f, 4.5f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddRect001
 * @tc.desc: Test the playback of the AddRect function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddRect001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Rect rect;
    recordingPath->AddRect(rect);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddRect002
 * @tc.desc: Test the playback of the AddRect function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddRect002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->AddRect(1.0f, 4.0f, 3.0f, 2.0f, PathDirection::CCW_DIRECTION);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddRect003
 * @tc.desc: Test the playback of the AddRect function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddRect003, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddOval001
 * @tc.desc: Test the playback of the AddOval function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddOval001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Rect oval;
    recordingPath->AddOval(oval, PathDirection::CCW_DIRECTION);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddOval002
 * @tc.desc: Test the playback of the AddOval function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddOval002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Rect oval;
    recordingPath->AddOval(oval);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddArc001
 * @tc.desc: Test the playback of the AddArc function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddArc001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Rect rect;
    recordingPath->AddArc(rect, 1.0f, 2.0f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddPoly001
 * @tc.desc: Test the playback of the AddPoly function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddPoly001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    std::vector<Point> points;
    Point point1;
    points.push_back(point1);
    int size = points.size();
    recordingPath->AddPoly(points, size, false);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddPoly002
 * @tc.desc: Test the playback of the AddPoly function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddPoly002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    std::vector<Point> points;
    Point point1;
    Point point2;
    points.push_back(point1);
    points.push_back(point2);
    int size = points.size();
    recordingPath->AddPoly(points, size, true);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddCircle001
 * @tc.desc: Test the playback of the AddCircle function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddCircle001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->AddCircle(1.0f, 0.5f, 0.5f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddCircle002
 * @tc.desc: Test the playback of the AddCircle function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddCircle002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->AddCircle(1.0f, 0.5f, 0.5f, PathDirection::CCW_DIRECTION);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddRoundRect001
 * @tc.desc: Test the playback of the AddRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddRoundRect001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Rect rect;
    recordingPath->AddRoundRect(rect, 0.5f, 0.5f, PathDirection::CCW_DIRECTION);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddRoundRect002
 * @tc.desc: Test the playback of the AddRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddRoundRect002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Rect rect;
    recordingPath->AddRoundRect(rect, 0.5f, 0.5f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddRoundRect003
 * @tc.desc: Test the playback of the AddRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddRoundRect003, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    RoundRect roundRect;
    recordingPath->AddRoundRect(roundRect, PathDirection::CCW_DIRECTION);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddRoundRect004
 * @tc.desc: Test the playback of the AddRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddRoundRect004, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Rect rect;
    RoundRect roundRect(rect, 12.6f, 77.4f);
    recordingPath->AddRoundRect(roundRect);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddPath001
 * @tc.desc: Test the playback of the AddPath function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddPath001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    RecordingPath sourcePath;
    recordingPath->AddPath(sourcePath, 0.5f, 0.5f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddPath002
 * @tc.desc: Test the playback of the AddPath function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddPath002, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    RecordingPath sourcePath;
    recordingPath->AddPath(sourcePath);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: AddPath003
 * @tc.desc: Test the playback of the AddPath function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, AddPath003, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    RecordingPath path1;
    Matrix matrix;
    recordingPath->AddPath(path1, matrix);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: ReverseAddPath001
 * @tc.desc: Test the playback of the ReverseAddPath function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, ReverseAddPath001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    RecordingPath path1;
    path1.AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    recordingPath->ReverseAddPath(path1);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: SetFillStyle001
 * @tc.desc: Test the playback of the SetFillStyle function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, SetFillStyle001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->SetFillStyle(PathFillType::WINDING);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: BuildFromInterpolate001
 * @tc.desc: Test the playback of the BuildFromInterpolate function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, BuildFromInterpolate001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    RecordingPath path1;
    RecordingPath path2;
    recordingPath->BuildFromInterpolate(path1, path2, 1.0f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: Transform001
 * @tc.desc: Test the playback of the Transform function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, Transform001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Matrix matrix;
    recordingPath->Transform(matrix);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: Offset001
 * @tc.desc: Test the playback of the Offset function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, Offset001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    Matrix matrix;
    recordingPath->Offset(1.0f, 2.3f);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: Op001
 * @tc.desc: Test the playback of the Op function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, Op001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    RecordingPath path1;
    RecordingPath path2;
    recordingPath->Op(path1, path2, PathOp::INTERSECT);
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: Reset001
 * @tc.desc: Test the playback of the Reset function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, Reset001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->Reset();
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}

/**
 * @tc.name: Close001
 * @tc.desc: Test the playback of the Close function.
 * @tc.type: FUNC
 * @tc.require: I77FZN
 */
HWTEST_F(RecordingPathTest, Close001, TestSize.Level1)
{
    auto recordingPath = std::make_shared<RecordingPath>();
    EXPECT_TRUE(recordingPath != nullptr);
    recordingPath->Close();
    auto pathCmdList = recordingPath->GetCmdList();
    EXPECT_TRUE(pathCmdList != nullptr);
    auto path = pathCmdList->Playback();
    EXPECT_TRUE(path != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
