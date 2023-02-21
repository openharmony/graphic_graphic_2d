/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

 /**
 * @file refbase.h
 *
 * @brief Provide smart pointer implemented in c_utils.
 */

 /**
  * @defgroup SmartPointer
  * @{
  * @brief Smart Pointer are pointer-like classes.
  *
  * They simulates a pointer while providing added features,
  * such as automatic memory management.\n
  * Automatic memory management is mainly about deallocating
  * the related memory correctly when an object beyonds its life cycle.
  */

#ifndef UTILS_BASE_REFBASE_H
#define UTILS_BASE_REFBASE_H

#include <atomic>
#include <functional>
#ifdef DEBUG_REFBASE
#include <mutex>
#endif

namespace OHOS {
/**
 * @ingroup SmartPointer
 * @brief A value indicates no strong references exist ever.
 */
#define INITIAL_PRIMARY_VALUE (1 << 28)

class RefBase;

#ifdef DEBUG_REFBASE
class RefTracker;
#endif

/**
 * @ingroup SmartPointer
 * @brief Reference counter. A class records two kinds of count of references to
 * the corresponding RefBase object, and a count of references to the RefCounter
 * itself.
 *
 * There are two different references for a single object.\n
 * Strong Reference holds a reference directly point to the object.
 * Objects which are strong referenced ought to be alive/existed
 * as long as this strong reference exists, thus the reference
 * is still valid.\n
 * Weak Reference holds a reference indirectly point to the object.
 * Objects which are weak referenced are not guaranteed to be alive/existed
 * even if the weak reference exists.
 * @note Descriptions above are valid only when smart pointers
 * are properly used.
 */
class RefCounter {
public:
    /**
     * @brief Callback function to destroy the corresponding RefBase object.
     */
    using RefPtrCallback = std::function<void()>;
    friend class RefBase;

    RefCounter();

    explicit RefCounter(RefCounter *counter);

    RefCounter &operator=(const RefCounter &counter);

    virtual ~RefCounter();

    /**
     * @brief Set the callback function.
     *
     * @param callback A function to be set to delete
     * the corresponding RefBase object.
     */
    void SetCallback(const RefPtrCallback& callback);

    /**
     * @brief Remove the current callback function. Set it to a `nullptr`.
     */
    void RemoveCallback();

    /**
     * @brief Get reference count to the RefCounter object.
     *
     * @return Value of the related count.
     */
    int GetRefCount();

    /**
     * @brief Increment the reference count to the RefCounter object by 1.
     */
    void IncRefCount();

    /**
     * @brief Decrement the reference count to the RefCounter object by 1.
     *
     * Once the count reaches 0 after being decremented,
     * it will call `delete this` to deallocate this RefCounter object.
     */
    void DecRefCount();

    /**
     * @brief Check if pointer to the call back function is a `nullptr`.
     *
     * @return Return true if it is not a `nullptr`.
     */
    bool IsRefPtrValid();

    /**
     * @brief Increment the strong reference count to the
     * corresponding RefBase object by 1.
     *
     * @return Original (before increment) value of the count.
     */
    int IncStrongRefCount(const void *objectId);

    /**
     * @brief Decrement the strong reference count to the
     * corresponding RefBase object by 1.
     *
     * @return Original (before decrement) value of the count.
     * @note If the strong reference has never existed,
     * decrement will cause no effection.
     */
    int DecStrongRefCount(const void *objectId);

    /**
     * @brief Get the strong reference count to the
     * corresponding RefBase object.
     *
     * @return Value of the related count.
     */
    int GetStrongRefCount();

    /**
     * @brief Increment the weak reference count to the
     * corresponding RefBase object by 1.
     *
     * @return Original (before increment) value of the count.
     */
    int IncWeakRefCount(const void *objectId);

    /**
     * @brief Decrement the weak reference count to the
     * corresponding RefBase object by 1.
     *
     * @return Original (before decrement) value of atomicWeak_.
     * @note When the count reaches 0 after being decremented, the
     * corresponding RefBase object with no strong reference ever,
     * or the object with strong reference count of 0 but has not been
     * deallocated due to its extended 'life time', will be deallocated.
     */
    int DecWeakRefCount(const void *objectId);

    /**
     * @brief Get the weak reference count to the
     * corresponding RefBase object.
     *
     * @return Value of the related count.
     */
    int GetWeakRefCount();

    /**
     * @brief Increment the times of attempts to increment.
     */
    void SetAttemptAcquire();

    /**
     * @brief Check if the times of attempts greater than 0.
     *
     * @return Return true if the times is greater than 0.
     */
    bool IsAttemptAcquireSet();

