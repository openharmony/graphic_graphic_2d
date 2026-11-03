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
 
#include <functional>
 
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
 
#include "pixel_map.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int CELL_GAP = 30;
constexpr int CELL_WIDTH = 340;
constexpr int CELL_HEIGHT = 240;
constexpr int GRID_COLUMN = 3;
constexpr uint32_t CELL_BG_COLOR = 0xFFFFFFFF;
 
constexpr int ARGB_ALPHA_SHIFT = 24;
constexpr int ARGB_RED_SHIFT = 16;
constexpr int ARGB_GREEN_SHIFT = 8;
constexpr uint32_t COLOR_MAX = 255;
constexpr uint32_t COLOR_MID = 128;
 
// Nine-patch source: border band is BORDER px wide, corners/edges/center use distinct colors.
constexpr int NINE_SRC_SIZE = 120;
constexpr int NINE_BORDER = 30;
constexpr uint32_t NINE_CORNER_COLOR = 0xFFE53935;
constexpr uint32_t NINE_EDGE_COLOR = 0xFF43A047;
constexpr uint32_t NINE_CENTER_COLOR = 0xFF1E88E5;
 
constexpr int CHECKER_SIZE = 64;
constexpr int CHECKER_CELL = 8;
constexpr uint32_t CHECKER_DARK = 0xFF202020;
constexpr uint32_t CHECKER_LIGHT = 0xFFF0F0F0;
 
constexpr int CHECKER_PARITY = 2;
 
constexpr int YUV_SIZE = 256;
constexpr int YUV_CHROMA_DIVISOR = 2;
constexpr uint8_t YUV_NEUTRAL = 128;
constexpr float YUV_ROTATE_DEGREES = 90.0f;
constexpr float YUV_HALF_SCALE = 0.5f;
constexpr float YUV_TRANSLATE_OFFSET = 40.0f;
constexpr int YUV_CROP_START = 32;
constexpr int YUV_CROP_SIZE = 128;
 
const std::string TEST_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";
} // namespace
 
class RSPixelMapDrawApiTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
 
