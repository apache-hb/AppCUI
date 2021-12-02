#include <AppCUI.hpp>
#include "ControlContext.hpp"

using namespace AppCUI::Controls;
using namespace AppCUI::Graphics;

ImageViewer::ImageViewer(const AppCUI::Utils::ConstString& caption, std::string_view layout, ViewerFlags flags)
    : CanvasViewer(caption,layout,16,16,flags)
{
}
bool ImageViewer::SetImage(
      const AppCUI::Graphics::Image& img,
      AppCUI::Graphics::ImageRenderingMethod method,
      AppCUI::Graphics::ImageScaleMethod scale)
{
    auto c = GetCanvas();
    CHECK(c.IsValid(), false, "");
    auto sz = c->ComputeRenderingSize(img, method, scale);
    CHECK((sz.Width > 0) && (sz.Height > 0), false, "Invalid image size (0x0) --> nothing to render");
    CHECK(c->Resize(sz.Width, sz.Height), false, "Fail to set canvas size to %ux%u", sz.Width, sz.Height);
    CREATE_TYPECONTROL_CONTEXT(CanvasControlContext, Members, false);
    Members->MoveScrollTo(0, 0);
    return c->DrawImage(img, 0, 0, method, scale);

}