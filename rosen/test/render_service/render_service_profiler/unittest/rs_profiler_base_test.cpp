/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include <climits>
#include <cmath>
#include <cstdint>
#include <regex>
#include "parcel.h"
#include <string>
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-ext.h"
#include "rs_profiler.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_pixelmap.h"
#include "draw/color.h"
#include "image/image.h"
#include "utils/data.h"

#include "rs_profiler_command.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/rs_render_service.h"
#include "transaction/rs_service_to_render_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

/*
 * @tc.name: IsPlaybackParcel
 * @tc.desc: Test IsPlaybackParcel
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, IsPlaybackParcel, Level1)
{
    auto oldSubMode = RSProfiler::GetSubMode();
    RSProfiler::SetSubMode(SubMode::READ_EMUL);
    Parcel parcel;
    EXPECT_FALSE(RSProfiler::IsPlaybackParcel(parcel));
    RSProfiler::SetSubMode(oldSubMode);
}

/*
 * @tc.name: SendReceiveRSLog
 * @tc.desc: Test SendReceiveRSLog
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, SendReceiveRSLog, Level1)
{
    std::string magicWord = "XYZABC";
    RSProfilerLogMsg msg;
    do {
        msg = RSProfiler::ReceiveRSLogBase();
    } while (msg.time_);
    RSProfiler::SendRSLogBase(RSProfilerLogType::PIXELMAP_YUV, magicWord);
    msg = RSProfiler::ReceiveRSLogBase();
    EXPECT_EQ((msg.type_ == RSProfilerLogType::PIXELMAP_YUV && msg.msg_ == magicWord), true);
}

/*
 * @tc.name: PixelMapPushCheckJSON1
 * @tc.desc: Test PixelMapPushCheckJSON
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, PixelMapPushCheckJSON1, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    Media::InitializationOptions options;
    options.size.width = 128;
    options.size.height = 256;
    options.srcPixelFormat = Media::PixelFormat::RGBA_8888;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    options.allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    options.useDMA = false;
    const auto pixelMap = Media::PixelMap::Create(options);
    ASSERT_NE(pixelMap, nullptr);
    ASSERT_EQ(pixelMap->GetAllocatorType(), Media::AllocatorType::SHARE_MEM_ALLOC);

    RSProfilerLogMsg msg;
    do {
        msg = RSProfiler::ReceiveRSLogBase();
    } while (msg.time_);

    constexpr uint64_t testId = 123;
    PixelMapStorage::Push(testId, *pixelMap);

    msg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);

    const std::string checkValue = "{\"id\":123,"
                                   "\"type\":\"SHARED\",\"width\":128,\"height\":256,\"stride\":512,\"format\":3}";
    EXPECT_EQ((msg.type_ == RSProfilerLogType::PIXELMAP && msg.msg_.find(checkValue) != std::string::npos), true)
        << "Message: " << msg.msg_;
}

/*
 * @tc.name: PixelMapPushCheckJSON2
 * @tc.desc: Test PixelMapPushCheckJSON
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, PixelMapPushCheckJSON2, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    Media::InitializationOptions options;
    options.size.width = 1024;
    options.size.height = 512;
    options.srcPixelFormat = Media::PixelFormat::RGBA_8888;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    options.allocatorType = Media::AllocatorType::DMA_ALLOC;
    options.useDMA = true;
    const auto pixelMap = Media::PixelMap::Create(options);
    ASSERT_NE(pixelMap, nullptr);
    ASSERT_EQ(pixelMap->GetAllocatorType(), Media::AllocatorType::DMA_ALLOC);

    RSProfilerLogMsg msg;
    do {
        msg = RSProfiler::ReceiveRSLogBase();
    } while (msg.time_);

    constexpr uint64_t testId = 321;
    PixelMapStorage::Push(testId, *pixelMap);

    msg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);
    const std::string checkValue = "{\"id\":321,"
                                   "\"type\":\"DMA\",\"width\":1024,\"height\":512,\"stride\":4096,\"format\":12}";
    EXPECT_EQ((msg.type_ == RSProfilerLogType::PIXELMAP && msg.msg_.find(checkValue) != std::string::npos), true)
        << "Message: " << msg.msg_;
}

/*
 * @tc.name: ReceiveSendMessageBase
 * @tc.desc: Test ReceiveSendMessageBase
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, ReceiveSendMessageBase, Level1)
{
    std::string msg;
    do {
        msg = RSProfiler::ReceiveMessageBase();
    } while (msg.length());

    std::string checkValue = "TEST_MESSGAE";
    RSProfiler::SendMessageBase(checkValue);
    msg = RSProfiler::ReceiveMessageBase();

    EXPECT_EQ((msg == checkValue), true);
}

/*
 * @tc.name: RSLOGE1
 * @tc.desc: Test RSLOGE
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, RSLOGE1, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    std::string msg;
    do {
        msg = RSProfiler::ReceiveMessageBase();
    } while (msg.length());

    RSProfilerLogMsg logmsg;
    do {
        logmsg = RSProfiler::ReceiveRSLogBase();
    } while (logmsg.time_);

    std::string checkValue = "Test 123";
    RS_LOGE("Test %{public}d", 123);

    logmsg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);

    EXPECT_EQ((logmsg.type_ == RSProfilerLogType::ERROR && logmsg.msg_ == checkValue), true);
}

/*
 * @tc.name: RSLOGW1
 * @tc.desc: Test RSLOGW
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, RSLOGW1, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::WRITE);

    std::string msg;
    do {
        msg = RSProfiler::ReceiveMessageBase();
    } while (msg.length());

    RSProfilerLogMsg logmsg;
    do {
        logmsg = RSProfiler::ReceiveRSLogBase();
    } while (logmsg.time_);

    std::string checkValue = "Test {private}";
    RS_LOGW("Test %d", 123);

    logmsg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);

    EXPECT_EQ((logmsg.type_ == RSProfilerLogType::WARNING && logmsg.msg_ == checkValue), true);
}

/*
 * @tc.name: RSLOGW2
 * @tc.desc: Test RSLOGW
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, RSLOGW2, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetSubMode(SubMode::READ_EMUL);

    std::string msg;
    do {
        msg = RSProfiler::ReceiveMessageBase();
    } while (msg.length());

    RSProfilerLogMsg logmsg;
    do {
        logmsg = RSProfiler::ReceiveRSLogBase();
    } while (logmsg.time_);

    std::string checkValue = "RS_LOGW: Test WOW {private}";
    RS_LOGW("Test %{public}s %d", "WOW", 321);

    msg = RSProfiler::ReceiveMessageBase();
    RSProfiler::SetMode(Mode::NONE);

    EXPECT_EQ(msg, checkValue);
}

/*
 * @tc.name: TimeNanoTest
 * @tc.desc: Test Profiler TimeNano
 * @tc.type: FUNC
 * @tc.require: 21658
 */
