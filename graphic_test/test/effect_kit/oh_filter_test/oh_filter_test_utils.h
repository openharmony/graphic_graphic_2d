/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef OH_FILTER_TEST_UTILS_H
#define OH_FILTER_TEST_UTILS_H

#include <string>
#include "effect_ndk/include/effect_filter.h"
#include "pixelmap_native_impl.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#define WIDTH_INDEX 2
#define HEIGHT_INDEX 3

#define REFRACTION_FACTOR_INDEX 0
#define HORIZONTAL_PATTERN_NUMBER_INDEX 1
#define GRID_LIGHT_STRENGTH_INDEX 2
#define GRID_LIGHT_POSITION_START_INDEX 3
#define GRID_LIGHT_POSITION_END_INDEX 4
#define GRID_SHADOW_STRENGTH_INDEX 5
#define GRID_SHADOW_POSITION_START_INDEX 6
#define GRID_SHADOW_POSITION_END_INDEX 7
#define PORTAL_LIGHT_SIZE_X_INDEX 8
#define PORTAL_LIGHT_SIZE_Y_INDEX 9
#define PORTAL_LIGHT_TILT_X_INDEX 10
#define PORTAL_LIGHT_TILT_Y_INDEX 11
#define PORTAL_LIGHT_POSITION_X_INDEX 12
#define PORTAL_LIGHT_POSITION_Y_INDEX 13
#define PORTAL_LIGHT_DISPERSE_ATTENUATION_INDEX 14
#define PORTAL_LIGHT_DISPERSE_INDEX 15
#define PORTAL_LIGHT_SMOOTH_BORDER_INDEX 16
#define PORTAL_LIGHT_SHADOW_BORDER_INDEX 17
#define PORTAL_LIGHT_SHADOW_POSITION_SHIFT_INDEX 18
#define PORTAL_LIGHT_STRENGTH_INDEX 19

namespace OHOS::Rosen {

OH_PixelmapNative* CreateTestPixelMap(const std::string path);
OH_Filter* CreateFilter(OH_PixelmapNative* pixelMapNative);

}  // namespace OHOS::Rosen
#endif // OH_FILTER_TEST_UTILS_H