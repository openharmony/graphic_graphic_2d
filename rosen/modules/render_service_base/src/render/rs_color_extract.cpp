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
#include "render/rs_color_extract.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>

namespace OHOS {
namespace Rosen {

RSColorExtract::RSColorExtract(std::shared_ptr<Drawing::Pixmap> pixmap)
{
    if (pixmap == nullptr) {
        return ;
    }
    pixelmap_ = pixmap;
    colorValLen_ = static_cast<uint32_t>(pixmap->GetWidth() * pixmap->GetHeight());
    auto colorVal = new uint32_t[colorValLen_]();
    std::shared_ptr<uint32_t> colorShared(colorVal, [](uint32_t *ptr) {
        delete[] ptr;
    });
    colorVal_ = std::move(colorShared);
    for (int i = 0; i < pixmap->GetHeight(); i++) {
        for (int j = 0; j < pixmap->GetWidth(); j++) {
            colorVal[i * pixmap->GetWidth() + j] = pixmap->GetColor(j, i);
        }
    }
    GetNFeatureColors(specifiedFeatureColorNum_);
}

RSColorExtract::RSColorExtract(std::shared_ptr<Drawing::Pixmap> pixmap, double* coordinates)
{
    if (pixmap == nullptr) {
        return;
    }
    pixelmap_ = pixmap;
    uint32_t left = static_cast<uint32_t>(pixmap->GetWidth() * coordinates[0]); // 0 is index of left
    uint32_t top = static_cast<uint32_t>(pixmap->GetHeight() * coordinates[1]); // 1 is index of top
    uint32_t right = static_cast<uint32_t>(pixmap->GetWidth() * coordinates[2]); // 2 is index of right
    uint32_t bottom = static_cast<uint32_t>(pixmap->GetHeight() * coordinates[3]); // 3 is index of bottom
    colorValLen_ = (right - left) * (bottom -top);
    if (colorValLen_ <= 0) {
        return;
    }
    auto colorVal = new uint32_t[colorValLen_]();
    std::shared_ptr<uint32_t> colorShared(colorVal, [](uint32_t *ptr) {
        delete[] ptr;
    });
    colorVal_ = std::move(colorShared);
    for (uint32_t i = top; i < bottom; i++) {
        for (uint32_t j = left; j < right; j++) {
            colorVal[(i - top) * (right - left) + (j - left)] = pixmap->GetColor(j, i);
        }
    }
    GetNFeatureColors(specifiedFeatureColorNum_);
}

// Return red component of a quantized color
uint32_t RSColorExtract::QuantizedRed(uint32_t color)
{
    return (color >> (QUANTIZE_WORD_WIDTH + QUANTIZE_WORD_WIDTH)) & static_cast<uint32_t>(QUANTIZE_WORD_MASK);
}

// Return green component of a quantized color
uint32_t RSColorExtract::QuantizedGreen(uint32_t color)
{
    return (color >> QUANTIZE_WORD_WIDTH) & static_cast<uint32_t>(QUANTIZE_WORD_MASK);
}

// Return blue component of a quantized color
uint32_t RSColorExtract::QuantizedBlue(uint32_t color)
{
    return color & static_cast<uint32_t>(QUANTIZE_WORD_MASK);
}

uint32_t RSColorExtract::ModifyWordWidth(uint8_t color, int inWidth, int outWidth)
{
    uint32_t newValue;
    if (outWidth > inWidth) {
        newValue = color << (outWidth - inWidth);
    } else {
        newValue = color >> (inWidth - outWidth);
    }
    return newValue & ((1 << outWidth) - 1);
}

uint8_t RSColorExtract::GetARGB32ColorR(unsigned int color)
{
    return (color >> ARGB_R_SHIFT) & ARGB_MASK;
}
uint8_t RSColorExtract::GetARGB32ColorG(unsigned int color)
{
    return (color >> ARGB_G_SHIFT) & ARGB_MASK;
}
uint8_t RSColorExtract::GetARGB32ColorB(unsigned int color)
{
    return (color >> ARGB_B_SHIFT) & ARGB_MASK;
}


uint32_t RSColorExtract::QuantizeFromRGB888(uint32_t color)
{
    uint32_t r = ModifyWordWidth(GetARGB32ColorR(color), 8, QUANTIZE_WORD_WIDTH);
    uint32_t g = ModifyWordWidth(GetARGB32ColorG(color), 8, QUANTIZE_WORD_WIDTH);
    uint32_t b = ModifyWordWidth(GetARGB32ColorB(color), 8, QUANTIZE_WORD_WIDTH);
    return (r << (QUANTIZE_WORD_WIDTH + QUANTIZE_WORD_WIDTH)) | (g << QUANTIZE_WORD_WIDTH) | b;
}


uint32_t RSColorExtract::ApproximateToRGB888(uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t approximatedRed = ModifyWordWidth(r, QUANTIZE_WORD_WIDTH, 8);
    uint32_t approximatedGreen = ModifyWordWidth(g, QUANTIZE_WORD_WIDTH, 8);
    uint32_t approximatedBlue = ModifyWordWidth(b, QUANTIZE_WORD_WIDTH, 8);
    return (approximatedRed << ARGB_R_SHIFT) | (approximatedGreen << ARGB_G_SHIFT) | (approximatedBlue << ARGB_B_SHIFT);
}

uint32_t RSColorExtract::ApproximateToRGB888(uint32_t color)
{
    return ApproximateToRGB888(QuantizedRed(color), QuantizedGreen(color), QuantizedBlue(color));
}


std::vector<std::pair<uint32_t, uint32_t>> RSColorExtract::QuantizePixels(int colorNum)
{
    // Create a priority queue which is sorted by volume descending.
    std::priority_queue<VBox, std::vector<VBox>, std::less<VBox> > q;
    VBox v(0, distinctColorCount_ - 1, this);
    q.push(v);
    SplitBoxes(q, colorNum);
    return GenerateAverageColors(q);
}


void RSColorExtract::SplitBoxes(std::priority_queue<VBox, std::vector<VBox>, std::less<VBox> > &queue, int maxSize)
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

bool RSColorExtract::cmp(std::pair<uint32_t, uint32_t> &a, std::pair<uint32_t, uint32_t> &b)
{
    return a.second > b.second;
}

std::vector<std::pair<uint32_t, uint32_t>> RSColorExtract::GenerateAverageColors(std::priority_queue<VBox,
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

void RSColorExtract::SetFeatureColorNum(int N)
{
    specifiedFeatureColorNum_ = N;
    GetNFeatureColors(N);
    return;
}

uint8_t RSColorExtract::Rgb2Gray(uint32_t color)
{
    uint32_t r = GetARGB32ColorR(color);
    uint32_t g = GetARGB32ColorG(color);
    uint32_t b = GetARGB32ColorB(color);
    return static_cast<uint8_t>(r * RED_GRAY_RATIO + g * GREEN_GRAY_RATIO + b * BLUE_GRAY_RATIO);
}

uint32_t RSColorExtract::CalcGrayMsd() const
{
    if (colorValLen_ == 0) {
        return 0;
    }
    uint32_t *colorVal = colorVal_.get();
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

float RSColorExtract::NormalizeRgb(uint32_t val)
{
    float res = static_cast<float>(val) / 255;
    if (res <= 0.03928) { // 0.03928 is used to normalize rgb;
        res /= 12.92; // 12.92 is used to normalize rgb;
    } else {
        res = pow((res + 0.055) / 1.055, 2.4); // 0.055, 1.055, 2.4 is used to normalize rgb;
    }
    return res;
}

float RSColorExtract::CalcRelativeLum(uint32_t color)
{
    uint32_t r = GetARGB32ColorR(color);
    uint32_t g = GetARGB32ColorG(color);
    uint32_t b = GetARGB32ColorB(color);
    float R = NormalizeRgb(r);
    float G = NormalizeRgb(g);
    float B = NormalizeRgb(b);
    return R * RED_LUMINACE_RATIO + G * GREEN_LUMINACE_RATIO + B * BLUE_LUMINACE_RATIO;
}

float RSColorExtract::CalcContrastToWhite() const
{
    if (colorValLen_ == 0) {
        return 0.0;
    }
    uint32_t *colorVal = colorVal_.get();
    float lightDegree = 0;
    float luminanceSum = 0;
    for (uint32_t i = 0; i < colorValLen_; i++) {
        luminanceSum += CalcRelativeLum(colorVal[i]);
    }
    float luminanceAve = luminanceSum / colorValLen_;
     // 0.05 is used to ensure denominator is not 0;
    lightDegree = (1 + 0.05) / (luminanceAve + 0.05);
    return lightDegree;
}

void RSColorExtract::GetNFeatureColors(int colorNum)
{
    if (colorValLen_ == 0) {
        return;
    }
    uint32_t *colorVal = colorVal_.get();
    uint32_t histLen = (1 << (QUANTIZE_WORD_WIDTH * 3)); // 3 means left shift flag
    auto hist = new uint32_t[histLen]();
    std::shared_ptr<uint32_t> histShared(hist, [](uint32_t *ptr) {
        delete[] ptr;
    });
    hist_ = move(histShared);
    for (uint32_t i = 0; i < colorValLen_; i++) {
        uint32_t quantizedColor = QuantizeFromRGB888(colorVal[i]);
        hist[quantizedColor]++;
    }

    for (uint32_t color = 0; color < histLen; color++) {
        if (hist[color] > 0) {
            distinctColorCount_++;
        }
    }

    // Create an array consisting of only distinct colors
    auto colors = new uint32_t[distinctColorCount_]();
    std::shared_ptr<uint32_t> colorsShared(colors, [](uint32_t *ptr) {
        delete[] ptr;
    });
    colors_ = move(colorsShared);

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
