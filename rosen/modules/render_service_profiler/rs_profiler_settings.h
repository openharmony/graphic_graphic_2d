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

#ifndef RS_PROFILER_SETTINGS_H
#define RS_PROFILER_SETTINGS_H

#include <mutex>

#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

// SystemParameter
class RSB_EXPORT SystemParameter {
public:
    explicit SystemParameter(std::string name);
    explicit SystemParameter(const SystemParameter&) = delete;
    virtual ~SystemParameter();

    SystemParameter& operator=(const SystemParameter& parameter);

    const std::string& Name() const
    {
        return name_;
    }

    virtual void FromString(const std::string& value) = 0;
    virtual std::string ToString() const = 0;

    static SystemParameter* Find(const std::string& name);
    static bool Exists(const std::string& name);

    static bool Set(const std::string& name, const std::string& value);
    static std::string Get(const std::string& name);

    static std::string Dump();

protected:
    std::string name_;

private:
    static std::mutex mutex_;
    static std::vector<SystemParameter*> registry_;
};

// TemplateParameter
template<typename Value>
class RSB_EXPORT TemplateParameter : public SystemParameter {
public:
    explicit TemplateParameter(const std::string& name, Value value) : SystemParameter(name), value_(std::move(value))
    {}
    ~TemplateParameter() override = default;

    void Set(const Value& value)
    {
        value_ = value;
    }

    const Value& Get() const
    {
        return value_;
    }

    const Value& operator*() const
    {
        return Get();
    }

protected:
    Value value_;
};

// NumeralParameter
template<typename Numeral>
class RSB_EXPORT NumeralParameter final : public TemplateParameter<Numeral> {
public:
    explicit NumeralParameter(const std::string& name, Numeral value = 0) : TemplateParameter<Numeral>(name, value) {}
    ~NumeralParameter() override = default;

    void FromString(const std::string& value) override
    {
        NumeralParameter::Set(Utils::ToNumber<Numeral>(value));
    }

    std::string ToString() const override
    {
        return std::to_string(NumeralParameter::Get());
    }

    NumeralParameter& operator=(Numeral value)
    {
        Set(value);
        return *this;
    }
};

using Int8Parameter = NumeralParameter<int8_t>;
using Int16Parameter = NumeralParameter<int16_t>;
using Int32Parameter = NumeralParameter<int32_t>;
using Int64Parameter = NumeralParameter<int64_t>;
using Uint8Parameter = NumeralParameter<uint8_t>;
using Uint16Parameter = NumeralParameter<uint16_t>;
using Uint32Parameter = NumeralParameter<uint32_t>;
using Uint64Parameter = NumeralParameter<uint64_t>;
using FloatParameter = NumeralParameter<float>;
using DoubleParameter = NumeralParameter<double>;

// BoolParameter
class RSB_EXPORT BoolParameter final : public TemplateParameter<bool> {
public:
    explicit BoolParameter(const std::string& name, bool value = false) : TemplateParameter<bool>(name, value) {}
    ~BoolParameter() override = default;

    void FromString(const std::string& value) override
    {
        value_ = (value == "true") || (value == "on") || (value == "1");
    }

    std::string ToString() const override
    {
        return value_ ? "true" : "false";
    }

    BoolParameter& operator=(bool value)
    {
        Set(value);
        return *this;
    }
};

// StringParameter
class RSB_EXPORT StringParameter final : public TemplateParameter<std::string> {
public:
    explicit StringParameter(const std::string& name, const std::string& value = "") : TemplateParameter(name, value) {}
    ~StringParameter() override = default;

    void FromString(const std::string& value) override
    {
        Set(value);
    }

    std::string ToString() const override
    {
        return Get();
    }

    const char* Data() const
    {
        return value_.data();
    }

    StringParameter& operator=(const std::string& value)
    {
        Set(value);
        return *this;
    }
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_SETTINGS_H