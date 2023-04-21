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

#include "mock.h"

#include <filesystem>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
MockIFStream::MockIFStream(const std::string &filename,
                std::ios_base::openmode mode)
    : std::ifstream(filename, mode)
{
}

bool MockIFStream::StdFilesystemIsOpen() const
{
    return std::ifstream::is_open();
}

std::istream &MockIFStream::StdFilesystemSeekg(pos_type pos)
{
    return std::ifstream::seekg(pos);
}

std::istream &MockIFStream::StdFilesystemSeekg(off_type off, std::ios_base::seekdir dir)
{
    return std::ifstream::seekg(off, dir);
}

std::ifstream::pos_type MockIFStream::StdFilesystemTellg()
{
    return std::ifstream::tellg();
}

std::istream &MockIFStream::StdFilesystemRead(char_type *s, std::streamsize count)
{
    return std::ifstream::read(s, count);
}

void MockIFStream::StdFilestystemClose()
{
    std::ifstream::close();
}

bool StdFilesystemExists(const std::string &p, std::error_code &ec)
{
    return std::filesystem::exists(p, ec);
}

} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