    /**
     * @brief Clear the times of attempts to increment.
     */
    void ClearAttemptAcquire();

    /**
     * @brief Attempt to increment the strong reference count to the
     * corresponding RefBase object by 1.
     *
     * @param outCount If the attempt success, the original value
     * (before increment) of the count will be stored here.
     * @return Return true if the attempt success.
     */
    bool AttemptIncStrongRef(const void *objectId, int &outCount);

    // Only for IPC use.
    /**
     * @brief Attempt to increment the strong reference count to
     * the corresponding RefBase object by 1(Simplified).
     *
     * @return Return true if the attempt success.
     * @note  Ony for IPC use.
     */
    bool AttemptIncStrong(const void *objectId);

    /**
     * @brief Check if the corresponding RefBase object
     * own an extended life-time.
     *
     * @return Return true if its life-time has been extended.
     */
    bool IsLifeTimeExtended();

    /**
     * @brief Extend the life-time of corresponding RefBase object.
     *
     * It allows the corresponding object keep alive
     * even if there exists no strong reference to it.
     * @note Corresponding object will be deallocated
     * when related weak reference count also reach 0.
     */
    void ExtendObjectLifetime();

private:
    std::atomic<int> atomicStrong_; // = (num of sptr) or Initial-value
    std::atomic<int> atomicWeak_; // = (num of sptr)+(num of WeakRefCounter)
    std::atomic<int> atomicRefCount_; // = (num of WeakRefCounter) + 1
    std::atomic<unsigned int> atomicFlags_; // A life-time extended flag
    std::atomic<int> atomicAttempt_; // Times of attempts
    RefPtrCallback callback_ = nullptr; // A callback function to deallocate the corresponding RefBase object
    static constexpr unsigned int FLAG_EXTEND_LIFE_TIME = 0x00000002; // Extended life-time bit to be set via logic-OR
#ifdef DEBUG_REFBASE
    RefTracker* refTracker = nullptr;
    std::mutex trackerMutex;  // To ensure refTracker be thread-safe
    void GetNewTrace(const void* objectId);
    void PrintTracker();
#endif
};

#ifdef DEBUG_REFBASE
// RefTracker is a debug tool, used to record the trace of RefBase.
// RefTracker will save the information about the count of RefBase,
// including the pointer of sptr/wptr(The pointer of itself, not the pointer
// it manages), the amount of strong/weak/refcout and the PID&TID.
// The Tracker can live with RefCounter/RefBase(including its derivation).
// User should keep thread-safety of RefTracker.
class RefTracker {
public:
    RefTracker() {};

    RefTracker(RefTracker* exTracker, const void* id, int strong, int weak, int ref, int pid, int tid);

    void GetTrace(RefTracker* exTracker, const void* id, int strong, int weak, int ref, int pid, int tid);

    // Only used for tracking the amount of Strong Reference.
    void GetStrongTrace(RefTracker* exTracker, const void* id, int strong, int pid, int tid);

    // Only used for tracking the amount of Weak Reference.
    void GetWeakTrace(RefTracker* exTracker, const void* id, int weak, int pid, int tid);

    void PrintTrace(const void* root);

    void PrintStrongTrace(const void* root);

    void PrintWeakTrace(const void* root);

    RefTracker* GetexTrace();

    RefTracker* PopTrace(const void* root);

private:
    const void* ptrID;
    int strongRefCNT;
    int weakRefCNT;
    int refCNT;
    int PID;
    int TID;
    RefTracker* exTrace;
};
#endif

/**
 * @ingroup SmartPointer
 * @brief An intermediate class to represent the weak reference
 * to the correspond RefBase object.
 *
 * A WeakRefCounter object can be held by multiple wptr objects.\n
 * It holds references to the corresponding RefBase and RefCounter object.
 * Those two references will be set as `nullptr`s, when the weak referenced
 * target and its RefCounter object has been deallocated. Thus WeakRefCounter
 * object can still alive even if the target refereneced by this object
 * is vanished.
 */
class WeakRefCounter {
public:
    /**
     * @brief Construct a new Weak Ref Counter object.
     *
     * @param counter Pointer to corresponding RefCounter object.
     * @param cookie Pointer to corresponding RefBase object.
     */
    WeakRefCounter(RefCounter *counter, void *cookie);

    virtual ~WeakRefCounter();

    /**
     * @brief Get current pointer to the corresponding RefBase object.
     *
     * @return A void pointer to the RefBase object.
     * If the corresponding object does not alive, a `nullptr` will return.
     * @note Void pointer means you should cast it to the real type, since it
     * can be kinds of subclasses of RefBase.
     */
    void *GetRefPtr();

