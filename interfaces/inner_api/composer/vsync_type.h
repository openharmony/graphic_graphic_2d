/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef VSYNC_TYPE_H
#define VSYNC_TYPE_H

#include <cstdint>
namespace OHOS {
namespace Rosen {
    constexpr uint32_t VSYNC_MAX_REFRESHRATE = 360; // 360hz at most
} // Rosen

typedef enum {
    /*
    * VSync mode for LTPS indicates the refresh rate of software vsync won't change
    * without the hardware vsync refresh rate changed.
    */
    VSYNC_MODE_LTPS,
    /*
    * VSync mode for LTPO indicates the refresh rate of software vsync can be switched
    * without the hardware vsync refresh rate changed.
    */
    VSYNC_MODE_LTPO,
    /*
    * Invalid VSync mode.
    */
    VSYNC_MODE_INVALID
} VSyncMode;
} // OHOS

#endif // VSYNC_TYPE_H