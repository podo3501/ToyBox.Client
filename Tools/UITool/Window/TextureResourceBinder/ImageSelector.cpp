#include "pch.h"
#include "ImageSelector.h"
#include "TextureResBinderWindow.h"
#include "Window/Utils/EditUtility.h"
#include "Window/Utils/Common.h"
#include "../HelperClass.h"
#include "Toy/Locator/InputLocator.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "Toy/UserInterface/CommandHistory/TextureResource/TexResCommandHistory.h"
#include "Toy/UserInterface/UIComponent/UIUtility.h"
#include "Toy/UserInterface/UIComponent/UIHelperClass.h"
#include "Shared/Utils/GeometryExt.h"

ImageSelector::~ImageSelector() = default;
ImageSelector::ImageSelector(TextureResBinderWindow* textureWindow) :
    m_sourceTexture{ nullptr },
    m_cmdHistory{ nullptr },
    m_textureWindow{ textureWindow },
    m_renameNotifier{ make_unique<RenameNotifier>() },
    m_selectImagePart{ TextureSlice::One }
{}

void ImageSelector::SetTexture(PatchTextureStd1* pTex1) noexcept
{
    m_sourceTexture = pTex1;
    if (!pTex1)
    {
        m_areaList = {};
        return;
    }

    if (const auto& areaList = pTex1->GetTextureAreaList(); areaList)
        m_areaList = *areaList;

    DeselectArea();
}

void ImageSelector::Update()
{
    CheckSourcePartition();
    CheckSelectArea();
}

void ImageSelector::SelectArea() noexcept
{
    if (!m_hoveredAreas.size() || m_hoveredAreas[0] == Rectangle{}) return;
    m_selectedArea = make_unique<TextureSourceInfo>(m_sourceTexture->GetFilename(), m_selectImagePart, m_hoveredAreas);
}

bool ImageSelector::DeselectArea() noexcept
{
    m_selectedArea.reset();
    return true;
}

bool ImageSelector::RemoveArea() noexcept
{
    if (!m_selectedArea) return false;

    auto binder = m_cmdHistory->GetReceiver();
    const string& bindingKey = binder->GetBindingKey(*m_selectedArea);
    if (bindingKey.empty()) return false;
    m_cmdHistory->RemoveTextureKey(bindingKey);

    return DeselectArea();
}

void ImageSelector::CheckSelectArea() noexcept
{
    auto input = ToolInputLocator::GetService();
    if (input->IsInputAction(MouseButton::Left, InputState::Pressed)) SelectArea();
    if (input->IsInputAction(Keyboard::Escape, InputState::Pressed)) DeselectArea();
    if (input->IsInputAction(Keyboard::Delete, InputState::Pressed)) RemoveArea();
}

optional<Rectangle> ImageSelector::FindAreaFromMousePos(const XMINT2& pos) const noexcept
{
    auto it = ranges::find_if(m_areaList, [&pos](const Rectangle& rect) { return Contains(rect, pos); });
    return (it != m_areaList.end()) ? optional{ *it } : nullopt;
}

enum DivideType : int
{
    None = 0,
    X = 1 << 0,
    Y = 1 << 1,
    XY = X | Y
};

static void DivideLengthByThree(const Rectangle& area, DivideType divideType,
    vector<int>& outWidth, vector<int>& outHeight) noexcept
{
    if (area.IsEmpty()) return;
    outWidth = { area.width };
    outHeight = { area.height };

    auto divideByThree = [](int totalSize) -> vector<int> {
        int oneThird = totalSize / 3;
        return { oneThird, totalSize - oneThird * 2, oneThird };
        };

    if (divideType & DivideType::X) outWidth = divideByThree(area.width);
    if (divideType & DivideType::Y) outHeight = divideByThree(area.height);
}

static vector<Rectangle> GenerateSourceAreas(const Rectangle& area, DivideType divideType) noexcept
{
    if (divideType == DivideType::None) return { area };

    vector<int> widths, heights;
    DivideLengthByThree(area, divideType, widths, heights);
    return GetSourcesFromArea(area, widths, heights);
}