HWTEST(RSProfilerBaseTest, TimeNanoTest, Level1)
{
    RSProfiler::SetReplayStartTimeNano(0);
    ASSERT_EQ(RSProfiler::GetReplayStartTimeNano(), 0);

    RSProfiler::SetReplayStartTimeNano(Utils::ToNanoseconds(1));
    RSProfiler::SetTransactionTimeCorrection(1);
    ASSERT_EQ(RSProfiler::GetReplayStartTimeNano(), Utils::ToNanoseconds(1));
}

/*
 * @tc.name: LightBlurMetrics
 * @tc.desc: Test LightBlurMetrics
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, LightBlurMetrics, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialLightBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialLightBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = 5.f;
    RSProfiler::AddLightBlursMetrics(area);

    area = 10.f;
    RSProfiler::AddLightBlursMetrics(area);

    auto endLightBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endLightBlurOperationMetric, "2");
    EXPECT_EQ(endBlurOperationMetric, "2");
    EXPECT_EQ(endBlurAreaMetric, "15.000000");
}

/*
 * @tc.name: HPSBlursMetrics
 * @tc.desc: Test HPSBlursMetrics
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, HPSBlursMetrics, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialHPSBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_HPS_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialHPSBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

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
    EXPECT_EQ(endHPSBlurOperationMetric, "3");
    EXPECT_EQ(endBlurOperationMetric, "3");
    EXPECT_EQ(endBlurAreaMetric, "102.000000");
}

/*
 * @tc.name: KawaseBlurMetrics
 * @tc.desc: Test KawaseBlurMetrics
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, KawaseBlurMetrics, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialKawaseBlurOperationMetric =
        RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialKawaseBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = 9.9f;
    // Should become 9 because of convertion from float to uint32_t
    RSProfiler::AddKawaseBlursMetrics(area);

    auto endKawaseBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endKawaseBlurOperationMetric, "1");
    EXPECT_EQ(endBlurOperationMetric, "1");
    EXPECT_EQ(endBlurAreaMetric, "9.000000");
}

/*
 * @tc.name: MESABlurMetrics
 * @tc.desc: Test MESABlurMetrics
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, MESABlurMetrics, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialMESABlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialMESABlurOperationMetric, "0");
    EXPECT_EQ(initialBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = 0.1f;
    RSProfiler::AddMESABlursMetrics(area);

    auto endMESABlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endMESABlurOperationMetric, "1");
    EXPECT_EQ(endBlurOperationMetric, "1");
    EXPECT_EQ(endBlurAreaMetric, "0.000000");
}

/*
 * @tc.name: AllBlurMetrics
 * @tc.desc: Test AllBlurMetrics
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, AllBlurMetrics, Level1)
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
    EXPECT_EQ(initialLightBlurOperationMetric, "0");
    EXPECT_EQ(initialHPSBlurOperationMetric, "0");
    EXPECT_EQ(initialKawaseBlurOperationMetric, "0");
    EXPECT_EQ(initialMESABlurOperationMetric, "0");
    EXPECT_EQ(initialBlurOperationMetric, "0");
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
    EXPECT_EQ(endLightBlurOperationMetric, "2");
    EXPECT_EQ(endHPSBlurOperationMetric, "1");
    EXPECT_EQ(endKawaseBlurOperationMetric, "3");
    EXPECT_EQ(endMESABlurOperationMetric, "1");
    EXPECT_EQ(endBlurOperationMetric, "7");
    EXPECT_EQ(endBlurAreaMetric, "154.000000");
}

/*
 * @tc.name: ShaderCallMetric
 * @tc.desc: Test ShaderCallMetric
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, ShaderCallMetric, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bmp.Build(2, 4, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = std::make_shared<Drawing::Image>();
    image->BuildFromBitmap(bmp);

    RSProfiler::LogShaderCall("KAWASE_BLUR", image, Drawing::Rect(), nullptr);

    auto kawaseBlurShaderCallsMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_SHADER_CALLS);
    auto blurShaderCallsMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_SHADER_CALLS);
    auto blurAredShaderCallsMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_SHADER_CALLS);
    EXPECT_EQ(kawaseBlurShaderCallsMetric, "1");
    EXPECT_EQ(blurShaderCallsMetric, "1");
    EXPECT_EQ(blurAredShaderCallsMetric, "8.000000");
}

/*
 * @tc.name: DoNoAddNegativeBlurMetrics
 * @tc.desc: Test DoNoAddNegativeBlurMetrics
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, DoNoAddNegativeBlurMetrics, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    auto initialKawaseBlurOperationMetric =
        RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto initialBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto initialBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(initialKawaseBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurOperationMetric, "0");
    EXPECT_EQ(initialBlurAreaMetric, "0.000000");

    auto area = -1.1f;
    RSProfiler::AddKawaseBlursMetrics(area);

    auto endKawaseBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS);
    auto endBlurOperationMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_OPERATIONS);
    auto endBlurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS);
    EXPECT_EQ(endKawaseBlurOperationMetric, "1");
    EXPECT_EQ(endBlurOperationMetric, "1");
    EXPECT_NE(endBlurAreaMetric, "-1.100000");
}

class RecorRsProfileRecordTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static RSMainThread* mainThread_;
};
RSMainThread* RecorRsProfileRecordTest::mainThread_ = nullptr;

void RecorRsProfileRecordTest::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    if (mainThread_) {
        const auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
        mainThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
        mainThread_->handler_->eventRunner_->Run();
    }

    auto pipeline = std::make_shared<RSRenderPipeline>();
    pipeline->mainThread_ = mainThread_;

    const auto serviceToRenderConnection =
        sptr<RSServiceToRenderConnection>::MakeSptr(new RSRenderPipelineAgent(pipeline));
    RSProfiler::Init(pipeline, serviceToRenderConnection);
}

void RecorRsProfileRecordTest::TearDownTestCase()
{
    RSProfiler::Init(nullptr, nullptr);
    // Wait for all tasks in the main thread to complete.
    if (mainThread_) {
        mainThread_->ScheduleTask([]() -> int { return 0; }).get();
    }
}

void RecorRsProfileRecordTest::SetUp() {}
void RecorRsProfileRecordTest::TearDown() {}

/*
 * @tc.name: RecordStartFail
 * @tc.desc: Test RecordStartFail
 * @tc.type: FUNC
 */
