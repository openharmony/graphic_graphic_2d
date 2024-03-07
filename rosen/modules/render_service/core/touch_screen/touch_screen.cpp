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

#include <dlfcn.h>
#include "touch_screen.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string TOUCHSCREEN_WRAPPER_PATH = "../../vendor/lib64/chipsetsdk/libhw_touchscreen.default.so";
} // namespace

TouchScreen::TouchScreen() {}
TouchScreen::~TouchScreen()
{
    dlclose(touchScreenHandle_);
    touchScreenHandle_ = nullptr;
}

void TouchScreen::InitTouchScreen()
{
    touchScreenHandle_ = dlopen(TOUCHSCREEN_WRAPPER_PATH.c_str(), RTLD_NOW);
    if (touchScreenHandle_ == nullptr) {
        RS_LOGE("libhw_touchscreen.default.so was not loaded, error: %{public}s", dlerror());
        return;
    }

    tsSetFeatureConfig_ = (TS_SET_FEATURE_CONFIG_)dlsym(touchScreenHandle_, "ts_set_feature_config");
    if (tsSetFeatureConfig_ == nullptr) {
        RS_LOGE("touch screen get symbol failed, error: %{public}s", dlerror());
        return;
    }
    RS_LOGI("touch scree wrapper init success");
}
} // namespace MMI
} // namespace OHOS
