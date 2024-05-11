/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_PROFILER_CAPTUREDATA_H
#define RS_PROFILER_CAPTUREDATA_H

#include <map>
#include <string>
#include <vector>

namespace OHOS::Rosen {

class RSCaptureData final {
public:
    // every rs update
    inline static const std::string KEY_RS_FRAME_LEN = "rs_frame_len";
    inline static const std::string KEY_RS_CMD_COUNT = "rs_cmd_count";
    inline static const std::string KEY_RS_CMD_EXECUTE_COUNT = "rs_cmd_execute_count";
    inline static const std::string KEY_RS_CMD_PARCEL_LIST = "rs_cmd_parcel_list";
    inline static const std::string KEY_RS_PIXEL_IMAGE_ADDED = "rs_pixelimage_added";
    inline static const std::string KEY_RS_DIRTY_REGION = "rs_dirty_region";
    inline static const std::string KEY_RS_CPU_ID = "rs_cpu_id";

    // every 8ms
    inline static const std::string KEY_CPU_TEMP = "cpu_temp";
    inline static const std::string KEY_CPU_LOAD = "cpu_load";
    inline static const std::string KEY_CPU_FREQ = "cpu_freq";
    inline static const std::string KEY_CPU_CURRENT = "cpu_current";

    inline static const std::string KEY_GPU_LOAD = "gpu_load";
    inline static const std::string KEY_GPU_FREQ = "gpu_freq";

    RSCaptureData();
    ~RSCaptureData();

    void Reset();

    void SetTime(float time);

    float GetTime() const;

    void Serialize(std::vector<char>& out);
    void Deserialize(const std::vector<char>& in);

    void SetProperty(const std::string& name, const std::string& value);

    template<typename T>
    void SetProperty(const std::string& name, T value)
    {
        SetProperty(name, std::to_string(value));
    }

    const std::string& GetProperty(const std::string& name) const;
    float GetPropertyFloat(const std::string& name) const;
    double GetPropertyDouble(const std::string& name) const;
    int8_t GetPropertyInt8(const std::string& name) const;
    uint8_t GetPropertyUint8(const std::string& name) const;
    int16_t GetPropertyInt16(const std::string& name) const;
    uint16_t GetPropertyUint16(const std::string& name) const;
    int32_t GetPropertyInt32(const std::string& name) const;
    uint32_t GetPropertyUint32(const std::string& name) const;
    int64_t GetPropertyInt64(const std::string& name) const;
    uint64_t GetPropertyUint64(const std::string& name) const;

protected:
    void Serialize(class Archive& archive);

private:
    float time_ = 0.0f;
    std::map<std::string, std::string> properties_;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_CAPTUREDATA_H