#pragma once

#include "AppCUI.hpp"
#ifdef _WIN32
#    include "OS/Windows/OSDefinitions.hpp"
#else
#    include "OS/Unix/OSDefinitions.hpp"
#endif
#include <memory>

#define REPAINT_STATUS_COMPUTE_POSITION 1
#define REPAINT_STATUS_DRAW             2
#define REPAINT_STATUS_ALL              (REPAINT_STATUS_COMPUTE_POSITION | REPAINT_STATUS_DRAW)
#define REPAINT_STATUS_NONE             0

#define MOUSE_LOCKED_OBJECT_NONE        0
#define MOUSE_LOCKED_OBJECT_ACCELERATOR 1
#define MOUSE_LOCKED_OBJECT_CONTROL     2

#define MAX_MODAL_CONTROLS_STACK 16

#define LOOP_STATUS_NORMAL       0
#define LOOP_STATUS_STOP_CURRENT 1
#define LOOP_STATUS_STOP_APP     2

#define MAX_COMMANDBAR_SHIFTSTATES 8

#define NEW_LINE_CODE 10

namespace AppCUI
{
namespace Internal
{
    enum class SystemEventType : unsigned int
    {
        None = 0,
        MouseDown,
        MouseUp,
        MouseMove,
        MouseWheel,
        AppClosed,
        AppResized,
        KeyPressed,
        ShiftStateChanged,
        RequestRedraw,
    };
    struct SystemEvent
    {
        SystemEventType eventType;
        int mouseX, mouseY;
        unsigned int newWidth, newHeight;
        Input::MouseButton mouseButton;
        Input::MouseWheel mouseWheel;
        Input::Key keyCode;
        char16_t unicodeCharacter;
        bool updateFrames;
    };

    struct CommandBarField
    {
        int Command, StartScreenPos, EndScreenPos;
        Input::Key KeyCode;
        string_view KeyName;
        Graphics::CharacterBuffer Name;
        unsigned int ClearCommandUniqueID;
    };
    struct CommandBarFieldIndex
    {
        CommandBarField* Field;
    };
    class CommandBarController
    {
        CommandBarField Fields[MAX_COMMANDBAR_SHIFTSTATES][(unsigned int) Input::Key::Count];
        CommandBarFieldIndex VisibleFields[MAX_COMMANDBAR_SHIFTSTATES][(unsigned int) Input::Key::Count];
        int IndexesCount[MAX_COMMANDBAR_SHIFTSTATES];
        bool HasKeys[MAX_COMMANDBAR_SHIFTSTATES];

        struct
        {
            int Y, Width;
        } BarLayout;

        string_view ShiftStatus;

        Application::Config* Cfg;
        Input::Key CurrentShiftKey;
        int LastCommand;
        CommandBarField* PressedField;
        CommandBarField* HoveredField;
        unsigned int ClearCommandUniqueID;
        bool RecomputeScreenPos;

        void ComputeScreenPos();
        bool CleanFieldStatus();
        CommandBarField* MousePositionToField(int x, int y);

      public:
        CommandBarController(unsigned int desktopWidth, unsigned int desktopHeight, Application::Config* cfg);
        void Paint(Graphics::Renderer& renderer);
        void Clear();
        void SetDesktopSize(unsigned int width, unsigned int height);
        bool Set(Input::Key keyCode, const ConstString& caption, int Command);
        bool SetShiftKey(Input::Key keyCode);
        bool OnMouseMove(int x, int y, bool& repaint);
        bool OnMouseDown();
        bool OnMouseUp(int& command);
        int GetCommandForKey(Input::Key keyCode);
    };

    struct MenuBarItem
    {
        Controls::Menu Mnu;
        Graphics::CharacterBuffer Name;
        Input::Key HotKey;
        unsigned int HotKeyOffset;
        int X;
        MenuBarItem();
    };
    class MenuBar
    {
        static const constexpr unsigned int MAX_ITEMS = 32;
        unique_ptr<MenuBarItem> Items[MAX_ITEMS];
        Application::Config* Cfg;
        Controls::Control* Parent;
        unsigned int ItemsCount;
        unsigned int OpenedItem;
        unsigned int HoveredItem;
        unsigned int Width;
        int X, Y;

        unsigned int MousePositionToItem(int x, int y);
        void Open(unsigned int menuIndex);

      public:
        MenuBar(Controls::Control* parent = nullptr, int x = 0, int y = 0);

        Controls::ItemHandle AddMenu(const ConstString& name);
        Controls::Menu* GetMenu(Controls::ItemHandle itemHandle);
        void RecomputePositions();
        void SetWidth(unsigned int value);
        void Paint(Graphics::Renderer& renderer);
        bool OnMouseMove(int x, int y, bool& repaint);
        bool OnMousePressed(int x, int y, Input::MouseButton button);
        void Close();
        bool IsOpened();
        bool OnKeyEvent(Input::Key keyCode);
    };

    class TextControlDefaultMenu
    {
        Controls::ItemHandle itemCopy;
        Controls::ItemHandle itemCut;
        Controls::ItemHandle itemDelete;
        Controls::ItemHandle itemPaste;
        Controls::ItemHandle itemSelectAll;
        Controls::ItemHandle itemToUpper;
        Controls::ItemHandle itemToLower;
        Controls::Menu menu;

