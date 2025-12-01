/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <gtest/gtest.h>
#include <memory>

#include "gtest/hwext/gtest-tag.h"
#include "parameters.h"
#include "render_service_profiler/rs_profiler.h"
#include "rs_profiler.h"
#include "test_utils.h"

#include "transaction/rs_marshalling_helper.h"
#include "utils/data.h"

#ifdef ROSEN_OHOS
#include "buffer_utils.h"
#endif
#include "recording/mask_cmd_list.h"

#include "property/rs_properties_def.h"

#include "rs_profiler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSMarshallingHelperReliabilityTest : public testing::Test {
    static bool profilerEnabled_;

public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

bool RSMarshallingHelperReliabilityTest::profilerEnabled_ = false;

void RSMarshallingHelperReliabilityTest::SetUpTestCase()
{
    profilerEnabled_ = system::GetBoolParameter("persist.graphic.profiler.enabled", false);
}
void RSMarshallingHelperReliabilityTest::TearDownTestCase()
{
    system::SetParameter("persist.graphic.profiler.enabled", profilerEnabled_ ? "1" : "0");
}
void RSMarshallingHelperReliabilityTest::SetUp() {}
void RSMarshallingHelperReliabilityTest::TearDown() {}

/**
 * @tc.name: ConsistencyWithSmallData
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithSmallDataWithoutProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");
    // data should be std::shared_ptr<Drawing::Data>
    // because Marshalling working for std::shared_ptr<Drawing::Data>
    auto data = std::make_shared<Drawing::Data>();
    const size_t length = 100;
    const size_t position = 108;

    void* allocated = malloc(length);
    EXPECT_TRUE(data->BuildFromMalloc(allocated, length));

    EXPECT_TRUE(CheckConsistencyWithData(data, position));
}

/**
 * @tc.name: ConsistencyWithMediumData
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithMediumDataWithoutProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");
    // data should be std::shared_ptr<Drawing::Data>
    // because Marshalling working for std::shared_ptr<Drawing::Data>
    auto data = std::make_shared<Drawing::Data>();
    const size_t length = 40'000;
    const size_t position = 36;

    void* allocated = malloc(length);
    EXPECT_TRUE(data->BuildFromMalloc(allocated, length));

    EXPECT_TRUE(CheckConsistencyWithData(data, position));
}

/**
 * @tc.name: ConsistencyWithSmallPixelMapWithoutProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithSmallPixelMapWithoutProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");

    Media::InitializationOptions options;
    options.size.width = 1;
    options.size.height = 1;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    const size_t position = 88; // or 92
    const size_t pixelMapSize = 84;

    std::shared_ptr pixelMap = Media::PixelMap::Create(options);
    EXPECT_TRUE(pixelMap != nullptr);
    EXPECT_TRUE(CheckConsistencyWithPixelMap(pixelMap, position, pixelMapSize));
}

/**
 * @tc.name: ConsistencyWithMediumPixelMapWithoutProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithMediumPixelMapWithoutProfiler,
    TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");

    Media::InitializationOptions options;
    options.size.width = 860;
    options.size.height = 520;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    const size_t position = 88;
    const size_t pixelMapSize = 84;

    std::shared_ptr pixelMap = Media::PixelMap::Create(options);
    EXPECT_TRUE(pixelMap != nullptr);
    EXPECT_TRUE(CheckConsistencyWithPixelMap(pixelMap, position, pixelMapSize));
}

/**
 * @tc.name: ConsistencyWithBigPixelMapWithoutProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithBigPixelMapWithoutProfiler,
    TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "0");

    Media::InitializationOptions options;
    options.size.width = 2123;
    options.size.height = 1987;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    const size_t position = 88;
    const size_t pixelMapSize = 84;

    std::shared_ptr pixelMap = Media::PixelMap::Create(options);
    EXPECT_TRUE(pixelMap != nullptr);
    EXPECT_TRUE(CheckConsistencyWithPixelMap(pixelMap, position, pixelMapSize));
}

/**
 * @tc.name: ConsistencyWithSmallDataWithProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithSmallDataWithProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    // data should be std::shared_ptr<Drawing::Data>
    // because Marshalling working for std::shared_ptr<Drawing::Data>
    auto data = std::make_shared<Drawing::Data>();
    const size_t length = 100;
    const size_t position = 108;

    void* allocated = malloc(length);
    EXPECT_TRUE(data->BuildFromMalloc(allocated, length));

    EXPECT_TRUE(CheckConsistencyWithData(data, position));
}

/**
 * @tc.name: ConsistencyWithMediumDataWithProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithMediumDataWithProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    Parcel parcel;
    // data should be std::shared_ptr<Drawing::Data>
    // because Marshalling working for std::shared_ptr<Drawing::Data>
    auto data = std::make_shared<Drawing::Data>();
    const size_t length = 40'000;
    const size_t position = 44;

    void* allocated = malloc(length);
    EXPECT_TRUE(data->BuildFromMalloc(allocated, length));

    EXPECT_TRUE(CheckConsistencyWithData(data, position));
}

/**
 * @tc.name: ConsistencyWithSmallPixelMapWithProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithSmallPixelMapWithProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");

    Media::InitializationOptions options;
    options.size.width = 1;
    options.size.height = 1;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    const size_t position = 96;
    const size_t pixelMapSize = 92;

    std::shared_ptr pixelMap = Media::PixelMap::Create(options);
    EXPECT_TRUE(pixelMap != nullptr);
    EXPECT_TRUE(CheckConsistencyWithPixelMap(pixelMap, position, pixelMapSize));
}

/**
 * @tc.name: ConsistencyWithMediumPixelMapWithProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithMediumPixelMapWithProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");

    Media::InitializationOptions options;
    options.size.width = 860;
    options.size.height = 520;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    const size_t position = 96;
    const size_t pixelMapSize = 92;

    std::shared_ptr pixelMap = Media::PixelMap::Create(options);
    EXPECT_TRUE(pixelMap != nullptr);
    EXPECT_TRUE(CheckConsistencyWithPixelMap(pixelMap, position, pixelMapSize));
}

/**
 * @tc.name: ConsistencyWithBigPixelMapWithProfiler
 * @tc.desc: Verify function marshal, unmarshal and skip is consistent.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSMarshallingHelperReliabilityTest, ConsistencyWithBigPixelMapWithProfiler, TestSize.Level1 | Reliability)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");

    Media::InitializationOptions options;
    options.size.width = 2123;
    options.size.height = 1987;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    const size_t position = 96;
    const size_t pixelMapSize = 92;

    std::shared_ptr pixelMap = Media::PixelMap::Create(options);
    EXPECT_TRUE(pixelMap != nullptr);
    EXPECT_TRUE(CheckConsistencyWithPixelMap(pixelMap, position, pixelMapSize));
}

HWTEST(RSMarshallingHelperTest, UnmarshallingEmpty, Level1)
{
    Parcel parcel;
    uint64_t val = 0;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusId(parcel, val);
    EXPECT_FALSE(ret);
    EXPECT_EQ(val, 0);
}

HWTEST(RSMarshallingHelperTest, UnmarshallingPidPlusId0, Level1)
{
    Parcel parcel;
    uint64_t val = 0;
    parcel.WriteUint64(val);
    val = 42;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusId(parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, 0);
}

HWTEST(RSMarshallingHelperTest, UnmarshallingPidPlusId, Level1)
{
    Parcel parcel;
    uint64_t val = 42;
    parcel.WriteUint64(val);
    val = 0;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusId(parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, 42);
}

HWTEST(RSMarshallingHelperTest, UnmarshallingPidPlusIdNoChangeIfZero0, Level1)
{
    Parcel parcel;
    uint64_t val = 0;
    parcel.WriteUint64(val);
    val = 42;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusIdNoChangeIfZero(parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, 0);
}

HWTEST(RSMarshallingHelperTest, UnmarshallingPidPlusIdNoChangeIfZero, Level1)
{
    Parcel parcel;
    uint64_t val = 42;
    parcel.WriteUint64(val);
    val = 0;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusIdNoChangeIfZero(parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, 42);
}

class RSMarshallingHelperMockParcelTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        RSProfiler::testing_ = true;
        RSProfiler::SetMode(Mode::READ);
    }

    static void TearDownTestCase()
    {
        RSProfiler::testing_ = false;
        RSProfiler::SetMode(Mode::NONE);
    }

    void SetUp() override
    {
        parcel = new (parcelMemory + 1) Parcel;
    }

    void TearDown() override
    {
        parcel->~Parcel();
    }

private:
    Parcel* parcel;
    uint8_t parcelMemory[sizeof(Parcel) + 1];

    static constexpr const uint64_t FLAG = 1uLL << 62u;
};

HWTEST_F(RSMarshallingHelperMockParcelTest, UnmarshallingPidPlusId0, Level1)
{
    uint64_t val = 0;
    parcel->WriteUint64(val);
    val = 42;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusId(*parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, FLAG | 0);
}

HWTEST_F(RSMarshallingHelperMockParcelTest, UnmarshallingPidPlusId, Level1)
{
    uint64_t val = 42;
    parcel->WriteUint64(val);
    val = 0;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusId(*parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, FLAG | 42);
}

HWTEST_F(RSMarshallingHelperMockParcelTest, UnmarshallingPidPlusIdNoChangeIfZero0, Level1)
{
    uint64_t val = 0;
    parcel->WriteUint64(val);
    val = 42;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusIdNoChangeIfZero(*parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, 0);
}

HWTEST_F(RSMarshallingHelperMockParcelTest, UnmarshallingPidPlusIdNoChangeIfZero, Level1)
{
    uint64_t val = 42;
    parcel->WriteUint64(val);
    val = 0;
    bool ret = RSMarshallingHelper::UnmarshallingPidPlusIdNoChangeIfZero(*parcel, val);
    EXPECT_TRUE(ret);
    EXPECT_EQ(val, FLAG | 42);
}

} // namespace Rosen
} // namespace OHOS
