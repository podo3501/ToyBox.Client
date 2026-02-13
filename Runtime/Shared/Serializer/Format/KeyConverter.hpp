#pragma once
//새로운 키값을 만들때에는 이 파일에서 새로운 키값을 정의해서 만든다.
//키 변환을 간단하게 선언하는 Define 코드. 키가 템플릿 특수화로 돼 있어서 똑같은 선언을 계속 해야 하기 때문.
#ifndef DECLARE_KEY_CONVERTER
#define DECLARE_KEY_CONVERTER(Type)                 \
	template<>                                         \
	struct KeyConverter<Type> {                        \
		static string ToKey(const Type&);     \
		static Type FromKey(const string&);   \
	};
#endif //DECLARE_KEY_CONVERTER

//Key 변환 헬퍼
template<typename K>
struct KeyConverter {
	static string ToKey(const K& key) { return key; }
	static K FromKey(const string& key) { return key; }
};

//Key 특수화
DECLARE_KEY_CONVERTER(int)
DECLARE_KEY_CONVERTER(wstring)

template<typename K> string ToKeyString(const K& key) { return KeyConverter<K>::ToKey(key); }
template<typename K> K FromKeyString(const string& key) { return KeyConverter<K>::FromKey(key); }
