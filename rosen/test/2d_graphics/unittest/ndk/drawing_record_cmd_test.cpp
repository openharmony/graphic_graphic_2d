/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_canvas.h"
#include "drawing_error_code.h"
#include "drawing_record_cmd.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingRecordCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingRecordCmdTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingRecordCmdTest::TearDownTestCase() {}
void NativeDrawingRecordCmdTest::SetUp() {}
void NativeDrawingRecordCmdTest::TearDown() {}

/*
 * @tc.name: NativeRecordCmdTest_RecordCmdUtilsCreate
 * @tc.desc: test for OH_Drawing_RecordCmdUtilsCreate and OH_Drawing_RecordCmdUtilsDestroy.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRecordCmdTest, NativeRecordCmdTest_RecordCmdUtilsCreate, TestSize.Level1)
{
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils != nullptr);
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    code = OH_Drawing_RecordCmdUtilsDestroy(nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeRecordCmdTest_RecordCmdUtilsBeginRecording001
 * @tc.desc: test for OH_Drawing_RecordCmdUtilsBeginRecording.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRecordCmdTest, NativeRecordCmdTest_RecordCmdUtilsBeginRecording001, TestSize.Level1)
{
    int32_t width = 10; // canvas width is 10
    int32_t height = 20; // canvas height is 20
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils != nullptr);
    OH_Drawing_Canvas* canvas = nullptr;
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    EXPECT_TRUE(canvas != nullptr);
    int32_t width1 = OH_Drawing_CanvasGetWidth(canvas);
    int32_t height2 = OH_Drawing_CanvasGetHeight(canvas);
    EXPECT_TRUE(width1 == width);
    EXPECT_TRUE(height2 == height);

    code = OH_Drawing_RecordCmdUtilsBeginRecording(nullptr, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    code = OH_Drawing_RecordCmdUtilsBeginRecording(nullptr, width, height, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    code = OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
}

/*
 * @tc.name: NativeRecordCmdTest_RecordCmdUtilsBeginRecording002
 * @tc.desc: test for OH_Drawing_RecordCmdUtilsBeginRecording.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRecordCmdTest, NativeRecordCmdTest_RecordCmdUtilsBeginRecording002, TestSize.Level1)
{
    int32_t width = -1; // canvas width is -1
    int32_t height = 20; // canvas height is 20
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils != nullptr);
    OH_Drawing_Canvas* canvas = nullptr;
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    width = 10; // canvas width is 10
    height = -1; // canvas height is -1
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    width = -1; // canvas width is -1
    height = -2; // canvas height is -2
    canvas = nullptr;
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    code = OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
}

/*
 * @tc.name: NativeRecordCmdTest_RecordCmdUtilsBeginRecording003
 * @tc.desc: test for OH_Drawing_RecordCmdUtilsBeginRecording.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRecordCmdTest, NativeRecordCmdTest_RecordCmdUtilsBeginRecording003, TestSize.Level1)
{
    int32_t width = 0; // canvas width is 0
    int32_t height = 20; // canvas height is 20
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils != nullptr);
    OH_Drawing_Canvas* canvas = nullptr;
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    width = 10; // canvas width is 10
    height = 0; // canvas width is 0
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    width = -1; // canvas width is -1
    height = 0; // canvas width is 0
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    width = 0; // canvas width is 0
    height = -1; // canvas width is -1
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    width = 0; // canvas width is 0
    height = 0; // canvas width is -1
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    code = OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
}

/*
 * @tc.name: NativeRecordCmdTest_RecordCmdUtilsFinishRecording001
 * @tc.desc: test for OH_Drawing_RecordCmdUtilsFinishRecording.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRecordCmdTest, NativeRecordCmdTest_RecordCmdUtilsFinishRecording001, TestSize.Level1)
{
    int32_t width = 10;
    int32_t height = 20;
    OH_Drawing_RecordCmd* recordCmd = nullptr;
    OH_Drawing_Canvas* canvas = nullptr;
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils != nullptr);
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils, &recordCmd);
    EXPECT_TRUE(code != OH_DRAWING_SUCCESS);

    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_TRUE(canvas != nullptr);
    code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils, &recordCmd);
    EXPECT_TRUE(code == OH_DRAWING_SUCCESS);
    EXPECT_TRUE(recordCmd != nullptr);
    code = OH_Drawing_RecordCmdDestroy(recordCmd);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);

    code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    code = OH_Drawing_RecordCmdUtilsFinishRecording(nullptr, &recordCmd);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    code = OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