static inline DivideType ImagePartToDivideType(TextureSlice texSlice) noexcept
{
    return texSlice == TextureSlice::One ? DivideType::None :
           texSlice == TextureSlice::ThreeH ? DivideType::X :
           texSlice == TextureSlice::ThreeV ? DivideType::Y :
           texSlice == TextureSlice::Nine ? DivideType::XY : DivideType::None;
}

void ImageSelector::CheckSourcePartition() noexcept
{
    if (!m_sourceTexture) return;

    auto input = ToolInputLocator::GetService();
    const XMINT2& pos = input->GetPosition();
    m_hoveredAreas = GetAreas(m_cmdHistory->GetReceiver(), m_sourceTexture->GetFilename(), m_selectImagePart, pos);
    if (!m_hoveredAreas.empty()) return; //먼저 저장돼 있는 것을 찾아보고 없으면 새로운걸 만든다.

    if(auto currRectangle = FindAreaFromMousePos(pos); currRectangle)
        m_hoveredAreas = GenerateSourceAreas(*currRectangle, ImagePartToDivideType(m_selectImagePart));
}

void ImageSelector::Render()
{
	SelectImagePart();
    EditSelectArea();
    RenderHoveredArea();
    RenderSelectedArea();
    RenderLabeledAreas();
}

void ImageSelector::SelectImagePart()
{
    if (!m_sourceTexture) return;

    static int selected{ 0 };
    int pre{ selected };
    if (ImGui::RadioButton("1 Part", &selected, 0)) m_selectImagePart = TextureSlice::One; ImGui::SameLine();
    if (ImGui::RadioButton("3H Part", &selected, 1)) m_selectImagePart = TextureSlice::ThreeH; ImGui::SameLine();
    if (ImGui::RadioButton("3V Part", &selected, 2)) m_selectImagePart = TextureSlice::ThreeV; ImGui::SameLine();
    if (ImGui::RadioButton("9 Part", &selected, 3)) m_selectImagePart = TextureSlice::Nine;
    if (pre != selected) DeselectArea();
}

void ImageSelector::RenderHoveredArea() const
{
    DrawRectangles(m_textureWindow->GetWindow(), m_hoveredAreas, ToColor(Colors::White), ToColor(Colors::White, 0.3f));
}

void ImageSelector::RenderSelectedArea() const
{
    if (!m_selectedArea) return;
    DrawRectangles(m_textureWindow->GetWindow(), m_selectedArea->sources,
        ToColor(Colors::White), ToColor(Colors::White, 0.3f));
}

static void DrawTextureAreas(ImGuiWindow* wnd, const vector<TextureSourceInfo>& sourceInfos, const ImColor& color) 
{
    for (const auto& info : sourceInfos) DrawRectangles(wnd, info.sources, color);
}

void ImageSelector::RenderLabeledAreas() const
{
    if (!m_sourceTexture) return;

    const auto& wnd = m_textureWindow->GetWindow();
    const auto& filename = m_sourceTexture->GetFilename();

    auto binder = m_cmdHistory->GetReceiver();
    DrawTextureAreas(wnd, binder->GetTotalAreas(filename), ToColor(Colors::DarkSlateBlue));
    DrawTextureAreas(wnd, GetAreas(binder, filename, m_selectImagePart), ToColor(Colors::Blue));
}

void ImageSelector::EditSelectArea()
{
    if (!m_selectedArea) return;

    auto binder = m_cmdHistory->GetReceiver();
    const string& bindingKey = binder->GetBindingKey(*m_selectedArea);
    SourceDivider sourceDivider = GetSourceDivider(*m_selectedArea);
    if (EditSourceAndDivider("Source Area", "Deviders", sourceDivider))
    {
        m_selectedArea->sources = ComputeSliceRects(m_selectImagePart, sourceDivider);
        if (!bindingKey.empty()) m_cmdHistory->ModifyTextureSourceInfo(bindingKey, *m_selectedArea);
    }
        
    m_renameNotifier->EditName("Tex Bind Key", bindingKey, [this, &bindingKey](const string& newKey) {
        if (bindingKey.empty()) return m_cmdHistory->AddTextureKey(newKey, *m_selectedArea);
        if (newKey.empty()) return RemoveArea();
        return m_cmdHistory->RenameTextureKey(bindingKey, newKey);
        });
}