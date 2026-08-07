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

#include <variant>

#include "gtest/gtest.h"
#include "property/rs_spatial_effect_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SpatialEffectDefTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SpatialEffectDefTest::SetUpTestCase() {}
void SpatialEffectDefTest::TearDownTestCase() {}
void SpatialEffectDefTest::SetUp() {}
void SpatialEffectDefTest::TearDown() {}

// ==================== DepthCameraPara tests ====================

/**
 * @tc.name: DepthCameraParaEquality001
 * @tc.desc: Verify default DepthCameraPara equals another default
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthCameraParaEquality001, TestSize.Level1)
{
    DepthCameraPara para1;
    DepthCameraPara para2;
    EXPECT_TRUE(para1 == para2);
}

/**
 * @tc.name: DepthCameraParaEquality002
 * @tc.desc: Verify DepthCameraPara with same values are equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthCameraParaEquality002, TestSize.Level1)
{
    DepthCameraPara para1;
    para1.position = {1.0f, 2.0f, 3.0f};
    para1.quaternion = {0.0f, 0.0f, 0.0f, 1.0f};
    para1.yFov = 60.0f;
    para1.zNear = 0.1f;
    para1.zFar = 100.0f;

    DepthCameraPara para2;
    para2.position = {1.0f, 2.0f, 3.0f};
    para2.quaternion = {0.0f, 0.0f, 0.0f, 1.0f};
    para2.yFov = 60.0f;
    para2.zNear = 0.1f;
    para2.zFar = 100.0f;

    EXPECT_TRUE(para1 == para2);
}

/**
 * @tc.name: DepthCameraParaEquality003
 * @tc.desc: Verify DepthCameraPara with different position are not equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthCameraParaEquality003, TestSize.Level1)
{
    DepthCameraPara para1;
    para1.position = {1.0f, 2.0f, 3.0f};
    DepthCameraPara para2;
    para2.position = {4.0f, 5.0f, 6.0f};
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: DepthCameraParaEquality004
 * @tc.desc: Verify DepthCameraPara with different yFov are not equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthCameraParaEquality004, TestSize.Level1)
{
    DepthCameraPara para1;
    para1.yFov = 45.0f;
    DepthCameraPara para2;
    para2.yFov = 90.0f;
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: DepthCameraParaEquality005
 * @tc.desc: Verify DepthCameraPara with different quaternion are not equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthCameraParaEquality005, TestSize.Level1)
{
    DepthCameraPara para1;
    para1.quaternion = {0.0f, 0.0f, 0.0f, 1.0f};
    DepthCameraPara para2;
    para2.quaternion = {1.0f, 0.0f, 0.0f, 0.0f};
    EXPECT_FALSE(para1 == para2);
}

// ==================== DepthLightPara tests ====================

/**
 * @tc.name: DepthLightParaEquality001
 * @tc.desc: Verify default DepthLightPara equals another default
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthLightParaEquality001, TestSize.Level1)
{
    DepthLightPara para1;
    DepthLightPara para2;
    EXPECT_TRUE(para1 == para2);
}

/**
 * @tc.name: DepthLightParaEquality002
 * @tc.desc: Verify DepthLightPara with same values are equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthLightParaEquality002, TestSize.Level1)
{
    DepthLightPara para1;
    para1.direction = {0.0f, -1.0f, 0.0f};
    para1.color = {1.0f, 1.0f, 1.0f};
    para1.intensity = 0.8f;

    DepthLightPara para2;
    para2.direction = {0.0f, -1.0f, 0.0f};
    para2.color = {1.0f, 1.0f, 1.0f};
    para2.intensity = 0.8f;

    EXPECT_TRUE(para1 == para2);
}

/**
 * @tc.name: DepthLightParaEquality003
 * @tc.desc: Verify DepthLightPara with different direction are not equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthLightParaEquality003, TestSize.Level1)
{
    DepthLightPara para1;
    para1.direction = {0.0f, -1.0f, 0.0f};
    DepthLightPara para2;
    para2.direction = {1.0f, 0.0f, 0.0f};
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: DepthLightParaEquality004
 * @tc.desc: Verify DepthLightPara with different intensity are not equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthLightParaEquality004, TestSize.Level1)
{
    DepthLightPara para1;
    para1.intensity = 0.5f;
    DepthLightPara para2;
    para2.intensity = 1.0f;
    EXPECT_FALSE(para1 == para2);
}

// ==================== DepthEffectPara tests ====================

/**
 * @tc.name: DepthEffectParaDefaultConstructor001
 * @tc.desc: Verify DepthEffectPara default values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthEffectParaDefaultConstructor001, TestSize.Level1)
{
    DepthEffectPara para;
    EXPECT_FLOAT_EQ(para.depth, 0.0f);
    EXPECT_FLOAT_EQ(para.occlusionWeight, 0.0f);
}

/**
 * @tc.name: DepthEffectParaEquality001
 * @tc.desc: Verify DepthEffectPara operator== with equal values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthEffectParaEquality001, TestSize.Level1)
{
    DepthEffectPara para1;
    para1.depth = 1.0f;
    para1.occlusionWeight = 0.5f;

    DepthEffectPara para2;
    para2.depth = 1.0f;
    para2.occlusionWeight = 0.5f;

    EXPECT_TRUE(para1 == para2);
}

/**
 * @tc.name: DepthEffectParaEquality002
 * @tc.desc: Verify DepthEffectPara operator== with different depth
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, DepthEffectParaEquality002, TestSize.Level1)
{
    DepthEffectPara para1;
    para1.depth = 1.0f;
    DepthEffectPara para2;
    para2.depth = 2.0f;
    EXPECT_FALSE(para1 == para2);
}

// ==================== SpatialEffectPara tests ====================

/**
 * @tc.name: SpatialEffectParaDefaultConstructor001
 * @tc.desc: Verify SpatialEffectPara default values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectParaDefaultConstructor001, TestSize.Level1)
{
    SpatialEffectPara para;
    EXPECT_FLOAT_EQ(para.occlusionWeight, 0.0f);
    EXPECT_EQ(para.spatialEffectMode, SpatialEffectMode::WORLD_XYZ_MODE);
}

/**
 * @tc.name: SpatialEffectParaEquality001
 * @tc.desc: Verify SpatialEffectPara operator== with equal values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectParaEquality001, TestSize.Level1)
{
    SpatialEffectPara para1;
    para1.leftTop = {1.0f, 2.0f, 3.0f};
    para1.rightTop = {4.0f, 5.0f, 6.0f};
    para1.leftBottom = {7.0f, 8.0f, 9.0f};
    para1.rightBottom = {10.0f, 11.0f, 12.0f};
    para1.occlusionWeight = 0.5f;

    SpatialEffectPara para2;
    para2.leftTop = {1.0f, 2.0f, 3.0f};
    para2.rightTop = {4.0f, 5.0f, 6.0f};
    para2.leftBottom = {7.0f, 8.0f, 9.0f};
    para2.rightBottom = {10.0f, 11.0f, 12.0f};
    para2.occlusionWeight = 0.5f;

    EXPECT_TRUE(para1 == para2);
}

/**
 * @tc.name: SpatialEffectParaEquality002
 * @tc.desc: Verify SpatialEffectPara operator== with different leftTop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectParaEquality002, TestSize.Level1)
{
    SpatialEffectPara para1;
    para1.leftTop = {1.0f, 2.0f, 3.0f};
    SpatialEffectPara para2;
    para2.leftTop = {9.0f, 8.0f, 7.0f};
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: SpatialEffectParaEquality003
 * @tc.desc: Verify SpatialEffectPara operator== with different occlusionWeight
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectParaEquality003, TestSize.Level1)
{
    SpatialEffectPara para1;
    para1.occlusionWeight = 0.1f;
    SpatialEffectPara para2;
    para2.occlusionWeight = 0.9f;
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: SpatialEffectParaAssignment001
 * @tc.desc: Verify SpatialEffectPara operator= works correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectParaAssignment001, TestSize.Level1)
{
    SpatialEffectPara para1;
    para1.leftTop = {1.0f, 2.0f, 3.0f};
    para1.rightTop = {4.0f, 5.0f, 6.0f};
    para1.leftBottom = {7.0f, 8.0f, 9.0f};
    para1.rightBottom = {10.0f, 11.0f, 12.0f};
    para1.occlusionWeight = 0.5f;

    SpatialEffectPara para2;
    para2 = para1;

    EXPECT_FLOAT_EQ(para2.leftTop.x_, 1.0f);
    EXPECT_FLOAT_EQ(para2.leftTop.y_, 2.0f);
    EXPECT_FLOAT_EQ(para2.leftTop.z_, 3.0f);
    EXPECT_FLOAT_EQ(para2.rightTop.x_, 4.0f);
    EXPECT_FLOAT_EQ(para2.leftBottom.z_, 9.0f);
    EXPECT_FLOAT_EQ(para2.rightBottom.x_, 10.0f);
    EXPECT_FLOAT_EQ(para2.occlusionWeight, 0.5f);
}

// ==================== SpatialEffectVariantPara tests ====================

/**
 * @tc.name: SpatialEffectVariantParaDefaultConstructor001
 * @tc.desc: Verify SpatialEffectVariantPara default constructor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaDefaultConstructor001, TestSize.Level1)
{
    SpatialEffectVariantPara para;
    EXPECT_FALSE(para.PerspectiveEnabled());
    EXPECT_FLOAT_EQ(para.occlusionWeight, 0.0f);
    EXPECT_EQ(para.spatialEffectMode, SpatialEffectMode::WORLD_XYZ_MODE);
}

/**
 * @tc.name: SpatialEffectVariantParaDepthEffectConstructor001
 * @tc.desc: Verify SpatialEffectVariantPara constructor from DepthEffectPara
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaDepthEffectConstructor001, TestSize.Level1)
{
    DepthEffectPara depthPara;
    depthPara.depth = 0.8f;
    depthPara.occlusionWeight = 0.3f;

    SpatialEffectVariantPara para(depthPara);

    EXPECT_FALSE(para.PerspectiveEnabled());
    EXPECT_FLOAT_EQ(para.occlusionWeight, 0.3f);
    float depthValue = std::get<float>(para.position);
    EXPECT_FLOAT_EQ(depthValue, 0.8f);
}

/**
 * @tc.name: SpatialEffectVariantParaSpatialEffectConstructor001
 * @tc.desc: Verify SpatialEffectVariantPara constructor from SpatialEffectPara
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaSpatialEffectConstructor001, TestSize.Level1)
{
    SpatialEffectPara spatialPara;
    spatialPara.leftTop = {1.0f, 2.0f, 3.0f};
    spatialPara.rightTop = {4.0f, 5.0f, 6.0f};
    spatialPara.leftBottom = {7.0f, 8.0f, 9.0f};
    spatialPara.rightBottom = {10.0f, 11.0f, 12.0f};
    spatialPara.occlusionWeight = 0.7f;
    spatialPara.spatialEffectMode = SpatialEffectMode::NDC_XY_WORLD_Z_MODE;

    SpatialEffectVariantPara para(spatialPara);

    EXPECT_TRUE(para.PerspectiveEnabled());
    EXPECT_FLOAT_EQ(para.occlusionWeight, 0.7f);
    EXPECT_EQ(para.spatialEffectMode, SpatialEffectMode::NDC_XY_WORLD_Z_MODE);
    auto corners = std::get<SpatialEffectPara::CornerPositions>(para.position);
    EXPECT_FLOAT_EQ(corners[0].x_, 1.0f);
    EXPECT_FLOAT_EQ(corners[1].y_, 5.0f);
    EXPECT_FLOAT_EQ(corners[2].z_, 9.0f);
    EXPECT_FLOAT_EQ(corners[3].x_, 10.0f);
}

/**
 * @tc.name: SpatialEffectVariantParaEquality001
 * @tc.desc: Verify SpatialEffectVariantPara operator== with position holds float
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaEquality001, TestSize.Level1)
{
    SpatialEffectVariantPara para1;
    para1.position = 1.0f;
    para1.occlusionWeight = 0.5f;

    SpatialEffectVariantPara para2;
    para2.position = 1.0f;
    para2.occlusionWeight = 0.5f;

    EXPECT_TRUE(para1 == para2);

    para2.position = 2.0f;
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: SpatialEffectVariantParaEquality002
 * @tc.desc: Verify SpatialEffectVariantPara operator== with position holds CornerPositions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaEquality002, TestSize.Level1)
{
    SpatialEffectPara::CornerPositions corners = {Vector3f(1.0f, 2.0f, 3.0f), Vector3f(4.0f, 5.0f, 6.0f),
        Vector3f(7.0f, 8.0f, 9.0f), Vector3f(10.0f, 11.0f, 12.0f)};

    SpatialEffectVariantPara para1;
    para1.position = corners;
    para1.occlusionWeight = 0.5f;

    SpatialEffectVariantPara para2;
    para2.position = corners;
    para2.occlusionWeight = 0.5f;

    EXPECT_TRUE(para1 == para2);

    para2.occlusionWeight = 0.6f;
    EXPECT_FALSE(para1 == para2);
    para2.occlusionWeight = 0.5f;

    std::get<SpatialEffectPara::CornerPositions>(para1.position)[0].x_ = 0.0f;
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: SpatialEffectVariantParaEquality003
 * @tc.desc: Verify SpatialEffectVariantPara operator== with position holds different types
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaEquality003, TestSize.Level1)
{
    SpatialEffectPara::CornerPositions corners = {Vector3f(1.0f, 2.0f, 3.0f), Vector3f(4.0f, 5.0f, 6.0f),
        Vector3f(7.0f, 8.0f, 9.0f), Vector3f(10.0f, 11.0f, 12.0f)};

    SpatialEffectVariantPara para1;
    para1.position = 1.0f;
    para1.occlusionWeight = 0.5f;

    SpatialEffectVariantPara para2;
    para2.position = corners;
    para2.occlusionWeight = 0.5f;

    EXPECT_FALSE(para1 == para2);

    para1.position = corners;
    para2.position = 1.0f;
    EXPECT_FALSE(para1 == para2);
}

/**
 * @tc.name: SpatialEffectVariantParaPerspectiveEnabled001
 * @tc.desc: Verify PerspectiveEnabled returns false for float position
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaPerspectiveEnabled001, TestSize.Level1)
{
    SpatialEffectVariantPara para;
    para.position = 0.5f;
    para.occlusionWeight = 0.2f;

    EXPECT_FALSE(para.PerspectiveEnabled());
}

/**
 * @tc.name: SpatialEffectVariantParaPerspectiveEnabled002
 * @tc.desc: Verify PerspectiveEnabled returns true for CornerPositions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SpatialEffectDefTest, SpatialEffectVariantParaPerspectiveEnabled002, TestSize.Level1)
{
    SpatialEffectVariantPara para;
    SpatialEffectPara::CornerPositions corners = {Vector3f(1.0f, 2.0f, 3.0f), Vector3f(4.0f, 5.0f, 6.0f),
        Vector3f(7.0f, 8.0f, 9.0f), Vector3f(10.0f, 11.0f, 12.0f)};
    para.position = corners;
    para.occlusionWeight = 0.4f;

    EXPECT_TRUE(para.PerspectiveEnabled());
}
} // namespace OHOS::Rosen