    /**
     * @brief Increment the reference count to this WeakRefCounter object.
     *
     * @note Notice the difference between this count and the weak reference
     * count in RefCounter. This value equals to the number of wptrs directly
     * referencing this WeakRefCount object.
     * @see RefCounter
     */
    void IncWeakRefCount(const void *objectId);

    /**
     * @brief Decrement the reference count to this WeakRefCounter object.
     *
     * @note This WeakRefCounter object will be deallocated when this count
     * reaches 0.
     */
    void DecWeakRefCount(const void *objectId);

    /**
     * @brief Get the count recorded by this WeakRefCounter object.
     *
     * @return Value of the count.
     * @note This value of count is different from that in RefCounter.
     * @see RefCounter::GetWeakRefCount()
     */
    int GetWeakRefCount() const;

    /**
     * @brief Attempt to increment the strong reference count of
     * the corresponding RefBase object(Used in promoting a wptr to a sptr).
     *
     * @return Return `true` after a success increment.
     */
    bool AttemptIncStrongRef(const void *objectId);

private:
    std::atomic<int> atomicWeak_; // Count of references to this WeakRefCounter object
                                  // The value equals to the total amount of wptrs which
                                  // reference this WeakRefCounter object
    RefCounter *refCounter_ = nullptr; // reference to the RefCounter object of corresponding RefBase Object
    void *cookie_ = nullptr; // Pointer to the corresponding RefBase object
};

/**
 * @ingroup SmartPointer
 * @brief A base class which can be managed by a smart pointer.
 *
 * @note All classes which intend to be managed by smart pointers should be 
 * derived from RefBase.
 */
class RefBase {
public:
    RefBase();

    /**
     * @brief Copy constructor of RefBase.
     *
     * @note  Note that this method will construct a new RefCounter object,
     * and bind with it.
     */
    RefBase(const RefBase &);

    /**
     * @brief Copy assignment operator of RefBase.
     *
     * @note This method will unbind the current RefBase object and its
     * original RefCounter object, then bind a newly constructed
     * RefCounter object.
     */
    RefBase &operator=(const RefBase &);

    /**
     * @brief Move constructor of RefBase.
     */
    RefBase(RefBase &&other) noexcept;

    /**
     * @brief Move assignment operator of RefBase.
     *
     * @note  This method will bind this RefBase object with the RefCounter
     * object of the argument `other`, then `other` will unbind the RefCounter
     * object.\n No counts operation will be poccessed.
     */
    RefBase &operator=(RefBase &&other) noexcept;

    virtual ~RefBase();

    /**
     * @brief A callback method to deallocate this object.
     *
     * This method has default implement to simply deallocate this RefBase
     * object simply by calling `delete(this)`.
     */
    virtual void RefPtrCallback();

    /**
     * @brief Extend life time of the RefBase object.
     *
     * @note The object whose life time has been extended will not be
     * deallocated when the the weak reference count reach 0 instead of the
     * strong one.
     */
    void ExtendObjectLifetime();

    /**
     * @brief Increment the strong reference count.
     *
     * `OnFirstStrongRef()`, which is an empty method by default, will be
     * called when the first strong reference are established.、
     *
     * @note It will atomically increment the weak reference count meanwhile.
     */
    void IncStrongRef(const void *objectId);

    /**
     * @brief Decrement the strong reference count.
     *
     * This object will be deallocated when the count reaches 0, if it owns a
     * normal life time.\n `OnLastStrongRef()`, which is an empty method by
     * default, will be called when the last strong reference vanishes.
     */
    void DecStrongRef(const void *objectId);

    /**
     * @brief Get the strong reference count.
     *
     * @return Related count value. Return 0 when corresponding RefCounter
     * object does not exist.
     * @note Only valid when corresponding RefCounter object exists.
     */
    int GetSptrRefCount();

   /**
    * @brief Create weak reference to this RefBase object.
    *
    * Create a WeakRefCounter object which holds reference to this RefBase
    * object and set the reference count.
    *
    * @param cookie Void pointer to this RefBase object.
    * @return Pointer to the newly created WeakRefCounter object.
    * @note Avoid using it independently. Use related methods of wptr.
    */
    WeakRefCounter *CreateWeakRef(void *cookie);

    /**
     * @brief Get the pointer to corresponding counter object.
     *
     * @return Pointer to the counter object.
     */
    RefCounter *GetRefCounter() const;

    /**
     * @brief Increment the weak reference count.
     *
     * @note Only valid when corresponding RefCounter object exists.
     */
    void IncWeakRef(const void *objectId);

    /**
     * @brief Decrement the weak reference count.
     *
     * @note Only valid when corresponding RefCounter object exists.
     */
    void DecWeakRef(const void *objectId);

