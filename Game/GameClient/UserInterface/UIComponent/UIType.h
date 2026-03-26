#pragma once

enum class ComponentID
{
	Panel,
	PatchTextureStd1,
	PatchTextureStd3,
	PatchTextureStd9,
	PatchTextureLite1,
	PatchTextureLite3,
	PatchTextureLite9,
	Button,
	TextArea,
	Dialog,
	ListArea,
	Container,
	RenderTexture,
	ScrollBar,
	TextureSwitcher,
	UIModuleAsComponent,
	Unknown, //부모 기본 UIComponent. 이게 문제가 되면 UICompnent가 혼자서 생성되었다는 건데, 생성자가 protected에서 public이 되었을 것이다. 아니면 enum에서 추가를 안시켰던지.
	Count
};

template<>
inline constexpr auto EnumUtil::EnumToStringMap<ComponentID> = std::array{
	"Panel",
	"PatchTextureStd1",
	"PatchTextureStd3",
	"PatchTextureStd9",
	"PatchTextureLite1",
	"PatchTextureLite3",
	"PatchTextureLite9",
	"Button",
	"TextArea",
	"Dialog",
	"ListArea",
	"Container",
	"RenderTexture",
	"ScrollBar",
	"TextureSwitcher",
	"UIModuleAsComponent",
	"Unknown"
};
ASSERT_ENUM_COUNT(ComponentID);

enum class TextureSlice : int
{
	One,
	ThreeH, //Horizontal
	ThreeV, //Vertical
	Nine,
	Count
};

///////////////////////////////////////////////////////////////

enum class Origin
{
	LeftTop,
	RightTop,
	Center,
	Count
};

template<>
inline constexpr auto EnumUtil::EnumToStringMap<Origin > = std::array{
	"LeftTop",
	"RightTop",
	"Center"
};
ASSERT_ENUM_COUNT(Origin);

///////////////////////////////////////////////////////////////

enum class InteractState : int //이미지같은 보여주는 상태 변화 InputKeyState는 키의 상태 변화이기 때문에 비슷해 보여도 둘이 다르다.
{
	Normal,
	Hovered,
	Pressed,
	Count
};

template<>
inline constexpr auto EnumUtil::EnumToStringMap<InteractState> = std::array{
	"Normal",
	"Hovered",
	"Pressed"
};
ASSERT_ENUM_COUNT(InteractState);

///////////////////////////////////////////////////////////////

//렌더링시 탐색을 바꿔가면서 해야한다. 그렇지 않으면 이미지가 덮혀지거나 순서가 꼬이게 된다.
enum class RenderTraversal
{
	Inherited, //윗 노드가 사용한 방식을 따라서. 기본설정. 이 설정이면 BFS가 돌아간다.
	BFS,
	DFS,
	Count
};

enum class TraverseResult : int
{
	Continue, //계속 탐색한다.
	Stop, //탐색을 중단하고 싶을때.
	ChildrenSkip, //자식만 스킵 다른 노드들은 돌아간다.
	Count
};

//bit enum을 템플릿화 해서 다른 bit enum들도 동일하게 함수를 사용하게끔 한다.
namespace BitEnum
{
	template <typename EnumType>
	constexpr typename std::enable_if_t<std::is_enum_v<EnumType>, EnumType>
		operator|(EnumType lhs, EnumType rhs) {
		using UnderlyingType = std::underlying_type_t<EnumType>;
		return static_cast<EnumType>(static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs));
	}

	template <typename EnumType>
	constexpr typename std::enable_if_t<std::is_enum_v<EnumType>, EnumType>
		operator&(EnumType lhs, EnumType rhs) {
		using UnderlyingType = std::underlying_type_t<EnumType>;
		return static_cast<EnumType>(static_cast<UnderlyingType>(lhs) & static_cast<UnderlyingType>(rhs));
	}

	template <typename EnumType>
	constexpr typename std::enable_if_t<std::is_enum_v<EnumType>, EnumType>
		operator~(EnumType flag) {
		using UnderlyingType = std::underlying_type_t<EnumType>;
		return static_cast<EnumType>(~static_cast<UnderlyingType>(flag));
	}

	// SetBitEnum & HasBitEnum 함수 템플릿화
	template <typename EnumType>
	inline bool Set(EnumType& stateFlag, EnumType flag, bool enabled) noexcept {
		EnumType previous = stateFlag;
		stateFlag = enabled ? (stateFlag | flag) : (stateFlag & ~flag);
		return previous != stateFlag;
	}

	template <typename EnumType>
	inline bool Has(EnumType stateFlag, EnumType flag) noexcept {
		return (stateFlag & flag) != 0;
	}
}

// namespac + enum을 쓰는 이유는 함수 인자로 int를 쓰는 것보다 나아서. enum만 쓰면 쟤들 이름이 흔해서 자꾸 이름 충돌된다.
// enum class는 오퍼레이터 함수가 많이 생성되고 타입 변환을 계속 해 줘야 해서 귀찮다.
// 비트플래그는 컴포넌트가 변할 수 있는 '상태'를 표현할때 사용. 예를 들면 pickable 클래스를 상속받았는가?(왜냐면 변하지 않는거니까) 이런걸 여기에 추가하면 안됨.
namespace StateFlag
{
	enum Type : int //갯수는 32개까지 가능. 
	{
		Update = 1 << 0,
		Render = 1 << 1,
		Attach = 1 << 2,
		Detach = 1 << 3,
		X_SizeLocked = 1 << 4,
		Y_SizeLocked = 1 << 5,
		LockPosOnResize = 1 << 6, //사이즈를 조절할때 자식 클래스의 위치값을 변하지 않게 한다.
		Active = Update | Render,
		Default = Active | Attach | Detach, // 기본 옵션(모든 옵션 포함)
	};

	constexpr StateFlag::Type operator|(StateFlag::Type lhs, StateFlag::Type rhs) { return static_cast<StateFlag::Type>(static_cast<int>(lhs) | static_cast<int>(rhs)); }
}

enum class DirectionType
{
	Horizontal,
	Vertical,
	Count
};

TextureSlice DirTypeToTextureSlice(DirectionType dirType) noexcept;
optional<DirectionType> TextureSliceToDirType(TextureSlice texSlice) noexcept;

