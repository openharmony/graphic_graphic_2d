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
#ifndef COLORSPACE
#define COLORSPACE

namespace OHOS {
namespace ColorManager {

enum ColorSpaceName : uint32_t {
    NONE = 0,
    ADOBE_RGB = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    CUSTOM = 5,
    BT709 = 6,
    BT601_EBU = 7,
    BT601_SMPTE_C = 8,
    BT2020_HLG = 9,
    BT2020_PQ = 10,
    P3_HLG = 11,
    P3_PQ = 12,
    ADOBE_RGB_LIMIT = 13,
    DISPLAY_P3_LIMIT = 14,
    SRGB_LIMIT = 15,
    BT709_LIMIT = 16,
    BT601_EBU_LIMIT = 17,
    BT601_SMPTE_C_LIMIT = 18,
    BT2020_HLG_LIMIT = 19,
    BT2020_PQ_LIMIT = 20,
    P3_HLG_LIMIT = 21,
    P3_PQ_LIMIT = 22,
    LINEAR_P3 = 23,
    LINEAR_SRGB = 24,
    LINEAR_BT709 = LINEAR_SRGB,
    LINEAR_BT2020 = 25,
    DISPLAY_SRGB = SRGB,
    DISPLAY_P3_SRGB = DISPLAY_P3,
    DISPLAY_P3_HLG = P3_HLG,
    DISPLAY_P3_PQ = P3_PQ,
    DISPLAY_BT2020_SRGB = 26,
    DISPLAY_BT2020_HLG = BT2020_HLG,
    DISPLAY_BT2020_PQ = BT2020_PQ,
    BT2020 = 27,
    NTSC_1953 = 28,
    PRO_PHOTO_RGB = 29,
    ACES = 30,
    ACESCG = 31,
    CIE_LAB = 32,
    CIE_XYZ = 33,
    EXTENDED_SRGB = 34,
    LINEAR_EXTENDED_SRGB = 35,
    SMPTE_C = 36,
};

class ColorSpace {
public:
    explicit ColorSpace(ColorSpaceName name)
    {
        mColorSpaceName = name;
    }

    ColorSpaceName GetColorSpaceName() const
    {
        return mColorSpaceName;
    }

private:
    ColorSpaceName mColorSpaceName = ColorSpaceName::SRGB;
};
} // namespace ColorSpace
} // namespace OHOS
#endif  // COLORSPACE

