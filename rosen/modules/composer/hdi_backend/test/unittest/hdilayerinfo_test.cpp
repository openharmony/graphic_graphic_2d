/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_layer_info.h"
#include "sync_fence.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiLayerInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiLayerInfo> hdiLayerInfo_;
};

void HdiLayerInfoTest::SetUpTestCase()
{
    hdiLayerInfo_ = HdiLayerInfo::CreateHdiLayerInfo();
}

void HdiLayerInfoTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: GetZorder001
 * @tc.desc: Verify the GetZorder of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetZorder001, Function | MediumTest| Level3)
{
    HdiLayerInfoTest::hdiLayerInfo_->SetZorder(1);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetZorder(), 1u);
}

/**
 * @tc.name: GetSurface001
 * @tc.desc: Verify the GetSurface of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetSurface001, Function | MediumTest| Level3)
{
    sptr<Surface> surface = nullptr;
    HdiLayerInfoTest::hdiLayerInfo_->SetSurface(surface);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetSurface(), nullptr);
}

/**
 * @tc.name: GetBuffer001
 * @tc.desc: Verify the GetBuffer of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetBuffer001, Function | MediumTest| Level3)
{
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    sptr<SurfaceBuffer> sbuffer = nullptr;
    sptr<SurfaceBuffer> preBuffer = nullptr;
    sptr<SyncFence> preAcquireFence = SyncFence::INVALID_FENCE;
    HdiLayerInfoTest::hdiLayerInfo_->SetBuffer(sbuffer, acquireFence, preBuffer, preAcquireFence);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBuffer(), nullptr);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetPreBuffer(), nullptr);
}

/**
 * @tc.name: GetAcquireFence001
 * @tc.desc: Verify the GetAcquireFence of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetAcquireFence001, Function | MediumTest| Level3)
{
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->GetAcquireFence(), nullptr);
}

/**
 * @tc.name: GetPreAcquireFence001
 * @tc.desc: Verify the GetPreAcquireFence of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetPreAcquireFence001, Function | MediumTest| Level3)
{
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->GetPreAcquireFence(), nullptr);
}

/**
 * @tc.name: GetAlpha001
 * @tc.desc: Verify the GetAlpha of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetAlpha001, Function | MediumTest| Level3)
{
    LayerAlpha layerAlpha = {
        .enGlobalAlpha = true,
        .enPixelAlpha = true,
        .alpha0 = 0,
        .alpha1 = 0,
        .gAlpha = 0,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetAlpha(layerAlpha);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().enGlobalAlpha, layerAlpha.enGlobalAlpha);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().enPixelAlpha, layerAlpha.enPixelAlpha);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().alpha0, layerAlpha.alpha0);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().alpha1, layerAlpha.alpha1);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().gAlpha, layerAlpha.gAlpha);
}

/**
 * @tc.name: GetTransformType001
 * @tc.desc: Verify the GetTransformType of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetTransformType001, Function | MediumTest| Level3)
{
    TransformType type = TransformType::ROTATE_90;
    HdiLayerInfoTest::hdiLayerInfo_->SetTransform(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTransformType(), TransformType::ROTATE_90);

    type = TransformType::ROTATE_180;
    HdiLayerInfoTest::hdiLayerInfo_->SetTransform(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTransformType(), TransformType::ROTATE_180);

    type = TransformType::ROTATE_270;
    HdiLayerInfoTest::hdiLayerInfo_->SetTransform(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTransformType(), TransformType::ROTATE_270);
}

/**
 * @tc.name: GetCompositionType001
 * @tc.desc: Verify the GetCompositionType of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetCompositionType001, Function | MediumTest| Level3)
{
    CompositionType type = CompositionType::COMPOSITION_CLIENT;
    HdiLayerInfoTest::hdiLayerInfo_->SetCompositionType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCompositionType(), CompositionType::COMPOSITION_CLIENT);

    type = CompositionType::COMPOSITION_DEVICE;
    HdiLayerInfoTest::hdiLayerInfo_->SetCompositionType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCompositionType(), CompositionType::COMPOSITION_DEVICE);

    type = CompositionType::COMPOSITION_CURSOR;
    HdiLayerInfoTest::hdiLayerInfo_->SetCompositionType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCompositionType(), CompositionType::COMPOSITION_CURSOR);
}

/**
 * @tc.name: GetVisibleNum001
 * @tc.desc: Verify the GetVisibleNum of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetVisibleNum001, Function | MediumTest| Level3)
{
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetVisibleRegion(1, iRect);
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->GetVisibleNum(), 0u);
}

/**
 * @tc.name: GetVisibleRegion001
 * @tc.desc: Verify the GetVisibleRegion of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetVisibleRegion001, Function | MediumTest| Level3)
{
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetVisibleRegion(1, iRect);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetVisibleRegion().x, iRect.x);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetVisibleRegion().y, iRect.y);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetVisibleRegion().w, iRect.w);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetVisibleRegion().h, iRect.h);
}

/**
 * @tc.name: GetDirtyRegion001
 * @tc.desc: Verify the GetDirtyRegion of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetDirtyRegion001, Function | MediumTest| Level3)
{
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetDirtyRegion(iRect);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetDirtyRegion().x, iRect.x);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetDirtyRegion().y, iRect.y);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetDirtyRegion().w, iRect.w);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetDirtyRegion().h, iRect.h);
}

/**
 * @tc.name: GetBlendType001
 * @tc.desc: Verify the GetBlendType of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetBlendType001, Function | MediumTest| Level3)
{
    BlendType type = BlendType::BLEND_CLEAR;
    HdiLayerInfoTest::hdiLayerInfo_->SetBlendType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBlendType(), BlendType::BLEND_CLEAR);

    type = BlendType::BLEND_SRC;
    HdiLayerInfoTest::hdiLayerInfo_->SetBlendType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBlendType(), BlendType::BLEND_SRC);

    type = BlendType::BLEND_SRCOVER;
    HdiLayerInfoTest::hdiLayerInfo_->SetBlendType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBlendType(), BlendType::BLEND_SRCOVER);
}

/**
 * @tc.name: GetCropRect001
 * @tc.desc: Verify the GetCropRect of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetCropRect001, Function | MediumTest| Level3)
{
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetCropRect(iRect);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().x, iRect.x);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().y, iRect.y);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().w, iRect.w);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().h, iRect.h);
}

/**
 * @tc.name: GetLayerSize001
 * @tc.desc: Verify the GetLayerSize of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetLayerSize001, Function | MediumTest| Level3)
{
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetLayerSize(iRect);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().x, iRect.x);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().y, iRect.y);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().w, iRect.w);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().h, iRect.h);
}

/**
 * @tc.name: IsPreMulti001
 * @tc.desc: Verify the IsPreMulti of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, IsPreMulti001, Function | MediumTest| Level3)
{
    HdiLayerInfoTest::hdiLayerInfo_->SetPreMulti(true);
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->IsPreMulti(), false);

    HdiLayerInfoTest::hdiLayerInfo_->SetPreMulti(false);
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->IsPreMulti(), true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS