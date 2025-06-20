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

#include "gtest/gtest.h"
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>
#include "param/sys_param.h" 

#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_base_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaeBaseDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr float EPSILON = 1e-6;
    static bool FloatEqual(float a, float b);
    static bool Vector2fEqual(const Vector2f& a, const Vector2f& b);
    static bool Vector4fEqual(const Vector4f& a, const Vector4f& b);

    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;
};

void RSHpaeBaseDataTest::SetUpTestCase() {
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.enabled",0);
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled",0);
    OHOS::system::SetParameter("debug.graphic.hpae.enabled",1);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled",1);

}
void RSHpaeBaseDataTest::TearDownTestCase() {
    OHOS::system::SetParameter("debug.graphic.hpae.enabled",hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled",hpaeAaeSwitch);
}
void RSHpaeBaseDataTest::SetUp() {}
void RSHpaeBaseDataTest::TearDown() {}

bool RSHpaeBaseDataTest::FloatEqual(float a, float b)
{
    return std::fabs(a - b) < EPSILON;
}

bool RSHpaeBaseDataTest::Vector2fEqual(const Vector2f& a, const Vector2f& b)
{
    bool ret = std::fabs(a.x_ - b.x_) < EPSILON;
    ret &=(std::fabs(a.y_ - b.y_) < EPSILON);
    return ret;
}

bool RSHpaeBaseDataTest::Vector4fEqual(const Vector4f& a, const Vector4f& b)
{
    bool ret = std::fabs(a.x_ - b.x_) < EPSILON;
    ret &=(std::fabs(a.y_ - b.y_) < EPSILON);
    ret &=(std::fabs(a.z_ - b.z_) < EPSILON);
    ret &=(std::fabs(a.w_ - b.w_) < EPSILON);
    return ret;
}

/**
 * @tc.name: SetHpaeInputBufferTest
 * @tc.desc: Verify function SetHpaeInputBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, SetHpaeInputBufferTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    HpaeBufferInfo inputBuffer;
    hpaeBaseData.SetHpaeInputBuffer(inputBuffer);
    EXPECT_EQ(hpaeBaseData.inputBufferQueue_.size(), 1);
}

/**
 * @tc.name: SetHpaeOutputBufferTest
 * @tc.desc: Verify function SetHpaeOutputBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, SetHpaeOutputBufferTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    HpaeBufferInfo outputBuffer;
    hpaeBaseData.SetHpaeInputBuffer(outputBuffer);
    EXPECT_EQ(hpaeBaseData.outputBufferQueue_.size(), 1);
}

/**
 * @tc.name: RequestHpaeInputBufferTest
 * @tc.desc: Verify function RequestHpaeInputBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, RequestHpaeInputBufferTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    HpaeBufferInfo inputBuffer;
    hpaeBaseData.SetHpaeInputBuffer(inputBuffer);
    EXPECT_EQ(hpaeBaseData.inputBufferQueue_.size(), 1);
    HpaeBufferInfo buffer = hpaeBaseData.RequestHpaeInputBuffer();
    EXPECT_EQ(hpaeBaseData.inputBufferQueue_.size(), 0);

    buffer = hpaeBaseData.RequestHpaeInputBuffer();
    EXPECT_EQ(hpaeBaseData.inputBufferQueue_.size(), 0);
}

/**
 * @tc.name: RequestHpaeOutputBufferTest
 * @tc.desc: Verify function RequestHpaeOutputBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, RequestHpaeOutputBufferTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    HpaeBufferInfo outputBuffer;
    hpaeBaseData.SetHpaeOutputBuffer(outputBuffer);
    EXPECT_EQ(hpaeBaseData.outputBufferQueue_.size(), 1);
    HpaeBufferInfo buffer = hpaeBaseData.RequestHpaeOutputBuffer();
    EXPECT_EQ(hpaeBaseData.outputBufferQueue_.size(), 0);

    buffer = hpaeBaseData.RequestHpaeOutputBuffer();
    EXPECT_EQ(hpaeBaseData.outputBufferQueue_.size(), 0);
}

/**
 * @tc.name: SyncHpaeStatusTest
 * @tc.desc: Verify function SyncHpaeStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, SyncHpaeStatusTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    HpaeStatus status;
    status.gotHpaeBlurNode = true;
    status.blurNodeId = 1;
    status.hpaeBlurEnabled = true;
    status.tileMode = 2;
    status.blurRadius = 3.0f;
    status.brightness = 4.0f;
    status.saturation = 5.0f;

    hpaeBaseData.SyncHpaeStatus(status);
    EXPECT_TRUE(hpaeBaseData.hpaeStatus_hpaeBlurEnabled);

    bool gotHpaeBlurNode = hpaeBaseData.GetHasHpaeBlurNode();
    EXPECT_TRUE(gotHpaeBlurNode);
    NodeId blurNodeId = hpaeBaseData.GetBlurNodeId();
    EXPECT_EQ(blurNodeId, 1);
    int tileMode = hpaeBaseData.GetTileMode();
    EXPECT_EQ(tileMode, 2);
    float blurRadius = hpaeBaseData.GetBlurRadius();
    EXPECT_TRUE(FloatEqual(blurRadius, 3.0f));
    float brightness  = hpaeBaseData.GetBrightness();
    EXPECT_TRUE(FloatEqual(brightness, 4.0f));
    int saturation = hpaeBaseData.GetSaturation();
    EXPECT_TRUE(FloatEqual(saturation, 5.0f));
}

/**
 * @tc.name: GetPixelStretchTest
 * @tc.desc: Verify function GetPixelStretch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, GetPixelStretchTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    HpaeStatus status;

    status.pixelStretch = std::nullopt;
    hpaeBaseData.SyncHpaeStatus(status);
    Vector4f pixelStretch = hpaeBaseData.GetPixelStretch();
    EXPECT_TRUE(Vector4fEqual(pixelStretch, Vector4f(0.f, 0.f, 0.f, 0.f)));

    status.pixelStretch = Vector4f(1.f, 2.f, 3.f, 4.f);
    hpaeBaseData.SyncHpaeStatus(status);
    pixelStretch = hpaeBaseData.GetPixelStretch();
    EXPECT_TRUE(Vector4fEqual(pixelStretch, Vector4f(1.f, 2.f, 3.f, 4.f)));
}

/**
 * @tc.name: GetGreyCoefTest
 * @tc.desc: Verify function GetGreyCoef
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, GetGreyCoefTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    HpaeStatus status;

    status.greyCoef = std::nullopt;
    hpaeBaseData.SyncHpaeStatus(status);
    Vector2f greyCoef = hpaeBaseData.GetGreyCoef();
    EXPECT_TRUE(Vector2fEqual(greyCoef, Vector2f(0.f, 0.f)));

    status.greyCoef = Vector2f(1.f, 2.f);
    hpaeBaseData.SyncHpaeStatus(status);
    greyCoef = hpaeBaseData.GetGreyCoef();
    EXPECT_TRUE(Vector2fEqual(greyCoef, Vector2f(1.f, 2.f)));
}

/**
 * @tc.name: IsFirstFrameTest
 * @tc.desc: Verify function SetIsFirstFrame/GetIsFirstFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, IsFirstFrameTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;
    hpaeBaseData.SetIsFirstFrame(false);
    EXPECT_FALSE(hpaeBaseData.isFirstFrame_);

    hpaeBaseData.SetIsFirstFrame(true);
    EXPECT_TRUE(hpaeBaseData.isFirstFrame_);

    bool isFirstFrame = hpaeBaseData.GetIsFirstFrame();
    EXPECT_TRUE(isFirstFrame);
}

/**
 * @tc.name: NeedResetTest
 * @tc.desc: Verify function GetNeedReset/SetResetDone
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, NeedResetTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;

    hpaeBaseData.SetIsFirstFrame(true);
    bool needReset = hpaeBaseData.GetNeedReset();
    EXPECT_TRUE(needReset);

    hpaeBaseData.SetResetDone();
    EXPECT_FALSE(hpaeBaseData.needReset_);
}

/**
 * @tc.name: BlurContentChangedTest
 * @tc.desc: Verify function SetBlurContentChanged/GetBlurContentChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, BlurContentChangedTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;

    hpaeBaseData.SetBlurContentChanged(true);
    
    EXPECT_TRUE(hpaeBaseData.blurContentChanged_);

    bool blurContentChanged = hpaeBaseData.GetBlurContentChanged();
    EXPECT_TRUE(blurContentChanged);
}

/**
 * @tc.name: BufferUsedTest
 * @tc.desc: Verify function NotifyBufferUsed/GetBufferUsed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBaseDataTest, BufferUsedTest, TestSize.Level1)
{
    RSHpaeBaseData hpaeBaseData;

    hpaeBaseData.NotifyBufferUsed(true);
    EXPECT_TRUE(hpaeBaseData.bufferUsed_);

    bool bufferUsed = hpaeBaseData.GetBufferUsed();
    EXPECT_TRUE(bufferUsed);
}

} // namespace Rosen
} // namespace OHOS