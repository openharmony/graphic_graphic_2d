/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H

#include "platform/common/rs_log.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RSPropertyDrawableUtils {
public:
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr)
    {
        Drawing::Rect rect = Drawing::Rect(
            rr.rect_.left_, rr.rect_.top_, rr.rect_.left_ + rr.rect_.width_, rr.rect_.top_ + rr.rect_.height_);

        // set radius for all 4 corner of RRect
        constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4;
        std::vector<Drawing::Point> radii(NUM_OF_CORNERS_IN_RECT);
        for (uint32_t i = 0; i < NUM_OF_CORNERS_IN_RECT; i++) {
            radii.at(i).SetX(rr.radius_[i].x_);
            radii.at(i).SetY(rr.radius_[i].y_);
        }
        return {rect, radii};
    }

    static Drawing::Rect Rect2DrawingRect(const RectF& r)
    {
        return {r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_};
    }

    static RRect GetRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline)
    {
        if (!border) {
            return {};
        }

        return isOutline ?
            RRect(properties.GetRRect().rect_.MakeOutset(border->GetWidthFour()), border->GetRadiusFour()) :
            properties.GetRRect();
    }

    static RRect GetInnerRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline)
    {
        if (!border) {
            return {};
        }
        return isOutline ? properties.GetRRect() : properties.GetInnerRRect();
    }

    static bool PickColor(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& drPath,
        Drawing::Matrix& matrix, Drawing::RectI& deviceClipBounds, RSColor& colorPicked)
    {
        Drawing::Rect clipBounds = drPath.GetBounds();
        Drawing::RectI clipIBounds = { static_cast<int>(clipBounds.GetLeft()), static_cast<int>(clipBounds.GetTop()),
            static_cast<int>(clipBounds.GetRight()), static_cast<int>(clipBounds.GetBottom()) };
        Drawing::Surface* drSurface = canvas.GetSurface();
        if (drSurface == nullptr) {
            return false;
        }

        auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
        if (!colorPickerTask) {
            ROSEN_LOGE("RSPropertyDrawableUtils::PickColor colorPickerTask is null");
            return false;
        }
        colorPickerTask->SetIsShadow(true);
        int deviceWidth = 0;
        int deviceHeight = 0;
        int deviceClipBoundsW = drSurface->Width();
        int deviceClipBoundsH = drSurface->Height();
        if (!colorPickerTask->GetDeviceSize(deviceWidth, deviceHeight)) {
            colorPickerTask->SetDeviceSize(deviceClipBoundsW, deviceClipBoundsH);
            deviceWidth = deviceClipBoundsW;
            deviceHeight = deviceClipBoundsH;
        }
        int32_t fLeft = std::clamp(int(matrix.Get(Drawing::Matrix::Index::TRANS_X)), 0, deviceWidth - 1);
        int32_t fTop = std::clamp(int(matrix.Get(Drawing::Matrix::Index::TRANS_Y)), 0, deviceHeight - 1);
        int32_t fRight = std::clamp(int(fLeft + clipIBounds.GetWidth()), 0, deviceWidth - 1);
        int32_t fBottom = std::clamp(int(fTop + clipIBounds.GetHeight()), 0, deviceHeight - 1);
        if (fLeft == fRight || fTop == fBottom) {
            return false;
        }

        Drawing::RectI regionBounds = { fLeft, fTop, fRight, fBottom };
        std::shared_ptr<Drawing::Image> shadowRegionImage = drSurface->GetImageSnapshot(regionBounds);

        if (shadowRegionImage == nullptr) {
            return false;
        }

        // when color picker task resource is waitting for release, use color picked last frame
        if (colorPickerTask->GetWaitRelease()) {
            colorPickerTask->GetColorAverage(colorPicked);
            return true;
        }

        if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask, shadowRegionImage)
            && colorPickerTask->GetColor(colorPicked)) {
            colorPickerTask->GetColorAverage(colorPicked);
            colorPickerTask->SetStatus(CacheProcessStatus::WAITING);
            return true;
        }
        colorPickerTask->GetColorAverage(colorPicked);
        return true;
    }

    static void GetDarkColor(RSColor& color)
    {
        // convert to lab
        float minColorRange = 0;
        float maxColorRange = 255;
        float R = float(color.GetRed()) / maxColorRange;
        float G = float(color.GetGreen()) / maxColorRange;
        float B = float(color.GetBlue()) / maxColorRange;

        float X = 0.4124 * R + 0.3576 * G + 0.1805 * B;
        float Y = 0.2126 * R + 0.7152 * G + 0.0722 * B;
        float Z = 0.0193 * R + 0.1192 * G + 0.9505 * B;

        float Xn = 0.9505;
        float Yn = 1.0000;
        float Zn = 1.0889999;
        float Fx = (X / Xn) > 0.008856 ? pow((X / Xn), 1.0 / 3) : (7.787 * (X / Xn) + 16.0 / 116);
        float Fy = (Y / Yn) > 0.008856 ? pow((Y / Yn), 1.0 / 3) : (7.787 * (Y / Yn) + 16.0 / 116);
        float Fz = (Z / Zn) > 0.008856 ? pow((Z / Zn), 1.0 / 3) : (7.787 * (Z / Zn) + 16.0 / 116);
        float L = 116 * Fy - 16;
        float a = 500 * (Fx - Fy);
        float b = 200 * (Fy - Fz);

        float standardLightness = 75.0;
        if (L > standardLightness) {
            float L1 = standardLightness;
            float xw = 0.9505;
            float yw = 1.0000;
            float zw = 1.0889999;

            float fy = (L1 + 16) / 116;
            float fx = fy + (a / 500);
            float fz = fy - (b / 200);

            float X1 = xw * ((pow(fx, 3) > 0.008856) ? pow(fx, 3) : ((fx - 16.0 / 116) / 7.787));
            float Y1 = yw * ((pow(fy, 3) > 0.008856) ? pow(fy, 3) : ((fy - 16.0 / 116) / 7.787));
            float Z1 = zw * ((pow(fz, 3) > 0.008856) ? pow(fz, 3) : ((fz - 16.0 / 116) / 7.787));

            float DarkR = 3.2406 * X1 - 1.5372 * Y1 - 0.4986 * Z1;
            float DarkG = -0.9689 * X1 + 1.8758 * Y1 + 0.0415 * Z1;
            float DarkB = 0.0557 * X1 - 0.2040 * Y1 + 1.0570 * Z1;

            DarkR = std::clamp(maxColorRange * DarkR, minColorRange, maxColorRange);
            DarkG = std::clamp(maxColorRange * DarkG, minColorRange, maxColorRange);
            DarkB = std::clamp(maxColorRange * DarkB, minColorRange, maxColorRange);

            color = RSColor(DarkR, DarkG, DarkB, color.GetAlpha());
        }
    }
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H
