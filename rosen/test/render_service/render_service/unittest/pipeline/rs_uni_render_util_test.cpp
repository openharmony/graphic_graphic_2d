/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "rs_test_util.h"

#include "pipeline/rs_uni_render_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static sk_sp<SkImage> CreateSkImage();
};

sk_sp<SkImage> RSUniRenderUtilTest::CreateSkImage()
{
    const SkImageInfo info = SkImageInfo::MakeN32(200, 200, kOpaque_SkAlphaType);
    auto surface(SkSurface::MakeRaster(info));
    auto canvas = surface->getCanvas();
    canvas->clear(SK_ColorYELLOW);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    canvas->drawRect(SkRect::MakeXYWH(50, 50, 100, 100), paint);
    return surface->makeImageSnapshot();
}

void RSUniRenderUtilTest::SetUpTestCase() {}
void RSUniRenderUtilTest::TearDownTestCase() {}
void RSUniRenderUtilTest::SetUp() {}
void RSUniRenderUtilTest::TearDown() {}

/*
 * @tc.name: MergeDirtyHistory
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderUtilTest, MergeDirtyHistory, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    int32_t bufferAge = 0;
    RSUniRenderUtil::MergeDirtyHistory(node, bufferAge);
}

/*
 * @tc.name: MergeVisibleDirtyRegion
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderUtilTest, MergeVisibleDirtyRegion, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    (void)RSUniRenderUtil::MergeVisibleDirtyRegion(node);
}

/*
 * @tc.name: SrcRectScaleDown_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, SrcRectScaleDown_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    BufferDrawParam params;
    RSUniRenderUtil::SrcRectScaleDown(params, node);
}

/*
 * @tc.name: CreateBufferDrawParam_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    bool forceCPU = false;
    RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU);
}

/*
 * @tc.name: CreateBufferDrawParam_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    bool forceCPU = false;
    RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU);
}

/*
 * @tc.name: CreateLayerBufferDrawParam_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateLayerBufferDrawParam_001, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: CreateLayerBufferDrawParam_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateLayerBufferDrawParam_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    bool forceCPU = false;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(rsSurfaceRenderNode->GetBuffer(), rsSurfaceRenderNode->GetAcquireFence());
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: DrawCachedImage_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, DrawCachedImage_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    sk_sp<SkImage> image = nullptr;
    RSUniRenderUtil::DrawCachedImage(node, canvas, image);
}

/*
 * @tc.name: DrawCachedImage_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, DrawCachedImage_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    sk_sp<SkImage> image = CreateSkImage();
    RSUniRenderUtil::DrawCachedImage(node, canvas, image);
}

/*
 * @tc.name: AlignedDirtyRegion_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, AlignedDirtyRegion_001, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 0;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegion(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] == alignedRegion.GetRegionRects()[i]);
    }
}

/*
 * @tc.name: AlignedDirtyRegion_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, AlignedDirtyRegion_002, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 2;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegion(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] != alignedRegion.GetRegionRects()[i]);
    }
}

/*
 * @tc.name: HandleSubThreadNode_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, HandleSubThreadNode_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    node.SetIsMainThreadNode(false);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    ASSERT_TRUE(RSUniRenderUtil::HandleSubThreadNode(node, canvas));
}

/*
 * @tc.name: HandleSubThreadNode_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, HandleSubThreadNode_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    node.SetIsMainThreadNode(true);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    ASSERT_FALSE(RSUniRenderUtil::HandleSubThreadNode(node, canvas));
}
} // namespace OHOS::Rosen
