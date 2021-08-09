#ifndef __APPCUI_MAIN_HEADER__
#define __APPCUI_MAIN_HEADER__

#include "Features.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#ifdef BUILD_AS_DYNAMIC_LIB
#    ifdef BUILD_FOR_WINDOWS
#        define EXPORT __declspec(dllexport)
#    else
#        define EXPORT
#    endif
#else
#    define EXPORT
#endif

#if _DEBUG
#    define ENABLE_LOGGING
#endif

#ifdef ENABLE_LOGGING
#    define CHECK(c, returnValue, format, ...)                                                                         \
        {                                                                                                              \
            if (!(c))                                                                                                  \
            {                                                                                                          \
                AppCUI::Log::Report(                                                                                   \
                      AppCUI::Log::Severity::Error, __FILE__, __FUNCTION__, #c, __LINE__, format, ##__VA_ARGS__);      \
                return (returnValue);                                                                                  \
            }                                                                                                          \
        }
#    define CHECKBK(c, format, ...)                                                                                    \
        {                                                                                                              \
            if (!(c))                                                                                                  \
            {                                                                                                          \
                AppCUI::Log::Report(                                                                                   \
                      AppCUI::Log::Severity::Error, __FILE__, __FUNCTION__, #c, __LINE__, format, ##__VA_ARGS__);      \
                break;                                                                                                 \
            }                                                                                                          \
        }
#    define RETURNERROR(returnValue, format, ...)                                                                      \
        {                                                                                                              \
            AppCUI::Log::Report(                                                                                       \
                  AppCUI::Log::Severity::Error, __FILE__, __FUNCTION__, "", __LINE__, format, ##__VA_ARGS__);          \
            return (returnValue);                                                                                      \
        }
#    define NOT_IMPLEMENTED(returnValue)                                                                               \
        {                                                                                                              \
            AppCUI::Log::Report(                                                                                       \
                  AppCUI::Log::Severity::Warning,                                                                      \
                  __FILE__,                                                                                            \
                  __FUNCTION__,                                                                                        \
                  "",                                                                                                  \
                  __LINE__,                                                                                            \
                  "Current function/method is not implemented under current OS");                                      \
            return (returnValue);                                                                                      \
        }
#    define LOG_INFO(format, ...)                                                                                      \
        AppCUI::Log::Report(                                                                                           \
              AppCUI::Log::Severity::Information, __FILE__, __FUNCTION__, "", __LINE__, format, ##__VA_ARGS__);
#    define LOG_WARNING(format, ...)                                                                                   \
        AppCUI::Log::Report(                                                                                           \
              AppCUI::Log::Severity::Warning, __FILE__, __FUNCTION__, "", __LINE__, format, ##__VA_ARGS__);
#    define LOG_ERROR(format, ...)                                                                                     \
        AppCUI::Log::Report(AppCUI::Log::Severity::Error, __FILE__, __FUNCTION__, "", __LINE__, format, ##__VA_ARGS__);
#else
#    define CHECK(c, returnValue, format, ...)                                                                         \
        {                                                                                                              \
            if (!(c))                                                                                                  \
                return (returnValue);                                                                                  \
        }
#    define RETURNERROR(returnValue, format, ...)                                                                      \
        {                                                                                                              \
            return (returnValue);                                                                                      \
        }
#    define CHECKBK(c, format, ...)                                                                                    \
        if (!(c))                                                                                                      \
            break;
#    define NOT_IMPLEMENTED(returnValue)                                                                               \
        {                                                                                                              \
            return (returnValue);                                                                                      \
        }
#    define LOG_INFO(format, ...)
#    define LOG_WARNING(format, ...)
#    define LOG_ERROR(format, ...)
#endif

#define ADD_FLAG_OPERATORS(type, basic_type)                                                                           \
    inline constexpr type operator| (const type f1, const type f2)                                                     \
    {                                                                                                                  \
        return static_cast<type>(static_cast<basic_type>(f1) | static_cast<basic_type>(f2));                           \
    }                                                                                                                  \
    inline constexpr basic_type operator| (const basic_type f1, const type f2)                                         \
    {                                                                                                                  \
        return static_cast<basic_type>(f1) | static_cast<basic_type>(f2);                                              \
    }                                                                                                                  \
    inline constexpr basic_type operator| (const type f1, const basic_type f2)                                         \
    {                                                                                                                  \
        return static_cast<basic_type>(f1) | static_cast<basic_type>(f2);                                              \
    }                                                                                                                  \
    inline constexpr type operator& (const type f1, const type f2)                                                     \
    {                                                                                                                  \
        return static_cast<type>(static_cast<basic_type>(f1) & static_cast<basic_type>(f2));                           \
    }                                                                                                                  \
    inline constexpr type operator& (const basic_type f1, const type f2)                                               \
    {                                                                                                                  \
        return static_cast<type>(static_cast<basic_type>(f1) & static_cast<basic_type>(f2));                           \
    }                                                                                                                  \
    inline constexpr type operator& (const type f1, const basic_type f2)                                               \
    {                                                                                                                  \
        return static_cast<type>(static_cast<basic_type>(f1) & static_cast<basic_type>(f2));                           \
    }                                                                                                                  \
    inline constexpr bool operator&& (const type f1, const basic_type f2)                                              \
    {                                                                                                                  \
        return ((static_cast<basic_type>(f1) & static_cast<basic_type>(f2))!=0);                                       \
    }                                                                                                                  \
    inline constexpr bool operator&& (const basic_type f1, const type f2)                                              \
    {                                                                                                                  \
        return ((static_cast<basic_type>(f1) & static_cast<basic_type>(f2))!=0);                                       \
    }                                                                                                                  \
    inline constexpr type operator|=(type& f1, const type f2)                                                          \
    {                                                                                                                  \
        return f1 = static_cast<type>(static_cast<basic_type>(f1) | static_cast<basic_type>(f2));                      \
    }

#define PATH_SEPARATOR '\\'

#define MAXVALUE(x, y) ((x) > (y) ? (x) : (y))
#define MINVALUE(x, y) ((x) < (y) ? (x) : (y))

namespace AppCUI
{
namespace Utils
{
    using ConstString = std::variant<std::string_view, std::u8string_view>;
}
namespace Application
{
    struct Config;
    class CommandBar;
}; // namespace Application
namespace Graphics
{
    struct Size
    {
        unsigned int Width, Height;
        inline Size() : Width(0), Height(0)
        {
        }
        inline Size(unsigned int width, unsigned int height) : Width(width), Height(height)
        {
        }
        inline void Set(unsigned int width, unsigned int height)
        {
            Width  = width;
            Height = height;
        }
    };
    struct Point
    {
        int X, Y;
        inline Point() : X(0), Y(0)
        {
        }
        inline Point(int x, int y) : X(x), Y(y)
        {
        }
        inline void Set(int x, int y)
        {
            X = x;
            Y = y;
        }
    };
}; // namespace Console
namespace Input
{
    enum class Key : unsigned int
    {
        None = 0,
        F1   = 1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        Enter,
        Escape,
        Insert,
        Delete,
        Backspace,
        Tab,
        Left,
        Up,
        Down,
        Right,
        PageUp,
        PageDown,
        Home,
        End,
        Space,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        N0,
        N1,
        N2,
        N3,
        N4,
        N5,
        N6,
        N7,
        N8,
        N9,
        Count, // must be the last

        // Modifier flas
        Alt   = 0x1000,
        Ctrl  = 0x2000,
        Shift = 0x4000
    };
    enum class MouseButton: unsigned int
    {
        None    = 0,
        Left    = 0x01,
        Center  = 0x02,
        Right   = 0x04
    };
};
namespace Utils
{
    class EXPORT Array32
    {
        unsigned int* Data;
        unsigned int Count, Allocated;

        bool Grow(unsigned int newSize);

      public:
        Array32();
        ~Array32();
        void Destroy();

        bool Create(unsigned int allocatedCount = 64);
        bool Create(unsigned int* vector, unsigned int vectorSize, unsigned int elementsCount = 0);
        bool Create(int* vector, unsigned int vectorSize, unsigned int elementsCount = 0);

        inline unsigned int* GetUInt32Array() const
        {
            return Data;
        }
        inline int* GetInt32Array() const
        {
            return (int*) Data;
        }
        inline unsigned int Len() const
        {
            return Count;
        }
        inline unsigned int GetAllocatedSize() const
        {
            return Allocated & 0x7FFFFFFF;
        }

        inline void Clear()
        {
            Count = 0;
        }
        bool Reserve(unsigned int newSize);
        bool Resize(unsigned int newSize);
        bool Push(unsigned int value);
        bool Push(int value);
        bool Get(unsigned int index, unsigned int& value);
        bool Get(unsigned int index, int& value);

        bool Sort(int (*compare)(int elem1, int elem2, void* Context), bool ascendent, void* Context = nullptr);
        bool Sort(
              int (*compare)(unsigned int elem1, unsigned int elem2, void* Context),
              bool ascendent,
              void* Context = nullptr);
    };
    class EXPORT String
    {
        char* Text;
        unsigned int Size;
        unsigned int Allocated;

        bool Grow(unsigned int newSize);

      public:
        String(void);
        String(const String& s);
        ~String(void);

        // Static functions
        static unsigned int Len(const char* string);
        static bool Add(
              char* destination,
              const char* source,
              unsigned int maxDestinationSize,
              unsigned int destinationSize          = 0xFFFFFFFF,
              unsigned int sourceSize               = 0xFFFFFFFF,
              unsigned int* resultedDestinationSize = nullptr);
        static bool Set(
              char* destination,
              const char* source,
              unsigned int maxDestinationSize,
              unsigned int sourceSize               = 0xFFFFFFFF,
              unsigned int* resultedDestinationSize = nullptr);
        static bool Equals(const char* sir1, const char* sir2, bool ignoreCase = false);
        static bool StartsWith(const char* sir, const char* text, bool ignoreCase = false);
        static bool EndsWith(
              const char* sir,
              const char* text,
              bool ignoreCase          = false,
              unsigned int sirTextSize = 0xFFFFFFFF,
              unsigned int textSize    = 0xFFFFFFFF);
        static bool Contains(const char* sir, const char* textToFind, bool ignoreCase = false);
        static int Compare(const char* sir1, const char* sir2, bool ignoreCase = false);
        // Create string object
        bool Create(unsigned int initialAllocatedBuffer = 64);
        bool Create(const char* text);
        bool Create(char* buffer, unsigned int bufferSize, bool emptyString = false);

        const char* GetText() const
        {
            return Text;
        }
        unsigned int Len() const
        {
            return Size;
        }
        unsigned int GetAllocatedSize() const
        {
            return Allocated & 0x7FFFFFFF;
        }

        int GetChar(int index) const;
        bool SetChar(int index, char value);

        bool Add(const char* text, unsigned int size = 0xFFFFFFFF);
        bool Add(const String& text);
        bool Add(const String* text);
        bool AddChar(char ch);
        bool AddChars(char ch, unsigned int count);

        bool InsertChar(char character, unsigned int position);
        bool DeleteChar(unsigned int position);
        bool Delete(unsigned int start, unsigned int end);

        bool Set(const char* text, unsigned int size = 0xFFFFFFFF);
        bool Set(const String& text);
        bool Set(const String* text);
        bool SetChars(char ch, unsigned int count);

        bool SetFormat(const char* format, ...);
        bool AddFormat(const char* format, ...);
        const char* Format(const char* format, ...);

        bool Realloc(unsigned int newSize);
        void Destroy();
        bool Truncate(unsigned int newSize);
        void Clear();

        bool StartsWith(const char* text, bool ignoreCase = false) const;
        bool StartsWith(const String* text, bool ignoreCase = false) const;
        bool StartsWith(const String& text, bool ignoreCase = false) const;
        bool EndsWith(const char* text, bool ignoreCase = false) const;
        bool EndsWith(const String* text, bool ignoreCase = false) const;
        bool EndsWith(const String& text, bool ignoreCase = false) const;
        bool Equals(const char* text, bool ignoreCase = false) const;
        bool Equals(const String& ss, bool ignoreCase = false) const;
        bool Contains(const char* text, bool ignoreCase = false) const;
        bool Contains(const String& ss, bool ignoreCase = false) const;
        int CompareWith(const char* text, bool ignoreCase = false) const;

        void ConvertToInternalNewLineFormat();

        inline String& operator=(const String& s)
        {
            this->Set(s);
            return *this;
        }
        inline String& operator=(const char* text)
        {
            this->Set(text);
            return *this;
        }
        inline operator char*() const
        {
            return Text;
        }
        inline operator const char*() const
        {
            return Text;
        }
        inline String& operator+=(const String& s)
        {
            this->Add(s);
            return *this;
        }
        inline String& operator+=(const char* text)
        {
            this->Add(text);
            return *this;
        }

        inline bool operator==(const String& s) const
        {
            return this->Equals(s);
        }
        inline bool operator!=(const String& s) const
        {
            return !this->Equals(s);
        }
        char& operator[](int poz);
    };

    enum class NumberParseFlags : unsigned int
    {
        None       = 0,
        BaseAuto   = 0,
        Base10     = 0x00000001,
        Base8      = 0x00000002,
        Base16     = 0x00000003,
        Base2      = 0x00000004,
        TrimSpaces = 0x00000010,
    };
    namespace Number
    {
        EXPORT std::optional<unsigned long long> ToUInt64(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);
        EXPORT std::optional<unsigned int> ToUInt32(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);
        EXPORT std::optional<unsigned short> ToUInt16(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);
        EXPORT std::optional<unsigned char> ToUInt8(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);

        EXPORT std::optional<long long> ToInt64(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);
        EXPORT std::optional<int> ToInt32(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);
        EXPORT std::optional<short> ToInt16(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);
        EXPORT std::optional<char> ToInt8(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);

        EXPORT std::optional<float> ToFloat(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);
        EXPORT std::optional<double> ToDouble(
              std::string_view text, NumberParseFlags flags = NumberParseFlags::None, unsigned int* size = nullptr);

    }; // namespace Number

    template <int size>
    class LocalString : public String
    {
        char tempBuffer[size];

      public:
        LocalString()
        {
            Create(tempBuffer, size, true);
        }
    };

    class EXPORT KeyUtils
    {
      public:
        enum
        {
            KEY_SHIFT_MASK = 0x7000,
            KEY_SHIFT_BITS = 12,
            KEY_CODE_MASK  = 0xFF
        };

        // Returns the name of the Key without modifiers
        static const char* GetKeyName(AppCUI::Input::Key keyCode);
        static const char* GetKeyNamePadded(AppCUI::Input::Key keyCode, unsigned int* nameSize = nullptr);
        static const char* GetKeyModifierName(AppCUI::Input::Key keyCode, unsigned int* nameSize = nullptr);
        static bool ToString(AppCUI::Input::Key keyCode, char* text, int maxTextSize);
        static bool ToString(AppCUI::Input::Key keyCode, AppCUI::Utils::String& text);
        static AppCUI::Input::Key FromString(const char* stringRepresentation);
        static AppCUI::Input::Key FromString(AppCUI::Utils::String& text);
    };

    class EXPORT IniValue
    {
        void* Data;

      public:
        IniValue() : Data(nullptr)
        {
        }
        IniValue(void* data) : Data(data){};

        std::optional<unsigned long long> AsUInt64();
        std::optional<long long> AsInt64();
        std::optional<unsigned int> AsUInt32();
        std::optional<int> AsInt32();
        std::optional<bool> AsBool();
        std::optional<AppCUI::Input::Key> AsKey();
        std::optional<const char*> AsString();
        std::optional<std::string_view> AsStringView();
        std::optional<Graphics::Size> AsSize();
        std::optional<float> AsFloat();
        std::optional<double> AsDouble();

        unsigned long long ToUInt64(unsigned long long defaultValue = 0);
        unsigned int ToUInt32(unsigned int defaultValue = 0);
        long long ToInt64(long long defaultValue = -1);
        int ToInt32(int defaultValue = -1);
        bool ToBool(bool defaultValue = false);
        AppCUI::Input::Key ToKey(AppCUI::Input::Key defaultValue = AppCUI::Input::Key::None);
        const char* ToString(const char* defaultValue = nullptr);
        std::string_view ToStringView(std::string_view defaultValue = std::string_view{});
        AppCUI::Graphics::Size ToSize(AppCUI::Graphics::Size defaultValue = AppCUI::Graphics::Size());
        float ToFloat(float defaultValue = 0.0f);
        double ToDouble(double defaultValue = 0.0);
    };
    class EXPORT IniSection
    {
        void* Data;

      public:
        IniSection() : Data(nullptr)
        {
        }
        IniSection(void* data) : Data(data){};

        inline bool Exists() const
        {
            return Data != nullptr;
        }
        const char* GetName() const;
        IniValue GetValue(std::string_view keyName);
    };
    class EXPORT IniObject
    {
        void* Data;
        bool Init();

      public:
        IniObject();
        ~IniObject();

        bool CreateFromString(std::string_view text);
        bool CreateFromFile(const char* fileName);
        bool Create();

        bool HasSection(std::string_view name);
        IniSection GetSection(std::string_view name);
        IniValue GetValue(std::string_view valuePath);
        unsigned int GetSectionsCount();
    };
}; // namespace Utils
namespace OS
{
    class EXPORT Clipboard
    {
        Clipboard() = delete;

      public:
        static bool SetText(const char* text, unsigned int textLen = 0xFFFFFFFF);
        static bool SetText(const AppCUI::Utils::String& text);
        static bool GetText(AppCUI::Utils::String& text);
        static bool Clear();
    };
    class EXPORT IFile
    {
      public:
        virtual ~IFile();

        // virtual methods
        virtual bool ReadBuffer(void* buffer, unsigned int bufferSize, unsigned int& bytesRead);
        virtual bool WriteBuffer(const void* buffer, unsigned int bufferSize, unsigned int& bytesWritten);
        virtual unsigned long long GetSize();
        virtual unsigned long long GetCurrentPos();
        virtual bool SetSize(unsigned long long newSize);
        virtual bool SetCurrentPos(unsigned long long newPosition);
        virtual void Close();

        // other methods
        bool Read(void* buffer, unsigned int bufferSize, unsigned int& bytesRead);
        bool Write(const void* buffer, unsigned int bufferSize, unsigned int& bytesWritten);
        bool Read(void* buffer, unsigned int bufferSize);
        bool Write(const void* buffer, unsigned int bufferSize);
        bool Read(unsigned long long offset, void* buffer, unsigned int bufferSize, unsigned int& bytesRead);
        bool Write(unsigned long long offset, const void* buffer, unsigned int bufferSize, unsigned int& bytesWritten);
        bool Write(std::string_view text);
        bool Write(unsigned long long offset, std::string_view text, unsigned int& bytesWritten);
        std::unique_ptr<char[]> ReadContentToBuffer(unsigned int& bufferSize);
    };

    class EXPORT File : public IFile
    {
        union
        {
            void* Handle;
            unsigned long long u64Value;
            unsigned int u32Value;
            int fid;
        } FileID;

      public:
        File();
        ~File();

        /**
         * Opens a file for Read/Write. The file MUST exists. The file pointer will be set at the end of the file.
         * @param[in] filePath is the full path to an existing file.
         */
        bool OpenWrite(const char* filePath);
        /**
         * Opens a file for Read. The file MUST exists. The file pointer will be set at the begining of the file.
         * @param[in] filePath is the full path to an existing file.
         */
        bool OpenRead(const char* filePath);
        /**
         * Creates a new file. If the file exists and overwriteExisting parameter is set to true, it will be
         * overwritten.
         * @param[in] filePath is the full path to an existing file.
         * @param[in] overwriteExisting - if set to true and the file exists it will overwrite the file. If set to false
         * and a file exists, will fail to overwrite and the method will return false.
         */
        bool Create(const char* filePath, bool overwriteExisting = true);

        bool ReadBuffer(void* buffer, unsigned int bufferSize, unsigned int& bytesRead) override;
        bool WriteBuffer(const void* buffer, unsigned int bufferSize, unsigned int& bytesWritten) override;
        unsigned long long GetSize() override;
        unsigned long long GetCurrentPos() override;
        bool SetSize(unsigned long long newSize) override;
        bool SetCurrentPos(unsigned long long newPosition) override;
        void Close() override;
    };

    EXPORT void GetSpecialFolders(std::vector<std::pair<std::string, std::filesystem::path>>& specialFolderLists);

} // namespace OS
namespace Graphics
{
    enum class Color : unsigned char
    {
        Black       = 0x00,
        DarkBlue    = 0x01,
        DarkGreen   = 0x02,
        Teal        = 0x03,
        DarkRed     = 0x04,
        Magenta     = 0x05,
        Olive       = 0x06,
        Silver      = 0x07,
        Gray        = 0x08,
        Blue        = 0x09,
        Green       = 0x0A,
        Aqua        = 0x0B,
        Red         = 0x0C,
        Pink        = 0x0D,
        Yellow      = 0x0E,
        White       = 0x0F,
        Transparent = 0x10,
    };

    enum class Alignament : unsigned int
    {
        TopLeft = 0,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left,
        Center
    };
    enum class SpecialChars : unsigned int
    {
        BoxTopLeftCornerDoubleLine = 0,
        BoxTopRightCornerDoubleLine,
        BoxBottomRightCornerDoubleLine,
        BoxBottomLeftCornerDoubleLine,
        BoxHorizontalDoubleLine,
        BoxVerticalDoubleLine,
        BoxCrossDoubleLine,

        BoxTopLeftCornerSingleLine,
        BoxTopRightCornerSingleLine,
        BoxBottomRightCornerSingleLine,
        BoxBottomLeftCornerSingleLine,
        BoxHorizontalSingleLine,
        BoxVerticalSingleLine,
        BoxCrossSingleLine,

        // arrows
        ArrowUp,
        ArrowDown,
        ArrowLeft,
        ArrowRight,
        ArrowUpDown,
        ArrowLeftRight,

        // Blocks
        Block0,
        Block25,
        Block50,
        Block75,
        Block100,
        BlockUpperHalf,
        BlockLowerHalf,
        BlockLeftHalf,
        BlockRightHalf,
        BlockCentered,

        // Trangles
        TriangleUp,
        TriangleDown,
        TriangleLeft,
        TriangleRight,

        // symbols
        CircleFilled,
        CircleEmpty,
        CheckMark,

        // always last
        Count
    };
    enum class WriteCharacterBufferFlags : unsigned int
    {
        NONE             = 0,
        SINGLE_LINE      = 0x0000001,
        MULTIPLE_LINES   = 0x0000002,
        OVERWRITE_COLORS = 0x0000004,
        WRAP_TO_WIDTH    = 0x0000008,
        HIGHLIGHT_HOTKEY = 0x0000010,
        BUFFER_RANGE     = 0x0000020,
        PROCESS_NEW_LINE = 0x0000040,
    };
    enum class TextAlignament : unsigned int
    {
        Left    = 0x00,
        Center  = 0x01,
        Right   = 0x02,
        Padding = 0x04
    };

    namespace ProgressStatus
    {
        void EXPORT Init(const char* Title, unsigned long long maxValue = 0);
        bool EXPORT Update(unsigned long long value, const char* text = nullptr);
    }; // namespace ProgressStatus

    struct ColorPair
    {
        Color Foreground;
        Color Background;
    };
    constexpr ColorPair NoColorPair      = ColorPair{ Color::Transparent, Color::Transparent };
    constexpr ColorPair DefaultColorPair = ColorPair{ Color::White, Color::Black };
    struct Character
    {
        union
        {
            struct
            {
                char16_t Code;
                ColorPair Color;
            };
            unsigned int PackedValue;
        };
    };

    struct WriteCharacterBufferParams
    {
        WriteCharacterBufferFlags Flags;
        ColorPair Color;
        ColorPair HotKeyColor;
        unsigned int HotKeyPosition;
        unsigned int Start, End;
        unsigned int Width;
        WriteCharacterBufferParams() : Flags(WriteCharacterBufferFlags::NONE)
        {
        }
        WriteCharacterBufferParams(WriteCharacterBufferFlags _flg) : Flags(_flg)
        {
        }
    };

    class EXPORT Rect
    {
        int X, Y, Width, Height;

      public:
        Rect() : X(0), Y(0), Width(0), Height(0)
        {
        }
        bool Create(int x, int y, int width, int height, Alignament align);
        void Create(int left, int top, int right, int bottom);
        inline bool Contains(int x, int y) const
        {
            return (x >= this->X) && (y >= this->Y) && (x < (this->X + this->Width)) && (y < (this->Y + this->Height));
        }
        inline int GetCenterX() const
        {
            return X + (Width >> 1);
        }
        inline int GetCenterY() const
        {
            return Y + (Height >> 1);
        }
        inline int GetX() const
        {
            return X;
        }
        inline int GetY() const
        {
            return Y;
        }
        inline unsigned int GetWidth() const
        {
            return (unsigned int) Width;
        }
        inline unsigned int GetHeight() const
        {
            return (unsigned int) Height;
        }
        inline int GetLeft() const
        {
            return X;
        }
        inline int GetTop() const
        {
            return Y;
        }
        inline int GetRight() const
        {
            return X + Width - 1;
        }
        inline int GetBotom() const
        {
            return X + Height - 1;
        }
    };

    class EXPORT Clip
    {
      public:
        struct
        {
            int X, Y, Width, Height;
        } ClipRect;
        struct
        {
            int X, Y;
        } ScreenPosition;
        bool Visible;

      public:
        Clip();
        Clip(const Clip& parent, int relative_x, int relative_y, int width, int height);
        Clip(int x, int y, int width, int height);
        void Reset();
        void Set(const Clip& parent, int relativeLeft, int relativeTop, int relativeRight, int relativeBottom);
        void Set(int x, int y, int width, int height);
    };

    class EXPORT CharacterBuffer
    {
        Character* Buffer;
        size_t Count;
        size_t Allocated;


        bool Grow(size_t newSize);
        bool Add(const std::string_view text, const ColorPair color = NoColorPair);
        bool Add(const std::u8string_view text, const ColorPair color = NoColorPair);
        bool Set(const std::string_view text, const ColorPair color = NoColorPair);
        bool Set(const std::u8string_view text, const ColorPair color = NoColorPair);
        bool SetWithHotKey(
              const std::string_view text, unsigned int& hotKeyCharacterPosition, const ColorPair color = NoColorPair);
        bool SetWithHotKey(
              const std::u8string_view text,
              unsigned int& hotKeyCharacterPosition,
              const ColorPair color = NoColorPair);
        bool SetWithNewLines(const std::u8string_view text, const ColorPair color = NoColorPair);
        bool SetWithNewLines(const std::string_view text, const ColorPair color = NoColorPair);
        int  FindAscii(const std::string_view& text, bool ignoreCase) const;
        int  FindUTF8(const std::u8string_view& text, bool ignoreCase) const;
      public:
        void Swap(CharacterBuffer&) noexcept;
        CharacterBuffer();
        inline CharacterBuffer(const CharacterBuffer& obj)
        {
            Buffer = nullptr;
            Count = Allocated = 0;
            Set(obj);
        }
        inline CharacterBuffer(CharacterBuffer&& obj) noexcept
        {
            Buffer = nullptr;
            Count = Allocated = 0;
            Swap(obj);
        }
        ~CharacterBuffer();

        void Destroy();
        void Clear();

        inline size_t Len() const
        {
            return Count;
        }
        inline Character* GetBuffer() const
        {
            return Buffer;
        }
        inline bool IsValid() const
        {
            return Buffer != nullptr;
        }

        bool Set(const CharacterBuffer& obj);
        bool Add(const AppCUI::Utils::ConstString& text, const ColorPair color = NoColorPair);
        bool Set(const AppCUI::Utils::ConstString& text, const ColorPair color = NoColorPair);
        bool SetWithHotKey(
              const AppCUI::Utils::ConstString& text,
              unsigned int& hotKeyCharacterPosition,
              const ColorPair color = NoColorPair);

        bool SetWithNewLines(const AppCUI::Utils::ConstString& text, const ColorPair color = NoColorPair);

        bool Delete(unsigned int start, unsigned int end);
        bool DeleteChar(unsigned int position);
        bool Insert(
              const char* text,
              unsigned int position,
              const ColorPair color = NoColorPair,
              unsigned int textSize = 0xFFFFFFFF);
        bool Insert(const AppCUI::Utils::String& text, unsigned int position, const ColorPair color = NoColorPair);
        bool InsertChar(unsigned short characterCode, unsigned int position, const ColorPair color = NoColorPair);
        bool SetColor(unsigned int start, unsigned int end, const ColorPair color);
        void SetColor(const ColorPair color);
        bool CopyString(Utils::String& text, unsigned int start, unsigned int end);
        bool CopyString(Utils::String& text);

        int  Find(const AppCUI::Utils::ConstString& text, bool ignoreCase = true) const;
        inline bool Contains(const AppCUI::Utils::ConstString& text, bool ignoreCase = true) const
        {
            return Find(text, ignoreCase) != -1;
        }
        int CompareWith(const CharacterBuffer& obj, bool ignoreCase = true) const;

        bool ToString(std::string& output) const;
        bool ToString(std::u16string& output) const;

        inline CharacterBuffer& operator=(const CharacterBuffer& obj) { Set(obj); return *this; }
        inline CharacterBuffer& operator=(CharacterBuffer&& obj) noexcept { Swap(obj); return *this; }
        inline operator std::string() const
        {
            std::string temp;
            ToString(temp);
            return temp;
        }
        inline operator std::u16string() const
        {
            std::u16string temp;
            ToString(temp);
            return temp;
        }
    };

    class EXPORT Image
    {
        unsigned int* Pixels;
        unsigned int Width;
        unsigned int Height;

      public:
        Image();
        ~Image();
        bool Create(unsigned int width, unsigned int height);
        bool SetPixel(unsigned int x, unsigned int y, const Color color);
        bool SetPixel(unsigned int x, unsigned int y, unsigned int colorRGB);
        bool SetPixel(
              unsigned int x,
              unsigned int y,
              unsigned char Red,
              unsigned char Green,
              unsigned char Blue,
              unsigned char Alpha = 255);
        unsigned int GetPixel(unsigned int x, unsigned int y, unsigned int invalidIndexValue = 0);
        bool GetPixel(unsigned int x, unsigned int y, unsigned int& color);
        bool Clear(unsigned int color);
        bool Clear(const Color color);
        inline unsigned int GetWidth() const
        {
            return Width;
        }
        inline unsigned int GetHeight() const
        {
            return Height;
        }
        inline unsigned int* GetPixelsBuffer() const
        {
            return Pixels;
        }
    };

    class EXPORT Canvas;
    class EXPORT Renderer
    {
      protected:
        Character* Characters;
        Character** OffsetRows;
        unsigned int Width, Height;
        int TranslateX, TranslateY;
        struct
        {
            int Left, Top, Right, Bottom;
            bool Visible;
        } Clip, ClipCopy;
        bool ClipHasBeenCopied;
        struct
        {
            unsigned int X, Y;
            bool Visible;
        } Cursor;

        Renderer();
        ~Renderer();

        void _Destroy();
        bool _ClearEntireSurface(int character, const ColorPair color);
        bool _WriteCharacterBuffer_SingleLine(
              int x,
              int y,
              const CharacterBuffer& cb,
              const WriteCharacterBufferParams& params,
              unsigned int start,
              unsigned int end);
        bool _WriteCharacterBuffer_MultiLine_WithWidth(
              int x,
              int y,
              const CharacterBuffer& cb,
              const WriteCharacterBufferParams& params,
              unsigned int start,
              unsigned int end);
        bool _WriteCharacterBuffer_MultiLine_ProcessNewLine(
              int x,
              int y,
              const CharacterBuffer& cb,
              const WriteCharacterBufferParams& params,
              unsigned int start,
              unsigned int end);

      public:
        // Horizontal lines
        bool DrawHorizontalLine(int left, int y, int right, int charCode, const ColorPair color);
        bool DrawHorizontalLineSize(int x, int y, unsigned int size, int charCode, const ColorPair color);
        bool DrawHorizontalLineWithSpecialChar(int left, int y, int right, SpecialChars charID, const ColorPair color);

        // Vertical lines
        bool DrawVerticalLine(int x, int top, int bottom, int charCode, const ColorPair color);
        bool DrawVerticalLineSize(int x, int y, unsigned int size, int charCode, const ColorPair color);
        bool DrawVerticalLineWithSpecialChar(int x, int top, int bottom, SpecialChars charID, const ColorPair color);

        // Rectangle
        bool FillRect(int left, int top, int right, int bottom, int charCode, const ColorPair color);
        bool FillRectSize(int x, int y, unsigned int width, unsigned int height, int charCode, const ColorPair color);
        bool DrawRect(int left, int top, int right, int bottom, const ColorPair color, bool doubleLine);
        bool DrawRectSize(
              int x, int y, unsigned int width, unsigned int height, const ColorPair color, bool doubleLine);

        // Texts
        bool WriteSingleLineText(int x, int y, const char* text, const ColorPair color, int textSize = -1);
        bool WriteSingleLineText(
              int x,
              int y,
              const char* text,
              unsigned int width,
              const ColorPair color,
              TextAlignament align = TextAlignament::Left,
              int textSize         = -1);
        bool WriteSingleLineTextWithHotKey(
              int x, int y, const char* text, const ColorPair color, const ColorPair hotKeyColor, int textSize = -1);
        bool WriteSingleLineTextWithHotKey(
              int x,
              int y,
              const char* text,
              unsigned int width,
              const ColorPair color,
              const ColorPair hotKeyColor,
              unsigned int hotKeyOffset,
              TextAlignament align = TextAlignament::Left,
              int textSize         = -1);
        bool WriteMultiLineText(int x, int y, const char* text, const ColorPair color, int textSize = -1);
        bool WriteMultiLineTextWithHotKey(
              int x, int y, const char* text, const ColorPair color, const ColorPair hotKeyColor, int textSize = -1);

        // Characters
        bool WriteCharacter(int x, int y, int charCode, const ColorPair color);
        bool WriteSpecialCharacter(int x, int y, SpecialChars charID, const ColorPair color);

        // Character Buffer
        bool WriteCharacterBuffer(
              int x,
              int y,
              const AppCUI::Graphics::CharacterBuffer& cb,
              const AppCUI::Graphics::WriteCharacterBufferParams& params);
        bool WriteCharacterBuffer(
              int x,
              int y,
              unsigned int width,
              const AppCUI::Graphics::CharacterBuffer& cb,
              const ColorPair textColor,
              TextAlignament align);
        bool WriteCharacterBuffer(
              int x,
              int y,
              unsigned int width,
              const AppCUI::Graphics::CharacterBuffer& cb,
              const ColorPair textColor,
              const ColorPair hotKeyColor,
              unsigned int hotKeyOffset,
              TextAlignament align);

        // Canvas & Images
        bool DrawCanvas(int x, int y, const Canvas& canvas, const ColorPair overwriteColor = NoColorPair);

        // Clear
        bool ClearWithSpecialChar(SpecialChars charID, const ColorPair color);
        bool Clear(int charCode, const ColorPair color);

        // Clipping
        bool SetClipMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin);
        bool ResetClip();

        // Cursor
        void HideCursor();
        bool SetCursor(int x, int y);
    };

    class EXPORT Canvas : public Renderer
    {
      public:
        Canvas();
        ~Canvas();
        bool Create(
              unsigned int width,
              unsigned int height,
              int fillCharacter     = ' ',
              const ColorPair color = DefaultColorPair);
        bool Resize(
              unsigned int width,
              unsigned int height,
              int fillCharacter     = ' ',
              const ColorPair color = DefaultColorPair);

        // Clipping & Translate
        void SetAbsoluteClip(const AppCUI::Graphics::Clip& clip);
        void ExtendAbsoluteCliptToRightBottomCorner();
        void ClearClip();
        void SetTranslate(int offX, int offY);

        bool SetSize(unsigned int width, unsigned int height);
        void Reset();
        void Update();
        void DarkenScreen();
        bool ClearEntireSurface(int character, const ColorPair color);

        // inlines
        inline unsigned int GetWidth() const
        {
            return this->Width;
        }
        inline unsigned int GetHeight() const
        {
            return this->Height;
        }
        inline bool GetCursorVisibility() const
        {
            return this->Cursor.Visible;
        }
        inline unsigned int GetCursorX() const
        {
            return this->Cursor.X;
        }
        inline unsigned int GetCursorY() const
        {
            return this->Cursor.Y;
        }
        inline Character* GetCharactersBuffer() const
        {
            return this->Characters;
        }
    };

}; // namespace Console
namespace Controls
{
    enum class Event : unsigned int
    {
        EVENT_WINDOW_CLOSE,
        EVENT_WINDOW_ACCEPT,
        EVENT_BUTTON_CLICKED,
        EVENT_CHECKED_STATUS_CHANGED,
        EVENT_TEXT_CHANGED,
        EVENT_TEXTFIELD_VALIDATE,
        EVENT_TAB_CHANGED,
        EVENT_LISTVIEW_CURRENTITEM_CHANGED,
        EVENT_LISTVIEW_SELECTION_CHANGED,
        EVENT_LISTVIEW_ITEM_CHECKED,
        EVENT_LISTVIEW_ITEM_CLICKED,
        EVENT_COMBOBOX_SELECTED_ITEM_CHANGED,
        EVENT_COMBO_CLOSED,
        EVENT_TERMINATE_APPLICATION,
        EVENT_COMMAND,
        EVENT_CUSTOM,
    };
    typedef unsigned int ItemHandle;
    constexpr ItemHandle InvalidItemHandle = 0xFFFFFFFF;
    class EXPORT Control;
    class EXPORT TextField;
    class EXPORT ListView;
    namespace Handlers
    {
        typedef void (*AfterResizeHandler)(
              AppCUI::Controls::Control* control, int newWidth, int newHeight, void* Context);
        typedef bool (*BeforeResizeHandler)(
              AppCUI::Controls::Control* control, int newWidth, int newHeight, void* Context);
        typedef void (*AfterMoveHandler)(AppCUI::Controls::Control* control, int newX, int newY, void* Context);
        typedef bool (*UpdateCommandBarHandler)(AppCUI::Controls::Control* control, void* Context);
        typedef bool (*KeyEventHandler)(
              AppCUI::Controls::Control* control, AppCUI::Input::Key KeyCode, int AsciiCode, void* Context);
        typedef void (*PaintHandler)(AppCUI::Controls::Control* control, void* Context);
        typedef void (*OnFocusHandler)(AppCUI::Controls::Control* control, void* Context);
        typedef bool (*EventHandler)(
              AppCUI::Controls::Control* control,
              const void* sender,
              AppCUI::Controls::Event eventType,
              int controlID,
              void* Context);
        typedef void (*MousePressedHandler)(
              AppCUI::Controls::Control* control, int x, int y, AppCUI::Input::MouseButton button, void* Context);
        typedef void (*MouseReleasedHandler)(
              AppCUI::Controls::Control* control, int x, int y, AppCUI::Input::MouseButton button, void* Context);
        typedef void (*SyntaxHighlightHandler)(
              AppCUI::Controls::Control* control,
              AppCUI::Graphics::Character* characters,
              unsigned int charactersCount,
              void* Context);
        typedef int (*ListViewItemComparer)(
              AppCUI::Controls::ListView* control,
              ItemHandle item1,
              ItemHandle item2,
              unsigned int columnIndex,
              void* Context);

    } // namespace Handlers

    class EXPORT Control
    {
      public:
        void* Context;

      protected:
        bool IsMouseInControl(int x, int y);
        bool SetMargins(int left, int top, int right, int bottom);

        bool Init(Control* parent, const AppCUI::Utils::ConstString & caption, const std::string_view& layout, bool computeHotKey = false);

      public:
        Control();
        bool AddControl(Control* control);
        bool RemoveControl(Control* control);
        bool RemoveControl(unsigned int index);

        bool IsInitialized();

        // coordonates
        int GetX();
        int GetY();
        int GetWidth();
        int GetHeight();
        void GetSize(AppCUI::Graphics::Size& size);
        void GetClientSize(AppCUI::Graphics::Size& size);
        void MoveTo(int newX, int newY);
        bool Resize(int newWidth, int newHeight);
        void RecomputeLayout();

        // groups
        int GetGroup();
        void SetGroup(int newGroupID);
        void ClearGroup();

        // hot key
        bool SetHotKey(char hotKey);
        Input::Key GetHotKey();
        void ClearHotKey();

        // status
        void SetEnabled(bool value);
        void SetVisible(bool value);
        void SetChecked(bool value);
        bool IsEnabled();
        bool IsVisible();
        bool IsChecked();
        bool HasFocus();
        bool IsMouseOver();

        // childern and parent
        Control* GetParent();
        Control** GetChildrenList();
        Control* GetChild(unsigned int index);
        unsigned int GetChildernCount();
        bool GetChildIndex(Control* control, unsigned int& index);

        // Events
        void RaiseEvent(Event eventType);
        void RaiseEvent(Event eventType, int ID);

        // focus & expact/pack view
        bool SetFocus();
        void SetControlID(int newID);
        void ExpandView();
        void PackView();

        // Text
        bool SetText(const AppCUI::Utils::ConstString& caption, bool updateHotKey = false);
        bool SetText(const AppCUI::Graphics::CharacterBuffer& caption);
        const AppCUI::Graphics::CharacterBuffer& GetText();

        // Scroll bars
        void UpdateHScrollBar(unsigned long long value, unsigned long long maxValue);
        void UpdateVScrollBar(unsigned long long value, unsigned long long maxValue);

        // handlere
        void SetOnBeforeResizeHandler(Handlers::BeforeResizeHandler handler, void* Context = nullptr);
        void SetOnAfterResizeHandler(Handlers::AfterResizeHandler handler, void* Context = nullptr);
        void SetOnAfterMoveHandler(Handlers::AfterMoveHandler handler, void* Context = nullptr);
        void SetOnUpdateCommandBarHandler(Handlers::UpdateCommandBarHandler handler, void* Context = nullptr);
        void SetOnKeyEventHandler(Handlers::KeyEventHandler handler, void* Context = nullptr);
        void SetPaintHandler(Handlers::PaintHandler handler, void* Context = nullptr);
        void SetOnFocusHandler(Handlers::OnFocusHandler handler, void* Context = nullptr);
        void SetOnLoseFocusHandler(Handlers::OnFocusHandler handler, void* Context = nullptr);
        void SetEventHandler(Handlers::EventHandler handler, void* Context = nullptr);
        void SetMousePressedHandler(Handlers::MousePressedHandler handler, void* Context = nullptr);
        void SetMouseReleasedHandler(Handlers::MouseReleasedHandler handler, void* Context = nullptr);
        void SetMouseHandler(
              Handlers::MousePressedHandler mousePressedHandler,
              Handlers::MouseReleasedHandler mouseReleasedHandler,
              void* Context = nullptr);

        // paint
        virtual void Paint(Graphics::Renderer& renderer);

        // Evenimente
        virtual bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode);
        virtual void OnHotKey();
        virtual void OnFocus();
        virtual void OnLoseFocus();

        virtual void OnMousePressed(int x, int y, AppCUI::Input::MouseButton button);
        virtual void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button);
        virtual bool OnMouseDrag(int x, int y, AppCUI::Input::MouseButton button);

        virtual bool OnMouseEnter();
        virtual bool OnMouseOver(int x, int y);
        virtual bool OnMouseLeave();

        virtual void OnMouseWheel(int direction);
        virtual bool OnEvent(const void* sender, Event eventType, int controlID);
        virtual bool OnUpdateCommandBar(AppCUI::Application::CommandBar& commandBar);
        virtual void OnUpdateScrollBars();

        virtual bool OnBeforeResize(int newWidth, int newHeight);
        virtual void OnAfterResize(int newWidth, int newHeight);
        virtual bool OnBeforeAddControl(Control* ctrl);
        virtual void OnAfterAddControl(Control* ctrl);
        virtual bool OnBeforeSetText(const char* text);
        virtual void OnAfterSetText(const char* text);

        virtual void OnExpandView(AppCUI::Graphics::Clip& expandedClip);
        virtual void OnPackView();

        virtual ~Control();
    };

    enum class WindowFlags : unsigned int
    {
        None          = 0,
        Sizeable      = 0x000100,
        NotifyWindow  = 0x000200,
        ErrorWindow   = 0x000400,
        WarningWindow = 0x000800,
        NoCloseButton = 0x001000,
        FixedPosition = 0x004000,
        Maximized     = 0x008000
    };
    class EXPORT Window : public Control
    {
      public:
        bool Create(
              const AppCUI::Utils::ConstString& caption,
              const std::string_view& layout,
              WindowFlags windowsFlags = WindowFlags::None);
        void Paint(Graphics::Renderer& renderer) override;
        void OnMousePressed(int x, int y, AppCUI::Input::MouseButton button) override;
        void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseDrag(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseOver(int x, int y) override;
        bool OnMouseLeave() override;
        int Show();
        int GetDialogResult();
        bool MaximizeRestore();
        bool OnBeforeResize(int newWidth, int newHeight) override;
        void OnAfterResize(int newWidth, int newHeight) override;
        bool CenterScreen();
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        bool Exit(int dialogResult);
        bool IsWindowInResizeMode();

        virtual ~Window();
    };
    class EXPORT Label : public Control
    {
      public:
        bool Create(Control* parent, const AppCUI::Utils::ConstString& caption, const std::string_view& layout);
        void Paint(Graphics::Renderer& renderer) override;
    };
    class EXPORT Button : public Control
    {
      public:
        bool Create(
              Control* parent,
              const AppCUI::Utils::ConstString& caption,
              const std::string_view& layout,
              int controlID = 0);
        void OnMousePressed(int x, int y, AppCUI::Input::MouseButton button) override;
        void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseDrag(int x, int y, AppCUI::Input::MouseButton button) override;
        void Paint(Graphics::Renderer& renderer) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnHotKey() override;
        bool OnMouseEnter() override;
        bool OnMouseLeave() override;
    };
    class EXPORT CheckBox : public Control
    {
      public:
        bool Create(
              Control* parent,
              const AppCUI::Utils::ConstString& caption,
              const std::string_view& layout,
              int controlID = 0);
        void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button) override;
        void Paint(Graphics::Renderer& renderer) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnHotKey() override;
        bool OnMouseEnter() override;
        bool OnMouseLeave() override;
    };
    class EXPORT RadioBox : public Control
    {
      public:
        bool Create(
              Control* parent,
              const AppCUI::Utils::ConstString& caption,
              const std::string_view& layout,
              int groupID,
              int controlID = 0);
        void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button) override;
        void Paint(Graphics::Renderer& renderer) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnHotKey() override;
        bool OnMouseEnter() override;
        bool OnMouseLeave() override;
    };
    class EXPORT Splitter : public Control
    {
      public:
        bool Create(Control* parent, const std::string_view& layout, bool vertical);
        void Paint(Graphics::Renderer& renderer) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        bool SetSecondPanelSize(int newSize);
        bool HideSecondPanel();
        bool MaximizeSecondPanel();
        void OnAfterResize(int newWidth, int newHeight) override;
        void OnFocus() override;
        bool OnBeforeAddControl(Control* ctrl) override;
        void OnAfterAddControl(Control* ctrl) override;
        void OnMousePressed(int x, int y, AppCUI::Input::MouseButton button) override;
        void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseDrag(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseEnter() override;
        bool OnMouseLeave() override;
        int GetSplitterPosition();
        virtual ~Splitter();
    };
    class EXPORT Panel : public Control
    {
      public:
        bool Create(Control* parent, const AppCUI::Utils::ConstString& caption, const std::string_view& layout);
        bool Create(Control* parent, const std::string_view& layout);
        void Paint(Graphics::Renderer& renderer) override;
    };
    enum class TextFieldFlags : unsigned int
    {
        None               = 0,
        ProcessEnter       = 0x000100,
        Readonly           = 0x000200,
        SyntaxHighlighting = 0x000400,
    };
    class EXPORT TextField : public Control
    {
      public:
        bool Create(
              Control* parent,
              const AppCUI::Utils::ConstString& caption,
              const std::string_view& layout,
              TextFieldFlags flags                     = TextFieldFlags::None,
              Handlers::SyntaxHighlightHandler handler = nullptr,
              void* Context                            = nullptr);
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnAfterSetText(const char* text) override;
        void Paint(Graphics::Renderer& renderer) override;
        void OnFocus() override;
        bool OnMouseEnter() override;
        bool OnMouseLeave() override;

        void SelectAll();
        void ClearSelection();

        virtual ~TextField();
    };
    enum class TextAreaFlags : unsigned int
    {
        None               = 0x000000,
        Border             = 0x000100,
        ShowLineNumbers    = 0x000200,
        ProcessTabKey      = 0x000400,
        Readonly           = 0x000800,
        ScrollBars         = 0x001000,
        SyntaxHighlighting = 0x002000,
    };

    class EXPORT TextArea : public Control
    {
      public:
        bool Create(
              Control* parent,
              const AppCUI::Utils::ConstString& caption,
              const std::string_view& layout,
              TextAreaFlags flags                      = TextAreaFlags::None,
              Handlers::SyntaxHighlightHandler handler = nullptr,
              void* handlerContext                     = nullptr);
        void Paint(Graphics::Renderer& renderer) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnUpdateScrollBars() override;
        void OnFocus() override;
        void OnAfterResize(int newWidth, int newHeight) override;
        void OnAfterSetText(const char* newText) override;
        void SetReadOnly(bool value);
        bool IsReadOnly();
        void SetTabCharacter(char tabCharacter);
        virtual ~TextArea();
    };

    enum class TabFlags : unsigned int
    {
        TopTabs               = 0x000000, // default mode
        BottomTabs            = 0x000100,
        LeftTabs              = 0x000200,
        ListView              = 0x000300,
        TransparentBackground = 0x001000,
        TabsBar               = 0x002000,
    };
    class EXPORT TabPage : public Control
    {
      public:
        bool Create(Control* parent, const AppCUI::Utils::ConstString& caption);
        bool OnBeforeResize(int newWidth, int newHeight);
    };
    class EXPORT Tab : public Control
    {
      public:
        bool Create(
              Control* parent,
              const std::string_view& layout,
              TabFlags flags           = TabFlags::TopTabs,
              unsigned int tabPageSize = 16);
        bool SetCurrentTabPage(unsigned int index);
        bool SetTabPageTitleSize(unsigned int newSize);
        bool SetTabPageName(unsigned int index, const AppCUI::Utils::ConstString& name);
        void OnAfterResize(int newWidth, int newHeight) override;
        void OnFocus() override;
        void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseLeave() override;
        bool OnMouseOver(int x, int y) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnAfterAddControl(Control* ctrl) override;
        void Paint(Graphics::Renderer& renderer) override;
        Control* GetCurrentTab();
    };
    class EXPORT UserControl : public Control
    {
      public:
        bool Create(Control* parent, const AppCUI::Utils::ConstString& caption, const std::string_view& layout);
        bool Create(Control* parent, const std::string_view& layout);
    };
    enum class ViewerFlags : unsigned int
    {
        None   = 0,
        Border = 0x000100,
    };
    class EXPORT CanvasViewer : public Control
    {
      public:
        ~CanvasViewer();
        bool Create(
              Control* parent,
              const std::string_view& layout,
              unsigned int canvasWidth,
              unsigned int canvasHeight,
              ViewerFlags flags = ViewerFlags::None);
        bool Create(
              Control* parent,
              const AppCUI::Utils::ConstString& caption,
              const std::string_view& layout,
              unsigned int canvasWidth,
              unsigned int canvasHeight,
              ViewerFlags flags = ViewerFlags::None);
        void Paint(Graphics::Renderer& renderer) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        bool OnMouseLeave() override;
        bool OnMouseEnter() override;
        void OnUpdateScrollBars() override;
        Graphics::Canvas* GetCanvas();
    };

    enum class ListViewFlags : unsigned int
    {
        None                          = 0,
        HideColumns                   = 0x000100,
        CheckBoxes                    = 0x000200,
        HideColumnsSeparator          = 0x000400,
        Sortable                      = 0x000800,
        ItemSeparators                = 0x001000,
        HideCurrentItemWhenNotFocused = 0x002000,
        AllowMultipleItemsSelection   = 0x004000,
        SearchMode                    = 0x008000,
        HideSearchBar                 = 0x010000
    };
    enum class ListViewItemType : unsigned short
    {
        REGULAR             = 0,
        HIGHLIGHT           = 1,
        INACTIVE            = 2,
        ERROR_INFORMATION   = 3,
        WARNING_INFORMATION = 4,
        COLOR_1             = 5,
        COLOR_2             = 6,
        COLOR_3             = 7
    };
    union ItemData
    {
        void* Pointer;
        unsigned int UInt32Value;
        unsigned long long UInt64Value;
        ItemData() : Pointer(nullptr)
        {
        }
        ItemData(unsigned long long value) : UInt64Value(value)
        {
        }
    };
    class EXPORT ListView : public Control
    {
      public:
        bool Create(Control* parent, const std::string_view& layout, ListViewFlags flags = ListViewFlags::None);
        bool Reserve(unsigned int itemsCount);
        void Paint(Graphics::Renderer& renderer) override;
        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnMouseReleased(int x, int y, AppCUI::Input::MouseButton button) override;
        void OnMousePressed(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseDrag(int x, int y, AppCUI::Input::MouseButton button) override;
        bool OnMouseOver(int x, int y) override;
        bool OnMouseLeave() override;
        void OnFocus() override;
        void OnUpdateScrollBars() override;

        // coloane
        bool AddColumn(const AppCUI::Utils::ConstString& text, AppCUI::Graphics::TextAlignament Align, unsigned int Size = 10);
        bool SetColumnText(unsigned int columnIndex, const AppCUI::Utils::ConstString& text);
        bool SetColumnAlignament(unsigned int columnIndex, AppCUI::Graphics::TextAlignament Align);
        bool SetColumnWidth(unsigned int columnIndex, unsigned int width);
        bool SetColumnClipboardCopyState(unsigned int columnIndex, bool allowCopy);
        bool SetColumnFilterMode(unsigned int columnIndex, bool allowFilterForThisColumn);
        bool DeleteColumn(unsigned int columnIndex);
        void DeleteAllColumns();
        unsigned int GetColumnsCount();

        // items add
        ItemHandle AddItem(const AppCUI::Utils::ConstString& text);
        ItemHandle AddItem(const AppCUI::Utils::ConstString& text, const AppCUI::Utils::ConstString& subItem1);
        ItemHandle AddItem(const AppCUI::Utils::ConstString& text, const AppCUI::Utils::ConstString& subItem1, const AppCUI::Utils::ConstString& subItem2);
        ItemHandle AddItem(const AppCUI::Utils::ConstString& text, const AppCUI::Utils::ConstString& subItem1, const AppCUI::Utils::ConstString& subItem2, const AppCUI::Utils::ConstString& subItem3);
        ItemHandle AddItem(
              const AppCUI::Utils::ConstString& text, const AppCUI::Utils::ConstString& subItem1, const AppCUI::Utils::ConstString& subItem2, const AppCUI::Utils::ConstString& subItem3, const AppCUI::Utils::ConstString& subItem4);
        ItemHandle AddItem(
              const AppCUI::Utils::ConstString& text,
              const AppCUI::Utils::ConstString& subItem1,
              const AppCUI::Utils::ConstString& subItem2,
              const AppCUI::Utils::ConstString& subItem3,
              const AppCUI::Utils::ConstString& subItem4,
              const AppCUI::Utils::ConstString& subItem5);
        ItemHandle AddItem(
              const AppCUI::Utils::ConstString& text,
              const AppCUI::Utils::ConstString& subItem1,
              const AppCUI::Utils::ConstString& subItem2,
              const AppCUI::Utils::ConstString& subItem3,
              const AppCUI::Utils::ConstString& subItem4,
              const AppCUI::Utils::ConstString& subItem5,
              const AppCUI::Utils::ConstString& subItem6);
        ItemHandle AddItem(
              const AppCUI::Utils::ConstString& text,
              const AppCUI::Utils::ConstString& subItem1,
              const AppCUI::Utils::ConstString& subItem2,
              const AppCUI::Utils::ConstString& subItem3,
              const AppCUI::Utils::ConstString& subItem4,
              const AppCUI::Utils::ConstString& subItem5,
              const AppCUI::Utils::ConstString& subItem6,
              const AppCUI::Utils::ConstString& subItem7);
        ItemHandle AddItem(
              const AppCUI::Utils::ConstString& text,
              const AppCUI::Utils::ConstString& subItem1,
              const AppCUI::Utils::ConstString& subItem2,
              const AppCUI::Utils::ConstString& subItem3,
              const AppCUI::Utils::ConstString& subItem4,
              const AppCUI::Utils::ConstString& subItem5,
              const AppCUI::Utils::ConstString& subItem6,
              const AppCUI::Utils::ConstString& subItem7,
              const AppCUI::Utils::ConstString& subItem8);

        // items properties
        bool SetItemText(ItemHandle item, unsigned int subItemIndex, const AppCUI::Utils::ConstString& text);
        const AppCUI::Graphics::CharacterBuffer& GetItemText(ItemHandle item, unsigned int subItemIndex);
        bool SetItemCheck(ItemHandle item, bool check);
        bool SetItemSelect(ItemHandle item, bool select);
        bool SetItemColor(ItemHandle item, AppCUI::Graphics::ColorPair color);
        bool SetItemType(ItemHandle item, ListViewItemType type);
        bool IsItemChecked(ItemHandle item);
        bool IsItemSelected(ItemHandle item);
        bool SetItemData(ItemHandle item, ItemData Data);
        ItemData* GetItemData(ItemHandle item);
        bool SetItemXOffset(ItemHandle item, unsigned int XOffset);
        unsigned int GetItemXOffset(ItemHandle item);
        bool SetItemHeight(ItemHandle item, unsigned int Height);
        unsigned int GetItemHeight(ItemHandle item);
        void DeleteAllItems();
        unsigned int GetItemsCount();
        ItemHandle GetCurrentItem();
        bool SetCurrentItem(ItemHandle item);
        void SelectAllItems();
        void UnSelectAllItems();
        void CheckAllItems();
        void UncheckAllItems();
        unsigned int GetCheckedItemsCount();

        // misc
        void SetClipboardSeparator(char ch);

        // sort
        void SetItemCompareFunction(Handlers::ListViewItemComparer fnc, void* Context = nullptr);
        bool Sort();
        bool Sort(unsigned int columnIndex, bool ascendent);
        bool Sort(
              unsigned int columnIndex, bool ascendent, Handlers::ListViewItemComparer fnc, void* Context = nullptr);

        virtual ~ListView();
    };

    class EXPORT ComboBox : public Control
    {
      public:
        static const unsigned int NO_ITEM_SELECTED = 0xFFFFFFFF;

        //const char* GetUnsafeItemText(unsigned int index);
        //bool GetItemText(unsigned int index, Utils::String& itemText);
        //const char* GetUnsafeCurrentItemText();
        //bool GetCurrentItemtext(Utils::String& itemText);
        ItemData GetCurrentItemUserData();

        bool Create(
              Control* parent, const std::string_view& layout, const AppCUI::Utils::ConstString& text = std::string_view(), char itemsSeparator = ',');
        unsigned int GetItemsCount();
        unsigned int GetCurrentItemIndex();
        const AppCUI::Graphics::CharacterBuffer& GetCurrentItemText();
        ItemData GetItemUserData(unsigned int index);
        const AppCUI::Graphics::CharacterBuffer& GetItemText(unsigned int index);
        bool SetItemUserData(unsigned int index, ItemData userData);
        bool SetCurentItemIndex(unsigned int index);
        void SetNoIndexSelected();
        bool AddItem(const AppCUI::Utils::ConstString& caption, ItemData usedData = { 0 });
        void DeleteAllItems();

        bool OnKeyEvent(AppCUI::Input::Key keyCode, char AsciiCode) override;
        void OnHotKey() override;
        bool OnMouseLeave() override;
        bool OnMouseEnter() override;
        bool OnMouseOver(int x, int y) override;
        void OnMousePressed(int x, int y, AppCUI::Input::MouseButton button) override;
        void Paint(Graphics::Renderer& renderer) override;
        void OnExpandView(AppCUI::Graphics::Clip& expandedClip) override;
        void OnPackView() override;
        virtual ~ComboBox();
    };

}; // namespace Controls
namespace Dialogs
{
    enum class Result : int
    {
        None   = 0,
        Ok     = 1,
        Cancel = 2,
        Yes    = 3,
        No     = 4,
    };
    class EXPORT MessageBox
    {
        MessageBox() = delete;

      public:
        static void ShowError(const AppCUI::Utils::ConstString& title, const AppCUI::Utils::ConstString& message);
        static void ShowNotification(
              const AppCUI::Utils::ConstString& title, const AppCUI::Utils::ConstString& message);
        static void ShowWarning(const AppCUI::Utils::ConstString& title, const AppCUI::Utils::ConstString& message);
        static Result ShowOkCancel(const AppCUI::Utils::ConstString& title, const AppCUI::Utils::ConstString& message);
        static Result ShowYesNoCancel(
              const AppCUI::Utils::ConstString& title, const AppCUI::Utils::ConstString& message);
    };
    class EXPORT FileDialog
    {
        FileDialog() = delete;

      public:
        static const char* ShowSaveFileWindow(
              const AppCUI::Utils::ConstString& fileName, std::string_view extensionFilter, const char* path);
        static const char* ShowOpenFileWindow(
              const AppCUI::Utils::ConstString& fileName, std::string_view extensionFilter, const char* path);
    };

} // namespace Dialogs
namespace Log
{
    enum class Severity : unsigned int
    {
        InternalError = 3,
        Error         = 2,
        Warning       = 1,
        Information   = 0,
    };
    struct Message
    {
        Severity Type;
        const char* Content;
        const char* FileName;
        const char* Function;
        const char* Condition;
        int LineNumber;
    };
    void EXPORT Report(
          Severity type,
          const char* fileName,
          const char* function,
          const char* condition,
          int line,
          const char* format,
          ...);
    void EXPORT SetLogCallback(void (*callback)(const Message&));
    bool EXPORT ToFile(const char* fileName);
    bool EXPORT ToOutputDebugString();
    bool EXPORT ToStdErr();
    bool EXPORT ToStdOut();
} // namespace Log
namespace Application
{
    enum class InitializationFlags : unsigned int
    {
        NONE = 0,

        // possible backends
        FRONTEND_DEFAULT  = 0,
        FRONTEND_SDL      = 1,
        FRONTEND_TERMINAL = 2,
        FRONTEND_WINDOWS  = 3,

        // character size
        CHAR_SIZE_DEFAULT = 0,
        CHAR_SIZE_TINY    = 0x00000100,
        CHAR_SIZE_SMALL   = 0x00000200,
        CHAR_SIZE_NORMAL  = 0x00000300,
        CHAR_SIZE_LARGE   = 0x00000400,
        CHAR_SIZE_HUGE    = 0x00000500,

        // generic options
        HAS_COMMANDBAR = 0x00010000,
        HAS_MENU       = 0x00020000,
        MAXIMIZED      = 0x00040000,
        FULLSCREEN     = 0x00080000,
        FIXED_SIZE     = 0x00100000,
    };

    class EXPORT CommandBar
    {
        void* Controller;

      public:
        CommandBar();
        void Init(void* controller);
        bool SetCommand(AppCUI::Input::Key keyCode, const char* Name, int CommandID);
    };

    struct Config
    {
        struct
        {
            int DesktopFillCharacterCode;
            Graphics::ColorPair Color;
        } Desktop;
        struct
        {
            Graphics::ColorPair BackgroundColor;
            Graphics::ColorPair ShiftKeysColor;
            struct
            {
                Graphics::ColorPair KeyColor;
                Graphics::ColorPair NameColor;
            } Normal, Hover, Pressed;
        } CommandBar;
        struct
        {
            Graphics::ColorPair ActiveColor;
            Graphics::ColorPair InactiveColor;
            Graphics::ColorPair TitleActiveColor;
            Graphics::ColorPair TitleInactiveColor;
            Graphics::ColorPair ControlButtonColor;
            Graphics::ColorPair ControlButtonInactiveColor;
            Graphics::ColorPair ControlButtonHoverColor;
            Graphics::ColorPair ControlButtonPressedColor;
        } Window, DialogError, DialogNotify, DialogWarning;
        struct
        {
            Graphics::ColorPair NormalColor;
            Graphics::ColorPair HotKeyColor;
        } Label;
        struct
        {
            struct
            {
                Graphics::ColorPair TextColor, HotKeyColor;
            } Normal, Focused, Inactive, Hover;
        } Button;
        struct
        {
            struct
            {
                Graphics::ColorPair TextColor, HotKeyColor, StateSymbolColor;
            } Normal, Focused, Inactive, Hover;
        } StateControl;
        struct
        {
            Graphics::ColorPair NormalColor, ClickColor, HoverColor;
        } Splitter;
        struct
        {
            Graphics::ColorPair NormalColor, TextColor;
        } Panel;
        struct
        {
            struct
            {
                Graphics::ColorPair Text, Border, LineNumbers, CurrentLineNumber;
            } Normal, Focus, Inactive, Hover;
            Graphics::ColorPair SelectionColor;
        } Text;
        struct
        {
            Graphics::ColorPair PageColor, TabBarColor, HoverColor, PageHotKeyColor, TabBarHotKeyColor, HoverHotKeyColor;
            Graphics::ColorPair ListSelectedPageColor, ListSelectedPageHotKey;
        } Tab;
        struct
        {
            struct
            {
                Graphics::ColorPair Border, Text, Hotkey;
            } Normal, Focused, Inactive, Hover;
            Graphics::ColorPair InactiveCanvasColor;
        } View;
        struct
        {
            Graphics::ColorPair Bar, Arrows, Position;
        } ScrollBar;
        struct
        {
            struct
            {
                Graphics::ColorPair Border, LineSparators;
            } Normal, Focused, Inactive, Hover;
            struct
            {
                Graphics::ColorPair Text, HotKey, Separator;
            } ColumnNormal, ColumnHover, ColumnInactive, ColumnSort;
            struct
            {
                Graphics::ColorPair Regular, Highligheted, Inactive, Error, Warning;
            } Item;
            Graphics::ColorPair CheckedSymbol, UncheckedSymbol;
            Graphics::ColorPair InactiveColor;
            Graphics::ColorPair FocusColor;
            Graphics::ColorPair SelectionColor;
            Graphics::ColorPair FocusAndSelectedColor;
            Graphics::ColorPair FilterText;
            Graphics::ColorPair StatusColor;
        } ListView;
        struct
        {
            Graphics::ColorPair Border, Title, TerminateMessage, Text, Time, Percentage;
            Graphics::ColorPair EmptyProgressBar, FullProgressBar;
        } ProgressStatus;
        struct
        {
            struct
            {
                Graphics::ColorPair Text, Button;
            } Focus, Normal, Inactive, Hover;
            Graphics::ColorPair Selection, HoverOveItem;
        } ComboBox;

        void SetDarkTheme();
    };

    EXPORT Config* GetAppConfig();

    NODISCARD("Check the return of the Init function. If false, AppCUI has not been initialized properly")
    EXPORT bool Init(Application::InitializationFlags flags = Application::InitializationFlags::NONE);

    NODISCARD("Check the return of the Init function. If false, AppCUI has not been initialized properly")
    EXPORT bool Init(
          unsigned int width,
          unsigned int height,
          Application::InitializationFlags flags = Application::InitializationFlags::NONE);

    NODISCARD("Check the return of the Init function. If false, AppCUI has not been initialized properly")
    EXPORT bool Init(const char* iniFile);

    EXPORT bool Run();
    EXPORT bool AddWindow(AppCUI::Controls::Window* wnd);
    EXPORT bool GetApplicationSize(AppCUI::Graphics::Size& size);
    EXPORT bool GetDesktopSize(AppCUI::Graphics::Size& size);
    EXPORT void Repaint();
    EXPORT void RecomputeControlsLayout();
    EXPORT void RaiseEvent(
          AppCUI::Controls::Control* control,
          AppCUI::Controls::Control* sourceControl,
          AppCUI::Controls::Event eventType,
          int controlID);
    EXPORT void Close();
}; // namespace Application
} // namespace AppCUI

// inline operations for enum classes
ADD_FLAG_OPERATORS(AppCUI::Application::InitializationFlags, unsigned int)
ADD_FLAG_OPERATORS(AppCUI::Input::Key, unsigned int);
ADD_FLAG_OPERATORS(AppCUI::Input::MouseButton, unsigned int);
ADD_FLAG_OPERATORS(AppCUI::Graphics::WriteCharacterBufferFlags, unsigned int)
ADD_FLAG_OPERATORS(AppCUI::Graphics::TextAlignament, unsigned int);
ADD_FLAG_OPERATORS(AppCUI::Controls::TextAreaFlags, unsigned int);
ADD_FLAG_OPERATORS(AppCUI::Controls::ListViewFlags, unsigned int);
ADD_FLAG_OPERATORS(AppCUI::Controls::TabFlags, unsigned int)
ADD_FLAG_OPERATORS(AppCUI::Controls::WindowFlags, unsigned int)
ADD_FLAG_OPERATORS(AppCUI::Controls::TextFieldFlags, unsigned int)
ADD_FLAG_OPERATORS(AppCUI::Utils::NumberParseFlags, unsigned int)

#undef ADD_FLAG_OPERATORS

#endif
