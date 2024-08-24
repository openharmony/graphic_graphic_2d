/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include "rosen_text/properties/placeholder_run.h"

namespace rosen {
PlaceholderRun::PlaceholderRun(double width,
    double height, PlaceholderAlignment placeholderAlignment,
    TextBaseline textbaseline, double baselineOffset)
{
    width_ = width;
    height_ = height;
    placeholderalignment_ = placeholderAlignment;
    textbaseline_ = textbaseline;
    baselineOffset_ = baselineOffset;
}
} // namespace rosen
