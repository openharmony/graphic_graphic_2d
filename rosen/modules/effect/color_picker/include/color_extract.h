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
#include <cstdint>
#include <vector>
#include <utility>
#include <memory>
#include <limits>
#include <algorithm>
#include <math.h>
#include <queue>
#include "pixel_map.h"
#include "effect_type.h"


#ifndef COLOREXTRACT_H
#define COLOREXTRACT_H

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace ColorManager {
class Color;
}
namespace Rosen {
    
class ColorExtract {
public:
    virtual ~ColorExtract() {}
    std::shared_ptr<Media::PixelMap> pixelmap_;

    // Save the ARGB val of picture.
    std::vector<uint32_t> colorVal_;
    uint32_t colorValLen_ = 0;
    uint32_t grayMsd_ = 0;
    float contrastToWhite_ = 0;

    // The first element is color, the second element is pixel num of this color.
    std::vector<std::pair<uint32_t, uint32_t>> featureColors_;

    // Specified number of extracted theme colors, default value is 10;
    int specifiedFeatureColorNum_ = 10;

    std::vector<uint32_t> hist_;
    int distinctColorCount_ = 0;
    std::vector<uint32_t> colors_;
    Media::PixelFormat format_ = Media::PixelFormat::RGBA_8888;

    static constexpr uint8_t ARGB_MASK = 0xFF;
    static constexpr uint8_t ARGB_A_SHIFT = 24;
    static constexpr uint8_t ARGB_R_SHIFT = 16;
    static constexpr uint8_t ARGB_G_SHIFT = 8;
    static constexpr uint8_t ARGB_B_SHIFT = 0;

    static constexpr uint32_t RGBA1010102_RGB_MASK = 0x3FF; // Mask corresponding to the 10-bit color value
    static constexpr uint32_t RGBA1010102_ALPHA_MASK = 0x03; // Mask corresponding to the 2-bit alpha value
    static constexpr uint32_t RGBA1010102_A_SHIFT = 30; // 1010102 format alpha shift number
    static constexpr uint32_t RGBA1010102_R_SHIFT = 0; // 1010102 format red shift number
    static constexpr uint32_t RGBA1010102_G_SHIFT = 10; // 1010102 format green shift number
    static constexpr uint32_t RGBA1010102_B_SHIFT = 20; // 1010102 format blue shift number

    static constexpr uint8_t RGB888_COLOR_MAX = 255; // Maximum value corresponding to 8-bit color
    static constexpr uint8_t RGB101010_ALPHA_MAX = 3; // Maximum value corresponding to 2-bit alpha
    static constexpr uint32_t RGB101010_COLOR_MAX = 1023; // Maximum value corresponding to 10-bit color

    static uint8_t GetARGB32ColorA(unsigned int color);
    static uint8_t GetARGB32ColorR(unsigned int color);
    static uint8_t GetARGB32ColorG(unsigned int color);
    static uint8_t GetARGB32ColorB(unsigned int color);
    static uint32_t GetRGBA1010102ColorA(unsigned int color);
    static uint32_t GetRGBA1010102ColorR(unsigned int color);
    static uint32_t GetRGBA1010102ColorG(unsigned int color);
    static uint32_t GetRGBA1010102ColorB(unsigned int color);
    NATIVEEXPORT void SetFeatureColorNum(int N);
    void GetNFeatureColors(int colorNum);

protected:
    ColorExtract(std::shared_ptr<Media::PixelMap> pixmap);
    ColorExtract(std::shared_ptr<Media::PixelMap> pixmap, double* coordinates);

private:

    static constexpr int COMPONENT_RED = -3;
    static constexpr int COMPONENT_GREEN = -2;
    static constexpr int COMPONENT_BLUE = -1;
    static constexpr int QUANTIZE_WORD_WIDTH = 5;
    static constexpr int QUANTIZE_WORD_MASK = (1 << QUANTIZE_WORD_WIDTH) - 1;
    static uint32_t QuantizedRed(uint32_t color);
    static uint32_t QuantizedGreen(uint32_t color);
    static uint32_t QuantizedBlue(uint32_t color);
    static uint32_t QuantizeFromRGB888(uint32_t color);
    static uint32_t QuantizeFromRGB101010(uint32_t color);
    static uint32_t ModifyWordWidth(uint32_t color, int inWidth, int outWidth);
    static uint32_t ApproximateToRGB888(uint32_t r, uint32_t g, uint32_t b);
    static uint32_t ApproximateToRGB888(uint32_t color);
    static bool cmp(std::pair<uint32_t, uint32_t >& a, std::pair<uint32_t, uint32_t >& b);

