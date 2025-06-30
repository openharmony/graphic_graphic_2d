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

#include <memory>
#include "gtest/gtest.h"

#include "draw/surface.h"
#include "effect/color_space.h"
#include "image/gpu_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SurfaceTest::SetUpTestCase() {}
void SurfaceTest::TearDownTestCase() {}
void SurfaceTest::SetUp() {}
void SurfaceTest::TearDown() {}

/**
 * @tc.name: Surface001
 * @tc.desc: Test for creating Surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, Surface001, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
}

/**
 * @tc.name: SurfaceBind001
 * @tc.desc: Test for binding raster Surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, SurfaceBind001, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    Bitmap bitmap;
    ASSERT_FALSE(surface->Bind(bitmap));
}

/**
 * @tc.name: SurfaceBind002
 * @tc.desc: Test for binding raster Surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, SurfaceBind002, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));
}

#ifdef RS_ENABLE_GPU
/**
 * @tc.name: SurfaceBind003
 * @tc.desc: Test for binding texture Surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, SurfaceBind003, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    Image image;
    ASSERT_FALSE(surface->Bind(image));
}

/**
 * @tc.name: SurfaceBind004
 * @tc.desc: Test for binding FrameBuffer Surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, SurfaceBind004, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    FrameBuffer info;
    ASSERT_FALSE(surface->Bind(info));

    info.gpuContext = std::make_shared<GPUContext>();
    ASSERT_FALSE(surface->Bind(info));

    info.colorSpace = ColorSpace::CreateSRGB();
    ASSERT_FALSE(surface->Bind(info));
}
#endif

/**
 * @tc.name: GetCanvas001
 * @tc.desc: Test for geting Canvas that draws into Surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, GetCanvas001, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    auto ret = surface->GetCanvas();
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetCanvas002
 * @tc.desc: Test for geting Canvas that draws into Surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, GetCanvas002, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));
    auto ret = surface->GetCanvas();
    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetImageSnapshot001
 * @tc.desc: Test for geting Image capturing Surface contents.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, GetImageSnapshot001, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    auto ret = surface->GetImageSnapshot();
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetImageSnapshot002
 * @tc.desc: Test for geting Image capturing Surface contents.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, GetImageSnapshot002, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));
    auto ret = surface->GetImageSnapshot();
    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetImageSnapshot003
 * @tc.desc: Test for geting Image capturing Surface contents.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, GetImageSnapshot003, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    RectI bounds;
    auto ret = surface->GetImageSnapshot(bounds);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetImageSnapshot004
 * @tc.desc: Test for geting Image capturing Surface contents.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, GetImageSnapshot004, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));
    RectI bounds(1, 1, 15, 20);
    auto ret = surface->GetImageSnapshot(bounds);
    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetImageSnapshot005
 * @tc.desc: Test for geting Image capturing Surface contents, when bounds does not intersect the surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
 */
HWTEST_F(SurfaceTest, GetImageSnapshot005, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));
    RectI bounds(70, 75, 100, 100);
    auto ret = surface->GetImageSnapshot(bounds);
    ASSERT_TRUE(ret == nullptr);
}

#ifdef RS_ENABLE_GL
/**
 * @tc.name: Wait006
 * @tc.desc: Test wait for the surface.
 * @tc.type: FUNC
 * @tc.require:IALEIN
 */
HWTEST_F(SurfaceTest, Wait006, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    std::vector<GrGLsync> syncs;
    surface->Wait(syncs);
}
#endif

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ClearDrawingArea001
 * @tc.desc: Test wait for the surface.
 * @tc.type: FUNC
 * @tc.require:IALEIN
 */
HWTEST_F(SurfaceTest, ClearDrawingArea001, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    surface->ClearDrawingArea();
}
#endif

/**
 * @tc.name: GetHeadroom001
 * @tc.desc: Test for getting headroom from surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
*/
HWTEST_F(SurfaceTest, GetHeadroom001, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    ASSERT_EQ(surface->GetHeadroom(), 1.0f);
}

/**
 * @tc.name: SetHeadroom001
 * @tc.desc: Test for setting headroom to surface.
 * @tc.type: FUNC
 * @tc.require:I774GD
*/
HWTEST_F(SurfaceTest, SetHeadroom001, TestSize.Level1)
{
    auto surface = std::make_unique<Surface>();
    ASSERT_TRUE(surface != nullptr);
    surface->SetHeadroom(1.5f);
    ASSERT_EQ(surface->GetHeadroom(), 1.0f);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
