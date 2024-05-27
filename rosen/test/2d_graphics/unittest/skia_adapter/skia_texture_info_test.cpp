/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstddef>

#include "gtest/gtest.h"
#include "skia_adapter/skia_texture_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextureInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaTextureInfoTest::SetUpTestCase() {}
void SkiaTextureInfoTest::TearDownTestCase() {}
void SkiaTextureInfoTest::SetUp() {}
void SkiaTextureInfoTest::TearDown() {}

/**
 * @tc.name: ConvertToGrSurfaceOrigin001
 * @tc.desc: Test ConvertToGrSurfaceOrigin
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextureInfoTest, ConvertToGrSurfaceOrigin001, TestSize.Level1)
{
    GrSurfaceOrigin orign1 = SkiaTextureInfo::ConvertToGrSurfaceOrigin(TextureOrigin::TOP_LEFT);
    EXPECT_EQ(orign1, GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin);
    GrSurfaceOrigin orign2 = SkiaTextureInfo::ConvertToGrSurfaceOrigin(TextureOrigin::BOTTOM_LEFT);
    EXPECT_EQ(orign2, GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin);
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ConvertToGrBackendVKTexture001
 * @tc.desc: Test ConvertToGrBackendVKTexture
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextureInfoTest, ConvertToGrBackendVKTexture001, TestSize.Level1)
{
    TextureInfo info;
    SkiaTextureInfo::ConvertToGrBackendVKTexture(info);
}
#endif

/**
 * @tc.name: ConvertToTextureInfo001
 * @tc.desc: Test ConvertToTextureInfo
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextureInfoTest, ConvertToTextureInfo001, TestSize.Level1)
{
    GrMockTextureInfo mockInfo;
    GrBackendTexture texture(1280, 720, GrMipmapped::kNo, mockInfo);
    TextureInfo textureInfo = SkiaTextureInfo::ConvertToTextureInfo(texture);

    EXPECT_EQ(textureInfo.GetWidth(), 1280);
    EXPECT_EQ(textureInfo.GetHeight(), 720);
    EXPECT_FALSE(textureInfo.GetIsMipMapped());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS