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

#include <iostream>

#include <gtest/gtest.h>

#include "mock.h"
#include "texgine/dynamic_file_font_provider.h"

using namespace testing;
using namespace testing::ext;

struct MockVars {
    bool existsReturnValue_ = true;
    std::error_code errorCode_;
    bool openReturnValue_ = true;
    std::ios_base::iostate tellgState_ = std::ios_base::goodbit;
    std::ios_base::iostate seekg1State_ = std::ios_base::goodbit;
    std::ios_base::iostate seekg2State_ = std::ios_base::goodbit;
    std::ios_base::iostate readState_ = std::ios_base::goodbit;
} mockVars;

void InitDffpMockVars(MockVars vars)
{
    mockVars = vars;
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
bool StdFilesystemExists(const std::string &p, std::error_code &ec)
{
    ec = mockVars.errorCode_;
    return mockVars.existsReturnValue_;
}

bool MockIFStream::StdFilesystemIsOpen() const
{
    return mockVars.openReturnValue_;
}

std::istream &MockIFStream::StdFilesystemSeekg(std::ifstream::off_type off, std::ios_base::seekdir dir)
{
    this->clear();
    this->setstate(mockVars.seekg2State_);
    return *this;
}

std::istream &MockIFStream::StdFilesystemSeekg(pos_type pos)
{
    this->clear();
    this->setstate(mockVars.seekg1State_);
    return *this;
}

std::ifstream::pos_type MockIFStream::StdFilesystemTellg()
{
    this->clear();
    this->setstate(mockVars.tellgState_);
    return {};
}

std::istream &MockIFStream::StdFilesystemRead(char_type *s, std::streamsize count)
{
    this->clear();
    this->setstate(mockVars.readState_);
    return *this;
}

int DynamicFontProvider::LoadFont(const std::string &familyName,
                                  const void *data, size_t datalen) noexcept(true)
{
    return 0;
}

class DynamicFileFontProviderTest : public testing::Test {
public:
    std::shared_ptr<DynamicFileFontProvider> dynamicFileFontProvider = DynamicFileFontProvider::Create();
};

// 过程测试
// 调用Create函数
// 判定返回值为非空指针
HWTEST_F(DynamicFileFontProviderTest, Create, TestSize.Level1)
{
    EXPECT_NE(DynamicFileFontProvider::Create(), nullptr);
}

// 异常测试
// 重写StdFilesystemExists函数，控制ec的__val值为1
// 判定返回值为1
HWTEST_F(DynamicFileFontProviderTest, LoadFont1, TestSize.Level1)
{
    InitDffpMockVars({.errorCode_ = std::make_error_code(std::errc::file_exists)});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF1", {}), 1);
}

// 异常测试
// 重写StdFilesystemExists函数，控制返回值为false
// 判定返回值为1
HWTEST_F(DynamicFileFontProviderTest, LoadFont2, TestSize.Level1)
{
    InitDffpMockVars({.existsReturnValue_ = false});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF2", {}), 1);
}

// 异常测试
// 重写StdFilesystemIsOpen函数，控制返回值为false
// 判定返回值为1
HWTEST_F(DynamicFileFontProviderTest, LoadFont3, TestSize.Level1)
{
    InitDffpMockVars({.openReturnValue_ = false});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF3", {}), 1);
}

// 异常测试
// 重写StdFilesystemSeekg(off_type off, std::ios_base::seekdir dir)函数,将流状态设置为badbit
// 判定返回值为1
HWTEST_F(DynamicFileFontProviderTest, LoadFont4, TestSize.Level1)
{
    InitDffpMockVars({.seekg2State_ = std::ios_base::badbit});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF4", {}), 1);
}

// 异常测试
// 重写StdFilesystemTellg函数，将流状态设置为failbit
// 判定返回值为1
HWTEST_F(DynamicFileFontProviderTest, LoadFont5, TestSize.Level1)
{
    InitDffpMockVars({.tellgState_ = std::ios_base::failbit});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF5", {}), 1);
}

// 异常测试
// 重写StdFilesystemSeekg(pos_type pos)函数,将流状态设置为badbit
// 判定返回值为1
HWTEST_F(DynamicFileFontProviderTest, LoadFont6, TestSize.Level1)
{
    InitDffpMockVars({.seekg1State_ = std::ios_base::badbit});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF6", {}), 1);
}

// 异常测试
// 重写StdFilesystemRead函数,将流状态设置为badbit
// 判定返回值为1
HWTEST_F(DynamicFileFontProviderTest, LoadFont7, TestSize.Level1)
{
    InitDffpMockVars({.readState_ = std::ios_base::badbit});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF7", {}), 1);
}

// 逻辑测试
// 参数设为默认状态
// 判定返回值为0
HWTEST_F(DynamicFileFontProviderTest, LoadFont8, TestSize.Level1)
{
    InitDffpMockVars({});
    EXPECT_EQ(dynamicFileFontProvider->LoadFont("LF8", {}), 0);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
