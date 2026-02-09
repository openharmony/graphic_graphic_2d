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

namespace OHOS::Rosen {

OH_PixelmapNative* CreateTestPixelMap(const std::string path);
OH_Filter* CreateFilter(OH_PixelmapNative* pixelMapNative);

}  // namespace OHOS::Rosen
#endif // OH_FILTER_TEST_UTILS_H