HWTEST_F(RecorRsProfileRecordTest, RecordStartFail, Level1)
{
    ArgList args;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::READ); // Not NONE
    RSProfiler::RecordStart(args); // abort starting record
    EXPECT_FALSE(RSProfiler::IsWriteMode());
    EXPECT_FALSE(RSProfiler::IsRecordingMode());
}

/*
 * @tc.name: RecordStartSuccess
 * @tc.desc: Test RecordStartSuccess
 * @tc.type: FUNC
 */
HWTEST_F(RecorRsProfileRecordTest, RecordStartSuccess, Level1)
{
    ArgList args;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::NONE); // Set NONE
    RSProfiler::RecordStart(args); // record starting
    EXPECT_TRUE(RSProfiler::IsWriteMode());
    EXPECT_TRUE(RSProfiler::IsRecordingMode());
    usleep(200000);
}

/*
 * @tc.name: RecordUpdate
 * @tc.desc: Test RecordUpdate
 * @tc.type: FUNC
 */
HWTEST_F(RecorRsProfileRecordTest, RecordUpdate, Level1)
{
    ArgList args;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::NONE); // Set NONE
    RSProfiler::RecordUpdate(); // update without recording
    RSProfiler::RecordStart(args); // record starting
    RSProfiler::RecordUpdate();
    RSProfiler::RequestRecordAbort();
    EXPECT_TRUE(RSProfiler::IsWriteMode());
    RSProfiler::RecordUpdate(); //also call stop recording
    EXPECT_FALSE(RSProfiler::IsWriteMode());
}

