#include "ControlContext.hpp"

namespace AppCUI
{
constexpr int MIN_SPLITTER_SIZE = 4;

constexpr int NO_TOOLTIP_TEXT             = -1;
constexpr int SPLITTER_TOOLTIPTEXT_DRAG   = 0;
constexpr int SPLITTER_TOOLTIPTEXT_RIGHT  = 1;
constexpr int SPLITTER_TOOLTIPTEXT_BOTTOM = 2;
constexpr int SPLITTER_TOOLTIPTEXT_LEFT   = 3;
constexpr int SPLITTER_TOOLTIPTEXT_TOP    = 4;

constexpr uint32 GATTR_VERTICAL    = 1024;
constexpr uint32 SPLITTER_BAR_SIZE = 1;

constexpr string_view splitterToolTipTexts[] = { "Drag to resize panels",
                                                 "Click to maximize right panel",
                                                 "Click to maximize bottom panel",
                                                 "Click to maximize left panel",
                                                 "Click to maximize top panel" };

SplitterMouseStatus MousePozToSplitterMouseStatus(SplitterControlContext* Members, int x, int y, bool pressed)
{
    int v = 0;
    if (Members->Flags & GATTR_VERTICAL)
    {
        if (x != (Members->Layout.Width - (Members->SecondPanelSize + SPLITTER_BAR_SIZE)))
            return SplitterMouseStatus::None;
        if (Members->Layout.Height > MIN_SPLITTER_SIZE)
            v = ((y == 1) || (y == 2)) ? y : 0;
    }
    else
    {
        if (y != Members->Layout.Height - (Members->SecondPanelSize + SPLITTER_BAR_SIZE))
            return SplitterMouseStatus::None;
        if (Members->Layout.Width > MIN_SPLITTER_SIZE)
            v = ((x == 1) || (x == 2)) ? x : 0;
    }
    switch (v)
    {
    case 1:
        if (pressed)
            return SplitterMouseStatus::ClickOnButton1;
        else
            return SplitterMouseStatus::OnButton1;
    case 2:
        if (pressed)
            return SplitterMouseStatus::ClickOnButton2;
        else
            return SplitterMouseStatus::OnButton2;
    default:
        if (pressed)
            return SplitterMouseStatus::Drag;
        else
            return SplitterMouseStatus::OnBar;
    }
}
void Splitter_ResizeComponents(Splitter* control)
{
    CREATE_TYPE_CONTEXT(SplitterControlContext, control, Members, );
    Control* o;

    int sz = Members->SecondPanelSize + SPLITTER_BAR_SIZE;

    for (uint32 tr = 0; tr < Members->ControlsCount; tr++)
    {
        o = Members->Controls[tr];
        if (o != nullptr)
        {
            o->SetVisible(tr < 2);
            if (tr == 0)
            {
                if ((Members->Flags & GATTR_VERTICAL) != 0)
                {
                    o->Resize(Members->Layout.Width - sz, Members->Layout.Height);
                    o->MoveTo((Members->Layout.Width - sz) - o->GetWidth(), 0);
                }
                else
                {
                    o->Resize(Members->Layout.Width, Members->Layout.Height - sz);
                    o->MoveTo(0, (Members->Layout.Height - sz) - o->GetHeight());
                }
                continue;
            }
            if (tr == 1)
            {
                if ((Members->Flags & GATTR_VERTICAL) != 0)
                {
                    o->MoveTo(Members->Layout.Width - Members->SecondPanelSize, 0);
                    o->Resize(Members->SecondPanelSize, Members->Layout.Height);
                }
                else
                {
                    o->MoveTo(0, Members->Layout.Height - Members->SecondPanelSize);
                    o->Resize(Members->Layout.Width, Members->SecondPanelSize);
                }
                continue;
            }
        }
    }
}
Splitter::~Splitter()
{
    DELETE_CONTROL_CONTEXT(SplitterControlContext);
}
Splitter::Splitter(string_view layout, SplitterFlags flags) : Control(new SplitterControlContext(), "", layout, false)
{
    auto Members            = reinterpret_cast<SplitterControlContext*>(this->Context);
    Members->Flags          = GATTR_ENABLE | GATTR_VISIBLE | GATTR_TABSTOP;
    Members->mouseStatus    = SplitterMouseStatus::None;
    Members->Panel1.minSize = 0;
    Members->Panel1.maxSize = 0xFFFFFFFF;
    Members->Panel2.minSize = 0;
    Members->Panel2.maxSize = 0xFFFFFFFF;

    if ((flags & SplitterFlags::Vertical) == SplitterFlags::Vertical)
        Members->Flags |= GATTR_VERTICAL;
    if ((flags & SplitterFlags::Vertical) == SplitterFlags::Vertical)
        Members->SecondPanelSize = Members->Layout.Width / 2;
    else
        Members->SecondPanelSize = Members->Layout.Height / 2;
}

bool Splitter::SetSecondPanelSize(uint32 newSize)
{
    CHECK(newSize >= 0, false, "");
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    const auto iSz = ((Members->Flags & GATTR_VERTICAL) != 0) ? Members->Layout.Width : Members->Layout.Height;
    const auto sz =
          (iSz >= (int) SPLITTER_BAR_SIZE) && (iSz <= 0x7FFFFF) ? (uint32) (iSz - (int) SPLITTER_BAR_SIZE) : 0U;
    newSize = std::min<>(sz, newSize);

    // first panel check
    if ((sz - newSize) > Members->Panel1.maxSize)
        newSize = sz - Members->Panel1.maxSize;
    if ((sz - newSize) < Members->Panel1.minSize)
    {
        if (Members->Panel1.minSize > sz)
            newSize = 0;
        else
            newSize = sz - Members->Panel1.minSize;
    }
    // check the second panel
    newSize = std::min<>(newSize, Members->Panel2.maxSize);
    if (newSize < Members->Panel2.minSize)
    {
        if (Members->Panel2.minSize < sz)
            newSize = Members->Panel2.minSize;
        else
            newSize = sz;
    }
    // set the new value
    Members->SecondPanelSize = newSize;
    Splitter_ResizeComponents(this);
    RaiseEvent(Event::SplitterPositionChanged);
    return true;
}
bool Splitter::HideSecondPanel()
{
    return SetSecondPanelSize(0);
}
bool Splitter::MaximizeSecondPanel()
{
    return SetSecondPanelSize(0xFFFF);
}
void Splitter::Paint(Graphics::Renderer& renderer)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, );

    ColorPair c1  = Members->Cfg->Symbol.Arrows;
    ColorPair c2  = Members->Cfg->Symbol.Arrows;
    ColorPair col = Members->Cfg->Lines.Normal;
    uint32 poz;

    switch (Members->mouseStatus)
    {
    case SplitterMouseStatus::ClickOnButton1:
        c1 = Members->Cfg->Symbol.Pressed;
        break;
    case SplitterMouseStatus::OnButton1:
        c1 = Members->Cfg->Symbol.Hovered;
        break;
    case SplitterMouseStatus::ClickOnButton2:
        c2 = Members->Cfg->Symbol.Pressed;
        break;
    case SplitterMouseStatus::OnButton2:
        c2 = Members->Cfg->Symbol.Hovered;
        break;
    case SplitterMouseStatus::OnBar:
    case SplitterMouseStatus::Drag:
        col = Members->Cfg->Lines.Hovered;
        break;
    }

    if (!(Members->Flags & GATTR_ENABLE))
        col = Members->Cfg->Lines.Inactive;

    if ((Members->Flags & GATTR_VERTICAL) != 0)
    {
        poz = Members->Layout.Width - (int) (Members->SecondPanelSize + SPLITTER_BAR_SIZE);
        renderer.DrawVerticalLine(poz, 0, Members->Layout.Height - 1, col);
        if (Members->Layout.Height > MIN_SPLITTER_SIZE)
        {
            renderer.WriteSpecialCharacter(poz, 1, SpecialChars::TriangleLeft, c1);
            renderer.WriteSpecialCharacter(poz, 2, SpecialChars::TriangleRight, c2);
        }
    }
    else
    {
        poz = Members->Layout.Height - (int) (Members->SecondPanelSize + SPLITTER_BAR_SIZE);
        renderer.DrawHorizontalLine(0, poz, Members->Layout.Width - 1, col);
        if (Members->Layout.Width > MIN_SPLITTER_SIZE)
        {
            renderer.WriteSpecialCharacter(1, poz, SpecialChars::TriangleUp, c1);
            renderer.WriteSpecialCharacter(2, poz, SpecialChars::TriangleDown, c2);
        }
    }
}
bool Splitter::OnKeyEvent(Input::Key keyCode, char16)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    if ((Members->Flags & GATTR_VERTICAL) != 0)
    {
        switch (keyCode)
        {
        case Key::Alt | Key::Ctrl | Key::Left:
            SetSecondPanelSize(Members->SecondPanelSize + SPLITTER_BAR_SIZE);
            return true;
        case Key::Alt | Key::Ctrl | Key::Right:
            if (Members->SecondPanelSize > SPLITTER_BAR_SIZE)
                SetSecondPanelSize(Members->SecondPanelSize - SPLITTER_BAR_SIZE);
            else
                SetSecondPanelSize(0);
            return true;
        case Key::Alt | Key::Ctrl | Key::Shift | Key::Right:
            SetSecondPanelSize(0);
            return true;
        case Key::Alt | Key::Ctrl | Key::Shift | Key::Left:
            SetSecondPanelSize(0xFFFFFF);
            return true;
        };
    }
    else
    {
        switch (keyCode)
        {
        case Key::Alt | Key::Ctrl | Key::Up:
            SetSecondPanelSize(Members->SecondPanelSize + SPLITTER_BAR_SIZE);
            return true;
        case Key::Alt | Key::Ctrl | Key::Down:
            if (Members->SecondPanelSize > SPLITTER_BAR_SIZE)
                SetSecondPanelSize(Members->SecondPanelSize - SPLITTER_BAR_SIZE);
            else
                SetSecondPanelSize(0);
            return true;
        case Key::Alt | Key::Ctrl | Key::Shift | Key::Down:
            SetSecondPanelSize(0);
            return true;
        case Key::Alt | Key::Ctrl | Key::Shift | Key::Up:
            SetSecondPanelSize(0xFFFFFF);
            return true;
        };
    }
    return false;
}
void Splitter::OnAfterResize(int, int)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, );
    SetSecondPanelSize(Members->SecondPanelSize);
    Splitter_ResizeComponents(this);
}
void Splitter::OnFocus()
{
    // Splitter_ResizeComponents(this);    ==> remove as it will cause a stack overflow if called in OnFocus method
}
bool Splitter::OnBeforeAddControl(Reference<Control> c)
{
    CHECK(c != nullptr, false, "");
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    return (Members->ControlsCount < 2);
}
void Splitter::OnMousePressed(int x, int y, Input::MouseButton)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, );
    Members->mouseStatus = MousePozToSplitterMouseStatus(Members, x, y, true);
}
void Splitter::OnMouseReleased(int x, int y, Input::MouseButton)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, );
    Members->mouseStatus = MousePozToSplitterMouseStatus(Members, x, y, false);
    if (Members->mouseStatus == SplitterMouseStatus::OnButton2)
        SetSecondPanelSize(0);
    if (Members->mouseStatus == SplitterMouseStatus::OnButton1)
        SetSecondPanelSize(0xFFFFFF);

    Members->mouseStatus = SplitterMouseStatus::None;
}
bool Splitter::OnMouseDrag(int x, int y, Input::MouseButton)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    if (Members->mouseStatus == SplitterMouseStatus::Drag)
    {
        if (Members->Flags & GATTR_VERTICAL)
        {
            SetSecondPanelSize(Members->Layout.Width > x ? Members->Layout.Width - (x + 1) : 0);
        }
        else
        {
            SetSecondPanelSize(Members->Layout.Height > y ? Members->Layout.Height - (y + 1) : 0);
        }
        return true;
    }
    return false;
}
bool Splitter::OnMouseOver(int x, int y)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    auto res = MousePozToSplitterMouseStatus(Members, x, y, false);
    if (res == Members->mouseStatus)
        return false;

    Members->mouseStatus = res;
    int toolTipTextID    = NO_TOOLTIP_TEXT;

    switch (Members->mouseStatus)
    {
    case SplitterMouseStatus::OnBar:
        toolTipTextID = SPLITTER_TOOLTIPTEXT_DRAG;
        break;
    case SplitterMouseStatus::OnButton1:
        toolTipTextID = Members->Flags & GATTR_VERTICAL ? SPLITTER_TOOLTIPTEXT_RIGHT : SPLITTER_TOOLTIPTEXT_BOTTOM;
        break;
    case SplitterMouseStatus::OnButton2:
        toolTipTextID = Members->Flags & GATTR_VERTICAL ? SPLITTER_TOOLTIPTEXT_LEFT : SPLITTER_TOOLTIPTEXT_TOP;
        break;
    case SplitterMouseStatus::None:
        HideToolTip();
        break;
    }
    if (toolTipTextID != NO_TOOLTIP_TEXT)
        ShowToolTip(splitterToolTipTexts[toolTipTextID], x, y);
    return true;
}
bool Splitter::OnMouseEnter()
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    Members->mouseStatus = SplitterMouseStatus::None;
    return true;
}
bool Splitter::OnMouseLeave()
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    Members->mouseStatus = SplitterMouseStatus::None;
    return true;
}
void Splitter::OnAfterAddControl(Reference<Control>)
{
    Splitter_ResizeComponents(this);
}
uint32 Splitter::GetFirstPanelSize()
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, 0);
    const auto iSz = ((Members->Flags & GATTR_VERTICAL) != 0) ? Members->Layout.Width : Members->Layout.Height;
    const auto sz  = (iSz >= 0) && (iSz <= 0x7FFFFF) ? (uint32) iSz : 0U;
    if (sz > (Members->SecondPanelSize + SPLITTER_BAR_SIZE))
        return sz - (Members->SecondPanelSize + SPLITTER_BAR_SIZE);
    else
        return 0;
}
uint32 Splitter::GetSecondPanelSize()
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, 0);
    return Members->SecondPanelSize;
}
bool Splitter::SetPanel1Bounderies(uint32 minSize, uint32 maxSize)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    CHECK(minSize <= maxSize, false, "Expecting minSize(%d) to be smaller than maxSize(%d)", minSize, maxSize);
    Members->Panel1.minSize = minSize;
    Members->Panel1.maxSize = maxSize;
    this->SetSecondPanelSize(Members->SecondPanelSize); // update with new limits
    return true;
}
bool Splitter::SetPanel2Bounderies(uint32 minSize, uint32 maxSize)
{
    CREATE_TYPECONTROL_CONTEXT(SplitterControlContext, Members, false);
    CHECK(minSize <= maxSize, false, "Expecting minSize(%d) to be smaller than maxSize(%d)", minSize, maxSize);
    Members->Panel2.minSize = minSize;
    Members->Panel2.maxSize = maxSize;
    this->SetSecondPanelSize(Members->SecondPanelSize); // update with new limits
    return true;
}
} // namespace AppCUI