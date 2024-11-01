#include "../TerminalFactory.hpp"
#include "../WindowsTerminal/WindowsTerminal.hpp"
#include "../TestTerminal/TestTerminal.hpp"

#if HAS_SDL
#   include "../SDLTerminal/SDLTerminal.hpp"
#endif

namespace AppCUI::Internal
{
using namespace Application;

unique_ptr<AbstractTerminal> GetTerminal(const InitializationData& initData)
{
    unique_ptr<AbstractTerminal> term = nullptr;
    switch (initData.Frontend)
    {
    case FrontendType::Default:
    case FrontendType::WindowsConsole:
        term = std::make_unique<WindowsTerminal>();
        break;
#if HAS_SDL
    case FrontendType::SDL:
        term = std::make_unique<SDLTerminal>();
        break;
#endif
    case FrontendType::Tests:
        term = std::make_unique<TestTerminal>();
        break;
    default:
        RETURNERROR(nullptr, "Unsuported terminal type for Windows OS (%d)", (uint32) initData.Frontend);
    }
    CHECK(term, nullptr, "Fail to allocate memory for a terminal !");
    CHECK(term->Init(initData), nullptr, "Fail to initialize the terminal!");

    return term;
}
} // namespace AppCUI::Internal