/*
 * @tc.name: RecordMetrics
 * @tc.desc: Test RecordMetrics
 * @tc.type: FUNC
 */
HWTEST_F(RecorRsProfileRecordTest, RecordMetrics, Level1)
{
    EXPECT_NO_THROW({
        RSProfiler::RecordMetrics(ArgList());
    });
}

/*
 * @tc.name: MarshalNode
 * @tc.desc: Test MarshalNode
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMarshalTest, MarshalNode, TestSize.Level1 | Standard) {
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data;
    RSProfiler::MarshalNode(node, data, RSFILE_VERSION_LATEST);

    std::string buffer;
    size_t size = 0;
    size_t words = 0;
    while (data >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 45u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalNodes
 * @tc.desc: Test MarshalNodes
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMarshalTest, MarshalNodes, TestSize.Level1 | Standard) {
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data;
    RSProfiler::MarshalNodes(*context_sptr, data, RSFILE_VERSION_LATEST, nullptr);

    std::string buffer;
    size_t size = 0;
    size_t words = 0;
    while (data >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 77u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalTree
 * @tc.desc: Test MarshalTree
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMarshalTest, MarshalTree, TestSize.Level1 | Standard) {
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data;
    RSProfiler::MarshalTree(node, data, RSFILE_VERSION_LATEST);

    std::string buffer;
    size_t size = 0;
    size_t words = 0;
    while (data >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 12u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalSubTreeLo
 * @tc.desc: Test MarshalSubTreeLo
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMarshalTest, MarshalSubTreeLo, TestSize.Level1 | Standard) {
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data;
    RSProfiler::MarshalSubTreeLo(*context_sptr, data, node, RSFILE_VERSION_LATEST);

    std::string buffer;
    size_t size = 0;
    size_t words = 0;
    while (data >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 57u);

    EXPECT_EQ(words, 1u);
}

/*
 * @tc.name: MarshalNodeModifiers
 * @tc.desc: Test MarshalNodeModifiers
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMarshalTest, MarshalNodeModifiers, TestSize.Level1 | Standard) {
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data;
    RSProfiler::MarshalNodeModifiers(node, data, RSFILE_VERSION_LATEST, false, false);

    std::string buffer;
    size_t size = 0;
    size_t words = 0;
    while (data >> buffer) {
        size += buffer.size();
        ++words;
    }

    EXPECT_EQ(size, 20u);

    EXPECT_EQ(words, 1u);
}
/*
 * @tc.name: LogShaderCallNullImage
 * @tc.desc: Test LogShaderCall with null srcImage
 * @tc.type: FUNC
 */