    class VBox {
    private:
        int lowerIndex_ = INT_MAX;
        int upperIndex_ = 0;
        // total pixel nums in this box;
        int pixelNums_ = 0;
        uint32_t minRed_ = UINT32_MAX;
        uint32_t maxRed_ = 0;
        uint32_t minGreen_ = UINT32_MAX;
        uint32_t maxGreen_ = 0;
        uint32_t minBlue_ = UINT32_MAX;
        uint32_t maxBlue_ = 0;
        ColorExtract* colorExtract_ = nullptr;

    public:
        VBox(int lowerIndex, int upperIndex, ColorExtract* colorExtract)
        {
            lowerIndex_ = lowerIndex;
            upperIndex_ = upperIndex;
            colorExtract_ = colorExtract;
            fitBox();
        }

        uint32_t GetVolume() const
        {
            return (maxRed_ - minRed_ + 1) * (maxGreen_ - minGreen_ + 1) * (maxBlue_ - minBlue_ + 1);
        }

        bool CanSplit()
        {
            return GetColorCount() > 1;
        }

        int GetColorCount() const
        {
            return 1 + upperIndex_ - lowerIndex_;
        }

        bool operator < (const VBox &v) const
        {
            return this->GetVolume() < v.GetVolume();
        }

        // Recomputes the boundaries of this box to tightly fit the color within the box.
        void fitBox()
        {
            if (colorExtract_ == nullptr) {
                return;
            }
            uint32_t* colors = colorExtract_->colors_.data();
            uint32_t* hist = colorExtract_->hist_.data();
            if (colors == nullptr || hist == nullptr) {
                return;
            }

            uint32_t minR = UINT32_MAX;
            uint32_t minG = UINT32_MAX;
            uint32_t minB = UINT32_MAX;
            uint32_t maxRed = 0;
            uint32_t maxGreen = 0;
            uint32_t maxBlue = 0;

            int count = 0;

            for (int i = lowerIndex_; i <= upperIndex_; i++) {
                uint32_t color = colors[i];
                count += static_cast<int>(hist[color]);

                uint32_t r = QuantizedRed(color);
                uint32_t g = QuantizedGreen(color);
                uint32_t b = QuantizedBlue(color);
                if (r > maxRed) {
                    maxRed = r;
                }
                if (r < minR) {
                    minR = r;
                }
                if (g > maxGreen) {
                    maxGreen = g;
                }
                if (g < minG) {
                    minG = g;
                }
                if (b > maxBlue) {
                    maxBlue = b;
                }
                if (b < minB) {
                    minB = b;
                }
            }
            minRed_ = minR;
            maxRed_ = maxRed;
            minGreen_ = minG;
            maxGreen_ = maxGreen;
            minBlue_ = minB;
            maxBlue_ = maxBlue;
            pixelNums_ = count;
        }

        // Split the color box at the mid-point along its longest dimension
        VBox SplitBox()
        {
            int splitPoint = FindSplitPoint();

            VBox newBox = VBox(splitPoint + 1, upperIndex_, colorExtract_);
            upperIndex_ = splitPoint;
            fitBox();
            return newBox;
        }

        // return the longest dimension of the box
        int GetLongestColorDimension()
        {
            uint32_t redLen = maxRed_ - minRed_;
            uint32_t greenLen = maxGreen_ - minGreen_;
            uint32_t blueLen = maxBlue_ - minBlue_;

            if (redLen >=greenLen && redLen >= blueLen) {
                return COMPONENT_RED;
            } else if (greenLen >= redLen && greenLen >= blueLen) {
                return COMPONENT_GREEN;
            } else {
                return COMPONENT_BLUE;
            }
        }

        int FindSplitPoint()
        {
            int longestDimension = GetLongestColorDimension();
            uint32_t* colors = colorExtract_->colors_.data();
            uint32_t* hist = colorExtract_->hist_.data();

            // Sort the color in the box based on the longest color dimension
            ModifySignificantOctet(colors, longestDimension, lowerIndex_, upperIndex_);
            std::sort(colors + lowerIndex_, colors + upperIndex_ + 1);
            // Revert all of the colors so that they are packed as RGB again
            ModifySignificantOctet(colors, longestDimension, lowerIndex_, upperIndex_);

            int midPoint = pixelNums_ / 2;
            for (int i = lowerIndex_, count = 0; i <= upperIndex_; i++) {
                count += static_cast<int>(hist[colors[i]]);
                if (count >= midPoint) {
                    return std::min(upperIndex_ - 1, i);
                }
            }

            return lowerIndex_;
        }

