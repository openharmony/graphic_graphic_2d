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
#include <gmock/gmock.h>

#include <frame_collector.h>
#include <frame_painter.h>
#include <include/core/SkCanvas.h>
#include <test_header.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FramePainterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FramePainterTest::SetUpTestCase() {}
void FramePainterTest::TearDownTestCase() {}
void FramePainterTest::SetUp() {}
void FramePainterTest::TearDown() {}

class MockSkCanvas : public SkCanvas {
public:
    MOCK_METHOD2(onDrawRect, void(const SkRect& rect, const SkPaint& paint));
    MOCK_CONST_METHOD0(onImageInfo, SkImageInfo());
};

/*
 * Function: Draw
 * Type: Function
 * EnvConditions: Enable and ClearEvents FrameCollector
 * CaseDescription: 1. mock SkCanvas
 *                  2. expect SkCanvas.onDrawRect be call
 *                  3. mark events
 *                  4. draw
 */
HWTEST_F(FramePainterTest, Draw, Function | LargeTest | Level1)
{
    auto &collector = FrameCollector::GetInstance();
    PART("EnvConditions") {
        STEP("Enable and ClearEvents FrameCollector") {
            collector.SetEnabled(true);
            collector.ClearEvents();
        }
    }

    PART("CaseDescription") {
        std::unique_ptr<MockSkCanvas> mockSkCanvas = nullptr;
        STEP("1. mock SkCanvas") {
            mockSkCanvas = std::make_unique<MockSkCanvas>();
        }

        constexpr int32_t width = 100;
        constexpr int32_t height = 100;
        STEP("2. expect SkCanvas.onDrawRect be call") {
            EXPECT_CALL(*mockSkCanvas, onImageInfo()).Times(0x2)
                .WillRepeatedly(Return(SkImageInfo::MakeA8(width, height)));
            // 4 = line{16ms} + line{32ms} + block{Upload} + block{Flush}
            EXPECT_CALL(*mockSkCanvas, onDrawRect(_, _)).Times(0x4);
        }

        STEP("3. mark events") {
            collector.MarkFrameEvent(FrameEventType::UploadStart, 1e6);
            collector.MarkFrameEvent(FrameEventType::UploadEnd, 2e6);
            collector.MarkFrameEvent(FrameEventType::FlushStart, 2e6);
            collector.MarkFrameEvent(FrameEventType::FlushEnd, 4e6);
        }

        STEP("4. draw") {
            FramePainter painter(collector);
            painter.Draw(*mockSkCanvas);
        }
    }
}
} // namespace Rosen
} // namespace OHOS
