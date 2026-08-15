#pragma once
#include <vector>

namespace Core
{
    using Byte = std::byte;
    using ByteBuffer = std::vector<Byte>;

    //static 변수가 한번만 만들어진다는 것과 주소가 다르다는 성질을 이용해서 이 클래스가 어떤 클래스인지 알게끔 하는 코드.
    using TypeID = size_t;
    inline constexpr TypeID InvalidTypeID = 0;

    template<typename T>
    TypeID GetTypeID()
    {
        static const char unique;
        return reinterpret_cast<TypeID>(&unique);
    }
}

// 상속 계층의 "루트"에서 쓰는 매크로
#define CORE_DECLARE_TYPE_ROOT(Type)                        \
public:                                                      \
    static Core::TypeID StaticTypeID()                       \
    {                                                         \
        return Core::GetTypeID<Type>();                       \
    }                                                         \
    virtual Core::TypeID GetTypeID() const                    \
    {                                                          \
        return StaticTypeID();                                 \
    }                                                          \
    virtual bool IsKindOf(Core::TypeID id) const                \
    {                                                            \
        return id == StaticTypeID();                             \
    }

// 파생 타입에서 쓰는 매크로 (Base를 명시)
#define CORE_DECLARE_TYPE(Type, Base)                        \
public:                                                        \
    static Core::TypeID StaticTypeID()                         \
    {                                                            \
        return Core::GetTypeID<Type>();                          \
    }                                                             \
    virtual Core::TypeID GetTypeID() const override                \
    {                                                               \
        return StaticTypeID();                                      \
    }                                                                \
    virtual bool IsKindOf(Core::TypeID id) const override             \
    {                                                                   \
        if (id == StaticTypeID()) return true;                         \
        return Base::IsKindOf(id);                                      \
    }

//값과 값이 없는 표현으로 nullopt가 '꼭' 존재해야 하는 부분에만 적용해야함.
//데이터가 큰 경우 &로 보내고 싶을때 이것을 사용. const optinalRef<data>& 이렇게 사용.
//안에 데이터는 &로 날라가기 때문에 값복사 안됨. 인자로 data를 날리면 optionRef 임시객체는 생성되지만,
//const optionalRef<data>& 인 경우에는 optionalRef로는 복사안됨. &가 없으면 optionalRef가 복사됨.
// 이 함수에서 endPos를 안 넣었을때 startPos를 하고 싶은ㄷ endPos = startPos 이렇게 디폴트 인자로 쓸수 없을때 유용
//리턴할때 큰 데이터를 리턴하면 &로 보내고 싶어지는데 이때 optionalRef로 해서 값이 없음을 표현할 수 있다.
//아니면 static data empty; 를 만들어서 리턴해야하는데 이건 이상하니까.
template <typename T>
using optionalRef = std::optional<std::reference_wrapper<const T>>;