    /**
     * @brief Get the weak reference count.
     *
     * @return Value of related count. Return 0 when corresponding
     * RefCounter object doesn't exist.
     */
    int GetWptrRefCount();

    /**
     * @brief Attempt to increment the strong reference count.
     *
     * `OnFirstStrongRef()`, which is an empty method by default, will be
     * called when the first strong reference are established.
     *
     * @return Return true if successfully increment the count.
     * @note Note that count of times of attempts will increment by 1
     * after a successful increment.
     */
    bool AttemptAcquire(const void *objectId);

    /**
     * @brief Attempt to increment the strong reference count.
     *
     * `OnFirstStrongRef()`, which is an empty method by default, will be
     * called when the first strong reference are established.
     * @return Return true if successfully increment the count.
     * @note Used in various copy constructor of sptr in scenario of
     * interaction between sptr and wptr. Avoid using it independently.
     */
    bool AttemptIncStrongRef(const void *objectId);

    // Only for IPC use.
    /**
     * @brief Attempt to increment the strong reference count.
     *
     * @return Return true if successfully increment the count, otherwise
     * return false.
     * @note Note that times of successful attempts will increment by 1 after
     * a successful increment of the related count.
     * @note This method is a simplified version of `AttemptAcquire`, but only
     * for IPC use.
     */
    bool AttemptIncStrong(const void *objectId);

    /**
     * @brief check if the times of successful attempts of greater than 0.
     *
     * @return Return true if times of successful attempts is greater than 0;
     * Return false if it is not greater than 0, or the corresponding
     * RefCounter object does not exist.
     */
    bool IsAttemptAcquireSet();

    /**
     * @brief Check if the life time of this RefBase object has been extended.
     *
     * @return Return false when have a normal life time, or the corresponding
     * RefCounter object does not exist.
     */
    bool IsExtendLifeTimeSet();

    /**
     * @brief An event-drive method, which will be automatically called when
     * first strong reference comes up.
     *
     * @note It is an empty function by default.
     */
    virtual void OnFirstStrongRef(const void *);

    /**
     * @brief An event-drive method, which will be automatically called when
     * last strong reference eliminates.
     *
     * @note It is an empty function by default.
     */
    virtual void OnLastStrongRef(const void *);

    /**
     * @brief An event-drive method, which will be automatically called when
     * last weak reference eliminates.
     *
     * @note It is an empty function by default.
     */
    virtual void OnLastWeakRef(const void *);

    /**
     * @brief An event-drive method, which will be autmatically called when
     * use `wptr::Promote()`.
     *
     * @note Directly return true by default.
     * @return Return true if success, otherwise return false.
     */
    virtual bool OnAttemptPromoted(const void *);

private:
    RefCounter *refs_ = nullptr; // Pointer to the corresponding reference counter of this RefBase object
};

template <typename T>
class wptr;

/**
 * @ingroup SmartPointer
 * @brief Strong reference smart pointer to a RefBase(or its subclass) object.
 *
 * It directly reference the RefBase object.
 *
 * @tparam T Specific class type managed by sptr. This class must inherit
 * from RefBase.
 */
template <typename T>
class sptr {
    friend class wptr<T>;

public:
    sptr();

    ~sptr();

    /**
     * @brief Constructor with specified object to be managed.
     *
     * @note Null sptr will be created if `other` is a `nullptr`.
     * @param other Object to be managed by wptr.
     */
    sptr(T *other);

    /**
     * @brief Copy Constructor for sptr with different managed class type(T).
     *
     * @param other Input sptr object.
     */
    sptr(const sptr<T> &other);

    /**
     * @brief Move constructor.
     *
     * @note `other` will be set to a null sptr.
     * @param other Input sptr object.
     */
    sptr(sptr<T> &&other);

    /**
     * @brief Move assignment operator.
     *
     * @param other Input sptr object.
     * @note Original strong reference in target sptr object will be removed.
     */
    sptr<T> &operator=(sptr<T> &&other);

    /**
     * @brief Copy Constructor for sptr with different managed class type(O).
     *
     * @tparam O Another specific class type managed by `other`.
     * @param other Input sptr object.
     */
    template <typename O>
    sptr(const sptr<O> &other);

    /**
     * @brief Constructor only used in promote process of wptr.
     *
     * @param p WeakRefCounter object which hold the reference to the
     * managed object.
     * @param force Only used to identify from other constructor.
     */
    inline sptr(WeakRefCounter *p, bool force);

    /**
     * @brief Get the pointer to the managed object.
     *
     * @return Pointer of the specific managed class type.
     */
    inline T *GetRefPtr() const
    {
        return refs_;
    }