protected:
    // Creates a white canvas node placed at grid cell |index| and registers it to the root node.
    std::shared_ptr<RSCanvasNode> CreateCellNode(int index)
    {
        int row = index / GRID_COLUMN;
        int col = index % GRID_COLUMN;
        float x = CELL_GAP + (CELL_WIDTH + CELL_GAP) * col;
        float y = CELL_GAP + (CELL_HEIGHT + CELL_GAP) * row;
        auto canvasNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({ x, y, CELL_WIDTH, CELL_HEIGHT });
        canvasNode->SetFrame({ x, y, CELL_WIDTH, CELL_HEIGHT });
        canvasNode->SetBackgroundColor(CELL_BG_COLOR);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
        return canvasNode;
    }
 
    void FinishCell(const std::shared_ptr<RSCanvasNode>& canvasNode)
    {
        canvasNode->FinishRecording();
        RSGraphicTestDirector::Instance().rsUiDirector_->SendMessages();
    }
 
    void DrawWithImageFit(const std::shared_ptr<RSCanvasNode>& canvasNode,
        const std::shared_ptr<Media::PixelMap>& pixelMap, ImageFit fit)
    {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(fit));
        Drawing::Rect frameRect;
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);
        auto drawing = canvasNode->BeginRecording(CELL_WIDTH, CELL_HEIGHT);
        drawing->DrawPixelMapWithParm(pixelMap, imageInfo, Drawing::SamplingOptions());
        FinishCell(canvasNode);
    }
 
    static std::shared_ptr<Media::PixelMap> CreateFromColors(std::vector<uint32_t>& colors, int width, int height,
        bool editable = true)
    {
        Media::InitializationOptions opts;
        opts.size.width = width;
        opts.size.height = height;
        opts.pixelFormat = Media::PixelFormat::RGBA_8888;
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        opts.editable = editable;
        return std::shared_ptr<Media::PixelMap>(
            Media::PixelMap::Create(colors.data(), colors.size(), opts).release());
    }
 
    static std::shared_ptr<Media::PixelMap> CreateGradientPixelMap(int width, int height)
    {
        if (width <= 0 || height <= 0) {
            return nullptr;
        }
        std::vector<uint32_t> colors(width * height);
        for (int i = 0; i < width * height; i++) {
            uint32_t r = static_cast<uint32_t>((i % width) * COLOR_MAX / width);
            uint32_t g = static_cast<uint32_t>((i / width) * COLOR_MAX / height);
            colors[i] = (COLOR_MAX << ARGB_ALPHA_SHIFT) | (r << ARGB_RED_SHIFT) | (g << ARGB_GREEN_SHIFT) | COLOR_MID;
        }
        return CreateFromColors(colors, width, height);
    }
 
    // Corners, edges and center get different colors so stretching of each region is visible.
    static std::shared_ptr<Media::PixelMap> CreateNinePatchPixelMap()
    {
        std::vector<uint32_t> colors(NINE_SRC_SIZE * NINE_SRC_SIZE);
        const int innerEnd = NINE_SRC_SIZE - NINE_BORDER;
        for (int y = 0; y < NINE_SRC_SIZE; y++) {
            for (int x = 0; x < NINE_SRC_SIZE; x++) {
                bool borderX = x < NINE_BORDER || x >= innerEnd;
                bool borderY = y < NINE_BORDER || y >= innerEnd;
                uint32_t color = NINE_CENTER_COLOR;
                if (borderX && borderY) {
                    color = NINE_CORNER_COLOR;
                } else if (borderX || borderY) {
                    color = NINE_EDGE_COLOR;
                }
                colors[y * NINE_SRC_SIZE + x] = color;
            }
        }
        return CreateFromColors(colors, NINE_SRC_SIZE, NINE_SRC_SIZE);
    }
 
    static std::shared_ptr<Media::PixelMap> CreateCheckerPixelMap()
    {
        std::vector<uint32_t> colors(CHECKER_SIZE * CHECKER_SIZE);
        for (int y = 0; y < CHECKER_SIZE; y++) {
            for (int x = 0; x < CHECKER_SIZE; x++) {
                bool dark = ((x / CHECKER_CELL) + (y / CHECKER_CELL)) % CHECKER_PARITY == 0;
                colors[y * CHECKER_SIZE + x] = dark ? CHECKER_DARK : CHECKER_LIGHT;
            }
        }
        return CreateFromColors(colors, CHECKER_SIZE, CHECKER_SIZE);
    }
 
    // Y is a horizontal gradient, U grows with x and V grows with y, so rotation and flip are visible.
    static std::shared_ptr<Media::PixelMap> CreateYuvPixelMap(Media::PixelFormat format)
    {
        const size_t ySize = YUV_SIZE * YUV_SIZE;
        const int chromaWidth = YUV_SIZE / YUV_CHROMA_DIVISOR;
        const int chromaHeight = YUV_SIZE / YUV_CHROMA_DIVISOR;
        std::vector<uint8_t> yuvData(ySize + ySize / YUV_CHROMA_DIVISOR, YUV_NEUTRAL);
        for (size_t i = 0; i < ySize; i++) {
            yuvData[i] = static_cast<uint8_t>((i % YUV_SIZE) * COLOR_MAX / YUV_SIZE);
        }
        bool uFirst = (format == Media::PixelFormat::NV12);
        for (int cy = 0; cy < chromaHeight; cy++) {
            for (int cx = 0; cx < chromaWidth; cx++) {
                size_t base = ySize + static_cast<size_t>(cy * chromaWidth + cx) * YUV_CHROMA_DIVISOR;
                uint8_t u = static_cast<uint8_t>(cx * COLOR_MAX / chromaWidth);
                uint8_t v = static_cast<uint8_t>(cy * COLOR_MAX / chromaHeight);
                yuvData[base] = uFirst ? u : v;
                yuvData[base + 1] = uFirst ? v : u;
            }
        }
        Media::InitializationOptions opts;
        opts.size.width = YUV_SIZE;
        opts.size.height = YUV_SIZE;
        opts.pixelFormat = format;
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts).release());
        if (pixelMap != nullptr) {
            pixelMap->WritePixels(yuvData.data(), yuvData.size());
        }
        return pixelMap;
    }
};
 
