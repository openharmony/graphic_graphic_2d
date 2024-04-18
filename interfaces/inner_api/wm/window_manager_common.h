/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_WM_WINDOW_MANAGER_COMMON_H
#define INTERFACES_INNERKITS_WM_WINDOW_MANAGER_COMMON_H

#include "wmclient/wm_common.h"

namespace OHOS {
typedef enum {
    WINDOW_LAYER_DEFINE_NORMAL_ID = 5000,
    WINDOW_LAYER_DEFINE_STATUSBAR_ID = 6000,
    WINDOW_LAYER_DEFINE_NAVIBAR_ID = 7000,
    WINDOW_LAYER_DEFINE_ALARM_ID = 8000,
} LayerID;

constexpr int32_t LAYER_ID_APP_TYPE_BASE = 5000;
constexpr int32_t LAYER_ID_TYPE_OFSSET = 1000;

typedef enum {
    SHOT_WINDOW = 0,
    SHOT_SCREEN,
    SHOT_INVALID
} ShotType;

struct MoveReq {
    int32_t id;
    int32_t pos_x;
    int32_t pos_y;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WM_WINDOW_MANAGER_COMMON_H
