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

#include <gtest/gtest.h>

#include <memory>
#include "egl_blob_cache.h"

#include "egl_defs.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglBlobFileTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: EglBlobFileInit001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, EglBlobFileInit001, Level1)
{
    BlobCache* ret = BlobCache::Get();
    ASSERT_NE(ret, nullptr);
}


/**
 * @tc.name: EglBlobFileInit002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, EglBlobFileInit002, Level1)
{
    BlobCache* ret = BlobCache::Get();
    ret->SetCacheDir("/data/log");
    ASSERT_NE(ret, nullptr);

    void *a = malloc(4);
    void *b = malloc(4);
    int* intPtr = static_cast<int*>(a);
    int* intPtr2 = static_cast<int*>(b);
    *intPtr = 1;
    *intPtr2 = 2;
    const void* key = static_cast<const void*>(a);
    void *value = static_cast<void*>(b);
    int d = *static_cast<int *>(value);
    ASSERT_EQ(d, 2);
    EGLsizeiANDROID keysize = 4;
    EGLsizeiANDROID valuesize = 4;
    void *value2 = malloc(4);
    BlobCache::SetBlobFunc(key, keysize, value, valuesize);
    BlobCache::GetBlobFunc(key, keysize, value2, valuesize);
    int c = *static_cast<int *>(value2);
    ASSERT_EQ(c, 2);
    BlobCache::Get()->Terminate();
    sleep(5);
    ret = BlobCache::Get();
    ret->SetCacheDir("/data/log");
    int h = ret->GetMapSize();
    ASSERT_EQ(h, 1);
}

/**
 * @tc.name: EglBlobFileInit003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglBlobFileTest, EglBlobFileInit003, Level1)
{
    BlobCache* ret = BlobCache::Get();
    ret->SetCacheDir("/data/log");
    ASSERT_NE(ret, nullptr);

    void *a = malloc(4);
    void *b = malloc(4);
    int* intPtr = static_cast<int*>(a);
    int* intPtr2 = static_cast<int*>(b);
    *intPtr = 1;
    *intPtr2 = 2;
    const void* key = static_cast<const void*>(a);
    void *value = static_cast<void*>(b);
    int d = *static_cast<int *>(value);
    ASSERT_EQ(d, 2);
    EGLsizeiANDROID keysize = 4;
    EGLsizeiANDROID valuesize = 4;
    void *value2 = malloc(4);
    BlobCache::SetBlobFunc(key, keysize, value, valuesize);
    BlobCache::GetBlobFunc(key, keysize, value2, valuesize);
    int c = *static_cast<int *>(value2);
    ASSERT_EQ(c, 2);
    BlobCache::Get()->Terminate();
    sleep(5);
    ret = BlobCache::Get();
    ret->SetCacheDir("/data/log");
    int h = ret->GetMapSize();
    ASSERT_EQ(h, 1);
    BlobCache::GetBlobFunc(key, keysize, value2, valuesize);
    c = *static_cast<int *>(value2);
    ASSERT_EQ(c, 2);
}

} // OHOS::Rosen
