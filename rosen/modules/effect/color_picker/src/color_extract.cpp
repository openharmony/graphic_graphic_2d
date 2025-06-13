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
#include "color_extract.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>
#include "hilog/log.h"
#include "effect_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using OHOS::HiviewDFX::HiLog;

ColorExtract::ColorExtract(std::shared_ptr<Media::PixelMap> pixmap)
{
    if (pixmap == nullptr) {
        EFFECT_LOG_I("[ColorExtract]failed to construct ColorExtract with null pixmap.");
        return;
    }
    pixelmap_ = pixmap;
    if (pixmap->GetWidth() <= 0 || pixmap->GetHeight() <= 0) {
        EFFECT_LOG_I("[ColorExtract]failed to construct ColorExtract with non-positive pixmap width or height.");
        return;
    }
    format_ = pixmap->GetPixelFormat();
    if (format_ == Media::PixelFormat::RGBA_1010102) {
        InitColorValBy1010102Color(pixmap, 0, 0, pixmap->GetWidth(), pixmap->GetHeight());
    } else {
        InitColorValBy8888Color(pixmap, 0, 0, pixmap->GetWidth(), pixmap->GetHeight());
    }
    grayMsd_ = CalcGrayMsd();
    contrastToWhite_ = CalcContrastToWhite();
    GetNFeatureColors(specifiedFeatureColorNum_);
}

ColorExtract::ColorExtract(std::shared_ptr<Media::PixelMap> pixmap, double* coordinates)
{
    if (pixmap == nullptr) {
        EFFECT_LOG_I("[ColorExtract]failed to construct ColorExtract with null pixmap.");
        return;
    }
    pixelmap_ = pixmap;
    if (pixmap->GetWidth() <= 0 || pixmap->GetHeight() <= 0) {
        EFFECT_LOG_I("[ColorExtract]failed to construct ColorExtract with non-positive pixmap width or height.");
        return;
    }
    uint32_t left = static_cast<uint32_t>(pixmap->GetWidth() * coordinates[0]); // 0 is index of left
    uint32_t top = static_cast<uint32_t>(pixmap->GetHeight() * coordinates[1]); // 1 is index of top
    uint32_t right = static_cast<uint32_t>(pixmap->GetWidth() * coordinates[2]); // 2 is index of right
    uint32_t bottom = static_cast<uint32_t>(pixmap->GetHeight() * coordinates[3]); // 3 is index of bottom
    format_ = pixmap->GetPixelFormat();
    if (format_ == Media::PixelFormat::RGBA_1010102) {
        InitColorValBy1010102Color(pixmap, left, top, right, bottom);
    } else {
        InitColorValBy8888Color(pixmap, left, top, right, bottom);
    }
    grayMsd_ = CalcGrayMsd();
    contrastToWhite_ = CalcContrastToWhite();
    GetNFeatureColors(specifiedFeatureColorNum_);
}

void ColorExtract::InitColorValBy1010102Color(std::shared_ptr<Media::PixelMap> pixmap, uint32_t left, uint32_t top,
    uint32_t right, uint32_t bottom)
{
    if (pixmap == nullptr) {
        EFFECT_LOG_E("[ColorExtract]failed to InitColorValBy1010102Color with null pixmap.");
        return;
    }
    colorValLen_ = (right - left) * (bottom -top);
    if (colorValLen_ <= 0) {
        return;
    }
    colorVal_.resize(colorValLen_);
    uint32_t realColorCnt = 0;
    for (uint32_t i = top; i < bottom; i++) {
        for (uint32_t j = left; j < right; j++) {
            uint32_t pixelColor;
            pixmap->GetRGBA1010102Color(j, i, pixelColor);
            if (GetRGBA1010102ColorA(pixelColor) != 0) {
                colorVal_.data()[realColorCnt] = pixelColor;
                realColorCnt++;
            }
        }
    }
    colorValLen_ = realColorCnt;
}

