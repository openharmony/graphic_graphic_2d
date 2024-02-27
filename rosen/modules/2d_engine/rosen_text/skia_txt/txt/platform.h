/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_PLATFORM_H
#define ROSEN_MODULES_SPTEXT_PLATFORM_H

#include <string>
#include <vector>

#include "include/core/SkFontMgr.h"
#include "text/font_mgr.h"
#include "utils.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
std::vector<std::string> GetDefaultFontFamilies();

#ifndef USE_ROSEN_DRAWING
sk_sp<SkFontMgr> GetDefaultFontManager();
#else
std::shared_ptr<Drawing::FontMgr> GetDefaultFontManager();
#endif
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PLATFORM_H
