
src = [
    'AppCUI.cpp',

    'src/Application/AbstractTerminal.cpp',
    'src/Application/Application.cpp',
    'src/Application/CommandBar.cpp',
    'src/Application/CommandBarController.cpp',
    'src/Application/Config.cpp',
    'src/Application/MenuBar.cpp',
    'src/Application/ToolTip.cpp',

    'src/Controls/Button.cpp',
    'src/Controls/CanvasViewer.cpp',
    'src/Controls/CharacterTable.cpp',
    'src/Controls/CheckBox.cpp',
    'src/Controls/ColorPicker.cpp',
    'src/Controls/ColumnsHeader.cpp',
    'src/Controls/ColumnsHeaderView.cpp',
    'src/Controls/ComboBox.cpp',
    'src/Controls/Control.cpp',
    'src/Controls/Desktop.cpp',
    'src/Controls/Factory.cpp',
    'src/Controls/Grid.cpp',
    'src/Controls/ImageView.cpp',
    'src/Controls/KeySelector.cpp',
    'src/Controls/Label.cpp',
    'src/Controls/ListView.cpp',
    'src/Controls/Menu.cpp',
    'src/Controls/NumericSelector.cpp',
    'src/Controls/Panel.cpp',
    'src/Controls/Password.cpp',
    'src/Controls/PropertyList.cpp',
    'src/Controls/RadioBox.cpp',
    'src/Controls/Splitter.cpp',
    'src/Controls/Tab.cpp',
    'src/Controls/TextArea.cpp',
    'src/Controls/TextControlDefaultMenu.cpp',
    'src/Controls/TextField.cpp',
    'src/Controls/TreeView.cpp',
    'src/Controls/UserControl.cpp',
    'src/Controls/Window.cpp',
    'src/Dialogs/FileDialog.cpp',
    'src/Dialogs/MessageBox.cpp',
    'src/Dialogs/ThemeEditor.cpp',
    'src/Dialogs/WindowManager.cpp',
    'src/Graphics/BitmapLoader.cpp',
    'src/Graphics/Canvas.cpp',
    'src/Graphics/CharacterBuffer.cpp',
    'src/Graphics/Clip.cpp',
    'src/Graphics/CodePage.cpp',
    'src/Graphics/Image.cpp',
    'src/Graphics/JPGLoader.cpp',
    'src/Graphics/PNGLoader.cpp',
    'src/Graphics/ProgressStatus.cpp',
    'src/Graphics/Rect.cpp',
    'src/Graphics/Renderer.cpp',
    'src/Log/Log.cpp',
    'src/OS/DataObject.cpp',
    'src/OS/DateTime.cpp',
    'src/OS/MemoryFile.cpp',
    'src/OS/StaticFileMethods.cpp',
    'src/OS/Clipboard/Clipboard.cpp',

    # 'src/Terminal/SDLTerminal/SDLTerminal.cpp',
    # 'src/Terminal/SDLTerminal/SDLTerminalInput.cpp',
    # 'src/Terminal/SDLTerminal/SDLTerminalScreen.cpp',

    'src/Terminal/TestTerminal/TestTerminal.cpp',

    'src/Utils/Array32.cpp',
    'src/Utils/Buffer.cpp',
    'src/Utils/ColorUtils.cpp',
    'src/Utils/IniObject.cpp',
    'src/Utils/KeyUtils.cpp',
    'src/Utils/KeyValueParser.cpp',
    'src/Utils/Number.cpp',
    'src/Utils/NumericFormatter.cpp',
    'src/Utils/Size.cpp',
    'src/Utils/String.cpp',
    'src/Utils/UnicodeStringBuilder.cpp',
]

system = host_machine.system()
cpp = meson.get_compiler('cpp')
inc = include_directories('include')
cpp_args = cpp.get_supported_arguments(
    '/Zc:char8_t', '/Zc:__cplusplus',
    '-Wno-switch', '-Wno-unused-parameter',
    '-Wno-sign-compare', '-Wno-unused-variable',
    '-Wno-missing-field-initializers',
    '-Wno-c++11-narrowing', '-Wno-logical-op-parentheses')

if system == 'windows'
    src += [
        'src/OS/Windows/File.cpp',
        'src/OS/Windows/Library.cpp',
        'src/OS/Windows/OS.cpp',

        'src/Terminal/WindowsTerminal/WindowsTerminal.cpp',

        'src/Terminal/Factory/Windows.cpp',
    ]

    cpp_args += '-DBUILD_FOR_WINDOWS'
else
    src += [
        'src/OS/Unix/File.cpp',
        'src/OS/Unix/Library.cpp',
        'src/OS/Unix/OS.cpp',
        'src/OS/Unix/PlatformFolders.cpp',
        'src/OS/Unix/Whereami.cpp',

        'src/Terminal/NcursesTerminal/Color.cpp',
        'src/Terminal/NcursesTerminal/NcursesTerminal.cpp',

        'src/Terminal/Factory/Unix.cpp',
    ]
endif


libappcui = library('appcui', src,
    cpp_args : cpp_args,
    include_directories : [ inc, 'src' ],
    override_options : [ 'unity=off' ],
)

appcui = declare_dependency(
    include_directories : inc,
    link_with : libappcui,
)