void ColorExtract::InitColorValBy8888Color(std::shared_ptr<Media::PixelMap> pixmap, uint32_t left, uint32_t top,
    uint32_t right, uint32_t bottom)
{
    if (pixmap == nullptr) {
        EFFECT_LOG_E("[ColorExtract]failed to InitColorValBy8888Color with null pixmap.");
        return;
    }
    colorValLen_ = (right - left) * (bottom -top);
    if (colorValLen_ <= 0) {
        return;
    }
    colorVal_.resize(colorValLen_);
    uint32_t realColorCnt = 0;
    for (uint32_t i = top; i < bottom; i++) {
        for (uint32_t j = left; j < right; j++) {
            uint32_t pixelColor;
            pixmap->GetARGB32Color(j, i, pixelColor);
            if (GetARGB32ColorA(pixelColor) != 0) {
                colorVal_.data()[realColorCnt] = pixelColor;
                realColorCnt++;
            }
        }
    }
    colorValLen_ = realColorCnt;
}

// Return red component of a quantized color
uint32_t ColorExtract::QuantizedRed(uint32_t color)
{
    return (color >> (QUANTIZE_WORD_WIDTH + QUANTIZE_WORD_WIDTH)) & static_cast<uint32_t>(QUANTIZE_WORD_MASK);
}

// Return green component of a quantized color
uint32_t ColorExtract::QuantizedGreen(uint32_t color)
{
    return (color >> QUANTIZE_WORD_WIDTH) & static_cast<uint32_t>(QUANTIZE_WORD_MASK);
}

// Return blue component of a quantized color
uint32_t ColorExtract::QuantizedBlue(uint32_t color)
{
    return color & static_cast<uint32_t>(QUANTIZE_WORD_MASK);
}

uint32_t ColorExtract::ModifyWordWidth(uint32_t color, int inWidth, int outWidth)
{
    uint32_t newValue;
    if (outWidth > inWidth) {
        newValue = color << (outWidth - inWidth);
    } else {
        newValue = color >> (inWidth - outWidth);
    }
    return newValue & ((1 << outWidth) - 1);
}

uint8_t ColorExtract::GetARGB32ColorA(unsigned int color)
{
    return (color >> ARGB_A_SHIFT) & ARGB_MASK;
}

uint8_t ColorExtract::GetARGB32ColorR(unsigned int color)
{
    return (color >> ARGB_R_SHIFT) & ARGB_MASK;
}

uint8_t ColorExtract::GetARGB32ColorG(unsigned int color)
{
    return (color >> ARGB_G_SHIFT) & ARGB_MASK;
}

uint8_t ColorExtract::GetARGB32ColorB(unsigned int color)
{
    return (color >> ARGB_B_SHIFT) & ARGB_MASK;
}

uint32_t ColorExtract::GetRGBA1010102ColorA(unsigned int color)
{
    return (color >> RGBA1010102_A_SHIFT) & RGBA1010102_ALPHA_MASK;
}

uint32_t ColorExtract::GetRGBA1010102ColorR(unsigned int color)
{
    return (color >> RGBA1010102_R_SHIFT) & RGBA1010102_RGB_MASK;
}

uint32_t ColorExtract::GetRGBA1010102ColorG(unsigned int color)
{
    return (color >> RGBA1010102_G_SHIFT) & RGBA1010102_RGB_MASK;
}

uint32_t ColorExtract::GetRGBA1010102ColorB(unsigned int color)
{
    return (color >> RGBA1010102_B_SHIFT) & RGBA1010102_RGB_MASK;
}

uint32_t ColorExtract::QuantizeFromRGB888(uint32_t color)
{
    uint32_t r = ModifyWordWidth(GetARGB32ColorR(color), 8, QUANTIZE_WORD_WIDTH);
    uint32_t g = ModifyWordWidth(GetARGB32ColorG(color), 8, QUANTIZE_WORD_WIDTH);
    uint32_t b = ModifyWordWidth(GetARGB32ColorB(color), 8, QUANTIZE_WORD_WIDTH);
    return (r << (QUANTIZE_WORD_WIDTH + QUANTIZE_WORD_WIDTH)) | (g << QUANTIZE_WORD_WIDTH) | b;
}