    /**
     * @brief Set the pointer to the managed object.
     *
     * @param other Another pointer object to be managed by sptr.
     * @note Avoid using independently, otherwise it will
     * cause mismatch of reference count and thus memory problems.
     */
    inline void ForceSetRefPtr(T *other);

    /**
     * @brief Remove the reference to the managed object held by current sptr.
     *
     * @note It will make this sptr a "null sptr".
     */
    void clear();

    /**
     * @brief Type conversion operator.
     *
     * @return Raw pointer to the managed object.
     * @note Sptr object itself will not be converted, only the member raw
     * pointer returns.
     */
    inline operator T *() const
    {
        return refs_;
    }

    /**
     * @brief Dereference operator.
     *
     * It will return the object managed by this sptr.
     *
     * @return Return reference of specific object managed by sptr.
     */
    inline T &operator*() const
    {
        return *refs_;
    }

    /**
     * @brief Member selection operator.
     *
     * It will return the specified member of the object managed by this sptr.
     */
    inline T *operator->() const
    {
        return refs_;
    }

    /**
     * @brief Logical-NOT operator. Check if sptr is a "null sptr".
     *
     * @return Return true if sptr is a "null sptr".
     */
    inline bool operator!() const
    {
        return refs_ == nullptr;
    }

    /**
     * @brief Copy assignment operator with specified object to be managed.
     *
     * @note Original reference will be removed, then new reference to the
     * input object will be established.
     * @param other Another object to be managed by this sptr.
     */
    sptr<T> &operator=(T *other);

    /**
     * @brief Copy assignment operator for sptr with
     * same managed class type(T).
     *
     * @note Original reference will be removed, this sptr will manage the
     * same object with the input sptr object.
     * @param other Another sptr object with same managed class type(T).
     */
    sptr<T> &operator=(const sptr<T> &other);

    /**
     * @brief Copy assignment operator for wptr with
     * same managed class type(T).
     *
     * @note Original reference will be removed, this sptr will manage the
     * same object with the input wptr object.
     * @note This may fail, then this sptr will turn to be a "null sptr".
     * @param other Another wptr object with same managed class type(T).
     */
    sptr<T> &operator=(const wptr<T> &other);

    /**
     * @brief Copy assignment operator for sptr with
     * different managed class type(O).
     *
     * @note Original reference will be removed, this sptr will manage the
     * same object with the input sptr object.
     * @note This sptr will interpret the managed object as a type T.
     * @param other Another sptr object with different managed class type(O).
     */
    template <typename O>
    sptr<T> &operator=(const sptr<O> &other);

    /**
     * @brief Equal-to operator between sptr and a raw pointer.
     *
     * @param other Input raw pointer.
     * @return Return true if sptr point to the same object with input
     * raw pointer.
     */
    bool operator==(const T *other) const;

    /**
     * @brief Not-equal-to operator between sptr and a raw pointer.
     *
     * @param other Input raw pointer.
     * @return Return true if sptr does not point to the same object with input
     * raw pointer.
     */
    inline bool operator!=(const T *other) const
    {
        return !operator==(other);
    }

    /**
     * @brief Equal-to operator between sptr and a wptr.
     *
     * @param other Input wptr.
     * @return Return true if sptr and wptr are managing same object.
     */
    bool operator==(const wptr<T> &other) const;

    /**
     * @brief Not-equal-to operator between sptr and a wptr.
     *
     * @param other Input wptr.
     * @return Return true if sptr and wptr are not managing same object.
     */
    inline bool operator!=(const wptr<T> &other) const
    {
        return !operator==(other);
    }

    /**
     * @brief Equal-to operator between sptrs.
     *
     * @param other Input sptr.
     * @return Return true if two sptrs are managing same object.
     */
    bool operator==(const sptr<T> &other) const;

    /**
     * @brief Not-equal-to operator between sptrs.
     *
     * @param other Input sptr.
     * @return Return true if two sptrs are not managing same object.
     */
    inline bool operator!=(const sptr<T> &other) const
    {
        return !operator==(other);
    }

private:
    T *refs_ = nullptr; // Raw pointer to the managed specific object
};

template <typename T>
inline void sptr<T>::ForceSetRefPtr(T *other)
{
    refs_ = other;
}

template <typename T>
inline sptr<T>::sptr()
{
    refs_ = nullptr;
}

template <typename T>
inline sptr<T>::sptr(T *other)
{
    refs_ = other;
    if (refs_ != nullptr) {
        refs_->IncStrongRef(this);
    }
}

template <typename T>
inline sptr<T>::sptr(const sptr<T> &other)
{
    refs_ = other.GetRefPtr();
    if (refs_ != nullptr) {
        refs_->IncStrongRef(this);
    }
}

template <typename T>
sptr<T>::sptr(sptr<T> &&other)
{
    refs_ = other.GetRefPtr();
    other.ForceSetRefPtr(nullptr);
}

template <typename T>
sptr<T> &sptr<T>::operator=(sptr<T> &&other)
{
    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }
    refs_ = other.GetRefPtr();
    other.ForceSetRefPtr(nullptr);
    return *this;
}