      public:
        TextControlDefaultMenu();
        ~TextControlDefaultMenu();
        void Show(Utils::Reference<Controls::Control> parent, int x, int y, bool hasSelection);

        static constexpr int TEXTCONTROL_CMD_COPY            = 1200001;
        static constexpr int TEXTCONTROL_CMD_CUT             = 1200002;
        static constexpr int TEXTCONTROL_CMD_PASTE           = 1200003;
        static constexpr int TEXTCONTROL_CMD_SELECT_ALL      = 1200004;
        static constexpr int TEXTCONTROL_CMD_DELETE_SELECTED = 1200005;
        static constexpr int TEXTCONTROL_CMD_TO_UPPER        = 1200006;
        static constexpr int TEXTCONTROL_CMD_TO_LOWER        = 1200007;
    };

    class ToolTipController
    {
        Graphics::CharacterBuffer Text;
        Application::Config* Cfg;
        Graphics::Rect TextRect;
        Graphics::Point Arrow;
        Graphics::SpecialChars ArrowChar;
        Graphics::WriteTextParams TxParams;

      public:
        Graphics::Clip ScreenClip;

        bool Visible;

      public:
        ToolTipController();
        bool Show(const ConstString& text, Graphics::Rect& objRect, int screenWidth, int screenHeight);
        void Hide();
        void Paint(Graphics::Renderer& renderer);
    };

    class AbstractTerminal
    {
      protected:
        AbstractTerminal();

      public:
        unsigned int LastCursorX, LastCursorY;
        Graphics::Canvas OriginalScreenCanvas, ScreenCanvas;
        bool Inited, LastCursorVisibility;

        virtual bool OnInit(const Application::InitializationData& initData) = 0;
        virtual void RestoreOriginalConsoleSettings()                        = 0;
        virtual void OnUninit()                                              = 0;
        virtual void OnFlushToScreen()                                       = 0;
        virtual bool OnUpdateCursor()                                        = 0;
        virtual void GetSystemEvent(Internal::SystemEvent& evnt)             = 0;
        virtual bool IsEventAvailable()                                      = 0;

        virtual ~AbstractTerminal();

        bool Init(const Application::InitializationData& initData);
        void Uninit();
        void Update();
    };

    struct ApplicationImpl
    {
        Application::Config config;
        Utils::IniObject settings;
        unique_ptr<AbstractTerminal> terminal;
        unique_ptr<CommandBarController> cmdBar;
        unique_ptr<MenuBar> menu;
        vector<Controls::Control*> toDelete;

        Controls::Desktop* AppDesktop;
        ToolTipController ToolTip;
        Application::CommandBar CommandBarWrapper;

        Controls::Control* ModalControlsStack[MAX_MODAL_CONTROLS_STACK];
        Controls::Control* MouseLockedControl;
        Controls::Control* MouseOverControl;
        Controls::Control* ExpandedControl;
        Controls::Menu* VisibleMenu;
        unsigned int ModalControlsCount;
        int LoopStatus;
        unsigned int RepaintStatus;
        int MouseLockedObject;

        Application::InitializationFlags InitFlags;
        unsigned int LastWindowID;
        int LastMouseX, LastMouseY;
        bool Inited;
        bool cmdBarUpdate;

        ApplicationImpl();
        ~ApplicationImpl();

        void Destroy();
        void ComputePositions();
        void ProcessKeyPress(Input::Key keyCode, char16_t unicodeCharacter);
        void ProcessShiftState(Input::Key ShiftState);
        void ProcessMenuMouseClick(Controls::Menu* mnu, int x, int y);
        bool ProcessMenuAndCmdBarMouseMove(int x, int y);
        void OnMouseDown(int x, int y, Input::MouseButton button);
        void OnMouseUp(int x, int y, Input::MouseButton button);
        void OnMouseMove(int x, int y, Input::MouseButton button);
        void OnMouseWheel(int x, int y, Input::MouseWheel direction);
        void SendCommand(int command);
        void Terminate();

        // Pack/Expand
        void PackControl(bool redraw);
        bool ExpandControl(Controls::Control* ctrl);

        // Menu
        void CloseContextualMenu();
        void ShowContextualMenu(Controls::Menu* mnu);

        // Common implementations
        void LoadSettingsFile(Application::InitializationData& initData);
        bool Init(Application::InitializationData& initData);
        bool Uninit();
        bool ExecuteEventLoop(Controls::Control* control = nullptr);
        void Paint();
        void RaiseEvent(
              Utils::Reference<Controls::Control> control,
              Utils::Reference<Controls::Control> sourceControl,
              Controls::Event eventType,
              int controlID);
        bool SetToolTip(Controls::Control* control, const ConstString& text);
        bool SetToolTip(Controls::Control* control, const ConstString& text, int x, int y);

        void ArrangeWindows(Application::ArangeWindowsMethod method);
    };
} // namespace Internal
namespace Application
{
    Internal::ApplicationImpl* GetApplication();
}
namespace Utils
{
    struct UnicodeChar
    {
        unsigned short Value;
        unsigned int Length;
    };
    bool ConvertUTF8CharToUnicodeChar(const char8_t* p, const char8_t* end, UnicodeChar& result);
} // namespace Utils
namespace Log
{
    void Unit(); // needed to release some alocation buffers
}
namespace Controls
{
    void UninitTextFieldDefaultMenu();
}
} // namespace AppCUI