/*
 * @tc.name: RS_PIXELMAP_DRAW_RECT_SRC_TEST
 * @tc.desc: DrawPixelMapRect with full, partial, edge and out-of-bounds src rects into a fixed dst
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_DRAW_RECT_SRC_TEST)
{
    constexpr int srcSize = 200;
    auto pixelMap = CreateGradientPixelMap(srcSize, srcSize);
    ASSERT_NE(pixelMap, nullptr);
    const Drawing::Rect srcRects[] = {
        { 0, 0, 200, 200 },     // full image
        { 0, 0, 100, 100 },     // top-left quarter
        { 50, 50, 150, 150 },   // center
        { 100, 100, 200, 200 }, // bottom-right quarter
        { 0, 90, 200, 110 },    // thin horizontal strip
        { -50, -50, 100, 100 }, // partially out of bounds
        { 150, 150, 300, 300 }, // mostly out of bounds
        { 100, 100, 100, 100 }, // empty rect
        { 150, 150, 50, 50 },   // inverted rect
    };
    const Drawing::Rect dst(0, 0, CELL_WIDTH, CELL_HEIGHT);
    int index = 0;
    for (const auto& src : srcRects) {
        auto canvasNode = CreateCellNode(index++);
        auto drawing = canvasNode->BeginRecording(CELL_WIDTH, CELL_HEIGHT);
        drawing->DrawPixelMapRect(pixelMap, src, dst, Drawing::SamplingOptions());
        FinishCell(canvasNode);
    }
}
 
/*
 * @tc.name: RS_PIXELMAP_DRAW_RECT_CONSTRAINT_TEST
 * @tc.desc: DrawPixelMapRect with STRICT/FAST constraint combined with NEAREST/LINEAR sampling
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_DRAW_RECT_CONSTRAINT_TEST)
{
    auto pixelMap = CreateCheckerPixelMap();
    ASSERT_NE(pixelMap, nullptr);
    const Drawing::SrcRectConstraint constraints[] = {
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT,
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT,
    };
    const Drawing::FilterMode filters[] = { Drawing::FilterMode::NEAREST, Drawing::FilterMode::LINEAR };
    // Src sits on checker cell boundaries so bleeding from neighbour texels shows up with FAST + LINEAR.
    const Drawing::Rect srcRects[] = {
        { 8, 8, 24, 24 },
        { 0, 0, 64, 64 },
        { 4, 4, 60, 60 },
    };
    const Drawing::Rect dst(0, 0, CELL_WIDTH, CELL_HEIGHT);
    int index = 0;
    for (const auto& src : srcRects) {
        for (auto constraint : constraints) {
            for (auto filter : filters) {
                auto canvasNode = CreateCellNode(index++);
                auto drawing = canvasNode->BeginRecording(CELL_WIDTH, CELL_HEIGHT);
                Drawing::SamplingOptions sampling(filter, Drawing::MipmapMode::NONE);
                drawing->DrawPixelMapRect(pixelMap, src, dst, sampling, constraint);
                FinishCell(canvasNode);
            }
        }
    }
}
 
/*
 * @tc.name: RS_PIXELMAP_DRAW_NINE_CENTER_TEST
 * @tc.desc: DrawPixelMapNine with normal, tiny, full-size, edge-touching and out-of-bounds center rects
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_DRAW_NINE_CENTER_TEST)
{
    auto pixelMap = CreateNinePatchPixelMap();
    ASSERT_NE(pixelMap, nullptr);
    const Drawing::RectI centers[] = {
        { 30, 30, 90, 90 },    // matches the colored border exactly
        { 59, 59, 61, 61 },    // tiny center
        { 0, 0, 120, 120 },    // center covers the whole image
        { 0, 30, 120, 90 },    // touches left and right edges
        { 30, 0, 90, 120 },    // touches top and bottom edges
        { 10, 10, 110, 110 },  // thin borders
        { -20, -20, 140, 140 }, // out of bounds
        { 60, 60, 60, 60 },    // empty center
        { 90, 90, 30, 30 },    // inverted center
    };
    const Drawing::Rect dst(0, 0, CELL_WIDTH, CELL_HEIGHT);
    int index = 0;
    for (const auto& center : centers) {
        auto canvasNode = CreateCellNode(index++);
        auto drawing = canvasNode->BeginRecording(CELL_WIDTH, CELL_HEIGHT);
        drawing->DrawPixelMapNine(pixelMap, center, dst, Drawing::FilterMode::LINEAR);
        FinishCell(canvasNode);
    }
}
 
/*
 * @tc.name: RS_PIXELMAP_DRAW_NINE_DST_FILTER_TEST
 * @tc.desc: DrawPixelMapNine into dst rects smaller and larger than the source, with NEAREST and LINEAR
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_DRAW_NINE_DST_FILTER_TEST)
{
    auto pixelMap = CreateNinePatchPixelMap();
    ASSERT_NE(pixelMap, nullptr);
    const Drawing::RectI center(NINE_BORDER, NINE_BORDER, NINE_SRC_SIZE - NINE_BORDER, NINE_SRC_SIZE - NINE_BORDER);
    const Drawing::Rect dstRects[] = {
        { 10, 10, 50, 50 },    // smaller than both borders
        { 10, 10, 130, 130 },  // same as source
        { 10, 10, 330, 230 },  // stretched
        { 10, 10, 330, 70 },   // wide and short
        { 10, 10, 70, 230 },   // narrow and tall
    };
    const Drawing::FilterMode filters[] = { Drawing::FilterMode::NEAREST, Drawing::FilterMode::LINEAR };
    int index = 0;
    for (auto filter : filters) {
        for (const auto& dst : dstRects) {
            auto canvasNode = CreateCellNode(index++);
            auto drawing = canvasNode->BeginRecording(CELL_WIDTH, CELL_HEIGHT);
            drawing->DrawPixelMapNine(pixelMap, center, dst, filter);
            FinishCell(canvasNode);
        }
    }
}
 
/*
 * @tc.name: RS_PIXELMAP_DRAW_IMAGE_NINE_BRUSH_TEST
 * @tc.desc: DrawImageNineWithPixelMap with null brush and brushes of different alpha
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_DRAW_IMAGE_NINE_BRUSH_TEST)
{
    auto pixelMap = CreateNinePatchPixelMap();
    ASSERT_NE(pixelMap, nullptr);
    const Drawing::RectI center(NINE_BORDER, NINE_BORDER, NINE_SRC_SIZE - NINE_BORDER, NINE_SRC_SIZE - NINE_BORDER);
    const Drawing::Rect dst(0, 0, CELL_WIDTH, CELL_HEIGHT);
    const int alphas[] = { 255, 192, 128, 64, 0 };
    constexpr int brushCellCount = sizeof(alphas) / sizeof(alphas[0]);
 
    auto nullBrushNode = CreateCellNode(0);
    auto nullBrushDrawing = nullBrushNode->BeginRecording(CELL_WIDTH, CELL_HEIGHT);
    nullBrushDrawing->DrawImageNineWithPixelMap(pixelMap, center, dst, Drawing::FilterMode::LINEAR, nullptr);
    FinishCell(nullBrushNode);
 
    for (int i = 0; i < brushCellCount; i++) {
        auto canvasNode = CreateCellNode(i + 1);
        auto drawing = canvasNode->BeginRecording(CELL_WIDTH, CELL_HEIGHT);
        Drawing::Brush brush;
        brush.SetARGB(alphas[i], COLOR_MAX, COLOR_MAX, COLOR_MAX);
        drawing->DrawImageNineWithPixelMap(pixelMap, center, dst, Drawing::FilterMode::LINEAR, &brush);
        FinishCell(canvasNode);
    }
}
 
/*
 * @tc.name: RS_PIXELMAP_ALLOCATOR_TYPE_DRAW_TEST
 * @tc.desc: Draw decoded RGBA pixelmaps allocated by DMA and shared memory with several ImageFit modes.
 *           Non-editable DMA pixelmaps go through the zero-copy and purgeable paths in RSImageBase.
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ALLOCATOR_TYPE_DRAW_TEST)
{
    auto dmaPixelMap = DecodePixelMap(TEST_IMAGE_PATH, Media::AllocatorType::DMA_ALLOC);
    auto shmPixelMap = DecodePixelMap(TEST_IMAGE_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
    ASSERT_NE(dmaPixelMap, nullptr);
    ASSERT_NE(shmPixelMap, nullptr);
    const ImageFit fits[] = { ImageFit::CONTAIN, ImageFit::COVER, ImageFit::FILL };
    const std::shared_ptr<Media::PixelMap> pixelMaps[] = { dmaPixelMap, shmPixelMap };
    int index = 0;
    for (const auto& pixelMap : pixelMaps) {
        for (auto fit : fits) {
            DrawWithImageFit(CreateCellNode(index++), pixelMap, fit);
        }
    }
}
 
/*
 * @tc.name: RS_PIXELMAP_REDRAW_AFTER_WRITE_TEST
 * @tc.desc: Draw a pixelmap, then rewrite its pixels and record again; the screen must show the new content
 *           instead of a cached copy of the first upload
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_REDRAW_AFTER_WRITE_TEST)
{
    constexpr int srcSize = 160;
    const uint32_t fillColors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
    int index = 0;
    for (uint32_t fillColor : fillColors) {
        auto pixelMap = CreateGradientPixelMap(srcSize, srcSize);
        ASSERT_NE(pixelMap, nullptr);
        auto canvasNode = CreateCellNode(index++);
        DrawWithImageFit(canvasNode, pixelMap, ImageFit::CONTAIN);
 
        pixelMap->WritePixels(fillColor);
        pixelMap->MarkDirty();
        DrawWithImageFit(canvasNode, pixelMap, ImageFit::CONTAIN);
    }
    // Same flow on a single pixelmap shared by two nodes: both nodes must show the rewritten pixels.
    auto sharedPixelMap = CreateGradientPixelMap(srcSize, srcSize);
    ASSERT_NE(sharedPixelMap, nullptr);
    auto firstNode = CreateCellNode(index++);
    auto secondNode = CreateCellNode(index++);
    DrawWithImageFit(firstNode, sharedPixelMap, ImageFit::CONTAIN);
    sharedPixelMap->WritePixels(fillColors[0]);
    sharedPixelMap->MarkDirty();
    DrawWithImageFit(secondNode, sharedPixelMap, ImageFit::CONTAIN);
}
 
/*
 * @tc.name: RS_PIXELMAP_SCALE_ANTIALIASING_TEST
 * @tc.desc: PixelMap::scale with every common AntiAliasingOption, for both downscale and upscale
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_SCALE_ANTIALIASING_TEST)
{
    const Media::AntiAliasingOption options[] = {
        Media::AntiAliasingOption::NONE,
        Media::AntiAliasingOption::LOW,
        Media::AntiAliasingOption::MEDIUM,
        Media::AntiAliasingOption::HIGH,
        Media::AntiAliasingOption::SLR,
    };
    constexpr float downScale = 0.3f;
    constexpr float upScale = 3.0f;
    const float scales[] = { downScale, upScale };
    int index = 0;
    for (float scale : scales) {
        for (auto option : options) {
            auto pixelMap = CreateCheckerPixelMap();
            ASSERT_NE(pixelMap, nullptr);
            pixelMap->scale(scale, scale, option);
            DrawWithImageFit(CreateCellNode(index++), pixelMap, ImageFit::CENTER);
        }
    }
}
 
/*
 * @tc.name: RS_PIXELMAP_YUV_TRANSFORM_TEST
 * @tc.desc: NV12/NV21 pixelmaps with rotate, flip, scale, crop and translate (PixelYuv overrides)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDrawApiTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_YUV_TRANSFORM_TEST)
{
    using Transform = std::function<void(Media::PixelMap&)>;
    const Transform transforms[] = {
        [](Media::PixelMap&) {},
        [](Media::PixelMap& pm) { pm.rotate(YUV_ROTATE_DEGREES); },
        [](Media::PixelMap& pm) { pm.flip(true, false); },
        [](Media::PixelMap& pm) { pm.flip(false, true); },
        [](Media::PixelMap& pm) { pm.scale(YUV_HALF_SCALE, YUV_HALF_SCALE); },
        [](Media::PixelMap& pm) { pm.crop({ YUV_CROP_START, YUV_CROP_START, YUV_CROP_SIZE, YUV_CROP_SIZE }); },
        [](Media::PixelMap& pm) { pm.translate(YUV_TRANSLATE_OFFSET, YUV_TRANSLATE_OFFSET); },
    };
    const Media::PixelFormat formats[] = { Media::PixelFormat::NV12, Media::PixelFormat::NV21 };
    constexpr int transformCount = sizeof(transforms) / sizeof(transforms[0]);
    int index = 0;
    for (auto format : formats) {
        for (int i = 0; i < transformCount; i++) {
            auto pixelMap = CreateYuvPixelMap(format);
            ASSERT_NE(pixelMap, nullptr);
            transforms[i](*pixelMap);
            DrawWithImageFit(CreateCellNode(index++), pixelMap, ImageFit::CONTAIN);
        }
    }
}
} // namespace OHOS::Rosen