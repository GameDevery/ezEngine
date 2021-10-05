#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>
#include <Foundation/Types/SharedPtr.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHI_LIB
#    define EZ_RHI_DLL __declspec(dllexport)
#  else
#    define EZ_RHI_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHI_DLL
#endif

#define EZ_ENUM_CONVERSION_OPERATOR(T) \
  inline std::underlying_type<T>::type ToNumber(const T v) { return static_cast<std::underlying_type<T>::type>(v); }

#define EZ_ENUM_BITWISE_OPERATORS(T)                                                                                                                                         \
  inline T operator~(const T v) { return static_cast<T>(~static_cast<std::underlying_type<T>::type>(v)); }                                                                   \
  inline bool operator||(const T lhs, const T rhs) { return (static_cast<std::underlying_type<T>::type>(lhs) || static_cast<std::underlying_type<T>::type>(rhs)); }          \
  inline bool operator&&(const T lhs, const T rhs) { return (static_cast<std::underlying_type<T>::type>(lhs) && static_cast<std::underlying_type<T>::type>(rhs)); }          \
  inline T operator|(const T lhs, const T rhs) { return static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) | static_cast<std::underlying_type<T>::type>(rhs)); } \
  inline T operator&(const T lhs, const T rhs) { return static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) & static_cast<std::underlying_type<T>::type>(rhs)); } \
  inline T operator^(const T lhs, const T rhs) { return static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) ^ static_cast<std::underlying_type<T>::type>(rhs)); } \
  inline T operator+(const T lhs, const T rhs) { return static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) + static_cast<std::underlying_type<T>::type>(rhs)); } \
  inline T operator+(const T lhs, bool rhs) { return static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) + rhs); }                                                \
  inline void operator|=(T& lhs, const T rhs) { lhs = static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) | static_cast<std::underlying_type<T>::type>(rhs)); }   \
  inline void operator&=(T& lhs, const T rhs) { lhs = static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) & static_cast<std::underlying_type<T>::type>(rhs)); }   \
  inline void operator^=(T& lhs, const T rhs) { lhs = static_cast<T>(static_cast<std::underlying_type<T>::type>(lhs) ^ static_cast<std::underlying_type<T>::type>(rhs)); }   \
  inline bool HasFlag(const T flags, const T flagToTest)                                                                                                                     \
  {                                                                                                                                                                          \
    using ValueType = std::underlying_type<T>::type;                                                                                                                         \
    EZ_ASSERT_DEV((static_cast<ValueType>(flagToTest) & (static_cast<ValueType>(flagToTest) - 1)) == 0, "More than one flag specified");                                     \
    return (static_cast<ValueType>(flags) & static_cast<ValueType>(flagToTest)) != 0;                                                                                        \
  }                                                                                                                                                                          \
  inline bool HasAnyFlags(const T flags, const T flagsToTest)                                                                                                                \
  {                                                                                                                                                                          \
    using ValueType = std::underlying_type<T>::type;                                                                                                                         \
    return (static_cast<ValueType>(flags) & static_cast<ValueType>(flagsToTest)) != 0;                                                                                       \
  }                                                                                                                                                                          \
  inline bool HasAllFlags(const T flags, const T flagsToTest)                                                                                                                \
  {                                                                                                                                                                          \
    using ValueType = std::underlying_type<T>::type;                                                                                                                         \
    return (static_cast<ValueType>(flags) & static_cast<ValueType>(flagsToTest)) == static_cast<ValueType>(flagsToTest);                                                     \
  }                                                                                                                                                                          \
  inline T AddFlags(const T flags, const T flagsToAdd) { return (flags | flagsToAdd); }                                                                                      \
  inline T RemoveFlags(const T flags, const T flagsToRemove) { return (flags & ~flagsToRemove); }                                                                            \
  EZ_ENUM_CONVERSION_OPERATOR(T)









template <typename Actor>
class EZ_RHI_DLL ezRHIAgent : public Actor
{
public:
  ezRHIAgent() noexcept = delete;

  explicit ezRHIAgent(Actor* const pActor) noexcept
    : Actor()
    , m_pActor(pActor)
  {
  }

  ~ezRHIAgent() override = default;

  ezRHIAgent(ezRHIAgent const&) = delete;

  ezRHIAgent(ezRHIAgent&&) = delete;

  ezRHIAgent& operator=(ezRHIAgent const&) = delete;

  ezRHIAgent& operator=(ezRHIAgent&&) = delete;

  inline Actor* GetActor() const noexcept { return m_pActor; }

protected:
  Actor* m_pActor{nullptr};
};
