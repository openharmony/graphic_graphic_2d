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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "ui/rs_effect_node.h"
#include "pixel_map.h"
#include "image_source.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSPixelMapDisplayTest : public RSGraphicTest {
private:
    // ARGB color bit shift positions
    static constexpr int ARGB_ALPHA_SHIFT = 24;
    static constexpr int ARGB_RED_SHIFT = 16;
    static constexpr int ARGB_GREEN_SHIFT = 8;
    static constexpr int ARGB_BLUE_SHIFT = 0;

    const int screenWidth = 1200;
    const int screenHeight = 2000;
    Drawing::SamplingOptions sampling;
    Drawing::Rect frameRect;

    std::shared_ptr<Media::PixelMap> testPixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> largePixelMap_ = nullptr;

    bool LoadTestImages()
    {
        if (testPixelMap_ == nullptr) {
            testPixelMap_ = DecodePixelMap("/data/local/tmp/fg_test.jpg",
                Media::AllocatorType::SHARE_MEM_ALLOC);
        }
        if (largePixelMap_ == nullptr) {
            largePixelMap_ = DecodePixelMap("/data/local/tmp/3200x2000.jpg",
                Media::AllocatorType::SHARE_MEM_ALLOC);
        }
        return testPixelMap_ != nullptr && largePixelMap_ != nullptr;
    }

    std::shared_ptr<Media::PixelMap> CreateTestPixelMapWithFormat(Media::PixelFormat format,
        int32_t width, int32_t height)
    {
        if (width <= 0 || height <= 0) {
            return nullptr;
        }

        Media::InitializationOptions opts;
        opts.size.width = width;
        opts.size.height = height;
        opts.pixelFormat = format;
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        opts.editable = true;

        std::vector<uint32_t> colors(width * height);
        for (int32_t i = 0; i < width * height; i++) {
            // Create a gradient pattern
            uint8_t r = static_cast<uint8_t>((i % width) * 255 / width);
            uint8_t g = static_cast<uint8_t>((i / width) * 255 / height);
            uint8_t b = static_cast<uint8_t>(128);
            uint8_t a = 255;
            colors[i] = (a << ARGB_ALPHA_SHIFT) | (r << ARGB_RED_SHIFT) |
                        (g << ARGB_GREEN_SHIFT) | (b << ARGB_BLUE_SHIFT);
        }

        return std::shared_ptr<Media::PixelMap>(
            Media::PixelMap::Create(colors.data(), colors.size(), opts).release());
    }

    std::shared_ptr<Media::PixelMap> CreateTestPixelMapWithAlphaType(Media::AlphaType alphaType,
        int32_t width, int32_t height)
    {
        if (width <= 0 || height <= 0) {
            return nullptr;
        }

        Media::InitializationOptions opts;
        opts.size.width = width;
        opts.size.height = height;
        opts.pixelFormat = Media::PixelFormat::RGBA_8888;
        opts.alphaType = alphaType;
        opts.editable = true;

        std::vector<uint32_t> colors(width * height);
        for (int32_t i = 0; i < width * height; i++) {
            uint8_t r = 255;
            uint8_t g = static_cast<uint8_t>((i % width) * 255 / width);
            uint8_t b = static_cast<uint8_t>((i / width) * 255 / height);
            uint8_t a = 200;
            colors[i] = (a << ARGB_ALPHA_SHIFT) | (r << ARGB_RED_SHIFT) |
                        (g << ARGB_GREEN_SHIFT) | (b << ARGB_BLUE_SHIFT);
        }

        return std::shared_ptr<Media::PixelMap>(
            Media::PixelMap::Create(colors.data(), colors.size(), opts).release());
    }

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        LoadTestImages();
    }
};

// ============================================================================
// Single Interface Tests (White Box Tests)
// ============================================================================

