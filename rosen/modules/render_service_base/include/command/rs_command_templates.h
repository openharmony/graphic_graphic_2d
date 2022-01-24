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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_TEMPLATES_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_TEMPLATES_H

#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSUIDirector;

// avoiding C++ macros spilting parameters
#ifndef ARG
#define ARG(...) __VA_ARGS__
#endif

// Add new RSCommand as alias of template class
// Explicit instantiating templates will register the unmarshalling function into RSCommandFactory.
// To avoid redundant registary, make sure templates only instantiated once.
#ifdef ROSEN_INSTANTIATE_COMMAND_TEMPLATE
#define ADD_COMMAND(ALIAS, TYPE)           \
    using ALIAS = RSCommandTemplate<TYPE>; \
    template class RSCommandTemplate<TYPE>;
#else
#define ADD_COMMAND(ALIAS, TYPE) using ALIAS = RSCommandTemplate<TYPE>;
#endif

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename... Ts>
class RSCommandTemplate;

template<uint16_t commandType, uint16_t commandSubType, auto processFunc>
class RSCommandTemplate<commandType, commandSubType, processFunc> : public RSCommand {
public:
    RSCommandTemplate() {}
    virtual ~RSCommandTemplate() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        return new RSCommandTemplate();
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        (*processFunc)(context);
    }

private:
};

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename T1>
class RSCommandTemplate<commandType, commandSubType, processFunc, T1> : public RSCommand {
public:
    RSCommandTemplate(const T1& p1) : parameter1_(p1) {}
    virtual ~RSCommandTemplate() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter1_);
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        T1 parameter1;
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter1)) {
            return nullptr;
        }
        return new RSCommandTemplate(parameter1);
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        (*processFunc)(context, parameter1_);
    }

private:
    T1 parameter1_;
};

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename T1, typename T2>
class RSCommandTemplate<commandType, commandSubType, processFunc, T1, T2> : public RSCommand {
public:
    RSCommandTemplate(const T1& p1, const T2& p2) : parameter1_(p1), parameter2_(p2) {}
    virtual ~RSCommandTemplate() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter1_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter2_);
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        T1 parameter1;
        T2 parameter2;
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter1)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter2)) {
            return nullptr;
        }
        return new RSCommandTemplate(parameter1, parameter2);
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        (*processFunc)(context, parameter1_, parameter2_);
    }

private:
    T1 parameter1_;
    T2 parameter2_;
};

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename T1, typename T2, typename T3>
class RSCommandTemplate<commandType, commandSubType, processFunc, T1, T2, T3> : public RSCommand {
public:
    RSCommandTemplate(const T1& p1, const T2& p2, const T3& p3) : parameter1_(p1), parameter2_(p2), parameter3_(p3) {}
    virtual ~RSCommandTemplate() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter1_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter2_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter3_);
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        T1 parameter1;
        T2 parameter2;
        T3 parameter3;
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter1)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter2)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter3)) {
            return nullptr;
        }
        return new RSCommandTemplate(parameter1, parameter2, parameter3);
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        (*processFunc)(context, parameter1_, parameter2_, parameter3_);
    }

private:
    T1 parameter1_;
    T2 parameter2_;
    T3 parameter3_;
};

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename T1, typename T2, typename T3,
    typename T4>
class RSCommandTemplate<commandType, commandSubType, processFunc, T1, T2, T3, T4> : public RSCommand {
public:
    RSCommandTemplate(const T1& p1, const T2& p2, const T3& p3, const T4& p4)
        : parameter1_(p1), parameter2_(p2), parameter3_(p3), parameter4_(p4)
    {}
    virtual ~RSCommandTemplate() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter1_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter2_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter3_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter4_);
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        T1 parameter1;
        T2 parameter2;
        T3 parameter3;
        T4 parameter4;
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter1)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter2)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter3)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter4)) {
            return nullptr;
        }
        return new RSCommandTemplate(parameter1, parameter2, parameter3, parameter4);
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        (*processFunc)(context, parameter1_, parameter2_, parameter3_, parameter4_);
    }

private:
    T1 parameter1_;
    T2 parameter2_;
    T3 parameter3_;
    T4 parameter4_;
};

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename T1, typename T2, typename T3,
    typename T4, typename T5>
class RSCommandTemplate<commandType, commandSubType, processFunc, T1, T2, T3, T4, T5> : public RSCommand {
public:
    RSCommandTemplate(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5)
        : parameter1_(p1), parameter2_(p2), parameter3_(p3), parameter4_(p4), parameter5_(p5)
    {}
    virtual ~RSCommandTemplate() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter1_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter2_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter3_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter4_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter5_);
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        T1 parameter1;
        T2 parameter2;
        T3 parameter3;
        T4 parameter4;
        T5 parameter5;
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter1)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter2)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter3)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter4)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter5)) {
            return nullptr;
        }
        return new RSCommandTemplate(parameter1, parameter2, parameter3, parameter4, parameter5);
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        (*processFunc)(context, parameter1_, parameter2_, parameter3_, parameter4_, parameter5_);
    }

private:
    T1 parameter1_;
    T2 parameter2_;
    T3 parameter3_;
    T4 parameter4_;
    T5 parameter5_;
};

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename T1, typename T2, typename T3,
    typename T4, typename T5, typename T6>
class RSCommandTemplate<commandType, commandSubType, processFunc, T1, T2, T3, T4, T5, T6> : public RSCommand {
public:
    RSCommandTemplate(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6)
        : parameter1_(p1), parameter2_(p2), parameter3_(p3), parameter4_(p4), parameter5_(p5), parameter6_(p6)
    {}
    virtual ~RSCommandTemplate() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter1_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter2_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter3_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter4_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter5_);
        success &= RSMarshallingHelper::Marshalling(parcel, parameter6_);
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        T1 parameter1;
        T2 parameter2;
        T3 parameter3;
        T4 parameter4;
        T5 parameter5;
        T6 parameter6;
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter1)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter2)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter3)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter4)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter5)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, parameter6)) {
            return nullptr;
        }
        return new RSCommandTemplate(parameter1, parameter2, parameter3, parameter4, parameter5, parameter6);
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        (*processFunc)(context, parameter1_, parameter2_, parameter3_, parameter4_, parameter5_, parameter6_);
    }

private:
    T1 parameter1_;
    T2 parameter2_;
    T3 parameter3_;
    T4 parameter4_;
    T5 parameter5_;
    T6 parameter6_;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_TEMPLATES_H
