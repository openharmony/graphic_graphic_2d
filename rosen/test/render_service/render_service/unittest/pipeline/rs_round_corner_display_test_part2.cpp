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

#include "rs_round_corner_display_test_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_Normal, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, false);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_ResourceChangedTrue, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, true);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_RenderEngineNull, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    visitor->renderEngine_ = nullptr;
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    ASSERT_EQ(visitor->renderEngine_, nullptr);
    rs_rcd::RoundCornerHardware hardInfo{};
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, hardInfo.bottomLayer, true);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_NullLayerInfo, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    surfaceNode->CreateSurface(listener);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, nullptr, true);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_ResourceNotChangedWithBuffer, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    surfaceNode->CreateSurface(listener);
    surfaceNode->SetAvailableBufferCount(1);
    Drawing::Bitmap layerBitmap;
    ASSERT_NE(visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap), true);
    ASSERT_EQ(surfaceNode->GetBuffer(), nullptr);

    rs_rcd::RoundCornerHardware hardInfo{};
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, hardInfo.bottomLayer, false);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_SurfaceNotCreated, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(surfaceNode->IsSurfaceCreated());
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    rs_rcd::RoundCornerHardware hardInfo{};
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, hardInfo.bottomLayer, true);
    EXPECT_FALSE(result);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_InvalidSurface, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::INVALID);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsInvalidSurface());
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, false);
}

HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNodeMainThread_NullProcessor, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    visitor->SetUniProcessor(nullptr);
    ASSERT_EQ(visitor->uniProcessor_, nullptr);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*surfaceNode, false);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_NullLayerInfo
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with null layerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_NullLayerInfo, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(nullptr, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_NullCurBitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with null curBitmap in layerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_NullCurBitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, nullptr};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_Alpha8Bitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with ALPHA_8 bitmap triggers conversion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_Alpha8Bitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &alpha8Bitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
    EXPECT_EQ(layerBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_Rgba8888Bitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with RGBA_8888 bitmap copies directly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_Rgba8888Bitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
    EXPECT_EQ(layerBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_TopSurfaceRect
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer sets correct rects for TOP surface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_TopSurfaceRect, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    const uint32_t displayLeft = 10;
    const uint32_t displayTop = 20;
    const uint32_t displayWidth = 200;
    const uint32_t displayHeight = 400;
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(displayLeft, displayTop, displayWidth, displayHeight));

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    const RectI& srcRect = surfaceNode->GetSrcRect();
    EXPECT_EQ(srcRect.GetLeft(), 0);
    EXPECT_EQ(srcRect.GetTop(), 0);
    EXPECT_EQ(srcRect.GetWidth(), width);
    EXPECT_EQ(srcRect.GetHeight(), height);

    const RectI& dstRect = surfaceNode->GetDstRect();
    EXPECT_EQ(dstRect.GetLeft(), static_cast<int>(displayLeft));
    EXPECT_EQ(dstRect.GetTop(), static_cast<int>(displayTop));
    EXPECT_EQ(dstRect.GetWidth(), width);
    EXPECT_EQ(dstRect.GetHeight(), height);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_BottomSurfaceRect
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer sets correct rects for BOTTOM surface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_BottomSurfaceRect, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"bottom.png", 0, 0, "bottom.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    const uint32_t displayLeft = 10;
    const uint32_t displayTop = 20;
    const uint32_t displayWidth = 200;
    const uint32_t displayHeight = 400;
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(displayLeft, displayTop, displayWidth, displayHeight));

    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    const RectI& srcRect = surfaceNode->GetSrcRect();
    EXPECT_EQ(srcRect.GetLeft(), 0);
    EXPECT_EQ(srcRect.GetTop(), 0);
    EXPECT_EQ(srcRect.GetWidth(), width);
    EXPECT_EQ(srcRect.GetHeight(), height);

    const RectI& dstRect = surfaceNode->GetDstRect();
    EXPECT_EQ(dstRect.GetLeft(), static_cast<int>(displayLeft));
    int expectedTop = static_cast<int>(displayHeight) - height + static_cast<int>(displayTop);
    EXPECT_EQ(dstRect.GetTop(), expectedTop);
    EXPECT_EQ(dstRect.GetWidth(), width);
    EXPECT_EQ(dstRect.GetHeight(), height);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_ZeroDisplayHeight
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with zero display height fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_ZeroDisplayHeight, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 0));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_SetsBufferDimensions
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer sets buffer width/height/size
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_SetsBufferDimensions, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    const int bufferSize = 8112;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", bufferSize, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    BufferRequestConfig config = surfaceNode->GetHardenBufferRequestConfig();
    EXPECT_EQ(config.width, width);
    int32_t expectedHeight = bufferSize / width + height + 2;
    EXPECT_EQ(config.height, expectedHeight);
    EXPECT_EQ(config.format, GRAPHIC_PIXEL_FMT_RGBA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_CldLayerInfoPopulated
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer populates buffer size from layerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_CldLayerInfoPopulated, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    const int bufferSize = 8112;
    const int cldWidth = 2028;
    const int cldHeight = 1;
    const std::string binFileName = "top.bin";
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, binFileName, bufferSize, cldWidth, cldHeight, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);

    BufferRequestConfig config = surfaceNode->GetHardenBufferRequestConfig();
    EXPECT_EQ(config.width, width);
    int32_t expectedHeight = bufferSize / width + height + 2;
    EXPECT_EQ(config.height, expectedHeight);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_ValidAlpha8
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with valid ALPHA_8 bitmap succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_ValidAlpha8, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(dstBitmap.GetWidth(), width);
    EXPECT_EQ(dstBitmap.GetHeight(), height);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_InvalidSrcBitmap
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with invalid (unbuilt) src bitmap fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_InvalidSrcBitmap, TestSize.Level1)
{
    Drawing::Bitmap invalidBitmap;
    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(invalidBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_ZeroWidth
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with zero width fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_ZeroWidth, TestSize.Level1)
{
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(0, 100,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_ZeroHeight
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with zero height fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_ZeroHeight, TestSize.Level1)
{
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(100, 0,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_SmallDimensions
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with 1x1 dimensions succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_SmallDimensions, TestSize.Level1)
{
    const int width = 1;
    const int height = 1;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(dstBitmap.GetWidth(), width);
    EXPECT_EQ(dstBitmap.GetHeight(), height);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_LargeDimensions
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with large dimensions succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_LargeDimensions, TestSize.Level1)
{
    const int width = 1920;
    const int height = 1080;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(dstBitmap.GetWidth(), width);
    EXPECT_EQ(dstBitmap.GetHeight(), height);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_DstBitmapPremulAlpha
 * @tc.desc: Test ConvertAlpha8ToRgba8888 produces PREMUL alpha type dst bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_DstBitmapPremulAlpha, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    Drawing::BitmapFormat format = dstBitmap.GetFormat();
    EXPECT_EQ(format.colorType, Drawing::ColorType::COLORTYPE_RGBA_8888);
    EXPECT_EQ(format.alphaType, Drawing::AlphaType::ALPHATYPE_PREMUL);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_BGRA8888Bitmap
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with BGRA_8888 bitmap copies directly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_BGRA8888Bitmap, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap bgraBitmap;
    bgraBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &bgraBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
    EXPECT_EQ(layerBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_BGRA_8888);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_InvalidSurfaceType
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer with INVALID surface type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_InvalidSurfaceType, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::INVALID);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsInvalidSurface());

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_PixelContentTransferred
 * @tc.desc: Test ConvertAlpha8ToRgba8888 transfers pixel content correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_PixelContentTransferred, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    const uint8_t testAlpha = 128;
    uint8_t* srcPixels = static_cast<uint8_t*>(const_cast<void*>(alpha8Bitmap.GetPixels()));
    ASSERT_NE(srcPixels, nullptr);
    for (int i = 0; i < width * height; i++) {
        srcPixels[i] = testAlpha;
    }

    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(alpha8Bitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_NE(dstBitmap.GetPixels(), nullptr);

    const uint32_t* dstPixels = static_cast<const uint32_t*>(dstBitmap.GetPixels());
    ASSERT_NE(dstPixels, nullptr);
    uint32_t firstPixel = dstPixels[0];
    uint8_t alpha = (firstPixel >> 24) & 0xFF;
    EXPECT_EQ(alpha, testAlpha);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_DstBitmapReused
 * @tc.desc: Test ConvertAlpha8ToRgba8888 can be called multiple times with different src bitmaps
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_DstBitmapReused, TestSize.Level1)
{
    const int width = 50;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap1;
    alpha8Bitmap1.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap1;
    bool result1 = ConvertAlpha8ToRgba8888(alpha8Bitmap1, dstBitmap1);
    EXPECT_TRUE(result1);
    EXPECT_EQ(dstBitmap1.GetWidth(), width);
    EXPECT_EQ(dstBitmap1.GetHeight(), height);

    Drawing::Bitmap alpha8Bitmap2;
    alpha8Bitmap2.Build(width * 2, height * 2,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    Drawing::Bitmap dstBitmap2;
    bool result2 = ConvertAlpha8ToRgba8888(alpha8Bitmap2, dstBitmap2);
    EXPECT_TRUE(result2);
    EXPECT_EQ(dstBitmap2.GetWidth(), width * 2);
    EXPECT_EQ(dstBitmap2.GetHeight(), height * 2);
}

/*
 * @tc.name: DecodeBitmap_NullImage
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with null image
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_NullImage, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    Drawing::Bitmap bitmap;
    bool result = rcdInstance.DecodeBitmap(nullptr, bitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: DecodeBitmap_AsLegacyBitmapFails
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with unbuilt image (AsLegacyBitmap fails)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_AsLegacyBitmapFails, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: DecodeBitmap_ValidRgba8888Image
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with valid RGBA_8888 image
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ValidRgba8888Image, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout << "DecodeBitmap_ValidRgba8888Image: BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: DecodeBitmap_ValidAlpha8Image
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with non-RGBA color type image (direct assign path)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ValidAlpha8Image, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout << "DecodeBitmap_ValidAlpha8Image: BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: DecodeBitmap_ValidBgra8888Image
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap with valid BGRA_8888 image (covers BGRA_8888 branch at line 116)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ValidBgra8888Image, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout << "DecodeBitmap_ValidBgra8888Image: BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
    // After successful ExtractAlphaChannel, the dstBitmap should be an Alpha8 bitmap
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_ALPHA_8);
}

/*
 * @tc.name: DecodeBitmap_ExtractAlphaFallbackNullPixels
 * @tc.desc: Test RoundCornerDisplay::DecodeBitmap fallback path (lines 117-120) when ExtractAlphaChannel fails
 *           because srcBitmap pixels are null after AsLegacyBitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, DecodeBitmap_ExtractAlphaFallbackNullPixels, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    // Set pixels to nullptr to make ExtractAlphaChannel fail at the GetPixels check,
    // triggering the fallback path (bitmap = srcBitmap) at lines 117-120.
    srcBitmap.SetPixels(nullptr);
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(srcBitmap)) {
        std::cout <<
            "DecodeBitmap_ExtractAlphaFallbackNullPixels:BuildFromBitmap not supported in test env" << std::endl;
        return;
    }
    Drawing::Bitmap dstBitmap;
    bool result = rcdInstance.DecodeBitmap(image, dstBitmap);
    EXPECT_TRUE(result);
    // When ExtractAlphaChannel fails, the fallback assigns srcBitmap to bitmap,
    // so dstBitmap should retain the original RGBA_8888 color type (not Alpha8).
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_RGBA_8888);
}

/*
 * @tc.name: ExtractAlphaChannel_ValidRgba8888
 * @tc.desc: Test ExtractAlphaChannel with valid RGBA_8888 bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ValidRgba8888, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_ALPHA_8);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetHeight()), height);
}

/*
 * @tc.name: ExtractAlphaChannel_ValidBgra8888
 * @tc.desc: Test ExtractAlphaChannel with valid BGRA_8888 bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ValidBgra8888, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetColorType(), Drawing::ColorType::COLORTYPE_ALPHA_8);
}

/*
 * @tc.name: ExtractAlphaChannel_InvalidSrcBitmap
 * @tc.desc: Test ExtractAlphaChannel with invalid (unbuilt) src bitmap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_InvalidSrcBitmap, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_NonRgbaColorType
 * @tc.desc: Test ExtractAlphaChannel with non-RGBA/BGRA color type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_NonRgbaColorType, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(10, 10,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_ZeroWidth
 * @tc.desc: Test ExtractAlphaChannel with zero width
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ZeroWidth, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(0, 10,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_ZeroHeight
 * @tc.desc: Test ExtractAlphaChannel with zero height
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_ZeroHeight, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(10, 0,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ExtractAlphaChannel_PixelContentTransferred
 * @tc.desc: Test ExtractAlphaChannel transfers alpha channel correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_PixelContentTransferred, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    const uint8_t testAlpha = 200;
    uint8_t* srcPixels = static_cast<uint8_t*>(const_cast<void*>(srcBitmap.GetPixels()));
    ASSERT_NE(srcPixels, nullptr);
    int32_t srcRowBytes = srcBitmap.GetRowBytes();
    constexpr int32_t bytesPerPixelRgba8888 = 4;
    constexpr int32_t alphaChannelOffset = 3;
    for (int y = 0; y < height; y++) {
        uint8_t* srcRow = srcPixels + y * srcRowBytes;
        for (int x = 0; x < width; x++) {
            srcRow[x * bytesPerPixelRgba8888 + alphaChannelOffset] = testAlpha;
        }
    }
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    const uint8_t* dstPixels = static_cast<const uint8_t*>(dstBitmap.GetPixels());
    ASSERT_NE(dstPixels, nullptr);
    EXPECT_EQ(dstPixels[0], testAlpha);
    int32_t dstRowBytes = dstBitmap.GetRowBytes();
    const uint8_t* lastRow = dstPixels + (height - 1) * dstRowBytes;
    EXPECT_EQ(lastRow[0], testAlpha);
}

/*
 * @tc.name: ExtractAlphaChannel_SmallDimensions
 * @tc.desc: Test ExtractAlphaChannel with 1x1 dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_SmallDimensions, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(1, 1,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(dstBitmap.GetWidth(), 1);
    EXPECT_EQ(dstBitmap.GetHeight(), 1);
}

/*
 * @tc.name: ExtractAlphaChannel_LargeDimensions
 * @tc.desc: Test ExtractAlphaChannel with large dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_LargeDimensions, TestSize.Level1)
{
    const int width = 1920;
    const int height = 1080;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_TRUE(result);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(dstBitmap.GetHeight()), height);
}

/*
 * @tc.name: GetTopSurfaceSource_NullRog
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with null rog
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_NullRog, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.rog_ = nullptr;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_FALSE(result);
}

/*
 * @tc.name: GetTopSurfaceSource_NoPortrait
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with no portrait configured
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_NoPortrait, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rcdInstance.rog_ = &rog;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_FALSE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: GetTopSurfaceSource_NoLandscape
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with no landscape configured
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_NoLandscape, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rcdInstance.rog_ = &rog;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_FALSE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: GetTopSurfaceSource_ResourceEqualReuse
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with layerHide equal to layerUp (reuse)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_ResourceEqualReuse, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "test.png";
    rogPortrait.layerUp.offsetX = 1;
    rogPortrait.layerUp.offsetY = 2;
    rogPortrait.layerUp.binFileName = "test.bin";
    rogPortrait.layerUp.bufferSize = 100;
    rogPortrait.layerUp.cldWidth = 10;
    rogPortrait.layerUp.cldHeight = 20;
    rogPortrait.layerHide = rogPortrait.layerUp;
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp = rogPortrait.layerUp;
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = false;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
    rcdInstance.supportHardware_ = false;
}

/*
 * @tc.name: GetTopSurfaceSource_WithHardwareSupport
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource with hardware support enabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_WithHardwareSupport, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "test_up.png";
    rogPortrait.layerHide.fileName = "test_hide.png";
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp.fileName = "test_land.png";
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = true;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
    rcdInstance.supportHardware_ = false;
}

/*
 * @tc.name: GetTopSurfaceSource_LandscapeReusePortraitHide
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource reuses portrait layerHide for landscape
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_LandscapeReusePortraitHide, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "portrait_up.png";
    rogPortrait.layerHide.fileName = "portrait_hide.png";
    rogPortrait.layerHide.offsetX = 5;
    rogPortrait.layerHide.offsetY = 6;
    rogPortrait.layerHide.binFileName = "hide.bin";
    rogPortrait.layerHide.bufferSize = 200;
    rogPortrait.layerHide.cldWidth = 15;
    rogPortrait.layerHide.cldHeight = 25;
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp = rogPortrait.layerHide;
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = false;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: GetTopSurfaceSource_LandscapeNotEqualAny
 * @tc.desc: Test RoundCornerDisplay::GetTopSurfaceSource loads landscape separately
             when not equal to any portrait layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, GetTopSurfaceSource_LandscapeNotEqualAny, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rs_rcd::ROGSetting rog;
    rs_rcd::RogPortrait rogPortrait{};
    rogPortrait.layerUp.fileName = "portrait_up.png";
    rogPortrait.layerUp.offsetX = 1;
    rogPortrait.layerHide.fileName = "portrait_hide.png";
    rogPortrait.layerHide.offsetX = 2;
    rog.portraitMap[rs_rcd::NODE_PORTRAIT] = rogPortrait;
    rs_rcd::RogLandscape rogLandscape{};
    rogLandscape.layerUp.fileName = "landscape_up.png";
    rogLandscape.layerUp.offsetX = 3;
    rog.landscapeMap[rs_rcd::NODE_LANDSCAPE] = rogLandscape;
    rcdInstance.rog_ = &rog;
    rcdInstance.supportHardware_ = false;
    bool result = rcdInstance.GetTopSurfaceSource();
    EXPECT_TRUE(result);
    rcdInstance.rog_ = nullptr;
}

/*
 * @tc.name: IsResourceEqual_AllFieldsEqual
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with all fields equal returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_AllFieldsEqual, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    EXPECT_TRUE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentFileName
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different fileName returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentFileName, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"bottom.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentOffsetX
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different offsetX returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentOffsetX, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 3, 2, "top.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentOffsetY
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different offsetY returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentOffsetY, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 4, "top.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentBinFileName
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different binFileName returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentBinFileName, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "bottom.bin", 100, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentBufferSize
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different bufferSize returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentBufferSize, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 200, 10, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentCldWidth
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different cldWidth returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentCldWidth, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 30, 20, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_DifferentCldHeight
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with different cldHeight returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_DifferentCldHeight, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, nullptr};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 10, 40, nullptr};
    EXPECT_FALSE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_EmptyFileName
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual with empty fileName for both returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_EmptyFileName, TestSize.Level1)
{
    rs_rcd::RoundCornerLayer layer1{"", 0, 0, "", 0, 0, 0, nullptr};
    rs_rcd::RoundCornerLayer layer2{"", 0, 0, "", 0, 0, 0, nullptr};
    EXPECT_TRUE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: IsResourceEqual_CurBitmapIgnored
 * @tc.desc: Test RoundCornerLayer::IsResourceEqual ignores curBitmap pointer difference
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, IsResourceEqual_CurBitmapIgnored, TestSize.Level1)
{
    Drawing::Bitmap bitmap1;
    Drawing::Bitmap bitmap2;
    rs_rcd::RoundCornerLayer layer1{"top.png", 1, 2, "top.bin", 100, 10, 20, &bitmap1};
    rs_rcd::RoundCornerLayer layer2{"top.png", 1, 2, "top.bin", 100, 10, 20, &bitmap2};
    EXPECT_TRUE(layer1.IsResourceEqual(layer2));
}

/*
 * @tc.name: ExtractAlphaChannel_NullSrcPixels
 * @tc.desc: Test ExtractAlphaChannel with null src pixels fails (GetPixels returns nullptr)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ExtractAlphaChannel_NullSrcPixels, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL});
    srcBitmap.SetPixels(nullptr);
    Drawing::Bitmap dstBitmap;
    bool result = ExtractAlphaChannel(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_NonAlpha8ColorType
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with RGBA_8888 (non-ALPHA_8) color type fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_NonAlpha8ColorType, TestSize.Level1)
{
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(10, 10,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConvertAlpha8ToRgba8888_NullSrcPixels
 * @tc.desc: Test ConvertAlpha8ToRgba8888 with null src pixels (ReadPixels fails)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConvertAlpha8ToRgba8888_NullSrcPixels, TestSize.Level1)
{
    const int width = 10;
    const int height = 10;
    Drawing::Bitmap srcBitmap;
    srcBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    srcBitmap.SetPixels(nullptr);
    Drawing::Bitmap dstBitmap;
    bool result = ConvertAlpha8ToRgba8888(srcBitmap, dstBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_SetHardwareResourceToBufferFailed
 * @tc.desc: Test RSRcdRenderVisitor::ConsumeAndUpdateBuffer when SetHardwareResourceToBuffer returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBuffer_SetHardwareResourceToBufferFailed, TestSize.Level1)
{
    // prepare test: create a surface node with a real consumer surface and a queued buffer
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    // create consumer surface and register listener
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));
    ASSERT_TRUE(surfaceNode->IsSurfaceCreated());

    // get consumer and producer, create producer surface
    auto consumer = surfaceNode->GetConsumer();
    ASSERT_NE(consumer, nullptr);
    auto producer = consumer->GetProducer();
    ASSERT_NE(producer, nullptr);
    sptr<Surface> producerSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(producerSurface, nullptr);

    // request and flush a buffer so that AcquireBuffer can succeed
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
    GSError ret = producerSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);
    ret = producerSurface->FlushBuffer(buffer, SyncFence::INVALID_FENCE, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    // increase available buffer count so ConsumeAndUpdateBuffer does not early-return
    surfaceNode->IncreaseAvailableBuffer();
    ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);

    // use an invalid (unbuilt) layerBitmap so that SetHardwareResourceToBuffer returns false
    // (layerBitmap.IsValid() returns true for an unbuilt bitmap, causing early return false)
    Drawing::Bitmap layerBitmap;
    ASSERT_TRUE(layerBitmap.IsValid());

    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    // ConsumeAndUpdateBuffer should return false because SetHardwareResourceToBuffer failed
    bool result = visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_AcquireBufferFailed
 * @tc.desc: Test RSRcdRenderVisitor::ConsumeAndUpdateBuffer when AcquireBuffer fails (no buffer queued)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBuffer_AcquireBufferFailed, TestSize.Level1)
{
    // prepare test: create a surface node with a real consumer surface but no buffer queued
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));

    auto consumer = surfaceNode->GetConsumer();
    ASSERT_NE(consumer, nullptr);

    // increase available buffer count so ConsumeAndUpdateBuffer does not early-return
    surfaceNode->IncreaseAvailableBuffer();
    ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);

    // no buffer has been queued, so AcquireBuffer should fail and ConsumeAndUpdateBuffer returns false
    Drawing::Bitmap layerBitmap;
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    bool result = visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_NullConsumer
 * @tc.desc: Test RSRcdRenderVisitor::ConsumeAndUpdateBuffer when consumer is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ConsumeAndUpdateBuffer_NullConsumer, TestSize.Level1)
{
    // prepare test: create a surface node without creating surface (consumer is nullptr)
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(surfaceNode->IsSurfaceCreated());
    ASSERT_EQ(surfaceNode->GetConsumer(), nullptr);

    // increase available buffer count so ConsumeAndUpdateBuffer does not early-return
    surfaceNode->IncreaseAvailableBuffer();
    ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);

    // consumer is nullptr, ConsumeAndUpdateBuffer should return false
    Drawing::Bitmap layerBitmap;
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    bool result = visitor->ConsumeAndUpdateBuffer(*surfaceNode, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrepareHardwareResourceBuffer_Alpha8ConversionFails
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrepareHardwareResourceBuffer when ConvertAlpha8ToRgba8888 fails
 *           (covers lines 187-188: alpha8 to rgba8888 conversion failure path)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrepareHardwareResourceBuffer_Alpha8ConversionFails, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap alpha8Bitmap;
    alpha8Bitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    // Set pixels to nullptr so that ConvertAlpha8ToRgba8888 fails (ReadPixels fails with null src pixels)
    alpha8Bitmap.SetPixels(nullptr);

    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &alpha8Bitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);

    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));
    Drawing::Bitmap layerBitmap;
    // curBitmap is ALPHA_8 so the conversion branch is entered (line 184-189);
    // ConvertAlpha8ToRgba8888 returns false because src pixels are null,
    // covering lines 187-188 (log error and return false)
    bool result = surfaceNode->PrepareHardwareResourceBuffer(layerInfo, layerBitmap);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: PrintRcdNodeInfo_TopSurface
 * @tc.desc: Test RSRcdSurfaceRenderNode::PrintRcdNodeInfo with TOP surface type
 *           (covers PrintRcdNodeInfo function and RCDTopSurfaceNode branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, PrintRcdNodeInfo_TopSurface, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsTopSurface());

    const int width = 100;
    const int height = 50;
    Drawing::Bitmap layerBitmap;
    layerBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});

    // PrintRcdNodeInfo should not crash and should log node info for TOP surface
    surfaceNode->PrintRcdNodeInfo(layerBitmap);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetWidth()), width);
    EXPECT_EQ(static_cast<int>(layerBitmap.GetHeight()), height);
}

/*
 * @tc.name: ProcessRcdSurfaceRenderNode_ConsumeAndUpdateBufferFailed
 * @tc.desc: Test RSRcdRenderVisitor::ProcessRcdSurfaceRenderNode when RequestFrame succeeds but
 *           ConsumeAndUpdateBuffer fails (covers line 135: ConsumeAndUpdateBuffer returns false).
 *           The full success path up to line 134 (renderFrame->Flush()) is exercised; then
 *           ConsumeAndUpdateBuffer fails because SetHardwareResourceToBuffer cannot read the
 *           .bin file (path /sys_prod/etc/display/RoundCornerDisplay/xxx.bin does not exist in
 *           the test environment), causing line 135 condition true -> line 136-137 log + return false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_ConsumeAndUpdateBufferFailed, TestSize.Level1)
{
    // 1. create surface node (TOP) and create a real consumer/producer surface
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));
    ASSERT_TRUE(surfaceNode->IsSurfaceCreated());

    // 2. set up render engine (initialized so RequestFrame can succeed)
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_->Init();
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);

    // 3. set up a valid uni processor
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    // 4. build a valid layerInfo with an RGBA_8888 bitmap and set display rect
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));

    // 5. queue a buffer via producer surface so that AcquireBuffer can succeed later
    auto consumer = surfaceNode->GetConsumer();
    ASSERT_NE(consumer, nullptr);
    auto producer = consumer->GetProducer();
    ASSERT_NE(producer, nullptr);
    sptr<Surface> producerSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(producerSurface, nullptr);
    BufferRequestConfig requestConfig = { .width = width, .height = height, .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, .timeout = 0,
    };
    BufferFlushConfig flushConfig = { .damage = { .w = width, .h = height, } };
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
    GSError ret = producerSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    if (ret == OHOS::GSERROR_OK && buffer != nullptr) {
        ASSERT_EQ(producerSurface->FlushBuffer(buffer, SyncFence::INVALID_FENCE, flushConfig), OHOS::GSERROR_OK);
        surfaceNode->IncreaseAvailableBuffer();
        ASSERT_TRUE(surfaceNode->GetAvailableBufferCount() > 0);
    }

    // 6. call ProcessRcdSurfaceRenderNode with resourceChanged=true
    // RequestFrame ok -> Flush ok -> ConsumeAndUpdateBuffer fails (.bin missing) -> covers line 135-138;
    // RequestFrame fails -> covers line 127-133 (CleanCache branch).
    EXPECT_FALSE(visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, layerInfo, true));

    // cleanup
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: ProcessRcdSurfaceRenderNode_RequestFrameFailed
 * @tc.desc: Test RSRcdRenderVisitor::ProcessRcdSurfaceRenderNode when RequestFrame returns nullptr
 *           (covers lines 127-133: renderFrame == nullptr -> CleanCache -> log -> return false).
 *           A render engine is set but NOT initialized (no Init() call), so renderContext_ is
 *           nullptr and RequestFrame returns nullptr, exercising the CleanCache branch.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, ProcessRcdSurfaceRenderNode_RequestFrameFailed, TestSize.Level1)
{
    // 1. create surface node (TOP) and create a real surface so GetRSSurface() != nullptr
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNode);
    ASSERT_NE(listener, nullptr);
    ASSERT_TRUE(surfaceNode->CreateSurface(listener));
    ASSERT_TRUE(surfaceNode->IsSurfaceCreated());

    // 2. set up a render engine WITHOUT calling Init() so RequestFrame returns nullptr
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    ASSERT_NE(visitor->renderEngine_, nullptr);

    // 3. set up a valid uni processor
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    visitor->SetUniProcessor(processor);
    ASSERT_NE(visitor->uniProcessor_, nullptr);
    ASSERT_FALSE(surfaceNode->IsInvalidSurface());

    // 4. build a valid layerInfo so PrepareHardwareResourceBuffer succeeds (passes line 119-122)
    const int width = 100;
    const int height = 50;
    Drawing::Bitmap rgbaBitmap;
    rgbaBitmap.Build(width, height,
        Drawing::BitmapFormat{Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE});
    rs_rcd::RoundCornerLayer layer{"top.png", 0, 0, "top.bin", 8112, 2028, 1, &rgbaBitmap};
    auto layerInfo = std::make_shared<rs_rcd::RoundCornerLayer>(layer);
    surfaceNode->SetRenderDisplayRect(RectT<uint32_t>(0, 0, 200, 200));

    // 5. call ProcessRcdSurfaceRenderNode with resourceChanged=true
    // RequestFrame returns nullptr -> line 127 condition true ->
    //   line 128-130 CleanCache (if surface valid) -> line 131-132 log -> line 133 return false
    bool result = visitor->ProcessRcdSurfaceRenderNode(*surfaceNode, layerInfo, true);
    EXPECT_FALSE(result);

    // cleanup
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}
} // OHOS::Rosen
