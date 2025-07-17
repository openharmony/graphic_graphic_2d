/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "parcel.h"
#include <string>
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-ext.h"
#include "rs_profiler.h"
#include "rs_profiler_log.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_pixelmap.h"
#include "draw/color.h"
#include "image/image.h"
#include "utils/data.h"

#include "rs_profiler_command.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

/*
* @tc.name: IsPlaybackParcel
* @tc.desc: Test IsPlaybackParcel
* @tc.type: FUNC
* @tc.require:
*/
HWTEST(RSProfilerBaseTest, IsPlaybackParcel, Level1)
{
    RSProfiler::SetMode(Mode::READ_EMUL);
    Parcel parcel;
    EXPECT_FALSE(RSProfiler::IsPlaybackParcel(parcel));
}

/*
* @tc.name: Interface Test
* @tc.desc: RSProfiler Interface Test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST(RSProfilerBaseTest, SurfaceOnDrawMatchOptimize, Level1)
{
    EXPECT_NO_THROW({
        bool defaultValue = false;

        RSProfiler::testing_ = true;
        bool useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::READ_EMUL);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_TRUE(useNodeMatchOptimize);

        useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::NONE);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_FALSE(useNodeMatchOptimize);

        RSProfiler::testing_ = false;
        useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::READ_EMUL);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_FALSE(useNodeMatchOptimize);

        useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::NONE);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_FALSE(useNodeMatchOptimize);

        defaultValue = true;

        RSProfiler::testing_ = true;
        useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::READ_EMUL);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_TRUE(useNodeMatchOptimize);

        useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::NONE);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_TRUE(useNodeMatchOptimize);

        RSProfiler::testing_ = false;
        useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::READ_EMUL);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_TRUE(useNodeMatchOptimize);

        useNodeMatchOptimize = defaultValue;
        RSProfiler::SetMode(Mode::NONE);
        RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize);
        EXPECT_TRUE(useNodeMatchOptimize);
    });
}

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
    options.useDMA = false;
    auto pixelMap = Media::PixelMap::Create(options);

    RSProfilerLogMsg msg;
    do {
        msg = RSProfiler::ReceiveRSLogBase();
    } while (msg.time_);

    constexpr uint64_t testId = 123;
    std::string checkValue = "{\"id\":123,"
        "\"type\":\"SHARED\",\"width\":128,\"height\":256,\"stride\":512,\"format\":3}";

    PixelMapStorage::Push(testId, *pixelMap);

    msg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);
    EXPECT_EQ((msg.type_ == RSProfilerLogType::PIXELMAP && msg.msg_.find(checkValue) != std::string::npos), true);
}

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
    options.useDMA = true;
    auto pixelMap = Media::PixelMap::Create(options);

    RSProfilerLogMsg msg;
    do {
        msg = RSProfiler::ReceiveRSLogBase();
    } while (msg.time_);

    constexpr uint64_t testId = 321;
    std::string checkValue = "{\"id\":321,"
        "\"type\":\"DMA\",\"width\":1024,\"height\":512,\"stride\":4096,\"format\":12}";

    PixelMapStorage::Push(testId, *pixelMap);

    msg = RSProfiler::ReceiveRSLogBase();
    RSProfiler::SetMode(Mode::NONE);
    EXPECT_EQ((msg.type_ == RSProfilerLogType::PIXELMAP && msg.msg_.find(checkValue) != std::string::npos), true);
}

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

HWTEST(RSProfilerBaseTest, RSLOGW2, Level1)
{
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::READ_EMUL);

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
    protected:
    void SetUp() override
    {
        renderService = GetAndInitRenderService();
        RSProfiler::Init(renderService);
    }

    private:
    sptr<RSRenderService> GetAndInitRenderService()
    {
        sptr<RSRenderService> renderService(new RSRenderService());
        if (renderService) {
            renderService->mainThread_ = RSMainThread::Instance();
        }
        if (renderService->mainThread_) {
            renderService->mainThread_->context_ = std::make_shared<RSContext>();
            renderService->mainThread_->context_->Initialize();
        }
        return renderService;
    }

    private:
    sptr<RSRenderService> renderService;
};


HWTEST_F(RecorRsProfileRecordTest, RecordStartFail, Level1)
{
    ArgList args;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::READ); // Not NONE
    RSProfiler::RecordStart(args); // abort starting record
    EXPECT_FALSE(RSProfiler::IsWriteMode());
    EXPECT_FALSE(RSProfiler::IsRecordingMode());
}

HWTEST_F(RecorRsProfileRecordTest, RecordStartSuccess, Level1)
{
    ArgList args;
    RSProfiler::testing_ = true;
    RSProfiler::SetMode(Mode::NONE); // Set NONE
    RSProfiler::RecordStart(args); // record starting
    EXPECT_TRUE(RSProfiler::IsWriteMode());
    EXPECT_TRUE(RSProfiler::IsRecordingMode());
}

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

HWTEST_F(RecorRsProfileRecordTest, RecordMetrics, Level1)
{
    EXPECT_NO_THROW({
        RSProfiler::RecordMetrics(ArgList());
    });
}

} // namespace OHOS::Rosen