        /**
        * Modify the significant octet in a packed color int. Allows sorting based on the value of a
        * single color component.
        */
        void ModifySignificantOctet(uint32_t* colors, int dimension, int lower, int upper)
        {
            switch (dimension) {
                case COMPONENT_RED:
                    break;
                case COMPONENT_GREEN:
                    for (int i = lower; i <= upper ; i++) {
                        uint32_t color = colors[i];
                        colors[i] = (QuantizedGreen(color) << (QUANTIZE_WORD_WIDTH + QUANTIZE_WORD_WIDTH))
                                    | (QuantizedRed(color) << QUANTIZE_WORD_WIDTH)
                                    | QuantizedBlue(color);
                    }
                    break;
                case COMPONENT_BLUE:
                    for (int i = lower; i <= upper ; i++) {
                        uint32_t color = colors[i];
                        colors[i] = (QuantizedBlue(color) << (QUANTIZE_WORD_WIDTH + QUANTIZE_WORD_WIDTH))
                                    | (QuantizedGreen(color) << QUANTIZE_WORD_WIDTH)
                                    | QuantizedRed(color);
                    }
                    break;
            }
        }

        // Return the average color of the box
        std::pair<uint32_t, uint32_t> GetAverageColor()
        {
            uint32_t error_color = 0;
            if (colorExtract_ == nullptr) {
                return std::make_pair(error_color, error_color);
            }
            uint32_t* colors = colorExtract_->colors_.data();
            uint32_t* hist = colorExtract_->hist_.data();
            if (colors == nullptr || hist == nullptr) {
                return std::make_pair(error_color, error_color);
            }
            uint32_t redSum = 0;
            uint32_t greenSum = 0;
            uint32_t blueSum = 0;
            uint32_t totalPixelNum = 0;
            for (int i = lowerIndex_; i <=upperIndex_ ; i++) {
                uint32_t color = colors[i];
                uint32_t colorPixelNum = hist[color];
                totalPixelNum += colorPixelNum;
                redSum += colorPixelNum * QuantizedRed(color);
                greenSum += colorPixelNum * QuantizedGreen(color);
                blueSum += colorPixelNum * QuantizedBlue(color);
            }
            if (totalPixelNum == 0) {
                return std::make_pair(error_color, error_color);
            }
            uint32_t redMean = round(redSum / (float)totalPixelNum);
            uint32_t greenMean = round(greenSum / (float)totalPixelNum);
            uint32_t blueMean = round(blueSum / (float)totalPixelNum);

            return std::make_pair(ApproximateToRGB888(redMean, greenMean, blueMean), totalPixelNum);
        }
    }; // VBox

private:
    static constexpr double RED_GRAY_RATIO = 0.299;
    static constexpr double GREEN_GRAY_RATIO = 0.587;
    static constexpr double BLUE_GRAY_RATIO = 0.114;
    static constexpr double RED_LUMINACE_RATIO = 0.2126;
    static constexpr double GREEN_LUMINACE_RATIO = 0.7152;
    static constexpr double BLUE_LUMINACE_RATIO = 0.0722;
    void InitColorValBy1010102Color(std::shared_ptr<Media::PixelMap> pixmap, uint32_t left, uint32_t top,
        uint32_t right, uint32_t bottom);
    void InitColorValBy8888Color(std::shared_ptr<Media::PixelMap> pixmap, uint32_t left, uint32_t top,
        uint32_t right, uint32_t bottom);
    static uint8_t Rgb2Gray(uint32_t color, Media::PixelFormat format = Media::PixelFormat::RGBA_8888);
    uint32_t CalcGrayMsd() const;
    static float NormalizeRgb(uint32_t val, const uint32_t& colorMax = RGB888_COLOR_MAX);
    static float CalcRelativeLum(uint32_t color, Media::PixelFormat format = Media::PixelFormat::RGBA_8888);
    float CalcContrastToWhite() const;
    std::vector<std::pair<uint32_t, uint32_t>> QuantizePixels(int colorNum);
    void SplitBoxes(std::priority_queue<VBox, std::vector<VBox>, std::less<VBox> > &queue, int maxSize);
    std::vector<std::pair<uint32_t, uint32_t>> GenerateAverageColors(std::priority_queue<VBox, \
                                                        std::vector<VBox>, std::less<VBox> > &queue);
}; // ColorExtract
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
#endif // COLOREXTRACT_H
