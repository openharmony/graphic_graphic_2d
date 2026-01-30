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

<<<<<<< HEAD
=======
/*
 * @tc.name: RS_PIXELMAP_TRANSLATE_ROTATE_TEST
 * @tc.desc: Test pixelmap with combined translate and rotate
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_TRANSLATE_ROTATE_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    float translations[][2] = {{0.0f, 0.0f}, {50.0f, 50.0f}, {-50.0f, -50.0f}};
    float rotations[] = {0.0f, 45.0f, 90.0f};
    int32_t transCount = sizeof(translations) / sizeof(translations[0]);
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 300;
    int32_t gap = 50;

    for (int32_t i = 0; i < transCount; i++) {
        for (int32_t j = 0; j < rotCount; j++) {
            // Clone, translate and rotate
            Media::InitializationOptions opts;
            opts.size.width = testPixelMap_->GetWidth();
            opts.size.height = testPixelMap_->GetHeight();
            opts.pixelFormat = testPixelMap_->GetPixelFormat();
            opts.alphaType = testPixelMap_->GetAlphaType();

            std::shared_ptr<Media::PixelMap> transformedPixelMap(Media::PixelMap::Create(*testPixelMap_, opts).release());
            if (transformedPixelMap) {
                transformedPixelMap->translate(translations[i][0], translations[i][1]);
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
 * @tc.name: RS_PIXELMAP_SCALE_ROTATE_FLIP_TEST
 * @tc.desc: Test pixelmap with combined scale, rotate and flip
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_SCALE_ROTATE_FLIP_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    float scales[] = {0.75f, 1.0f, 1.25f};
    float rotations[] = {0.0f, 90.0f, 180.0f};
    bool flipHorizontal = true;

    int32_t scaleCount = sizeof(scales) / sizeof(scales[0]);
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 50;

    for (int32_t i = 0; i < scaleCount; i++) {
        for (int32_t j = 0; j < rotCount; j++) {
            // Clone, scale, rotate and flip
            Media::InitializationOptions opts;
            opts.size.width = testPixelMap_->GetWidth();
            opts.size.height = testPixelMap_->GetHeight();
            opts.pixelFormat = testPixelMap_->GetPixelFormat();
            opts.alphaType = testPixelMap_->GetAlphaType();

            std::shared_ptr<Media::PixelMap> transformedPixelMap(Media::PixelMap::Create(*testPixelMap_, opts).release());
            if (transformedPixelMap) {
                transformedPixelMap->scale(scales[i], scales[i]);
                transformedPixelMap->rotate(rotations[j]);
                transformedPixelMap->flip(flipHorizontal, false);
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
 * @tc.name: RS_PIXELMAP_SCALE_ROTATE_ALPHA_TEST
 * @tc.desc: Test pixelmap with combined scale, rotate and alpha
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_SCALE_ROTATE_ALPHA_TEST)
{
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    float scales[] = {0.8f, 1.0f, 1.2f};
    float rotations[] = {0.0f, 45.0f};
    float alphas[] = {0.5f, 0.75f, 1.0f};

    int32_t scaleCount = sizeof(scales) / sizeof(scales[0]);
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t alphaCount = sizeof(alphas) / sizeof(alphas[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 250;
    int32_t gap = 30;
    int32_t column = 3;

    int32_t index = 0;
    for (int32_t k = 0; k < alphaCount; k++) {
        for (int32_t i = 0; i < scaleCount; i++) {
            for (int32_t j = 0; j < rotCount; j++) {
                // Clone, scale, rotate and set alpha
                Media::InitializationOptions opts;
                opts.size.width = testPixelMap_->GetWidth();
                opts.size.height = testPixelMap_->GetHeight();
                opts.pixelFormat = testPixelMap_->GetPixelFormat();
                opts.alphaType = testPixelMap_->GetAlphaType();

                std::shared_ptr<Media::PixelMap> transformedPixelMap(Media::PixelMap::Create(*testPixelMap_, opts).release());
                if (transformedPixelMap) {
                    transformedPixelMap->scale(scales[i], scales[i]);
                    transformedPixelMap->rotate(rotations[j]);
                    transformedPixelMap->SetAlpha(alphas[k]);
                }

                auto rosenImage = std::make_shared<Rosen::RSImage>();
                rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
                auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

                int32_t row = index / column;
                int32_t col = index % column;

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
                canvasNode->SetBackgroundColor(0xFF808080);
                GetRootNode()->AddChild(canvasNode);

                auto drawing = canvasNode->BeginRecording(
                    (gap + nodeWidth) * (col + 1),
                    (gap + nodeHeight) * (row + 1)
                );
                drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
                canvasNode->FinishRecording();
                RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
                RegisterNode(canvasNode);

                index++;
            }
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_CROP_ROTATE_FLIP_TEST
 * @tc.desc: Test pixelmap with combined crop, rotate and flip
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_CROP_ROTATE_FLIP_TEST)
{
    if (!LoadTestImages() || largePixelMap_ == nullptr) {
        return;
    }

    int32_t originalWidth = largePixelMap_->GetWidth();
    int32_t originalHeight = largePixelMap_->GetHeight();

    Media::Rect cropRects[] = {
        {0, 0, originalWidth / 2, originalHeight / 2},
        {originalWidth / 4, originalHeight / 4, originalWidth / 2, originalHeight / 2}
    };
    float rotations[] = {0.0f, 90.0f};
    bool flipOptions[][2] = {{false, false}, {true, true}};

    int32_t cropCount = sizeof(cropRects) / sizeof(cropRects[0]);
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t flipCount = sizeof(flipOptions) / sizeof(flipOptions[0]);
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 300;
    int32_t gap = 50;
    int32_t column = 2;

    int32_t index = 0;
    for (int32_t i = 0; i < cropCount; i++) {
        for (int32_t j = 0; j < rotCount; j++) {
            for (int32_t k = 0; k < flipCount; k++) {
                // Clone, crop, rotate and flip
                Media::InitializationOptions opts;
                opts.size.width = largePixelMap_->GetWidth();
                opts.size.height = largePixelMap_->GetHeight();
                opts.pixelFormat = largePixelMap_->GetPixelFormat();
                opts.alphaType = largePixelMap_->GetAlphaType();

                std::shared_ptr<Media::PixelMap> transformedPixelMap(Media::PixelMap::Create(*largePixelMap_, opts).release());
                if (transformedPixelMap) {
                    transformedPixelMap->crop(cropRects[i]);
                    transformedPixelMap->rotate(rotations[j]);
                    transformedPixelMap->flip(flipOptions[k][0], flipOptions[k][1]);
                }

                auto rosenImage = std::make_shared<Rosen::RSImage>();
                rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
                auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

                int32_t row = index / column;
                int32_t col = index % column;

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
                drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
                canvasNode->FinishRecording();
                RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
                RegisterNode(canvasNode);

                index++;
            }
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_ALL_TRANSFORMS_TEST
 * @tc.desc: Test pixelmap with all transformations combined
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ALL_TRANSFORMS_TEST)
{
    if (!LoadTestImages() || largePixelMap_ == nullptr) {
        return;
    }

    int32_t originalWidth = largePixelMap_->GetWidth();
    int32_t originalHeight = largePixelMap_->GetHeight();

    struct TransformConfig {
        float scale;
        float rotate;
        float translateX;
        float translateY;
        bool flipX;
        bool flipY;
        float alpha;
        Media::Rect crop;
    };

    TransformConfig configs[] = {
        {1.0f, 0.0f, 0.0f, 0.0f, false, false, 1.0f, {0, 0, originalWidth, originalHeight}},
        {0.75f, 45.0f, 20.0f, 20.0f, true, false, 0.9f, {0, 0, originalWidth, originalHeight}},
        {1.25f, 90.0f, -20.0f, -20.0f, false, true, 0.8f, {0, 0, originalWidth, originalHeight}},
        {0.8f, 135.0f, 0.0f, 0.0f, true, true, 0.7f, {originalWidth/4, originalHeight/4, originalWidth/2, originalHeight/2}},
        {1.0f, 180.0f, 50.0f, 50.0f, false, false, 1.0f, {originalWidth/4, originalHeight/4, originalWidth/2, originalHeight/2}},
        {1.5f, 270.0f, -50.0f, -50.0f, true, false, 0.85f, {0, 0, originalWidth/2, originalHeight/2}}
    };

    int32_t count = sizeof(configs) / sizeof(configs[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 300;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t i = 0; i < count; i++) {
        // Clone and apply all transformations
        Media::InitializationOptions opts;
        opts.size.width = largePixelMap_->GetWidth();
        opts.size.height = largePixelMap_->GetHeight();
        opts.pixelFormat = largePixelMap_->GetPixelFormat();
        opts.alphaType = largePixelMap_->GetAlphaType();

        std::shared_ptr<Media::PixelMap> transformedPixelMap(Media::PixelMap::Create(*largePixelMap_, opts).release());
        if (transformedPixelMap) {
            transformedPixelMap->scale(configs[i].scale, configs[i].scale);
            transformedPixelMap->rotate(configs[i].rotate);
            transformedPixelMap->translate(configs[i].translateX, configs[i].translateY);
            transformedPixelMap->flip(configs[i].flipX, configs[i].flipY);
            transformedPixelMap->SetAlpha(configs[i].alpha);
            transformedPixelMap->crop(configs[i].crop);
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
        drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

// ============================================================================
// ASTC Format Tests - ASTC 格式测试
// ============================================================================

/*
 * @tc.name: RS_PIXELMAP_ASTC_IMAGEFIT_TEST
 * @tc.desc: Test ASTC pixelmap with different ImageFit modes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ASTC_IMAGEFIT_TEST)
{
    auto astcPixelMap = CreateAstcPixelMap(256, 256);
    if (!astcPixelMap) {
        return;
    }

    // Test all ImageFit modes
    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::FIT_WIDTH,
        ImageFit::FIT_HEIGHT,
        ImageFit::NONE,
        ImageFit::SCALE_DOWN,
        ImageFit::TOP_LEFT,
        ImageFit::TOP,
        ImageFit::TOP_RIGHT,
        ImageFit::LEFT,
        ImageFit::CENTER,
        ImageFit::RIGHT,
        ImageFit::BOTTOM_LEFT,
        ImageFit::BOTTOM,
        ImageFit::BOTTOM_RIGHT
    };
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < fitCount; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(imageFits[i]));
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
        drawing->DrawPixelMapWithParm(astcPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_ASTC_SCALE_IMAGEFIT_TEST
 * @tc.desc: Test ASTC pixelmap with scale and ImageFit combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ASTC_SCALE_IMAGEFIT_TEST)
{
    auto astcPixelMap = CreateAstcPixelMap(256, 256);
    if (!astcPixelMap) {
        return;
    }

    float scales[] = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f};
    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::NONE
    };
    int32_t scaleCount = sizeof(scales) / sizeof(scales[0]);
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < scaleCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            // For ASTC format, Create doesn't support cloning from another ASTC pixelmap
            // Create a new ASTC pixelmap for each scale operation
            auto scaledPixelMap = CreateAstcPixelMap(astcPixelMap->GetWidth(), astcPixelMap->GetHeight());
            if (scaledPixelMap) {
                scaledPixelMap->scale(scales[i], scales[i]);
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(imageFits[j]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
}

/*
 * @tc.name: RS_PIXELMAP_ASTC_ROTATE_IMAGEFIT_TEST
 * @tc.desc: Test ASTC pixelmap with rotate and ImageFit combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ASTC_ROTATE_IMAGEFIT_TEST)
{
    auto astcPixelMap = CreateAstcPixelMap(256, 256);
    if (!astcPixelMap) {
        return;
    }

    float rotations[] = {0.0f, 45.0f, 90.0f, 135.0f, 180.0f};
    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::SCALE_DOWN
    };
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < rotCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            // For ASTC format, Create doesn't support cloning from another ASTC pixelmap
            // Create a new ASTC pixelmap for each rotation operation
            auto rotatedPixelMap = CreateAstcPixelMap(astcPixelMap->GetWidth(), astcPixelMap->GetHeight());
            if (rotatedPixelMap) {
                rotatedPixelMap->rotate(rotations[i]);
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(imageFits[j]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
}

/*
 * @tc.name: RS_PIXELMAP_ASTC_FLIP_IMAGEFIT_TEST
 * @tc.desc: Test ASTC pixelmap with flip and ImageFit combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ASTC_FLIP_IMAGEFIT_TEST)
{
    auto astcPixelMap = CreateAstcPixelMap(256, 256);
    if (!astcPixelMap) {
        return;
    }

    bool flipOptions[][2] = {
        {false, false}, // No flip
        {true, false},  // Horizontal flip
        {false, true},  // Vertical flip
        {true, true}    // Both flip
    };
    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::CENTER
    };
    int32_t flipCount = sizeof(flipOptions) / sizeof(flipOptions[0]);
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < flipCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            // For ASTC format, Create doesn't support cloning from another ASTC pixelmap
            // Create a new ASTC pixelmap for each flip operation
            auto flippedPixelMap = CreateAstcPixelMap(astcPixelMap->GetWidth(), astcPixelMap->GetHeight());
            if (flippedPixelMap) {
                flippedPixelMap->flip(flipOptions[i][0], flipOptions[i][1]);
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(imageFits[j]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
            drawing->DrawPixelMapWithParm(flippedPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_ASTC_SIZE_IMAGEFIT_TEST
 * @tc.desc: Test ASTC pixelmap with different sizes and ImageFit combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ASTC_SIZE_IMAGEFIT_TEST)
{
    int32_t astcSizes[][2] = {
        {128, 128},
        {256, 256},
        {512, 256},
        {256, 512}
    };
    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::SCALE_DOWN
    };
    int32_t sizeCount = sizeof(astcSizes) / sizeof(astcSizes[0]);
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < sizeCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            auto astcPixelMap = CreateAstcPixelMap(astcSizes[i][0], astcSizes[i][1]);
            if (!astcPixelMap) {
                continue;
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(imageFits[j]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
            drawing->DrawPixelMapWithParm(astcPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_ASTC_ALL_TRANSFORMS_IMAGEFIT_TEST
 * @tc.desc: Test ASTC pixelmap with all transformations and ImageFit combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ASTC_ALL_TRANSFORMS_IMAGEFIT_TEST)
{
    auto astcPixelMap = CreateAstcPixelMap(256, 256);
    if (!astcPixelMap) {
        return;
    }

    struct AstcTransformConfig {
        float scale;
        float rotate;
        bool flipX;
        bool flipY;
        ImageFit imageFit;
    };

    AstcTransformConfig configs[] = {
        {1.0f, 0.0f, false, false, ImageFit::FILL},
        {0.75f, 45.0f, false, false, ImageFit::CONTAIN},
        {1.25f, 90.0f, true, false, ImageFit::COVER},
        {0.8f, 135.0f, false, true, ImageFit::SCALE_DOWN},
        {1.0f, 180.0f, true, true, ImageFit::NONE},
        {1.5f, 270.0f, false, false, ImageFit::CENTER}
    };

    int32_t count = sizeof(configs) / sizeof(configs[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 300;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t i = 0; i < count; i++) {
        // Clone and apply transformations to the ASTC pixelmap
        Media::InitializationOptions opts;
        opts.size.width = astcPixelMap->GetWidth();
        opts.size.height = astcPixelMap->GetHeight();
        opts.pixelFormat = astcPixelMap->GetPixelFormat();
        opts.alphaType = astcPixelMap->GetAlphaType();

        // For ASTC format, Create doesn't support cloning from another ASTC pixelmap
        // Create a new ASTC pixelmap for each transformation
        std::shared_ptr<Media::PixelMap> transformedPixelMap(CreateAstcPixelMap(astcPixelMap->GetWidth(), astcPixelMap->GetHeight()));
        if (transformedPixelMap) {
            transformedPixelMap->scale(configs[i].scale, configs[i].scale);
            transformedPixelMap->rotate(configs[i].rotate);
            transformedPixelMap->flip(configs[i].flipX, configs[i].flipY);
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(configs[i].imageFit));
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
        drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_ASTC_REPEAT_IMAGEFIT_TEST
 * @tc.desc: Test ASTC pixelmap with ImageRepeat and ImageFit combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_ASTC_REPEAT_IMAGEFIT_TEST)
{
    auto astcPixelMap = CreateAstcPixelMap(128, 128);
    if (!astcPixelMap) {
        return;
    }

    ImageRepeat imageRepeats[] = {
        ImageRepeat::NO_REPEAT,
        ImageRepeat::REPEAT_X,
        ImageRepeat::REPEAT_Y,
        ImageRepeat::REPEAT
    };
    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::SCALE_DOWN
    };
    int32_t repeatCount = sizeof(imageRepeats) / sizeof(imageRepeats[0]);
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < repeatCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::TOP_LEFT));
            rosenImage->SetImageRepeat(static_cast<int>(imageRepeats[i]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
            drawing->DrawPixelMapWithParm(astcPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

// ============================================================================
// HDR Format Tests - HDR 格式测试
// ============================================================================

/*
 * @tc.name: RS_PIXELMAP_HDR_FORMAT_IMAGEFIT_TEST
 * @tc.desc: Test HDR pixelmap formats (RGBA_1010102, YCBCR_P010, YCRCB_P010) with ImageFit
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_HDR_FORMAT_IMAGEFIT_TEST)
{
    Media::PixelFormat hdrFormats[] = {
        Media::PixelFormat::RGBA_1010102,
        Media::PixelFormat::YCBCR_P010,
        Media::PixelFormat::YCRCB_P010
    };
    int32_t formatCount = sizeof(hdrFormats) / sizeof(hdrFormats[0]);

    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::SCALE_DOWN,
        ImageFit::CENTER
    };
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (int32_t i = 0; i < formatCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            auto hdrPixelMap = CreateHDRPixelMap(hdrFormats[i], 256, 256);
            if (!hdrPixelMap) {
                continue;
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(imageFits[j]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
            canvasNode->SetBackgroundColor(0xFF000000); // Black background for HDR
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (col + 1),
                (gap + nodeHeight) * (row + 1)
            );
            drawing->DrawPixelMapWithParm(hdrPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_HDR_SIZE_VARIATION_TEST
 * @tc.desc: Test HDR pixelmap formats with different sizes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_HDR_SIZE_VARIATION_TEST)
{
    int32_t sizes[][2] = {
        {128, 128},
        {256, 256},
        {512, 256},
        {256, 512},
        {512, 512}
    };
    int32_t sizeCount = sizeof(sizes) / sizeof(sizes[0]);

    Media::PixelFormat hdrFormats[] = {
        Media::PixelFormat::RGBA_1010102,
        Media::PixelFormat::YCBCR_P010,
        Media::PixelFormat::YCRCB_P010
    };
    int32_t formatCount = sizeof(hdrFormats) / sizeof(hdrFormats[0]);
    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (int32_t i = 0; i < formatCount; i++) {
        for (int32_t j = 0; j < sizeCount; j++) {
            auto hdrPixelMap = CreateHDRPixelMap(hdrFormats[i], sizes[j][0], sizes[j][1]);
            if (!hdrPixelMap) {
                continue;
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * sizeCount / column + j / column;
            int32_t col = j % column;

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
            canvasNode->SetBackgroundColor(0xFF000000);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (col + 1),
                (gap + nodeHeight) * (row + 1)
            );
            drawing->DrawPixelMapWithParm(hdrPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

// ============================================================================
// ColorSpace Tests - 色彩空间测试
// ============================================================================

/*
 * @tc.name: RS_PIXELMAP_COLORSPACE_IMAGEFIT_TEST
 * @tc.desc: Test pixelmap with different color spaces and ImageFit combinations
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_COLORSPACE_IMAGEFIT_TEST)
{
    Media::ColorSpace colorSpaces[] = {
        Media::ColorSpace::SRGB,
        Media::ColorSpace::DISPLAY_P3,
        Media::ColorSpace::LINEAR_SRGB,
        Media::ColorSpace::ADOBE_RGB_1998
    };
    int32_t colorSpaceCount = sizeof(colorSpaces) / sizeof(colorSpaces[0]);

    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::SCALE_DOWN
    };
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t i = 0; i < colorSpaceCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            auto colorSpacePixelMap = CreateColorSpacePixelMap(colorSpaces[i], 256, 256);
            if (!colorSpacePixelMap) {
                continue;
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(imageFits[j]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
            canvasNode->SetBackgroundColor(0xFF808080); // Neutral gray background
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (col + 1),
                (gap + nodeHeight) * (row + 1)
            );
            drawing->DrawPixelMapWithParm(colorSpacePixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_COLORSPACE_SCALE_ROTATE_TEST
 * @tc.desc: Test pixelmap with color spaces combined with scale and rotate
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_COLORSPACE_SCALE_ROTATE_TEST)
{
    Media::ColorSpace colorSpaces[] = {
        Media::ColorSpace::SRGB,
        Media::ColorSpace::DISPLAY_P3,
        Media::ColorSpace::LINEAR_SRGB
    };
    int32_t colorSpaceCount = sizeof(colorSpaces) / sizeof(colorSpaces[0]);

    float scales[] = {0.75f, 1.0f, 1.25f};
    float rotations[] = {0.0f, 45.0f, 90.0f};
    int32_t scaleCount = sizeof(scales) / sizeof(scales[0]);
    int32_t rotCount = sizeof(rotations) / sizeof(rotations[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 250;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t k = 0; k < colorSpaceCount; k++) {
        for (int32_t i = 0; i < scaleCount; i++) {
            for (int32_t j = 0; j < rotCount; j++) {
                auto colorSpacePixelMap = CreateColorSpacePixelMap(colorSpaces[k], 256, 256);
                if (!colorSpacePixelMap) {
                    continue;
                }

                // Clone and apply transformations
                Media::InitializationOptions opts;
                opts.size.width = colorSpacePixelMap->GetWidth();
                opts.size.height = colorSpacePixelMap->GetHeight();
                opts.pixelFormat = colorSpacePixelMap->GetPixelFormat();
                opts.alphaType = colorSpacePixelMap->GetAlphaType();

                std::shared_ptr<Media::PixelMap> transformedPixelMap(Media::PixelMap::Create(*colorSpacePixelMap, opts).release());
                if (transformedPixelMap) {
                    transformedPixelMap->scale(scales[i], scales[i]);
                    transformedPixelMap->rotate(rotations[j]);
                }

                auto rosenImage = std::make_shared<Rosen::RSImage>();
                rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
                auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

                int32_t baseRow = k * scaleCount * rotCount / column;
                int32_t row = baseRow + (i * rotCount + j) / column;
                int32_t col = (i * rotCount + j) % column;

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
                canvasNode->SetBackgroundColor(0xFF808080);
                GetRootNode()->AddChild(canvasNode);

                auto drawing = canvasNode->BeginRecording(
                    (gap + nodeWidth) * (col + 1),
                    (gap + nodeHeight) * (row + 1)
                );
                drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
                canvasNode->FinishRecording();
                RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
                RegisterNode(canvasNode);
            }
        }
    }
}

// ============================================================================
// HDR ToSDR Tests - HDR转SDR测试
// ============================================================================

/*
 * @tc.name: RS_PIXELMAP_HDR_TOSDR_IMAGEFIT_TEST
 * @tc.desc: Test HDR pixelmap ToSDR conversion with different ImageFit modes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_HDR_TOSDR_IMAGEFIT_TEST)
{
    Media::PixelFormat hdrFormats[] = {
        Media::PixelFormat::RGBA_1010102,
        Media::PixelFormat::YCBCR_P010,
        Media::PixelFormat::YCRCB_P010
    };
    int32_t formatCount = sizeof(hdrFormats) / sizeof(hdrFormats[0]);

    ImageFit imageFits[] = {
        ImageFit::FILL,
        ImageFit::CONTAIN,
        ImageFit::COVER,
        ImageFit::SCALE_DOWN,
        ImageFit::CENTER
    };
    int32_t fitCount = sizeof(imageFits) / sizeof(imageFits[0]);
    int32_t nodeWidth = 200;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 5;

    for (int32_t i = 0; i < formatCount; i++) {
        for (int32_t j = 0; j < fitCount; j++) {
            // Create HDR pixel map with ToSDR support
            auto hdrPixelMap = CreateHDRPixelMap(hdrFormats[i], 256, 256, true);
            if (!hdrPixelMap) {
                continue;
            }

            // Call ToSDR to convert HDR to SDR
            hdrPixelMap->ToSdr();
            // Note: ToSDR might fail in test environment, continue anyway for visual verification

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(imageFits[j]));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * fitCount / column + j / column;
            int32_t col = j % column;

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
            canvasNode->SetBackgroundColor(0xFF000000); // Black background
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (col + 1),
                (gap + nodeHeight) * (row + 1)
            );
            drawing->DrawPixelMapWithParm(hdrPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_HDR_TOSDR_FORMAT_TEST
 * @tc.desc: Test HDR pixelmap ToSDR conversion with different output formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_HDR_TOSDR_FORMAT_TEST)
{
    Media::PixelFormat hdrFormats[] = {
        Media::PixelFormat::RGBA_1010102,
        Media::PixelFormat::YCBCR_P010,
        Media::PixelFormat::YCRCB_P010
    };
    int32_t formatCount = sizeof(hdrFormats) / sizeof(hdrFormats[0]);

    // Output formats for ToSDR
    Media::PixelFormat outFormats[] = {
        Media::PixelFormat::RGBA_8888,  // For RGB1010102
        Media::PixelFormat::NV12,        // For YCBCR_P010
        Media::PixelFormat::NV21         // For YCRCB_P010
    };
    int32_t outFormatCount = sizeof(outFormats) / sizeof(outFormats[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 300;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t i = 0; i < formatCount; i++) {
        for (int32_t j = 0; j < outFormatCount; j++) {
            // Create HDR pixel map with ToSDR support
            auto hdrPixelMap = CreateHDRPixelMap(hdrFormats[i], 256, 256, true);
            if (!hdrPixelMap) {
                continue;
            }

            // Call ToSDR with specific output format
            bool toSRGB = (j % 2 == 0); // Alternate between SRGB and P3
            hdrPixelMap->ToSdr(outFormats[j], toSRGB);

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = i * outFormatCount / column + j / column;
            int32_t col = j % column;

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
            canvasNode->SetBackgroundColor(0xFF000000);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (col + 1),
                (gap + nodeHeight) * (row + 1)
            );
            drawing->DrawPixelMapWithParm(hdrPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_HDR_TOSDR_BEFORE_AFTER_TEST
 * @tc.desc: Test HDR pixelmap before and after ToSDR conversion (side by side comparison)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_HDR_TOSDR_BEFORE_AFTER_TEST)
{
    Media::PixelFormat hdrFormats[] = {
        Media::PixelFormat::RGBA_1010102,
        Media::PixelFormat::YCBCR_P010
    };
    int32_t formatCount = sizeof(hdrFormats) / sizeof(hdrFormats[0]);

    int32_t nodeWidth = 400;
    int32_t nodeHeight = 300;
    int32_t gap = 50;

    for (int32_t i = 0; i < formatCount; i++) {
        // Create two HDR pixel maps - one original, one for ToSDR
        auto hdrPixelMapOriginal = CreateHDRPixelMap(hdrFormats[i], 256, 256, true);
        auto hdrPixelMapToSdr = CreateHDRPixelMap(hdrFormats[i], 256, 256, true);

        if (!hdrPixelMapOriginal || !hdrPixelMapToSdr) {
            continue;
        }

        // Convert the second one to SDR
        hdrPixelMapToSdr->ToSdr();

        // Display original HDR
        {
            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({
                gap + (nodeWidth + gap) * 0,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetFrame({
                gap + (nodeWidth + gap) * 0,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetBackgroundColor(0xFF000000);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(gap + nodeWidth, (gap + nodeHeight) * (i + 1));
            drawing->DrawPixelMapWithParm(hdrPixelMapOriginal, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }

        // Display SDR converted
        {
            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({
                gap + (nodeWidth + gap) * 1,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetFrame({
                gap + (nodeWidth + gap) * 1,
                gap + (nodeHeight + gap) * i,
                nodeWidth,
                nodeHeight
            });
            canvasNode->SetBackgroundColor(0xFF000000);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(gap + nodeWidth, (gap + nodeHeight) * (i + 1));
            drawing->DrawPixelMapWithParm(hdrPixelMapToSdr, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_HDR_TOSDR_TRANSFORM_TEST
 * @tc.desc: Test HDR pixelmap ToSDR with transformations (scale, rotate)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_HDR_TOSDR_TRANSFORM_TEST)
{
    Media::PixelFormat hdrFormats[] = {
        Media::PixelFormat::RGBA_1010102,
        Media::PixelFormat::YCBCR_P010
    };

    struct TransformConfig {
        float scale;
        float rotate;
        ImageFit imageFit;
    };

    TransformConfig transforms[] = {
        {0.75f, 0.0f, ImageFit::FILL},
        {1.0f, 45.0f, ImageFit::CONTAIN},
        {1.25f, 90.0f, ImageFit::COVER},
        {0.8f, 135.0f, ImageFit::SCALE_DOWN}
    };
    int32_t transformCount = sizeof(transforms) / sizeof(transforms[0]);
    int32_t formatCount = sizeof(hdrFormats) / sizeof(hdrFormats[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 40;
    int32_t column = 4;

    for (int32_t k = 0; k < formatCount; k++) {
        for (int32_t i = 0; i < transformCount; i++) {
            // Create HDR pixel map
            auto hdrPixelMap = CreateHDRPixelMap(hdrFormats[k], 256, 256, true);
            if (!hdrPixelMap) {
                continue;
            }

            // Apply transformations before ToSDR
            Media::InitializationOptions opts;
            opts.size.width = hdrPixelMap->GetWidth();
            opts.size.height = hdrPixelMap->GetHeight();
            opts.pixelFormat = hdrPixelMap->GetPixelFormat();
            opts.alphaType = hdrPixelMap->GetAlphaType();
            opts.allocatorType = Media::AllocatorType::DMA_ALLOC;

            std::shared_ptr<Media::PixelMap> transformedPixelMap(Media::PixelMap::Create(*hdrPixelMap, opts).release());
            if (transformedPixelMap) {
                transformedPixelMap->scale(transforms[i].scale, transforms[i].scale);
                transformedPixelMap->rotate(transforms[i].rotate);
            }

            // Convert to SDR after transformations
            if (transformedPixelMap) {
                transformedPixelMap->ToSdr();
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(transforms[i].imageFit));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t baseRow = k * transformCount / column;
            int32_t row = baseRow + i / column;
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
            canvasNode->SetBackgroundColor(0xFF000000);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording(
                (gap + nodeWidth) * (col + 1),
                (gap + nodeHeight) * (row + 1)
            );
            drawing->DrawPixelMapWithParm(transformedPixelMap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

// ============================================================================
// Static Interface Tests - 静态接口测试
// ============================================================================

/*
 * @tc.name: RS_PIXELMAP_STATIC_CREATE_COLORS_FORMAT_TEST
 * @tc.desc: Test PixelMap::Create with pixel data and multiple formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_CREATE_COLORS_FORMAT_TEST)
{
    // Test Create(const uint32_t *colors, uint32_t colorLength, const InitializationOptions &opts)
    // with all supported RGB pixel formats
    Media::PixelFormat formats[] = {
        Media::PixelFormat::ARGB_8888,
        Media::PixelFormat::RGBA_8888,
        Media::PixelFormat::BGRA_8888,
        Media::PixelFormat::RGB_565,
        Media::PixelFormat::RGB_888,
        Media::PixelFormat::ALPHA_8,
        Media::PixelFormat::RGBA_F16,
        Media::PixelFormat::RGBA_U16
    };
    int32_t count = sizeof(formats) / sizeof(formats[0]);
    int32_t width = 200;
    int32_t height = 200;
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 3;

    std::vector<uint32_t> colors(width * height);
    for (int32_t i = 0; i < width * height; i++) {
        // Create gradient pattern
        uint8_t r = static_cast<uint8_t>((i % width) * 255 / width);
        uint8_t g = static_cast<uint8_t>((i / width) * 255 / height);
        uint8_t b = 128;
        uint8_t a = 255;
        colors[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }

    for (int32_t i = 0; i < count; i++) {
        Media::InitializationOptions opts;
        opts.size.width = width;
        opts.size.height = height;
        opts.pixelFormat = formats[i];
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;

        std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(colors.data(), colors.size(), opts).release());
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
 * @tc.name: RS_PIXELMAP_STATIC_CREATE_OFFSET_STRIDE_TEST
 * @tc.desc: Test PixelMap::Create with offset and stride parameters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_CREATE_OFFSET_STRIDE_TEST)
{
    // Test Create(const uint32_t *colors, uint32_t colorLength, int32_t offset,
    //             int32_t stride, const InitializationOptions &opts)
    int32_t srcWidth = 300;
    int32_t srcHeight = 300;
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 250;
    int32_t gap = 50;

    // Different offset and stride combinations
    struct OffsetStrideParams {
        int32_t offset;
        int32_t stride;
        int32_t dstWidth;
        int32_t dstHeight;
    };

    OffsetStrideParams params[] = {
        {0, srcWidth, 200, 200},      // No offset, full stride
        {50, srcWidth, 150, 150},     // With offset, full stride
        {0, srcWidth + 20, 200, 200}, // No offset, extra stride
        {30, srcWidth + 30, 150, 150} // Both offset and extra stride
    };
    int32_t count = sizeof(params) / sizeof(params[0]);

    std::vector<uint32_t> colors(srcWidth * srcHeight);
    for (int32_t i = 0; i < srcWidth * srcHeight; i++) {
        uint8_t r = static_cast<uint8_t>((i % srcWidth) * 255 / srcWidth);
        uint8_t g = static_cast<uint8_t>((i / srcWidth) * 255 / srcHeight);
        uint8_t b = static_cast<uint8_t>(255 - (i % srcWidth) * 255 / srcWidth);
        uint8_t a = 255;
        colors[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }

    int32_t column = 2;
    for (int32_t i = 0; i < count; i++) {
        Media::InitializationOptions opts;
        opts.size.width = params[i].dstWidth;
        opts.size.height = params[i].dstHeight;
        opts.pixelFormat = Media::PixelFormat::RGBA_8888;
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;

        std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(colors.data(), colors.size(),
            params[i].offset, params[i].stride, opts).release());
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
        canvasNode->SetBackgroundColor(0xFF404040);
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
 * @tc.name: RS_PIXELMAP_STATIC_CREATE_SOURCE_TEST
 * @tc.desc: Test PixelMap::Create from source PixelMap with multiple formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_CREATE_SOURCE_TEST)
{
    // Test Create(PixelMap &source, const InitializationOptions &opts)
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    Media::PixelFormat formats[] = {
        Media::PixelFormat::ARGB_8888,
        Media::PixelFormat::RGBA_8888,
        Media::PixelFormat::BGRA_8888,
        Media::PixelFormat::RGB_565,
        Media::PixelFormat::RGB_888,
        Media::PixelFormat::ALPHA_8,
        Media::PixelFormat::RGBA_F16
    };
    int32_t count = sizeof(formats) / sizeof(formats[0]);
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 200;
    int32_t gap = 50;
    int32_t column = 2;

    for (int32_t i = 0; i < count; i++) {
        Media::InitializationOptions opts;
        opts.size.width = 250;
        opts.size.height = 180;
        opts.pixelFormat = formats[i];
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        opts.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;

        std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(*testPixelMap_, opts).release());
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
 * @tc.name: RS_PIXELMAP_STATIC_CREATE_SOURCE_RECT_TEST
 * @tc.desc: Test PixelMap::Create from source with rect region and multiple formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_CREATE_SOURCE_RECT_TEST)
{
    // Test Create(PixelMap &source, const Rect &srcRect, const InitializationOptions &opts)
    if (!LoadTestImages() || testPixelMap_ == nullptr) {
        return;
    }

    int32_t originalWidth = testPixelMap_->GetWidth();
    int32_t originalHeight = testPixelMap_->GetHeight();

    Media::PixelFormat formats[] = {
        Media::PixelFormat::RGBA_8888,
        Media::PixelFormat::BGRA_8888,
        Media::PixelFormat::RGB_565,
        Media::PixelFormat::RGB_888,
        Media::PixelFormat::ALPHA_8
    };

    // Different crop regions
    Media::Rect cropRegions[] = {
        {0, 0, originalWidth / 2, originalHeight / 2},           // Top-left quarter
        {originalWidth / 2, 0, originalWidth / 2, originalHeight / 2},  // Top-right quarter
        {0, originalHeight / 2, originalWidth / 2, originalHeight / 2}, // Bottom-left quarter
        {originalWidth / 4, originalHeight / 4, originalWidth / 2, originalHeight / 2} // Center region
    };

    int32_t formatCount = sizeof(formats) / sizeof(formats[0]);
    int32_t rectCount = sizeof(cropRegions) / sizeof(cropRegions[0]);
    int32_t nodeWidth = 250;
    int32_t nodeHeight = 180;
    int32_t gap = 40;
    int32_t column = 2;

    int32_t index = 0;
    for (int32_t f = 0; f < formatCount; f++) {
        for (int32_t r = 0; r < rectCount; r++) {
            Media::InitializationOptions opts;
            opts.size.width = cropRegions[r].width;
            opts.size.height = cropRegions[r].height;
            opts.pixelFormat = formats[f];
            opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;

            std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(*testPixelMap_, cropRegions[r], opts).release());
            if (!pixelMap) {
                continue;
            }

            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            int32_t row = index / column;
            int32_t col = index % column;

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

            index++;
        }
    }
}

/*
 * @tc.name: RS_PIXELMAP_STATIC_CREATE_EMPTY_TEST
 * @tc.desc: Test PixelMap::Create with empty InitializationOptions
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_CREATE_EMPTY_TEST)
{
    // Test Create(const InitializationOptions &opts)
    Media::PixelFormat formats[] = {
        Media::PixelFormat::RGBA_8888,
        Media::PixelFormat::BGRA_8888,
        Media::PixelFormat::RGB_565,
        Media::PixelFormat::RGB_888,
        Media::PixelFormat::ALPHA_8,
        Media::PixelFormat::RGBA_F16
    };
    int32_t count = sizeof(formats) / sizeof(formats[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t i = 0; i < count; i++) {
        Media::InitializationOptions opts;
        opts.size.width = 200;
        opts.size.height = 150;
        opts.pixelFormat = formats[i];
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        opts.editable = true;

        std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts).release());
        if (!pixelMap) {
            continue;
        }

        // Fill with gradient pattern
        int32_t width = pixelMap->GetWidth();
        int32_t height = pixelMap->GetHeight();
        std::vector<uint32_t> colors(width * height);
        for (int32_t j = 0; j < width * height; j++) {
            uint8_t r = static_cast<uint8_t>((j % width) * 255 / width);
            uint8_t g = static_cast<uint8_t>((j / width) * 255 / height);
            uint8_t b = static_cast<uint8_t>(128);
            uint8_t a = 255;
            colors[j] = (a << 24) | (r << 16) | (g << 8) | b;
        }
        pixelMap->WritePixels(reinterpret_cast<const uint8_t*>(colors.data()),
            colors.size() * sizeof(uint32_t));

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
        canvasNode->SetBackgroundColor(0xFFE0E0E0);
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
 * @tc.name: RS_PIXELMAP_STATIC_CONVERT_ASTC_TEST
 * @tc.desc: Test PixelMap::ConvertFromAstc with multiple destination formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_CONVERT_ASTC_TEST)
{
    // Test static ConvertFromAstc(PixelMap *source, uint32_t &errorCode, PixelFormat destFormat)
    auto astcPixelMap = CreateAstcPixelMap(256, 256);
    if (!astcPixelMap) {
        return;
    }

    Media::PixelFormat destFormats[] = {
        Media::PixelFormat::RGBA_8888,
        Media::PixelFormat::BGRA_8888,
        Media::PixelFormat::RGB_565,
        Media::PixelFormat::RGB_888,
        Media::PixelFormat::ARGB_8888
    };
    int32_t count = sizeof(destFormats) / sizeof(destFormats[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t i = 0; i < count; i++) {
        uint32_t errorCode = 0;
        std::shared_ptr<Media::PixelMap> convertedPixelMap(Media::PixelMap::ConvertFromAstc(astcPixelMap.get(), errorCode, destFormats[i]).release());
        if (!convertedPixelMap) {
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
        canvasNode->SetBackgroundColor(0xFF202020);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            (gap + nodeWidth) * (col + 1),
            (gap + nodeHeight) * (row + 1)
        );
        drawing->DrawPixelMapWithParm(convertedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_STATIC_MARSHALLING_TEST
 * @tc.desc: Test PixelMap Marshalling and Unmarshalling with multiple formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_MARSHALLING_TEST)
{
    // Test static Unmarshalling and virtual Marshalling
    Media::PixelFormat formats[] = {
        Media::PixelFormat::RGBA_8888,
        Media::PixelFormat::BGRA_8888,
        Media::PixelFormat::RGB_565,
        Media::PixelFormat::RGB_888,
        Media::PixelFormat::ALPHA_8,
        Media::PixelFormat::RGBA_F16
    };
    int32_t count = sizeof(formats) / sizeof(formats[0]);
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 200;
    int32_t gap = 50;
    int32_t column = 2;

    for (int32_t i = 0; i < count; i++) {
        // Create original pixel map
        Media::InitializationOptions opts;
        opts.size.width = 200;
        opts.size.height = 150;
        opts.pixelFormat = formats[i];
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        opts.editable = true;

        std::vector<uint32_t> colors(opts.size.width * opts.size.height);
        for (size_t j = 0; j < colors.size(); j++) {
            uint8_t r = static_cast<uint8_t>((j % opts.size.width) * 255 / opts.size.width);
            uint8_t g = static_cast<uint8_t>((j / opts.size.width) * 255 / opts.size.height);
            uint8_t b = static_cast<uint8_t>(j % 256);
            uint8_t a = 255;
            colors[j] = (a << 24) | (r << 16) | (g << 8) | b;
        }

        std::shared_ptr<Media::PixelMap> originalPixelMap(Media::PixelMap::Create(colors.data(), colors.size(), opts).release());
        if (!originalPixelMap) {
            continue;
        }

        // Marshal to parcel
        Parcel parcel;
        if (!originalPixelMap->Marshalling(parcel)) {
            continue;
        }

        // Unmarshal from parcel
        Media::PIXEL_MAP_ERR error;
        std::shared_ptr<Media::PixelMap> unmarshalledPixelMap(Media::PixelMap::Unmarshalling(parcel, error));
        if (!unmarshalledPixelMap) {
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
        drawing->DrawPixelMapWithParm(unmarshalledPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_STATIC_DECODE_TLV_TEST
 * @tc.desc: Test PixelMap EncodeTlv and static DecodeTlv with multiple formats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_DECODE_TLV_TEST)
{
    // Test static DecodeTlv and virtual EncodeTlv
    Media::PixelFormat formats[] = {
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
        // Create original pixel map
        Media::InitializationOptions opts;
        opts.size.width = 200;
        opts.size.height = 150;
        opts.pixelFormat = formats[i];
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        opts.editable = true;

        std::vector<uint32_t> colors(opts.size.width * opts.size.height);
        for (size_t j = 0; j < colors.size(); j++) {
            uint8_t r = static_cast<uint8_t>(255 - (j % opts.size.width) * 255 / opts.size.width);
            uint8_t g = static_cast<uint8_t>((j % opts.size.width) * 255 / opts.size.width);
            uint8_t b = static_cast<uint8_t>((j / opts.size.width) * 255 / opts.size.height);
            uint8_t a = 255;
            colors[j] = (a << 24) | (r << 16) | (g << 8) | b;
        }

        auto originalPixelMap = Media::PixelMap::Create(colors.data(), colors.size(), opts);
        if (!originalPixelMap) {
            continue;
        }

        // Encode to TLV buffer
        std::vector<uint8_t> buffer;
        if (!originalPixelMap->EncodeTlv(buffer)) {
            continue;
        }

        // Decode from TLV buffer
        std::shared_ptr<Media::PixelMap> decodedPixelMap(Media::PixelMap::DecodeTlv(buffer));
        if (!decodedPixelMap) {
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
        drawing->DrawPixelMapWithParm(decodedPixelMap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_STATIC_HDR_FORMAT_CREATE_TEST
 * @tc.desc: Test PixelMap::Create with HDR formats (RGBA_1010102, RGBA_U16)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_HDR_FORMAT_CREATE_TEST)
{
    // Test Create with HDR pixel formats
    Media::PixelFormat hdrFormats[] = {
        Media::PixelFormat::RGBA_1010102,
        Media::PixelFormat::RGBA_U16
    };
    int32_t count = sizeof(hdrFormats) / sizeof(hdrFormats[0]);
    int32_t width = 200;
    int32_t height = 200;
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 200;
    int32_t gap = 50;
    int32_t column = 2;

    for (int32_t i = 0; i < count; i++) {
        Media::InitializationOptions opts;
        opts.size.width = width;
        opts.size.height = height;
        opts.pixelFormat = hdrFormats[i];
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        opts.editable = true;

        std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts).release());
        if (!pixelMap) {
            continue;
        }

        // Fill with gradient pattern (use appropriate data type for each format)
        int32_t w = pixelMap->GetWidth();
        int32_t h = pixelMap->GetHeight();

        if (hdrFormats[i] == Media::PixelFormat::RGBA_1010102) {
            // RGBA_1010102: 10 bits per color channel, 2 bits for alpha
            std::vector<uint32_t> colors(w * h);
            for (int32_t j = 0; j < w * h; j++) {
                uint16_t r = static_cast<uint16_t>((j % w) * 1023 / w);
                uint16_t g = static_cast<uint16_t>((j / w) * 1023 / h);
                uint16_t b = 512;
                uint16_t a = 3;
                colors[j] = (a << 30) | (b << 20) | (g << 10) | r;
            }
            pixelMap->WritePixels(reinterpret_cast<const uint8_t*>(colors.data()),
                colors.size() * sizeof(uint32_t));
        } else if (hdrFormats[i] == Media::PixelFormat::RGBA_U16) {
            // RGBA_U16: 16 bits per channel
            std::vector<uint16_t> colors(w * h * 4);
            for (int32_t j = 0; j < w * h; j++) {
                colors[j * 4 + 0] = static_cast<uint16_t>((j % w) * 65535 / w);     // R
                colors[j * 4 + 1] = static_cast<uint16_t>((j / w) * 65535 / h);     // G
                colors[j * 4 + 2] = 32768;                                         // B
                colors[j * 4 + 3] = 65535;                                         // A
            }
            pixelMap->WritePixels(reinterpret_cast<const uint8_t*>(colors.data()),
                colors.size() * sizeof(uint16_t));
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
        canvasNode->SetBackgroundColor(0xFF202020);
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
 * @tc.name: RS_PIXELMAP_STATIC_ASTC_FORMATS_TEST
 * @tc.desc: Test PixelMap::Create with different ASTC formats (ASTC_4x4, ASTC_6x6, ASTC_8x8)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_ASTC_FORMATS_TEST)
{
    // Test Create with different ASTC block size formats
    struct AstcFormatConfig {
        Media::PixelFormat format;
        size_t blockSizeX;
        size_t blockSizeY;
        const char* name;
    };

    AstcFormatConfig astcConfigs[] = {
        {Media::PixelFormat::ASTC_4x4, 4, 4, "ASTC_4x4"},
        {Media::PixelFormat::ASTC_6x6, 6, 6, "ASTC_6x6"},
        {Media::PixelFormat::ASTC_8x8, 8, 8, "ASTC_8x8"}
    };
    int32_t count = sizeof(astcConfigs) / sizeof(astcConfigs[0]);
    int32_t nodeWidth = 340;
    int32_t nodeHeight = 200;
    int32_t gap = 30;
    int32_t column = 3;

    for (int32_t i = 0; i < count; i++) {
        constexpr uint8_t ASTC_MAGIC_0 = 0x13;
        constexpr uint8_t ASTC_MAGIC_1 = 0xAB;
        constexpr uint8_t ASTC_MAGIC_2 = 0xA1;
        constexpr uint8_t ASTC_MAGIC_3 = 0x5C;
        constexpr uint8_t ASTC_1TH_BYTES = 8;
        constexpr uint8_t ASTC_2TH_BYTES = 16;
        constexpr uint8_t MASKBITS_FOR_8BIT = 255;
        constexpr uint8_t ASTC_PER_BLOCK_BYTES = 16;

        int32_t width = 256;
        int32_t height = 256;
        size_t blockSizeX = astcConfigs[i].blockSizeX;
        size_t blockSizeY = astcConfigs[i].blockSizeY;

        // Calculate block count
        size_t blockNum = ((width + blockSizeX - 1) / blockSizeX) * ((height + blockSizeY - 1) / blockSizeY);
        size_t dataSize = blockNum * ASTC_PER_BLOCK_BYTES + 16;

        // Allocate data buffer
        std::vector<uint8_t> data(dataSize);
        uint8_t* tmp = data.data();

        // Generate ASTC header
        *tmp++ = ASTC_MAGIC_0;
        *tmp++ = ASTC_MAGIC_1;
        *tmp++ = ASTC_MAGIC_2;
        *tmp++ = ASTC_MAGIC_3;
        *tmp++ = static_cast<uint8_t>(blockSizeX);
        *tmp++ = static_cast<uint8_t>(blockSizeY);
        *tmp++ = 1; // 3D block size (1 for 2D)
        *tmp++ = static_cast<uint8_t>(width & MASKBITS_FOR_8BIT);
        *tmp++ = static_cast<uint8_t>((width >> ASTC_1TH_BYTES) & MASKBITS_FOR_8BIT);
        *tmp++ = static_cast<uint8_t>((width >> ASTC_2TH_BYTES) & MASKBITS_FOR_8BIT);
        *tmp++ = static_cast<uint8_t>(height & MASKBITS_FOR_8BIT);
        *tmp++ = static_cast<uint8_t>((height >> ASTC_1TH_BYTES) & MASKBITS_FOR_8BIT);
        *tmp++ = static_cast<uint8_t>((height >> ASTC_2TH_BYTES) & MASKBITS_FOR_8BIT);
        *tmp++ = 1; // z dimension for 2D
        *tmp++ = 0;
        *tmp++ = 0;

        // Construct ASTC body (use same block data for all blocks)
        constexpr uint8_t ASTC_BLOCK_DATA[ASTC_PER_BLOCK_BYTES] = {
            0x43, 0x80, 0xE9, 0xE8, 0xFA, 0xFC, 0x14, 0x17, 0xFF, 0xFF, 0x81, 0x42, 0x12, 0x5A, 0xD4, 0xE9
        };
        uint8_t* astcBody = tmp;
        for (size_t blockIdx = 0; blockIdx < blockNum; blockIdx++) {
            memcpy(astcBody + blockIdx * ASTC_PER_BLOCK_BYTES, ASTC_BLOCK_DATA, ASTC_PER_BLOCK_BYTES);
        }

        // Create ImageSource from ASTC data
        uint32_t errorCode = 0;
        Media::SourceOptions opts;
        auto imageSource = Media::ImageSource::CreateImageSource(data.data(), dataSize,
            opts, errorCode);
        if (!imageSource) {
            continue;
        }

        Media::ImageInfo imageInfo;
        errorCode = imageSource->GetImageInfo(imageInfo);
        if (errorCode != 0) {
            continue;
        }

        // Create PixelMap from ImageSource
        Media::DecodeOptions decodeOptions;
        decodeOptions.desiredPixelFormat = astcConfigs[i].format;

        std::shared_ptr<Media::PixelMap> pixelMap(imageSource->CreatePixelMap(decodeOptions, errorCode).release());
        if (!pixelMap) {
            continue;
        }

        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
        auto imageInfo_rs = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

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
        canvasNode->SetBackgroundColor(0xFF303030);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(
            (gap + nodeWidth) * (col + 1),
            (gap + nodeHeight) * (row + 1)
        );
        drawing->DrawPixelMapWithParm(pixelMap, imageInfo_rs, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

/*
 * @tc.name: RS_PIXELMAP_STATIC_YUV_NV12_TEST
 * @tc.desc: Test PixelMap::Create with YUV NV12 format
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_YUV_NV12_TEST)
{
    // Test Create with YUV NV12 format
    int32_t width = 256;
    int32_t height = 256;
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 250;
    int32_t gap = 50;

    // NV12 format: Y plane + interleaved UV plane
    // Y plane: width * height
    // UV plane: width * height / 2 (interleaved)
    size_t ySize = width * height;
    size_t uvSize = width * height / 2;
    size_t totalSize = ySize + uvSize;

    std::vector<uint8_t> nv12Data(totalSize);

    // Fill Y plane with gradient
    uint8_t* yPlane = nv12Data.data();
    for (int32_t i = 0; i < ySize; i++) {
        yPlane[i] = static_cast<uint8_t>((i % width) * 255 / width);
    }

    // Fill UV plane with pattern
    uint8_t* uvPlane = nv12Data.data() + ySize;
    for (size_t i = 0; i < uvSize; i += 2) {
        uvPlane[i] = 128;     // U
        uvPlane[i + 1] = 128; // V
    }

    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.pixelFormat = Media::PixelFormat::NV12;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;

    std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts).release());
    if (!pixelMap) {
        return;
    }

    // Write YUV data
    pixelMap->WritePixels(nv12Data.data(), totalSize);

    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
    auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

    int32_t row = 0;
    int32_t col = 0;

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
    canvasNode->SetBackgroundColor(0xFF404040);
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

/*
 * @tc.name: RS_PIXELMAP_STATIC_YUV_NV21_TEST
 * @tc.desc: Test PixelMap::Create with YUV NV21 format
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSPixelMapDisplayTest, CONTENT_DISPLAY_TEST, RS_PIXELMAP_STATIC_YUV_NV21_TEST)
{
    // Test Create with YUV NV21 format
    int32_t width = 256;
    int32_t height = 256;
    int32_t nodeWidth = 300;
    int32_t nodeHeight = 250;
    int32_t gap = 50;

    // NV21 format: Y plane + interleaved VU plane
    // Y plane: width * height
    // VU plane: width * height / 2 (interleaved, V first)
    size_t ySize = width * height;
    size_t vuSize = width * height / 2;
    size_t totalSize = ySize + vuSize;

    std::vector<uint8_t> nv21Data(totalSize);

    // Fill Y plane with gradient
    uint8_t* yPlane = nv21Data.data();
    for (int32_t i = 0; i < ySize; i++) {
        yPlane[i] = static_cast<uint8_t>(255 - (i % width) * 255 / width);
    }

    // Fill VU plane with pattern
    uint8_t* vuPlane = nv21Data.data() + ySize;
    for (size_t i = 0; i < vuSize; i += 2) {
        vuPlane[i] = 128;     // V
        vuPlane[i + 1] = 128; // U
    }

    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.pixelFormat = Media::PixelFormat::NV21;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;

    std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts).release());
    if (!pixelMap) {
        return;
    }

    // Write YUV data
    pixelMap->WritePixels(nv21Data.data(), totalSize);

    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetImageFit(static_cast<int>(ImageFit::CONTAIN));
    auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

    int32_t row = 0;
    int32_t col = 0;

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
    canvasNode->SetBackgroundColor(0xFF404040);
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
>>>>>>> fe3fc6ce37 (add native Hdr pixelMap test)
} // namespace OHOS::Rosen
