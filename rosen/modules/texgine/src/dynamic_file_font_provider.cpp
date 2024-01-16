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

#include "texgine/dynamic_file_font_provider.h"

#include "mock.h"
#include "texgine/utils/exlog.h"
#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SUCCESSED 0
#define FAILED 1
std::shared_ptr<DynamicFileFontProvider> DynamicFileFontProvider::Create() noexcept(true)
{
    return std::make_shared<DynamicFileFontProvider>();
}

int DynamicFileFontProvider::LoadFont(const std::string &familyName, const std::string &path) noexcept(true)
{
    LOGEX_FUNC_LINE_DEBUG(DEBUG) << "loading font: '" << path << "'";
#ifdef BUILD_NON_SDK_VER
    std::error_code ec;
    auto ret = StdFilesystemExists(path, ec);
    if (ec) {
        LOGEX_FUNC_LINE(ERROR) << "open file failed: " << ec.message();
        return FAILED;
    }
#else
    auto ret = StdFilesystemExists(path);
#endif
    if (!ret) {
        LOGEX_FUNC_LINE(ERROR) << "file is not exists";
        return FAILED;
    }

    MockIFStream ifs(path);
    if (!ifs.StdFilesystemIsOpen()) {
        LOGEX_FUNC_LINE(ERROR) << "file open failed";
        return FAILED;
    }

    ifs.StdFilesystemSeekg(0, ifs.end);
    if (!ifs.good()) {
        LOGEX_FUNC_LINE(ERROR) << "seekg(0, ifs.end) failed!";
        return FAILED;
    }

    auto size = ifs.StdFilesystemTellg();
    if (ifs.fail()) {
        LOGEX_FUNC_LINE(ERROR) << "tellg failed!";
        return FAILED;
    }

    ifs.StdFilesystemSeekg(ifs.beg);
    if (!ifs.good()) {
        LOGEX_FUNC_LINE(ERROR) << "seekg(ifs.beg) failed!";
        return FAILED;
    }

    std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);
    ifs.StdFilesystemRead(buf.get(), size);
    if (!ifs.good()) {
        LOGEX_FUNC_LINE(ERROR) << "read failed!";
        return FAILED;
    }

    ifs.StdFilestystemClose();
    return DynamicFontProvider::LoadFont(familyName, buf.get(), size);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