uint32_t ColorExtract::QuantizeFromRGB101010(uint32_t color)
{
    uint32_t r = ModifyWordWidth(GetRGBA1010102ColorR(color), 10, QUANTIZE_WORD_WIDTH);
    uint32_t g = ModifyWordWidth(GetRGBA1010102ColorG(color), 10, QUANTIZE_WORD_WIDTH);
    uint32_t b = ModifyWordWidth(GetRGBA1010102ColorB(color), 10, QUANTIZE_WORD_WIDTH);
    return (r << (QUANTIZE_WORD_WIDTH + QUANTIZE_WORD_WIDTH)) | (g << QUANTIZE_WORD_WIDTH) | b;
}

uint32_t ColorExtract::ApproximateToRGB888(uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t approximatedRed = ModifyWordWidth(r, QUANTIZE_WORD_WIDTH, 8);
    uint32_t approximatedGreen = ModifyWordWidth(g, QUANTIZE_WORD_WIDTH, 8);
    uint32_t approximatedBlue = ModifyWordWidth(b, QUANTIZE_WORD_WIDTH, 8);
    return (approximatedRed << ARGB_R_SHIFT) | (approximatedGreen << ARGB_G_SHIFT) | (approximatedBlue << ARGB_B_SHIFT);
}

uint32_t ColorExtract::ApproximateToRGB888(uint32_t color)
{
    return ApproximateToRGB888(QuantizedRed(color), QuantizedGreen(color), QuantizedBlue(color));
}

std::vector<std::pair<uint32_t, uint32_t>> ColorExtract::QuantizePixels(int colorNum)
{
    // Create a priority queue which is sorted by volume descending.
    std::priority_queue<VBox, std::vector<VBox>, std::less<VBox> > q;
    VBox v(0, distinctColorCount_ - 1, this);
    q.push(v);
    SplitBoxes(q, colorNum);
    return GenerateAverageColors(q);
}


void ColorExtract::SplitBoxes(std::priority_queue<VBox, std::vector<VBox>, std::less<VBox> > &queue, int maxSize)
{
    while (queue.size() < static_cast<uint32_t>(maxSize)) {
        VBox vBox = queue.top();
        queue.pop();
        if (vBox.CanSplit()) {
            queue.push(vBox.SplitBox());
            queue.push(vBox);
        } else {
            return;
        }
    }
    return;
}

bool ColorExtract::cmp(std::pair<uint32_t, uint32_t>&a, std::pair<uint32_t, uint32_t>&b) {return a.second > b.second;}

std::vector<std::pair<uint32_t, uint32_t>> ColorExtract::GenerateAverageColors(std::priority_queue<VBox, \
                                            std::vector<VBox>, std::less<VBox> > &queue)
{
    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    while (!queue.empty()) {
        VBox vBox = queue.top();
        queue.pop();
        featureColors.push_back(vBox.GetAverageColor());
    }
    return  featureColors;
}

void ColorExtract::SetFeatureColorNum(int N)
{
    specifiedFeatureColorNum_ = N;
    GetNFeatureColors(N);
    return;
}

uint8_t ColorExtract::Rgb2Gray(uint32_t color, Media::PixelFormat format)
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
    if (format == Media::PixelFormat::RGBA_1010102) {
        r = GetRGBA1010102ColorR(color);
        g = GetRGBA1010102ColorG(color);
        b = GetRGBA1010102ColorB(color);
    } else {
        r = GetARGB32ColorR(color);
        g = GetARGB32ColorG(color);
        b = GetARGB32ColorB(color);
    }
    return static_cast<uint8_t>(r * RED_GRAY_RATIO + g * GREEN_GRAY_RATIO + b * BLUE_GRAY_RATIO);
}

uint32_t ColorExtract::CalcGrayMsd() const
{
    if (colorValLen_ == 0) {
        return 0;
    }
    uint32_t *colorVal = const_cast<uint32_t *>(colorVal_.data());
    long long int graySum = 0;
    long long int grayVar = 0;
    for (uint32_t i = 0; i < colorValLen_; i++) {
        graySum += Rgb2Gray(colorVal[i]);
    }
    uint32_t grayAve = graySum / colorValLen_;
    for (uint32_t i = 0; i < colorValLen_; i++) {
        grayVar += pow(static_cast<long long int>(Rgb2Gray(colorVal[i])) - grayAve, 2); // 2 is square
    }
    grayVar /= colorValLen_;
    return static_cast<uint32_t>(grayVar);
}