/*
 * @tc.name: RS_PIXELMAP_SCALE_TEST
 * @tc.desc: Test pixelmap scale transformation with normal and edge case values (negative, zero, large)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_SCALE_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    // Normal values + edge cases: negative, zero, very small, very large
    float scales[] = {
        -2.0f, -1.5f, -1.0f, -0.75f, -0.5f, -0.25f, 0.0f,  // Negative values
        0.1f, 0.25f, 0.5f, 0.75f, 1.0f,  // Normal positive values
        1.25f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f  // Large values
    };
    int32_t count = sizeof(scales) / sizeof(scales[0]);
    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (int32_t i = 0; i < count; i++) {
        // Clone and scale the pixelmap
        Media::InitializationOptions opts;
        opts.size.width = testPixelMap_->GetWidth();
        opts.size.height = testPixelMap_->GetHeight();
        opts.pixelFormat = testPixelMap_->GetPixelFormat();
        opts.alphaType = testPixelMap_->GetAlphaType();

        std::shared_ptr<Media::PixelMap> scaledPixelMap(
            Media::PixelMap::Create(*testPixelMap_, opts).release());
        if (scaledPixelMap) {
            scaledPixelMap->scale(scales[i], scales[i]);
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        int32_t row = i / column;
        int32_t col = i % column;

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            (gap + nodeWidth) * (col + 1),
            (gap + nodeHeight) * (row + 1)
        );
        drawing->DrawPixelMapWithParm(scaledPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_ROTATE_TEST
 * @tc.desc: Test pixelmap rotation with normal, negative, and >360 degree angles
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ROTATE_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    // Normal values + edge cases: negative, zero, > 360, very large angles
    float angles[] = {
        -720.0f, -540.0f, -360.0f, -270.0f, -180.0f, -135.0f, -90.0f, -45.0f,  // Negative and large negative
        0.0f, 45.0f, 90.0f,  // Normal positive
        135.0f, 180.0f, 225.0f, 270.0f, 315.0f, 360.0f, 405.0f, 450.0f,  // >= 360
        540.0f, 720.0f, 1080.0f  // Very large angles
    };
    int32_t count = sizeof(angles) / sizeof(angles[0]);
    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (int32_t i = 0; i < count; i++) {
        // Clone and rotate the pixelmap
        Media::InitializationOptions opts;
        opts.size.width = testPixelMap_->GetWidth();
        opts.size.height = testPixelMap_->GetHeight();
        opts.pixelFormat = testPixelMap_->GetPixelFormat();
        opts.alphaType = testPixelMap_->GetAlphaType();

        std::shared_ptr<Media::PixelMap> rotatedPixelMap(
            Media::PixelMap::Create(*testPixelMap_, opts).release());
        if (rotatedPixelMap) {
            rotatedPixelMap->rotate(angles[i]);
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        int32_t row = i / column;
        int32_t col = i % column;

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            (gap + nodeWidth) * (col + 1),
            (gap + nodeHeight) * (row + 1)
        );
        drawing->DrawPixelMapWithParm(rotatedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_FLIP_TEST
 * @tc.desc: Test pixelmap flip transformation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_FLIP_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    bool flipOptions[][2] = {
        {false, false}, // No flip
        {true, false},  // Horizontal flip
        {false, true},  // Vertical flip
        {true, true}    // Both flip
    };
    int32_t count = sizeof(flipOptions) / sizeof(flipOptions[0]);
    int32_t nodeWidth = 400;
    int32_t nodeHeight = 300;
    int32_t gap = 50;

    for (int32_t i = 0; i < count; i++) {
        // Clone and flip the pixelmap
        Media::InitializationOptions opts;
        opts.size.width = testPixelMap_->GetWidth();
        opts.size.height = testPixelMap_->GetHeight();
        opts.pixelFormat = testPixelMap_->GetPixelFormat();
        opts.alphaType = testPixelMap_->GetAlphaType();

        std::shared_ptr<Media::PixelMap> flippedPixelMap(
            Media::PixelMap::Create(*testPixelMap_, opts).release());
        if (flippedPixelMap) {
            flippedPixelMap->flip(flipOptions[i][0], flipOptions[i][1]);
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap,
            gap + (nodeHeight + gap) * i,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap,
            gap + (nodeHeight + gap) * i,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            gap + nodeWidth,
            (gap + nodeHeight) * (i + 1)
        );
        drawing->DrawPixelMapWithParm(flippedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_CROP_TEST
 * @tc.desc: Test pixelmap crop with different regions including edge cases
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_CROP_TEST)
{
    if (!LoadTestImages() || largePixelMap_ == nullptr) {
        return;
    }

    int32_t originalWidth = largePixelMap_->GetWidth();
    int32_t originalHeight = largePixelMap_->GetHeight();

    // Normal regions + edge cases: zero size, negative values (will fail/clip), full size, small regions
    Media::Rect cropRects[] = {
        {0, 0, originalWidth, originalHeight},                    // Full size (no crop)
        {0, 0, originalWidth / 4, originalHeight / 4},              // Small region
        {originalWidth / 4, originalHeight / 4, originalWidth / 2, originalHeight / 2}, // Center region
        {0, 0, originalWidth / 2, originalHeight / 2},              // Top-left quarter
        {originalWidth / 2, 0, originalWidth / 2, originalHeight / 2},  // Top-right quarter
        {0, originalHeight / 2, originalWidth / 2, originalHeight / 2},  // Bottom-left quarter
        {0, 0, 1, 1},                                         // 1x1 pixel
        {originalWidth - 100, originalHeight - 100, 100, 100}, // Bottom-right corner
    };
    int32_t count = sizeof(cropRects) / sizeof(cropRects[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < count; i++) {
        // Clone and crop the pixelmap
        Media::InitializationOptions opts;
        opts.size.width = largePixelMap_->GetWidth();
        opts.size.height = largePixelMap_->GetHeight();
        opts.pixelFormat = largePixelMap_->GetPixelFormat();
        opts.alphaType = largePixelMap_->GetAlphaType();

        std::shared_ptr<Media::PixelMap> croppedPixelMap(
            Media::PixelMap::Create(*largePixelMap_, opts).release());
        if (croppedPixelMap) {
            croppedPixelMap->crop(cropRects[i]);
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        int32_t row = i / column;
        int32_t col = i % column;

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            (gap + nodeWidth) * (col + 1),
            (gap + nodeHeight) * (row + 1)
        );
        drawing->DrawPixelMapWithParm(croppedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_TRANSLATE_TEST
 * @tc.desc: Test pixelmap translate transformation with various values including negative
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_TRANSLATE_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    // Edge cases: negative, zero, positive, large values
    float translations[][2] = {
        {0.0f, 0.0f},         // No translation
        {10.0f, 0.0f},        // Small positive X
        {0.0f, 10.0f},        // Small positive Y
        {50.0f, 50.0f},       // Medium positive both
        {-10.0f, 0.0f},       // Small negative X
        {0.0f, -10.0f},       // Small negative Y
        {-50.0f, 0.0f},       // Medium negative X
        {0.0f, -50.0f},       // Medium negative Y
        {-50.0f, -50.0f},     // Both negative
        {100.0f, 0.0f},       // Large positive X
        {0.0f, 100.0f},       // Large positive Y
        {-100.0f, 0.0f},      // Large negative X
        {0.0f, -100.0f},      // Large negative Y
        {100.0f, 100.0f},     // Both large positive
        {-100.0f, -100.0f},    // Both large negative
        {200.0f, 200.0f},    // Very large positive
        {-200.0f, -200.0f},   // Very large negative
        {500.0f, 500.0f},     // Extreme positive
        {-500.0f, -500.0f},    // Extreme negative
    };
    int32_t count = sizeof(translations) / sizeof(translations[0]);
    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (int32_t i = 0; i < count; i++) {
        // Clone and translate the pixelmap
        Media::InitializationOptions opts;
        opts.size.width = testPixelMap_->GetWidth();
        opts.size.height = testPixelMap_->GetHeight();
        opts.pixelFormat = testPixelMap_->GetPixelFormat();
        opts.alphaType = testPixelMap_->GetAlphaType();

        std::shared_ptr<Media::PixelMap> translatedPixelMap(
            Media::PixelMap::Create(*testPixelMap_, opts).release());
        if (translatedPixelMap) {
            translatedPixelMap->translate(translations[i][0], translations[i][1]);
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        int32_t row = i / column;
        int32_t col = i % column;

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            (gap + nodeWidth) * (col + 1),
            (gap + nodeHeight) * (row + 1)
        );
        drawing->DrawPixelMapWithParm(translatedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_ALPHA_TEST
 * @tc.desc: Test pixelmap with different alpha values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ALPHA_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    // Alpha edge cases: negative, >1.0, zero, very small values
    float alphas[] = {
        -1.0f, -0.5f, -0.1f,  // Negative values (should be clamped to 0.0)
        0.0f,  // Fully transparent
        0.01f, 0.05f, 0.1f, 0.25f, 0.5f, 0.75f,  // Small to normal values
        1.0f,  // Fully opaque
        1.1f, 1.5f, 2.0f  // Values > 1.0 (should be clamped to 1.0)
    };
    int32_t count = sizeof(alphas) / sizeof(alphas[0]);
    int32_t nodeWidth = 400;
    int32_t nodeHeight = 300;
    int32_t gap = 50;

    for (int32_t i = 0; i < count; i++) {
        // Clone and set alpha
        Media::InitializationOptions opts;
        opts.size.width = testPixelMap_->GetWidth();
        opts.size.height = testPixelMap_->GetHeight();
        opts.pixelFormat = testPixelMap_->GetPixelFormat();
        opts.alphaType = testPixelMap_->GetAlphaType();

        std::shared_ptr<Media::PixelMap> alphaPixelMap(
            Media::PixelMap::Create(*testPixelMap_, opts).release());
        if (alphaPixelMap) {
            alphaPixelMap->SetAlpha(alphas[i]);
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap,
            gap + (nodeHeight + gap) * i,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap,
            gap + (nodeHeight + gap) * i,
            nodeWidth,
            nodeHeight
        });
        // Use a gradient background to visualize transparency
        canvasNode->SetBackgroundColor(0xFF808080);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            gap + nodeWidth,
            (gap + nodeHeight) * (i + 1)
        );
        drawing->DrawPixelMapWithParm(alphaPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_PIXEL_FORMAT_TEST
 * @tc.desc: Test pixelmap with different pixel formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_PIXEL_FORMAT_TEST)
{
    Media::PixelFormat formats[] = {
        Media::PixelFormat::ARGB_8888,
        Media::PixelFormat::RGBA_8888,
        Media::PixelFormat::BGRA_8888,
        Media::PixelFormat::RGB_565,
        Media::PixelFormat::RGB_888,
        Media::PixelFormat::ALPHA_8
    };
    int32_t count = sizeof(formats) / sizeof(formats[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t i = 0; i < count; i++) {
        auto pixelMap = CreateTestPixelMapWithFormat(formats[i], nodeWidth, nodeHeight);
        if (!pixelMap) {
            continue;
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        int32_t row = i / column;
        int32_t col = i % column;

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap + (nodeWidth + gap) * col,
            gap + (nodeHeight + gap) * row,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            (gap + nodeWidth) * (col + 1),
            (gap + nodeHeight) * (row + 1)
        );
        drawing->DrawPixelMapWithParm(pixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_ALPHA_TYPE_TEST
 * @tc.desc: Test pixelmap with different alpha types
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ALPHA_TYPE_TEST)
{
    Media::AlphaType alphaTypes[] = {
        Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE,
        Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL,
        Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL
    };
    int32_t count = sizeof(alphaTypes) / sizeof(alphaTypes[0]);
    int32_t nodeWidth = 400;
    int32_t nodeHeight = 300;
    int32_t gap = 50;

    for (int32_t i = 0; i < count; i++) {
        auto pixelMap = CreateTestPixelMapWithAlphaType(alphaTypes[i], nodeWidth, nodeHeight);
        if (!pixelMap) {
            continue;
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({
            gap,
            gap + (nodeHeight + gap) * i,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetFrame({
            gap,
            gap + (nodeHeight + gap) * i,
            nodeWidth,
            nodeHeight
        });
        canvasNode->SetBackgroundColor(0xFF404040);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            gap + nodeWidth,
            (gap + nodeHeight) * (i + 1)
        );
        drawing->DrawPixelMapWithParm(pixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_SIZE_TEST
 * @tc.desc: Test pixelmap with different dimensions
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_SIZE_TEST)
{
    // Size edge cases: very small, very large, extreme aspect ratios
    int32_t widths[] = {1, 2, 10, 50, 100, 200, 300, 400, 500, 800, 1000};
    int32_t heights[] = {1, 2, 10, 50, 100, 200, 300, 500, 800};
    int32_t widthCount = sizeof(widths) / sizeof(widths[0]);
    int32_t heightCount = sizeof(heights) / sizeof(heights[0]);
    int32_t gap = 30;

    int32_t row = 0;
    for (int32_t j = 0; j < heightCount; j++) {
        for (int32_t i = 0; i < widthCount; i++) {
            auto pixelMap = CreateTestPixelMapWithFormat(
                Media::PixelFormat::RGBA_8888, widths[i], heights[j]);
            if (!pixelMap) {
                continue;
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({
                gap + (500 + gap) * i,
                gap + (300 + gap) * row,
                500,
                300
            });
            canvasNode->SetFrame({
                gap + (500 + gap) * i,
                gap + (300 + gap) * row,
                500,
                300
            });
            canvasNode->SetBackgroundColor(0xFFFFFFFF);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                gap + 500 + (500 + gap) * i,
                gap + 300 + (300 + gap) * row
            );
            drawing->DrawPixelMapWithParm(pixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
        row++;
    }
}

// ============================================================================
// Combination Tests (Multiple Interface Combined)
// ============================================================================

/*
 * @tc.name: RS_PIXELMAP_SCALE_ROTATE_TEST
 * @tc.desc: Test pixelmap with combined scale and rotate
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_SCALE_ROTATE_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    float scales[] = {0.5f, 0.75f, 1.0f, 1.5f};
    float rotations[] = {0.0f, 45.0f, 90.0f, 135.0f};
    int32_t scaleCount = sizeof(scales) / sizeof(scales[0]);
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 300;
    int32_t gap = 50;

    for (int32_t i = 0; i < scaleCount; i++) {
        for (int32_t j = 0; j < rotCount; j++) {
            // Clone, scale and rotate
            Media::InitializationOptions opts;
            opts.size.width = testPixelMap_->GetWidth();
            opts.size.height = testPixelMap_->GetHeight();
            opts.pixelFormat = testPixelMap_->GetPixelFormat();
            opts.alphaType = testPixelMap_->GetAlphaType();

            std::shared_ptr<Media::PixelMap> transformedPixelMap(
                Media::PixelMap::Create(*testPixelMap_, opts).release());
            if (transformedPixelMap) {
                transformedPixelMap->scale(scales[i], scales[i]);
                transformedPixelMap->rotate(rotations[j]);
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({
                gap + (nodeWidth + gap) * j,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetFrame({
                gap + (nodeWidth + gap) * j,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetBackgroundColor(0xFFFFFFFF);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (j + 1),
                (gap + nodeHeight) * (i + 1)
            );
            drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_ROTATE_FLIP_TEST
 * @tc.desc: Test pixelmap with combined rotate and flip
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ROTATE_FLIP_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    float rotations[] = {0.0f, 90.0f, 180.0f, 270.0f};
    bool flipOptions[][2] = {
        {false, false},
        {true, false},
        {false, true},
        {true, true}
    };
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t flipCount = sizeof(flipOptions) / sizeof(flipOptions[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 50;

    for (int32_t i = 0; i < rotCount; i++) {
        for (int32_t j = 0; j < flipCount; j++) {
            // Clone, rotate and flip
            Media::InitializationOptions opts;
            opts.size.width = testPixelMap_->GetWidth();
            opts.size.height = testPixelMap_->GetHeight();
            opts.pixelFormat = testPixelMap_->GetPixelFormat();
            opts.alphaType = testPixelMap_->GetAlphaType();

            std::shared_ptr<Media::PixelMap> transformedPixelMap(
                Media::PixelMap::Create(*testPixelMap_, opts).release());
            if (transformedPixelMap) {
                transformedPixelMap->rotate(rotations[i]);
                transformedPixelMap->flip(flipOptions[j][0], flipOptions[j][1]);
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({
                gap + (nodeWidth + gap) * j,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetFrame({
                gap + (nodeWidth + gap) * j,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetBackgroundColor(0xFFFFFFFF);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (j + 1),
                (gap + nodeHeight) * (i + 1)
            );
            drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_SCALE_CROP_TEST
 * @tc.desc: Test pixelmap with combined scale and crop
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_SCALE_CROP_TEST)
{
    if (!LoadTestImages() || largePixelMap_ == nullptr) {
        return;
    }

    float scales[] = {0.5f, 1.0f, 1.5f};
    int32_t originalWidth = largePixelMap_->GetWidth();
    int32_t originalHeight = largePixelMap_->GetHeight();

    Media::Rect cropRects[] = {
        {0, 0, originalWidth / 2, originalHeight / 2},
        {originalWidth / 4, originalHeight / 4, originalWidth / 2, originalHeight / 2}
    };
    int32_t scaleCount = sizeof(scales) / sizeof(scales[0]);
    int32_t cropCount = sizeof(cropRects) / sizeof(cropRects[0]);
    int32_t nodeWidth = 400;
    int32_t nodeHeight = 300;
    int32_t gap = 50;

    for (int32_t i = 0; i < scaleCount; i++) {
        for (int32_t j = 0; j < cropCount; j++) {
            // Clone, scale and crop
            Media::InitializationOptions opts;
            opts.size.width = largePixelMap_->GetWidth();
            opts.size.height = largePixelMap_->GetHeight();
            opts.pixelFormat = largePixelMap_->GetPixelFormat();
            opts.alphaType = largePixelMap_->GetAlphaType();

            std::shared_ptr<Media::PixelMap> transformedPixelMap(
                Media::PixelMap::Create(*largePixelMap_, opts).release());
            if (transformedPixelMap) {
                transformedPixelMap->scale(scales[i], scales[i]);
                transformedPixelMap->crop(cropRects[j]);
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({
                gap + (nodeWidth + gap) * j,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetFrame({
                gap + (nodeWidth + gap) * j,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetBackgroundColor(0xFFFFFFFF);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (j + 1),
                (gap + nodeHeight) * (i + 1)
            );
            drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

} // namespace OHOS::Rosen
