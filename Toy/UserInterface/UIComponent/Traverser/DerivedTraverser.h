#pragma once
#include "HierarchyTraverser.h"

struct ITextureRender;
struct ITextureController;
class TextureResourceBinder;
namespace DX { class StepTimer; }
class DerivedTraverser : private HierarchyTraverser //UIComponent 상속되는 virtual 함수 및 private 멤버변수
{
public:
	~DerivedTraverser();
	DerivedTraverser();

	//UITraverser
	unique_ptr<UIComponent> Clone(UIComponent* c);
	bool ChangeSize(UIComponent* c, const XMUINT2& size, bool isForce = false) noexcept; //isForce는 크기가 변함이 없더라도 끝까지 실행시킨다.
	bool UpdatePositionsManually(UIComponent* c, bool isRoot = false) noexcept;

	//UIDetailTraverser
	bool BindTextureSourceInfo(UIComponent* c, TextureResourceBinder* resBinder) noexcept;
	bool Update(UIComponent* c, const DX::StepTimer& timer) noexcept;
	void Render(UIComponent* c, ITextureRender* render);
	bool EnableToolMode(UIComponent* c, bool enable) noexcept;
	void PropagateRoot(UIComponent* c, UIComponent* root) noexcept;

private:
	bool RecursivePositionUpdate(UIComponent* c, const XMINT2& position = {}) noexcept;
	bool RecursiveUpdate(UIComponent* c, const DX::StepTimer& timer, const XMINT2& position = {}) noexcept;
};
