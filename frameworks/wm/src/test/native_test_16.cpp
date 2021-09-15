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

#include "native_test_16.h"

#include <cinttypes>
#include <cstdio>
#include <securec.h>
#include <sys/time.h>

#include <zlib.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest16 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "compress perf test";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 16;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 1 << 30;
        return lastTime;
    }

    int64_t GetNowTime() const
    {
        struct timeval start = {};
        gettimeofday(&start, nullptr);
        constexpr uint32_t secToUsec = 1000 * 1000;
        return static_cast<int64_t>(start.tv_sec) * secToUsec + start.tv_usec;
    }

    void Run(int32_t argc, const char **argv) override
    {
        if (argc <= 1) {
            printf("need a compress size\n");
            ExitTest();
            return;
        }

        uint32_t size = -1;
        int ret = sscanf_s(argv[1], "%u", &size);
        if (ret == 0) {
            printf("%s parse argv[2] failed\n", __func__);
            ExitTest();
            return;
        }

        auto ptr = std::make_unique<uint8_t[]>(size);
        auto ptr32 = reinterpret_cast<uint32_t *>(ptr.get());
        for (uint32_t i = 0; i < size / (sizeof(uint32_t) / sizeof(uint8_t)); i++) {
            ptr32[i] = 0xff000000;
        }

        auto clength = compressBound(size);
        auto compressed = std::make_unique<uint8_t[]>(clength);

        unsigned long ulength = clength;
        if (Compress(compressed, ulength, ptr, size) != Z_OK) {
            printf("compress failed\n");
            ExitTest();
            return;
        }
        printf("compress length: %lu\n", ulength);

        int64_t start = GetNowTime();
        unsigned long uulength = size;
        if (Uncompress(ptr, uulength, compressed, ulength) != Z_OK) {
            printf("uncompress failed\n");
            ExitTest();
            return;
        }
        printf("uncompress time: %" PRIu64 "\n", GetNowTime() - start);
        ExitTest();
    }

    int32_t Compress(const std::unique_ptr<uint8_t[]> &c,
        unsigned long &ul, const std::unique_ptr<uint8_t[]> &p, uint32_t size) const
    {
        auto ret = compress(c.get(), &ul, p.get(), size);
        if (ret) {
            printf("compress failed, %d\n", ret);
        }
        return ret;
    }

    int32_t Uncompress(const std::unique_ptr<uint8_t[]> &c,
        unsigned long &ul, const std::unique_ptr<uint8_t[]> &p, uint32_t size) const
    {
        auto ret = uncompress(c.get(), &ul, p.get(), size);
        if (ret) {
            printf("uncompress failed, %d\n", ret);
        }
        return ret;
    }
} g_autoload;
} // namespace