template <typename T>
template <typename O>
sptr<T>::sptr(const sptr<O> &other) : refs_(other.GetRefPtr())
{
    if (refs_ != nullptr) {
        refs_->IncStrongRef(this);
    }
}

template <typename T>
inline sptr<T> &sptr<T>::operator=(T *other)
{
    if (other != nullptr) {
        other->IncStrongRef(this);
    }

    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }

    refs_ = other;
    return *this;
}

template <typename T>
inline sptr<T> &sptr<T>::operator=(const sptr<T> &other)
{
    T *otherRef(other.GetRefPtr());
    if (otherRef != nullptr) {
        otherRef->IncStrongRef(this);
    }

    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }

    refs_ = otherRef;
    return *this;
}

template <typename T>
inline sptr<T> &sptr<T>::operator=(const wptr<T> &other)
{
    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }
    if ((other != nullptr) && other.AttemptIncStrongRef(this)) {
        refs_ = other.GetRefPtr();
    } else {
        refs_ = nullptr;
    }

    return *this;
}

template <typename T>
template <typename O>
sptr<T> &sptr<T>::operator=(const sptr<O> &other)
{
    T *otherRef(other.GetRefPtr());
    if (otherRef != nullptr) {
        otherRef->IncStrongRef(this);
    }

    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }

    refs_ = otherRef;
    return *this;
}

template <typename T>
inline bool sptr<T>::operator==(const T *other) const
{
    return other == refs_;
}

template <typename T>
inline bool sptr<T>::operator==(const wptr<T> &other) const
{
    return refs_ == other.GetRefPtr();
}

template <typename T>
inline bool sptr<T>::operator==(const sptr<T> &other) const
{
    return refs_ == other.GetRefPtr();
}

template<typename T>
void sptr<T>::clear()
{
    if (refs_) {
        refs_->DecStrongRef(this);
        refs_ = 0;
    }
}

template <typename T>
inline sptr<T>::~sptr()
{
    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }
}

template <typename T>
inline sptr<T>::sptr(WeakRefCounter *p, bool /* force */)
{
    if ((p != nullptr) && p->AttemptIncStrongRef(this)) {
        refs_ = reinterpret_cast<T *>(p->GetRefPtr());
    } else {
        refs_ = nullptr;
    }
}

/**
 * @ingroup SmartPointer
 * @brief Weak reference smart pointer to a RefBase(or its subclass) object.
 *
 * Indirectly reference the RefBase object;
 * Directly reference the WeakRefCounter object.
 *
 * @tparam T Specific class type managed by wptr.
 * This class must inherit from RefBase.
 */
template <typename T>
class wptr {
    template <typename O>
    friend class wptr;

public:
    wptr();

    /**
     * @brief Constructor with specified object to be managed.
     *
     * This method will create WeakRefCounter object for `other` and set its
     * weak reference count to 1.
     *
     * @note WeakRefCounter object will not be created if `other` is a
     * `nullptr`.
     * @param other Object to be managed by wptr.
     */
    wptr(T *other);

    /**
     * @brief Copy constructor for wptr with same managed class type(T).
     *
     * This method will share the WeakRefCounter object of `other` with this
     * wptr. Weak reference count in this WeakRefCounter object will be set
     * properly.
     *
     * @param other Another wptr with same managed class type(T).
     */
    wptr(const wptr<T> &other);

    /**
     * @brief Copy constructor for sptr with same managed class type(T).
     *
     * This method will create WeakRefCounter object for the managed object of
     * `other`, and set its weak reference count properly.
     *
     * @param other Another sptr with same managed class type(T).
     * @tparam T Specific class type managed by `other`.
     */
    wptr(const sptr<T> &other);

    /**
     * @brief Copy constructor for wptr with different managed class type(O).
     *
     * Same with wptr<T>::wptr(const wptr<T> &other).
     *
     * @tparam O Class type managed by `other`.
     * @param other Another wptr with different managed class type(O).
     * @tparam T Specific class type managed by `other`.
     */
    template <typename O>
    wptr(const wptr<O> &other);

    /**
     * @brief Copy constructor for sptr with different managed class type(O).
     *
     * Same with wptr<T>::wptr(const sptr<T> &other).
     *
     * @param other Another sptr with same managed class type(O).
     * @tparam T Specific class type managed by `other`.
     */
    template <typename O>
    wptr(const sptr<O> &other);

