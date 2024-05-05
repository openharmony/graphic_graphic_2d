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
#ifndef RS_COLOREXTRACT_H
#define RS_COLOREXTRACT_H

#include <cstdint>
#include <vector>
#include <utility>
#include <memory>
#include <limits>
#include <algorithm>
#include <math.h>
#include <queue>

#include "image/pixmap.h"

namespace OHOS {
namespace Rosen {

#ifdef _WIN32
#define NATIVEEXPORT __declspec(dllexport)
#else
#define NATIVEEXPORT
#endif

const uint32_t RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE = 2; // Invalid value
const uint32_t RS_COLOR_PICKER_ERROR = 1; // Operation error
const uint32_t RS_COLOR_PICKER_SUCCESS = 0; // Operation success

class RSColorExtract {
public:
    virtual ~RSColorExtract() {};
    std::shared_ptr<Drawing::Pixmap> pixelmap_;

    // Save the ARGB val of picture.
    std::shared_ptr<uint32_t> colorVal_ = nullptr;
    uint32_t colorValLen_ = 0;
    uint32_t grayMsd_ = 0;
    float contrastToWhite_ = 0;

    // The first element is color, the second element is pixel num of this color.
    std::vector<std::pair<uint32_t, uint32_t>> featureColors_;

    // Specified number of extracted theme colors, default value is 10;
    int specifiedFeatureColorNum_ = 10;

    std::shared_ptr<uint32_t> hist_ = nullptr;
    int distinctColorCount_ = 0;
    std::shared_ptr<uint32_t> colors_ = nullptr;

    static constexpr uint8_t ARGB_MASK = 0xFF;
    static constexpr uint8_t ARGB_R_SHIFT = 16;
    static constexpr uint8_t ARGB_G_SHIFT = 8;
    static constexpr uint8_t ARGB_B_SHIFT = 0;
    static uint8_t GetARGB32ColorR(unsigned int color);
    static uint8_t GetARGB32ColorG(unsigned int color);
    static uint8_t GetARGB32ColorB(unsigned int color);
    NATIVEEXPORT void SetFeatureColorNum(int N);
    void GetNFeatureColors(int colorNum);
protected:
    RSColorExtract(std::shared_ptr<Drawing::Pixmap> pixmap);
    RSColorExtract(std::shared_ptr<Drawing::Pixmap> pixmap, double* coordinates);
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
    static uint32_t ModifyWordWidth(uint8_t color, int inWidth, int outWidth);
    static uint32_t ApproximateToRGB888(uint32_t r, uint32_t g, uint32_t b);
    static uint32_t ApproximateToRGB888(uint32_t color);
    static bool cmp(std::pair<uint32_t, uint32_t>& a, std::pair<uint32_t, uint32_t>& b);

    class VBox {
    private:
        int lowerIndex_, upperIndex_;
        // total pixel nums in this box;
        int pixelNums_;
        uint32_t minRed_, maxRed_;
        uint32_t minGreen_, maxGreen_;
        uint32_t minBlue_, maxBlue_;
        RSColorExtract *colorExtract_ = nullptr;
    public:
        VBox(int lowerIndex, int upperIndex, RSColorExtract *colorExtract)
        {
            lowerIndex_ = lowerIndex;
            upperIndex_ = upperIndex;
            colorExtract_ = colorExtract;
            fitBox();
        }

        uint32_t GetVolume() const
        {
            return (maxRed_ - minRed_ + 1) * (maxGreen_ - minGreen_ +1) * (maxBlue_ -minBlue_ + 1);
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
            uint32_t *colors = colorExtract_->colors_.get();
            uint32_t *hist = colorExtract_->hist_.get();

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
            uint32_t *colors = colorExtract_->colors_.get();
            uint32_t *hist = colorExtract_->hist_.get();

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
        void ModifySignificantOctet(uint32_t *colors, int dimension, int lower, int upper)
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
            uint32_t *colors = colorExtract_->colors_.get();
            uint32_t *hist = colorExtract_->hist_.get();
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
                uint32_t error_color = 0;
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
    static uint8_t Rgb2Gray(uint32_t color);
    uint32_t CalcGrayMsd() const;
    static float NormalizeRgb(uint32_t val);
    static float CalcRelativeLum(uint32_t color);
    float CalcContrastToWhite() const;
    std::vector<std::pair<uint32_t, uint32_t>> QuantizePixels(int colorNum);
    void SplitBoxes(std::priority_queue<VBox, std::vector<VBox>, std::less<VBox> > &queue, int maxSize);
    std::vector<std::pair<uint32_t, uint32_t>> GenerateAverageColors(std::priority_queue<VBox,
        std::vector<VBox>, std::less<VBox> > &queue);
}; // ColorExtract
} // namespace Rosen
} // namespace OHOS

#endif // COLOREXTRACT_H
