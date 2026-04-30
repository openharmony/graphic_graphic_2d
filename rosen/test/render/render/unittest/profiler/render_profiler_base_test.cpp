/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <regex>
#include <string>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-ext.h"
#include "parcel.h"
#include "render_server/rs_render_service.h"
#include "rs_profiler.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_command.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"
#include "rs_profiler_pixelmap.h"

#include "draw/color.h"
#include "image/image.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "utils/data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
/*
 * @tc.name: IsPlaybackParcel01
 * @tc.desc: Test IsPlaybackParcel
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, IsPlaybackParcel01, Level1)
{
    auto oldSubMode = RSProfiler::GetSubMode();
    RSProfiler::SetSubMode(SubMode::READ_EMUL);
    Parcel data;
    EXPECT_FALSE(RSProfiler::IsPlaybackParcel(data));
    RSProfiler::SetSubMode(oldSubMode);
}

/*
 * @tc.name: PixelMapPushCheckJSON201
 * @tc.desc: Test PixelMapPushCheckJSON
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, PixelMapPushCheckJSON201, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    Media::InitializationOptions options;
    options.size.width = 1024;
    options.size.height = 512;
    options.srcPixelFormat = Media::PixelFormat::RGBA_8888;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    options.useDMA = true;
    auto pixelMap = Media::PixelMap::Create(options);

    RSProfilerLogMsg logMsg;
    do {
        logMsg = RSProfiler::ReceiveRSLogBase();
    } while (logMsg.time_);

    constexpr uint64_t testId = 321;
    std::string checkValue = "{\"id\":321,"
                             "\"type\":\"DMA\",\"width\":1024,\"height\":512,\"stride\":4096,\"format\":12}";

    PixelMapStorage::Push(testId, *pixelMap);

    logMsg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);
    EXPECT_EQ((logMsg.type_ == RSProfilerLogType::PIXELMAP && logMsg.msg_.find(checkValue) != std::string::npos), true);
}

/*
 * @tc.name: ReceiveSendMessageBase01
 * @tc.desc: Test ReceiveSendMessageBase
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, ReceiveSendMessageBase01, Level1)
{
    std::string msgStr;
    do {
        msgStr = RSProfiler::ReceiveMessageBase();
    } while (msgStr.length());

    std::string checkValue = "TEST_MESSGAE";
    RSProfiler::SendMessageBase(checkValue);
    msgStr = RSProfiler::ReceiveMessageBase();

    EXPECT_EQ((msgStr == checkValue), true);
}

/*
 * @tc.name: SendReceiveRSLog01
 * @tc.desc: Test SendReceiveRSLog
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, SendReceiveRSLog01, Level1)
{
    std::string magicWord = "XYZABC";
    RSProfilerLogMsg logMsg;
    do {
        logMsg = RSProfiler::ReceiveRSLogBase();
    } while (logMsg.time_);
    RSProfiler::SendRSLogBase(RSProfilerLogType::PIXELMAP_YUV, magicWord);
    logMsg = RSProfiler::ReceiveRSLogBase();
    EXPECT_EQ((logMsg.type_ == RSProfilerLogType::PIXELMAP_YUV && logMsg.msg_ == magicWord), true);
}

/*
 * @tc.name: PixelMapPushCheckJSON101
 * @tc.desc: Test PixelMapPushCheckJSON
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, PixelMapPushCheckJSON101, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    Media::InitializationOptions options;
    options.size.width = 128;
    options.size.height = 256;
    options.srcPixelFormat = Media::PixelFormat::RGBA_8888;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    options.useDMA = false;
    auto pixelMap = Media::PixelMap::Create(options);

    RSProfilerLogMsg logMsg;
    do {
        logMsg = RSProfiler::ReceiveRSLogBase();
    } while (logMsg.time_);

    constexpr uint64_t testId = 123;
    std::string checkValue = "{\"id\":123,"
                             "\"type\":\"SHARED\",\"width\":128,\"height\":256,\"stride\":512,\"format\":3}";

    PixelMapStorage::Push(testId, *pixelMap);

    logMsg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);
    EXPECT_EQ(logMsgmsg.type_ == RSProfilerLogType::PIXELMAP &&
        logMsg.msg_.find(checkValue) != std::string::npos), true);
}

/*
 * @tc.name: RSLOGE01
 * @tc.desc: Test RSLOGE
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, RSLOGE01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    std::string msgStr;
    do {
        msgStr = RSProfiler::ReceiveMessageBase();
    } while (msgStr.length());

    RSProfilerLogMsg logMsg;
    do {
        logMsg = RSProfiler::ReceiveRSLogBase();
    } while (logMsg.time_);

    std::string checkValue = "Test 123";
    RS_LOGE("Test %{public}d", 123);

    logMsg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);

    EXPECT_EQ((logMsg.type_ == RSProfilerLogType::ERROR && logMsg.msg_ == checkValue), true);
}

/*
 * @tc.name: RSLOGW01
 * @tc.desc: Test RSLOGW
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, RSLOGW01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    std::string msgStr;
    do {
        msgStr = RSProfiler::ReceiveMessageBase();
    } while (msgStr.length());

    RSProfilerLogMsg logMsg;
    do {
        logMsg = RSProfiler::ReceiveRSLogBase();
    } while (logMsg.time_);

    std::string checkValue = "Test {private}";
    RS_LOGW("Test %d", 123);

    logMsg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);

    EXPECT_EQ((logMsg.type_ == RSProfilerLogType::WARNING && logMsg.msg_ == checkValue), true);
}

/*
 * @tc.name: HPSBlursMetrics01
 * @tc.desc: Test HPSBlursMetrics
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMetrics, HPSBlursMetrics01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialHPSBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_HPS_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialHPSBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurAreaMetric, "0.100000");

    auto area = 100.f;
    RSProfiler::AddHPSBlursMetrics(area);

    area = 2.f;
    RSProfiler::AddHPSBlursMetrics(area);

    area = 0.2f;
    // Should become 0 because of convertion from float to uint32_t
    RSProfiler::AddHPSBlursMetrics(area);

    auto endHPSBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_HPS_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endHPSBlurOperationMetric, "5");
    EXPECT_EQ(endBlurOperationMetric, "5");
    EXPECT_EQ(endBlurAreaMetric, "102.000000");
}

/*
 * @tc.name: KawaseBlurMetrics01
 * @tc.desc: Test KawaseBlurMetrics
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMetrics, KawaseBlurMetrics01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialKawaseBlurOperationMetric =
        RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialKawaseBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = 9.9f;
    // Should become 9 because of convertion from float to uint32_t
    RSProfiler::AddKawaseBlursMetrics(area);

    auto endKawaseBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endKawaseBlurOperationMetric, "6");
    EXPECT_EQ(endBlurOperationMetric, "6");
    EXPECT_EQ(endBlurAreaMetric, "9.000000");
}

/*
 * @tc.name: RSLOGW02
 * @tc.desc: Test RSLOGW
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, RSLOGW02, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetSubMode(SubMode::READ_EMUL);

    std::string msgStr;
    do {
        msgStr = RSProfiler::ReceiveMessageBase();
    } while (msgStr.length());

    RSProfilerLogMsg logMsg;
    do {
        logMsg = RSProfiler::ReceiveRSLogBase();
    } while (logMsg.time_);

    std::string checkValue = "RS_LOGW: Test WOW {private}";
    RS_LOGW("Test %{public}s %d", "WOW", 321);

    msgStr = RSProfiler::ReceiveMessageBase();
    RSProfiler::SetMode(Mode::NONE);

    EXPECT_EQ(msgStr, checkValue);
}

/*
 * @tc.name: TimeNanoTest01
 * @tc.desc: Test Profiler TimeNano
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerBaseTest, TimeNanoTest01, Level1)
{
    RSProfiler::SetReplayStartTimeNano(0);
    ASSERT_EQ(RSProfiler::GetReplayStartTimeNano(), 0);

    RSProfiler::SetReplayStartTimeNano(Utils::ToNanoseconds(1));
    RSProfiler::SetTransactionTimeCorrection(0);
    ASSERT_EQ(RSProfiler::GetReplayStartTimeNano(), Utils::ToNanoseconds(1));
}

/*
 * @tc.name: LightBlurMetrics01
 * @tc.desc: Test LightBlurMetrics
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMetrics, LightBlurMetrics01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialLightBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialLightBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurAreaMetric, "0.100000");

    auto area = 5.f;
    RSProfiler::AddLightBlursMetrics(area);

    area = 10.f;
    RSProfiler::AddLightBlursMetrics(area);

    auto endLightBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endLightBlurOperationMetric, "3");
    EXPECT_EQ(endBlurOperationMetric, "3");
    EXPECT_EQ(endBlurAreaMetric, "15.000000");
}

/*
 * @tc.name: MESABlurMetrics01
 * @tc.desc: Test MESABlurMetrics
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMetrics, MESABlurMetrics01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialMESABlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialMESABlurOperationMetric, "1");
    EXPECT_EQ(initialBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = 0.1f;
    RSProfiler::AddMESABlursMetrics(area);

    auto endMESABlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endMESABlurOperationMetric, "4");
    EXPECT_EQ(endBlurOperationMetric, "4");
    EXPECT_EQ(endBlurAreaMetric, "0.000000");
}

/*
 * @tc.name: DoNoAddNegativeBlurMetrics01
 * @tc.desc: Test DoNoAddNegativeBlurMetrics
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMetrics, DoNoAddNegativeBlurMetrics01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialKawaseBlurOperationMetric =
        RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialKawaseBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurOperationMetric, "1");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = -1.1f;
    RSProfiler::AddKawaseBlursMetrics(area);

    auto endKawaseBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endKawaseBlurOperationMetric, "2");
    EXPECT_EQ(endBlurOperationMetric, "3");
    EXPECT_NE(endBlurAreaMetric, "-1.100000");
}

/*
 * @tc.name: AllBlurMetrics01
 * @tc.desc: Test AllBlurMetrics
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMetrics, AllBlurMetrics01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialLightBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS);
    auto initialHPSBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_HPS_BLUR_OPERATIONS);
    auto initialKawaseBlurOperationMetric =
        RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto initialMESABlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialLightBlurOperationMetric, "1");
    EXPECT_EQ(initialHPSBlurOperationMetric, "1");
    EXPECT_EQ(initialKawaseBlurOperationMetric, "3");
    EXPECT_EQ(initialMESABlurOperationMetric, "3");
    EXPECT_EQ(initialBlurOperationMetric, "4");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = 0.1f;
    RSProfiler::AddMESABlursMetrics(area);

    area = 1.1f;
    RSProfiler::AddKawaseBlursMetrics(area);

    area = 0.5f;
    RSProfiler::AddLightBlursMetrics(area);

    area = 2.f;
    RSProfiler::AddHPSBlursMetrics(area);

    area = 1.f;
    RSProfiler::AddLightBlursMetrics(area);

    area = 100.f;
    RSProfiler::AddKawaseBlursMetrics(area);

    area = 50.f;
    RSProfiler::AddKawaseBlursMetrics(area);

    auto endLightBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS);
    auto endHPSBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_HPS_BLUR_OPERATIONS);
    auto endKawaseBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto endMESABlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endLightBlurOperationMetric, "3");
    EXPECT_EQ(endHPSBlurOperationMetric, "2");
    EXPECT_EQ(endKawaseBlurOperationMetric, "5");
    EXPECT_EQ(endMESABlurOperationMetric, "1");
    EXPECT_EQ(endBlurOperationMetric, "8");
    EXPECT_EQ(endBlurAreaMetric, "154.000000");
}

/*
 * @tc.name: ShaderCallMetric01
 * @tc.desc: Test ShaderCallMetric
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMetrics, ShaderCallMetric01, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bmp.Build(1, 2, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = std::make_shared<Drawing::Image>();
    image->BuildFromBitmap(bmp);

    RSProfiler::LogShaderCall("KAWASE_BLUR", image, Drawing::Rect(), nullptr);

    auto kawaseBlurShaderCallsMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_SHADER_CALLS);
    auto blurShaderCallsMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_SHADER_CALLS);
    auto blurAredShaderCallsMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_SHADER_CALLS);
    EXPECT_EQ(kawaseBlurShaderCallsMetric, "2");
    EXPECT_EQ(blurShaderCallsMetric, "2");
    EXPECT_EQ(blurAredShaderCallsMetric, "8.000000");
}

class RenderProfilerRecordTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static sptr<RSRenderService> renderService_;
    static RSMainThread* mainThread_;
};
sptr<RSRenderService> RenderProfilerRecordTest::renderService_ = nullptr;
RSMainThread* RenderProfilerRecordTest::mainThread_ = nullptr;

void RenderProfilerRecordTest::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    mainThread_->runner_ = AppExecFwk::EventRunner::Create(true);
    mainThread_->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread_->runner_);

    renderService_ = new RSRenderService();
    renderService_->mainThread_ = mainThread_;
    renderService_->screenManager_ = CreateOrGetScreenManager();

    RSProfiler::Init(renderService_);
}

void RenderProfilerRecordTest::TearDownTestCase()
{
    // Wait for all tasks in the main thread to complete.
    mainThread_->ScheduleTask([]() -> int { return 0; }).get();
}

void RenderProfilerRecordTest::SetUp() {}
void RenderProfilerRecordTest::TearDown() {}

/*
 * @tc.name: RecordUpdate01
 * @tc.desc: Test RecordUpdate
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerRecordTest, RecordUpdate01, Level1)
{
    ArgList argList;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::NONE);  // Set NONE
    RSProfiler::RecordUpdate();       // update without recording
    RSProfiler::RecordStart(argList); // record starting
    RSProfiler::RecordUpdate();
    RSProfiler::RequestRecordAbort();
    EXPECT_TRUE(RSProfiler::IsWriteMode());
    RSProfiler::RecordUpdate(); // also call stop recording
    EXPECT_FALSE(RSProfiler::IsWriteMode());
}

/*
 * @tc.name: RecordMetrics01
 * @tc.desc: Test RecordMetrics
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerRecordTest, RecordMetrics01, Level1)
{
    EXPECT_NO_THROW({ RSProfiler::RecordMetrics(ArgList()); });
}

/*
 * @tc.name: RecordStartFail01
 * @tc.desc: Test RecordStartFail
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerRecordTest, RecordStartFail01, Level1)
{
    ArgList argList;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::READ);  // Not NONE
    RSProfiler::RecordStart(argList); // abort starting record
    EXPECT_FALSE(RSProfiler::IsWriteMode());
    EXPECT_FALSE(RSProfiler::IsRecordingMode());
}

/*
 * @tc.name: RecordStartSuccess01
 * @tc.desc: Test RecordStartSuccess
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerRecordTest, RecordStartSuccess01, Level1)
{
    ArgList argList;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::NONE);  // Set NONE
    RSProfiler::RecordStart(argList); // record starting
    EXPECT_TRUE(RSProfiler::IsWriteMode());
    EXPECT_TRUE(RSProfiler::IsRecordingMode());
    usleep(200000);
}

/*
 * @tc.name: MarshalNode01
 * @tc.desc: Test MarshalNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMarshalTest, MarshalNode01, TestSize.Level1 | Standard)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(111, context_sptr, false);
    std::stringstream stream;
    RSProfiler::MarshalNode(node, stream, RSFILE_VERSION_LATEST);

    std::string buf;
    size_t size = 1;
    size_t words = 1;
    while (stream >> buf) {
        size += buf.size();
        ++words;
    }

    EXPECT_EQ(size, 45u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalSubTreeLo01
 * @tc.desc: Test MarshalSubTreeLo
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMarshalTest, MarshalSubTreeLo01, TestSize.Level1 | Standard)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(101, context_sptr, false);
    std::stringstream stream;
    RSProfiler::MarshalSubTreeLo(*context_sptr, stream, node, RSFILE_VERSION_LATEST);

    std::string buffer;
    size_t size = 1;
    size_t words = 2;
    while (stream >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 57u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalNodeModifiers01
 * @tc.desc: Test MarshalNodeModifiers
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMarshalTest, MarshalNodeModifiers01, TestSize.Level1 | Standard)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(102, context_sptr, false);
    std::stringstream stream;
    RSProfiler::MarshalNodeModifiers(node, stream, RSFILE_VERSION_LATEST, false, false);

    std::string buffer;
    size_t size = 2;
    size_t words = 0;
    while (stream >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 20u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalNodes01
 * @tc.desc: Test MarshalNodes
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMarshalTest, MarshalNodes01, TestSize.Level1 | Standard)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(100, context_sptr, false);
    std::stringstream stream;
    RSProfiler::MarshalNodes(*context_sptr, stream, RSFILE_VERSION_LATEST, nullptr);

    std::string buffer;
    size_t size = 1;
    size_t words = 0;
    while (stream >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 77u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalTree01
 * @tc.desc: Test MarshalTree
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RSProfilerMarshalTest, MarshalTree01, TestSize.Level1 | Standard)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(101, context_sptr, false);
    std::stringstream stream;
    RSProfiler::MarshalTree(node, stream, RSFILE_VERSION_LATEST);

    std::string buffer;
    size_t size = 0;
    size_t words = 1;
    while (stream >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 12u);

    EXPECT_EQ(words, 1u);
}
} // namespace OHOS::Rosen
