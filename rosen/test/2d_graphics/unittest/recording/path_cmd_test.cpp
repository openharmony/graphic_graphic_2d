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

#include "recording/path_cmd_list.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PathCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PathCmdTest::SetUpTestCase() {}
void PathCmdTest::TearDownTestCase() {}
void PathCmdTest::SetUp() {}
void PathCmdTest::TearDown() {}

/**
 * @tc.name: PathOpItemPlayback001
 * @tc.desc: Test the playback of the PathOpItem function.
 * @tc.type: FUNC
 * @tc.require: I7OAIR
 */
HWTEST_F(PathCmdTest, PathOpItemPlayback001, TestSize.Level1)
{
    auto path = std::make_shared<Path>();
    ASSERT_TRUE(path != nullptr);
    auto pathCmdList = std::make_unique<PathCmdList>();
    PathPlayer player = { *path, *pathCmdList};

    BuildFromSVGOpItem::Playback(player, nullptr);
    MoveToOpItem::Playback(player, nullptr);
    LineToOpItem::Playback(player, nullptr);
    ArcToOpItem::Playback(player, nullptr);
    CubicToOpItem::Playback(player, nullptr);
    QuadToOpItem::Playback(player, nullptr);
    AddRectOpItem::Playback(player, nullptr);
    AddOvalOpItem::Playback(player, nullptr);
    AddArcOpItem::Playback(player, nullptr);
    AddPolyOpItem::Playback(player, nullptr);
    AddCircleOpItem::Playback(player, nullptr);
    AddRoundRectOpItem::Playback(player, nullptr);
    AddPathOpItem::Playback(player, nullptr);
    AddPathWithMatrixOpItem::Playback(player, nullptr);
    ReverseAddPathOpItem::Playback(player, nullptr);
    SetFillStyleOpItem::Playback(player, nullptr);
    BuildFromInterpolateOpItem::Playback(player, nullptr);
    TransformOpItem::Playback(player, nullptr);
    OffsetOpItem::Playback(player, nullptr);
    PathOpWithOpItem::Playback(player, nullptr);
    ResetOpItem::Playback(player, nullptr);
    CloseOpItem::Playback(player, nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS