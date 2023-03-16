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
#include <iostream>
#include <vector>
#include <memory>
#include "hilog/log.h"


#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using OHOS::HiviewDFX::HiLog;

ColorExtract::ColorExtract(std::shared_ptr<Media::PixelMap> pixmap)
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
            pixmap->GetARGB32Color(j, i, colorVal[i * pixmap->GetWidth() + j]);
        }
    }
    GetNFeatureColors(specifiedFeatureColorNum_);
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

uint32_t ColorExtract::ModifyWordWidth(uint8_t color, int inWidth, int outWidth)
{
    uint32_t newValue;
    if (outWidth > inWidth) {
        newValue = color << (outWidth - inWidth);
    } else {
        newValue = color >> (inWidth - outWidth);
    }
    return newValue & ((1 << outWidth) - 1);
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


uint32_t ColorExtract::QuantizeFromRGB888(uint32_t color)
{
    uint32_t r = ModifyWordWidth(GetARGB32ColorR(color), 8, QUANTIZE_WORD_WIDTH);
    uint32_t g = ModifyWordWidth(GetARGB32ColorG(color), 8, QUANTIZE_WORD_WIDTH);
    uint32_t b = ModifyWordWidth(GetARGB32ColorB(color), 8, QUANTIZE_WORD_WIDTH);
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

void ColorExtract::GetNFeatureColors(int colorNum)
{
    uint32_t *colorVal = colorVal_.get();
    int histLen = (1 << (QUANTIZE_WORD_WIDTH * 3));
    auto hist = new uint32_t[histLen]();
    std::shared_ptr<uint32_t> histShared(hist, [](uint32_t *ptr) {
        delete[] ptr;
    });
    hist_ = move(histShared);
    for (uint32_t i = 0; i < colorValLen_; i++) {
        uint32_t quantizedColor = QuantizeFromRGB888(colorVal[i]);
        hist[quantizedColor]++;
    }

    for (int color = 0; color < histLen; color++) {
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

#ifdef __cplusplus
}
#endif