HWTEST(RSProfilerMetrics, LogShaderCallNullImage, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::ResetCustomMetrics();
    RSProfiler::SetMode(Mode::WRITE);

    RSProfiler::LogShaderCall("KAWASE_BLUR", nullptr, Drawing::Rect(), nullptr);
    auto blurAreaMetric = RSProfiler::GetCustomMetrics().Get(RSPROFILER_METRIC_BLUR_AREA_SHADER_CALLS);
    EXPECT_EQ(blurAreaMetric, "0.000000");

    RSProfiler::SetMode(Mode::NONE);
}

/*
 * @tc.name: BaseSetPlaybackSpeedNaN
 * @tc.desc: Test BaseSetPlaybackSpeed with NaN/negative/zero speed
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, BaseSetPlaybackSpeedNaN, Level1)
{
    EXPECT_FALSE(RSProfiler::BaseSetPlaybackSpeed(NAN));
    EXPECT_FALSE(RSProfiler::BaseSetPlaybackSpeed(-1.0));
    EXPECT_FALSE(RSProfiler::BaseSetPlaybackSpeed(0.0));
    EXPECT_TRUE(RSProfiler::BaseSetPlaybackSpeed(2.0));
}

/*
 * @tc.name: UnmarshalNodesCountOverflow
 * @tc.desc: Test UnmarshalNodes with count exceeding maxCount
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodesCountOverflow, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    uint32_t hugeCount = 1'000'001u;
    data.write(reinterpret_cast<const char*>(&hugeCount), sizeof(hugeCount));
    std::string error = RSProfiler::UnmarshalNodes(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodes: Node count out of range");
}

/*
 * @tc.name: UnmarshalNodesEmptyStream
 * @tc.desc: Test UnmarshalNodes with empty stream
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodesEmptyStream, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    std::string error = RSProfiler::UnmarshalNodes(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodes: Node count out of range");
}

/*
 * @tc.name: UnmarshalNodesTreeCountOverflow
 * @tc.desc: Test UnmarshalNodes with tree count exceeding maxCount
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodesTreeCountOverflow, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    uint32_t nodeCount = 0u;
    data.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
    uint32_t treeCount = 1'000'001u;
    data.write(reinterpret_cast<const char*>(&treeCount), sizeof(treeCount));
    std::string error = RSProfiler::UnmarshalNodes(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodes: Tree node count out of range");
}

/*
 * @tc.name: UnmarshalTreeEmptyStream
 * @tc.desc: Test UnmarshalTree with empty stream
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalTreeEmptyStream, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    std::string error = RSProfiler::UnmarshalTree(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalTree: Cannot read node id");
}

/*
 * @tc.name: UnmarshalTreeCountOverflow
 * @tc.desc: Test UnmarshalTree with count exceeding maxCount
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalTreeCountOverflow, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId nodeId = 123;
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));
    uint32_t count = 1'000'001u;
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    std::string error = RSProfiler::UnmarshalTree(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_TRUE(error.find("UnmarshalTree:") != std::string::npos);
}

/*
 * @tc.name: UnmarshalTreeInvalidNode
 * @tc.desc: Test UnmarshalTree with non-existent node id
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalTreeInvalidNode, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId nodeId = 123;
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));
    uint32_t count = 0u;
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    std::string error = RSProfiler::UnmarshalTree(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_TRUE(error.find("UnmarshalTree:") != std::string::npos);
}

/*
 * @tc.name: UnmarshalNodeModifiersReadInstanceRootFail
 * @tc.desc: Test UnmarshalNodeModifiers with empty stream
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodeModifiersReadInstanceRootFail, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    std::string error = RSProfiler::UnmarshalNodeModifiers(node, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodeModifiers: Cannot read instance root node id");
}

/*
 * @tc.name: UnmarshalNodeModifiersReadFirstLevelFail
 * @tc.desc: Test UnmarshalNodeModifiers with missing firstLevelNodeId
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodeModifiersReadFirstLevelFail, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId instanceRoot = 0;
    data.write(reinterpret_cast<const char*>(&instanceRoot), sizeof(instanceRoot));
    std::string error = RSProfiler::UnmarshalNodeModifiers(node, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodeModifiers: Cannot read first level node id");
}

/*
 * @tc.name: UnmarshalNodeModifiersCountOverflow
 * @tc.desc: Test UnmarshalNodeModifiers with modifier count exceeding maxCount
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodeModifiersCountOverflow, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId instanceRoot = 0;
    NodeId firstLevel = 0;
    data.write(reinterpret_cast<const char*>(&instanceRoot), sizeof(instanceRoot));
    data.write(reinterpret_cast<const char*>(&firstLevel), sizeof(firstLevel));
    int32_t count = 10001;
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    std::string error = RSProfiler::UnmarshalNodeModifiers(node, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodeModifiers: Modifier count out of range");
}

/*
 * @tc.name: UnmarshalNodeModifiersNegativeCount
 * @tc.desc: Test UnmarshalNodeModifiers with negative modifier count
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodeModifiersNegativeCount, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId instanceRoot = 0;
    NodeId firstLevel = 0;
    data.write(reinterpret_cast<const char*>(&instanceRoot), sizeof(instanceRoot));
    data.write(reinterpret_cast<const char*>(&firstLevel), sizeof(firstLevel));
    int32_t count = -1;
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    std::string error = RSProfiler::UnmarshalNodeModifiers(node, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodeModifiers: Modifier count out of range");
}

/*
 * @tc.name: UnmarshalNodeModifiersEofCheck
 * @tc.desc: Test UnmarshalNodeModifiers with truncated data causing eof
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodeModifiersEofCheck, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId instanceRoot = 0;
    NodeId firstLevel = 0;
    data.write(reinterpret_cast<const char*>(&instanceRoot), sizeof(instanceRoot));
    data.write(reinterpret_cast<const char*>(&firstLevel), sizeof(firstLevel));
    int32_t modifierCount = 0;
    data.write(reinterpret_cast<const char*>(&modifierCount), sizeof(modifierCount));
    std::string error = RSProfiler::UnmarshalNodeModifiers(node, data, RSFILE_VERSION_LATEST);
    EXPECT_TRUE(error.empty() || error == "UnmarshalNodeModifiers: File damaged");
}

/*
 * @tc.name: UnmarshalSubTreeLoEmptyStream
 * @tc.desc: Test UnmarshalSubTreeLo with empty stream
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalSubTreeLoEmptyStream, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode attachNode(999, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    std::string error = RSProfiler::UnmarshalSubTreeLo(*context_sptr, data, attachNode, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalSubTreeLo: Cannot read node id");
}

/*
 * @tc.name: UnmarshalSubTreeLoInvalidNode
 * @tc.desc: Test UnmarshalSubTreeLo with non-existent node id
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalSubTreeLoInvalidNode, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode attachNode(999, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    // Outer id that does not match any node created by UnmarshalNode
    NodeId outerId = 99999;
    data.write(reinterpret_cast<const char*>(&outerId), sizeof(outerId));
    // UnmarshalNode reads: nodeType(4), nodeId(8), isTextureExportNode(1)
    uint32_t nodeType = static_cast<uint32_t>(RSRenderNodeType::RS_NODE);
    NodeId innerNodeId = 12345;
    bool isTextureExportNode = false;
    data.write(reinterpret_cast<const char*>(&nodeType), sizeof(nodeType));
    data.write(reinterpret_cast<const char*>(&innerNodeId), sizeof(innerNodeId));
    data.write(reinterpret_cast<const char*>(&isTextureExportNode), sizeof(isTextureExportNode));
    // UnmarshalNode(nodeId) reads: positionZ(4), pivotZ(4), priority(1), isOnTree(1),
    // nodeGroupType(1) [if version >= RENDER_METRICS_ADDED], isRepaintBoundary(1) [if version >= ISREPAINT_BOUNDARY]
    float positionZ = 0.0f;
    float pivotZ = 0.0f;
    uint8_t priority = 0;
    bool isOnTree = false;
    uint8_t nodeGroupType = 0;
    bool isRepaintBoundary = false;
    data.write(reinterpret_cast<const char*>(&positionZ), sizeof(positionZ));
    data.write(reinterpret_cast<const char*>(&pivotZ), sizeof(pivotZ));
    data.write(reinterpret_cast<const char*>(&priority), sizeof(priority));
    data.write(reinterpret_cast<const char*>(&isOnTree), sizeof(isOnTree));
    data.write(reinterpret_cast<const char*>(&nodeGroupType), sizeof(nodeGroupType));
    data.write(reinterpret_cast<const char*>(&isRepaintBoundary), sizeof(isRepaintBoundary));
    // UnmarshalNodeModifiers reads: instanceRootNodeId_(8), firstLevelNodeId_(8), modifierCount(4)
    NodeId instanceRoot = 0;
    NodeId firstLevel = 0;
    int32_t modifierCount = 0;
    data.write(reinterpret_cast<const char*>(&instanceRoot), sizeof(instanceRoot));
    data.write(reinterpret_cast<const char*>(&firstLevel), sizeof(firstLevel));
    data.write(reinterpret_cast<const char*>(&modifierCount), sizeof(modifierCount));
    // Trailing byte to prevent eof() from being true after UnmarshalNodeModifiers
    char trailing = 0;
    data.write(&trailing, sizeof(trailing));
    std::string error = RSProfiler::UnmarshalSubTreeLo(*context_sptr, data, attachNode, RSFILE_VERSION_LATEST);
    EXPECT_TRUE(error.find("UnmarshalSubTreeLo:") != std::string::npos);
}

/*
 * @tc.name: RSFileGetEOFTimeEmptyLayers
 * @tc.desc: Test RSFile GetEOFTime with empty layerData
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, RSFileGetEOFTimeEmptyLayers, Level1)
{
    RSFile file;
    double time = file.GetEOFTime();
    EXPECT_EQ(time, 0.0);
}

/*
 * @tc.name: UnmarshalNodesTreeCountReadFail
 * @tc.desc: Test UnmarshalNodes with truncated stream causing second count read failure
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodesTreeCountReadFail, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    uint32_t nodeCount = 0u;
    data.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
    // Stream ends here -- second read (treeCount) will fail
    std::string error = RSProfiler::UnmarshalNodes(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodes: Tree node count out of range");
}

/*
 * @tc.name: UnmarshalNodeModifiersCountReadFail
 * @tc.desc: Test UnmarshalNodeModifiers with truncated stream causing count read failure
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalNodeModifiersCountReadFail, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    RSRenderNode node(123, context_sptr, false);
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId instanceRoot = 0;
    NodeId firstLevel = 0;
    data.write(reinterpret_cast<const char*>(&instanceRoot), sizeof(instanceRoot));
    data.write(reinterpret_cast<const char*>(&firstLevel), sizeof(firstLevel));
    // Stream ends here -- count read will fail
    std::string error = RSProfiler::UnmarshalNodeModifiers(node, data, RSFILE_VERSION_LATEST);
    EXPECT_EQ(error, "UnmarshalNodeModifiers: Modifier count out of range");
}

/*
 * @tc.name: UnmarshalTreeDepthExceeded
 * @tc.desc: Test UnmarshalTree with depth >= maxDepth
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalTreeDepthExceeded, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    std::string error = RSProfiler::UnmarshalTree(*context_sptr, data, RSFILE_VERSION_LATEST, 1024u);
    EXPECT_EQ(error, "UnmarshalTree: Max depth exceeded");
}

/*
 * @tc.name: UnmarshalTreeCountReadFail
 * @tc.desc: Test UnmarshalTree with truncated stream causing count read failure
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalTreeCountReadFail, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId nodeId = 123;
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));
    // Stream ends here -- count read will fail
    std::string error = RSProfiler::UnmarshalTree(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_TRUE(error.find("UnmarshalTree:") != std::string::npos);
}

/*
 * @tc.name: UnmarshalTreeChildNodeIdReadFail
 * @tc.desc: Test UnmarshalTree with truncated stream causing child nodeId read failure
 * @tc.type: FUNC
 */
HWTEST(RSProfilerBaseTest, UnmarshalTreeChildNodeIdReadFail, Level1)
{
    auto context_sptr = std::make_shared<RSContext>();
    std::stringstream data(std::ios::in | std::ios::out | std::ios::binary);
    NodeId nodeId = 123;
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));
    uint32_t count = 1u;
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));
    // Stream ends here -- child nodeId read will fail
    std::string error = RSProfiler::UnmarshalTree(*context_sptr, data, RSFILE_VERSION_LATEST);
    EXPECT_TRUE(error.find("UnmarshalTree:") != std::string::npos);
}

} // namespace OHOS::Rosen