    /**
     * @brief Copy assignment operator with specified object to be managed.
     *
     * @note Current wptr will unbind the original WeakRefCounter object and
     * create a new WeakRefCounter object, then set its weak reference count
     * properly.
     * @param other Another object to be managed by this wptr.
     */
    wptr<T> &operator=(T *other);

    /**
     * @brief Copy assignment operator with specified object to be managed.
     *
     * @note Same with wptr<T> &operator=(T *other), but a pointer type casting
     * which will not affect the type of `*other` is proccessed.
     * @tparam O Specific class type managed by `other`.
     * @param other Another object to be managed by this wptr.
     *
     */
    template <typename O>
    wptr<T> &operator=(O *other);

    /**
     * @brief Copy assignment operator for wptr with same managed class type(T).
     *
     * @note Current wptr will unbind the original WeakRefCounter object and
     * share the WeakRefCounter object with `other`, then set its weak
     * reference count properly.
     * @param other Another wptr. Object managed by it will also be managed by
     * this wptr.
     */
    wptr<T> &operator=(const wptr<T> &other);

    /**
     * @brief Copy assignment operator for sptr with
     * same managed class type(T).
     *
     * @note Current wptr will unbind the original WeakRefCounter object and
     * create a new WeakRefCounter object, then set its weak reference count
     * properly.
     * @param other A sptr object. Object managed by it will also be managed by
     * this wptr.
     */
    wptr<T> &operator=(const sptr<T> &other);

    /**
     * @brief Copy assignment operator for wptr with
     * different managed class type(O).
     *
     * @note Same with wptr<T> &operator=(const wptr<T> &). Note that no cast
     * here is proccessed.
     * @param other An wptr object. Object managed by it will also be managed by
     * this wptr.
     * @tparam O Specific class type managed by `other`.
     */
    template <typename O>
    wptr<T> &operator=(const wptr<O> &other);

    /**
     * @brief Copy assignment operator for sptr with
     * different managed class type(O).
     *
     * @note Same with wptr<T> &wptr<T>::operator=(const sptr<T> &). Note that
     * no cast here is proccessed.
     * @param other An sptr object. Object managed by it will also be managed by
     * this wptr.
     * @tparam O Specific class type managed by `other`.
     */
    template <typename O>
    wptr<T> &operator=(const sptr<O> &other);

    /**
     * @brief Dereference operator. It will return the object managed by this
     * wptr.
     *
     * @return Return specific object managed by wptr.
     */
    inline T &operator*() const
    {
        return *GetRefPtr();
    }

    /**
     * @brief Member selection operator.
     *
     * It will return the specified member of the object managed by this wptr.
     */
    inline T *operator->() const
    {
        return reinterpret_cast<T *>(refs_->GetRefPtr());
    }

    /**
     * @brief Equal-to operator between wptr and a raw pointer.
     *
     * @param other Input raw pointer.
     * @return Return true if two pointers have same value.
     */
    bool operator==(const T *other) const;

    /**
     * @brief Not-equal-to operator between wptr and a raw pointer.
     *
     * @param other Input raw pointer.
     * @return Return true if two pointers have different value.
     */
    inline bool operator!=(const T *other) const
    {
        return !operator==(other);
    };

    /**
     * @brief Equal-to operator between two wptrs.
     *
     * @param other Input reference of a wptr object.
     * @return Return if two pointers have same value.
     */
    bool operator==(const wptr<T> &other) const;

    /**
     * @brief Not-equal-to operator between two wptrs.
     *
     * @param other Input reference of a wptr object.
     * @return Return if two pointers have different value.
     */
    inline bool operator!=(const wptr<T> &other) const
    {
        return !operator==(other);
    }

    /**
     * @brief Equal-to operator between wptr and a input sptr object.
     *
     * @param other Input reference of an sptr object.
     * @return Comparison result.
     */
    bool operator==(const sptr<T> &other) const;

    /**
     * @brief Not-Equal-to operator between wptr and a input sptr object.
     *
     * @param other Input reference of an sptr object.
     * @return Comparison result.
     */
    inline bool operator!=(const sptr<T> &other) const
    {
        return !operator==(other);
    }

    /**
     * @brief Get the pointer to the RefBase object.
     *
     * @return Raw pointer to the RefBase object.
     * @note Return `nullptr` if the managed object has been deallocated.
     */
    T *GetRefPtr() const;

    /**
     * @brief Get the count value of corresponding WeakRefCounter object.
     *
     * The value indicates how many wptrs share the same WeakRefCounter object.
     *
     * @return Value of the count.
     * @note Only for test.
     */
    inline int GetWeakRefCount() const
    {
        return refs_->GetWeakRefCount();
    }