float ColorExtract::NormalizeRgb(uint32_t val, const uint32_t& colorMax)
{
    if (colorMax == 0) {
        return 0.0f;
    }
    float res = static_cast<float>(val) / colorMax;
    if (res <= 0.03928) { // 0.03928 is used to normalize rgb;
        res /= 12.92; // 12.92 is used to normalize rgb;
    } else {
        res = pow((res + 0.055) / 1.055, 2.4); // 0.055, 1.055, 2.4 is used to normalize rgb;
    }
    return res;
}

float ColorExtract::CalcRelativeLum(uint32_t color, Media::PixelFormat format)
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
    float rInFloat;
    float gInFloat;
    float bInFloat;
    if (format == Media::PixelFormat::RGBA_1010102) {
        r = GetRGBA1010102ColorR(color);
        g = GetRGBA1010102ColorG(color);
        b = GetRGBA1010102ColorB(color);
        rInFloat = NormalizeRgb(r, RGB101010_COLOR_MAX);
        gInFloat = NormalizeRgb(g, RGB101010_COLOR_MAX);
        bInFloat = NormalizeRgb(b, RGB101010_COLOR_MAX);
    } else {
        r = GetARGB32ColorR(color);
        g = GetARGB32ColorG(color);
        b = GetARGB32ColorB(color);
        rInFloat = NormalizeRgb(r, RGB888_COLOR_MAX);
        gInFloat = NormalizeRgb(g, RGB888_COLOR_MAX);
        bInFloat = NormalizeRgb(b, RGB888_COLOR_MAX);
    }
    return rInFloat * RED_LUMINACE_RATIO + gInFloat * GREEN_LUMINACE_RATIO + bInFloat * BLUE_LUMINACE_RATIO;
}

float ColorExtract::CalcContrastToWhite() const
{
    if (colorValLen_ == 0) {
        return 0.0;
    }
    uint32_t *colorVal = const_cast<uint32_t *>(colorVal_.data());
    float lightDegree = 0;
    float luminanceSum = 0;
    for (uint32_t i = 0; i < colorValLen_; i++) {
        luminanceSum += CalcRelativeLum(colorVal[i], format_);
    }
    float luminanceAve = luminanceSum / colorValLen_;
     // 0.05 is used to ensure denominator is not 0;
    lightDegree = (1 + 0.05) / (luminanceAve + 0.05);
    return lightDegree;
}

void ColorExtract::GetNFeatureColors(int colorNum)
{
    if (colorValLen_ == 0) {
        return;
    }
    uint32_t *colorVal = const_cast<uint32_t *>(colorVal_.data());
    uint32_t histLen = (1 << (QUANTIZE_WORD_WIDTH * 3));
    hist_.resize(histLen);
    uint32_t *hist = hist_.data();
    if (format_ == Media::PixelFormat::RGBA_1010102) {
        for (uint32_t i = 0; i < colorValLen_; i++) {
            uint32_t quantizedColor = QuantizeFromRGB101010(colorVal[i]);
            hist[quantizedColor]++;
        }
    } else {
        for (uint32_t i = 0; i < colorValLen_; i++) {
            uint32_t quantizedColor = QuantizeFromRGB888(colorVal[i]);
            hist[quantizedColor]++;
        }
    }

    for (uint32_t color = 0; color < histLen; color++) {
        if (hist[color] > 0) {
            distinctColorCount_++;
        }
    }

    colors_.resize(distinctColorCount_);
    uint32_t *colors = colors_.data();
    int distinctColorIndex = 0;
    for (uint32_t color = 0; color < histLen; color++) {
        if (hist[color] > 0) {
            colors[distinctColorIndex++] = color;
        }
    }
    if (distinctColorCount_ < colorNum) {
        //The picture has fewer colors than the maximum requested, just return the colors.
        for (int i = 0; i < distinctColorCount_; ++i) {
            std::pair<uint32_t, uint32_t> featureColor = \
                    std::make_pair(ApproximateToRGB888(colors[i]), hist[colors[i]]);
            featureColors_.push_back(featureColor);
        }
    } else {
        // Use quantization to reduce the number of color.
        featureColors_ = QuantizePixels(colorNum);
    }
    // Sort colors from more to less.
    sort(featureColors_.begin(), featureColors_.end(), cmp);
    return;
}

} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