    /**
     * @brief Attempt to increment the strong reference count of
     * the managed object.
     *
     * @return Return true after a success increment.
     * @note Avoid using it independently. Use `promote()`.
     */
    inline bool AttemptIncStrongRef(const void *objectId) const
    {
        return refs_->AttemptIncStrongRef(objectId);
    }

    /**
     * @brief Promote a wptr to an sptr.
     *
     * It will create an sptr object based on
     * object managed by this wptr.
     *
     * @note Original weak reference will be retained. Promote may fail,
     * and then return a "null sptr".
     */
    const sptr<T> promote() const;

    ~wptr();

private:
    WeakRefCounter *refs_ = nullptr; // Pointer to the corresponding weak reference counter object
};

template <typename T>
inline T *wptr<T>::GetRefPtr() const
{
    return (refs_ != nullptr) ? reinterpret_cast<T *>(refs_->GetRefPtr()) : nullptr;
}

template <typename T>
wptr<T>::wptr()
{
    refs_ = nullptr;
}

template <typename T>
wptr<T>::wptr(T *other)
{
    if (other != nullptr) {
        refs_ = other->CreateWeakRef(other);
        if (refs_ != nullptr) {
            refs_->IncWeakRefCount(this);
        }
    } else {
        refs_ = nullptr;
    }
}

template <typename T>
wptr<T>::wptr(const wptr<T> &other)
{
    refs_ = other.refs_;
    if (refs_ != nullptr) {
        refs_->IncWeakRefCount(this);
    }
}

template <typename T>
wptr<T>::wptr(const sptr<T> &other)
{
    if (other.GetRefPtr() != nullptr) {
        refs_ = other->CreateWeakRef(other.GetRefPtr());
        if (refs_ != nullptr) {
            refs_->IncWeakRefCount(this);
        }
    }
}

template <typename T>
template <typename O>
wptr<T>::wptr(const wptr<O> &other)
{
    refs_ = other.refs_;
    if (refs_ != nullptr) {
        refs_->IncWeakRefCount(this);
    }
}

template <typename T>
template <typename O>
wptr<T>::wptr(const sptr<O> &other)
{
    if (other.GetRefPtr() != nullptr) {
        refs_ = other->CreateWeakRef(other.GetRefPtr());
        if (refs_ != nullptr) {
            refs_->IncWeakRefCount(this);
        }
    }
}

template <typename T>
wptr<T> &wptr<T>::operator=(T *other)
{
    WeakRefCounter *newWeakRef = nullptr;
    if (other != nullptr) {
        newWeakRef = other->CreateWeakRef(other);
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
template <typename O>
wptr<T> &wptr<T>::operator=(O *other)
{
    T *object = reinterpret_cast<T *>(other);
    WeakRefCounter *newWeakRef = nullptr;
    if (object != nullptr) {
        newWeakRef = object->CreateWeakRef(object);
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
inline wptr<T> &wptr<T>::operator=(const wptr<T> &other)
{
    if (other.refs_ != nullptr) {
        other.refs_->IncWeakRefCount(this);
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = other.refs_;
    return *this;
}

template <typename T>
inline wptr<T> &wptr<T>::operator=(const sptr<T> &other)
{
    WeakRefCounter *newWeakRef = nullptr;
    if (other.GetRefPtr() != nullptr) {
        newWeakRef = other->CreateWeakRef(other.GetRefPtr());
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
template <typename O>
wptr<T> &wptr<T>::operator=(const wptr<O> &other)
{
    if (other.refs_ != nullptr) {
        other.refs_->IncWeakRefCount(this);
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = other.refs_;
    return *this;
}

template <typename T>
template <typename O>
wptr<T> &wptr<T>::operator=(const sptr<O> &other)
{
    WeakRefCounter *newWeakRef = nullptr;
    if (other.GetRefPtr() != nullptr) {
        newWeakRef = other->CreateWeakRef(other.GetRefPtr());
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
inline bool wptr<T>::operator==(const T *other) const
{
    return GetRefPtr() == other;
}

template <typename T>
inline bool wptr<T>::operator==(const wptr<T> &other) const
{
    return GetRefPtr() == other.GetRefPtr();
}

template <typename T>
inline bool wptr<T>::operator==(const sptr<T> &other) const
{
    return GetRefPtr() == other.GetRefPtr();
}

template <typename T>
inline const sptr<T> wptr<T>::promote() const
{
    return sptr<T>(refs_, true);
}

template <typename T>
inline wptr<T>::~wptr()
{
    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }
}

} // namespace OHOS

#endif

/**@}*/
