#include "AppCUI.hpp"
#include "Internal.hpp"
#include <string.h>

using namespace AppCUI::Graphics;
using namespace AppCUI::Utils;



#define VALIDATE_ALLOCATED_SPACE(requiredSpace, returnValue)                                                           \
    if ((requiredSpace) > (Allocated & 0x7FFFFFFF))                                                                    \
    {                                                                                                                  \
        CHECK(Grow(requiredSpace), returnValue, "Fail to allocate space for %d bytes", (requiredSpace));               \
    }

#define COMPUTE_TEXT_SIZE(text, textSize)                                                                              \
    if (textSize == 0xFFFFFFFF)                                                                                        \
    {                                                                                                                  \
        textSize = AppCUI::Utils::String::Len(text);                                                                   \
    }



template<typename T>
void CopyStringToCharBuffer(Character* dest, const T* source, size_t sourceCharactersCount, ColorPair col)
{
    // it is iassume that dest, source and sourceCharactersCount are valid values
    // all new-line formats will be converted into a single separator 
    const T* end = source + sourceCharactersCount;
    while (source < end)
    {
        dest->Color = col;
        if ((*source) == '\r')
        {
            dest->Code = NEW_LINE_CODE;
            dest++;
            source++;
            if ((source < end) && ((*source) == '\n'))
                source++;
            continue;
        }
        if ((*source) == '\n')
        {
            dest->Code = NEW_LINE_CODE;
            dest++;
            source++;
            if ((source < end) && ((*source) == '\r'))
                source++;
            continue;
        }
        // no new_line --> direct copy
        dest->Code = (char16_t) (*source);
        dest++;
        source++;
    }
}

constexpr bool IgnoreCaseEquals(char16_t code1, char16_t code2)
{
    // GDT: temporary implementation ==> should be addapted for UNICODE characters as well
    if ((code1 >= 'A') && (code1 <= 'Z'))
        code1 |= 0x20;
    if ((code2 >= 'A') && (code2 <= 'Z'))
        code2 |= 0x20;
    return code1 == code2;
}

CharacterBuffer::CharacterBuffer()
{
    //LOG_INFO("Default ctor for %p", this);
    Allocated = 0;
    Count     = 0;
    Buffer    = nullptr;
}

void CharacterBuffer::Swap(CharacterBuffer& obj) noexcept
{
    //LOG_INFO("Swap %p with %p", this, &obj);
    std::swap(this->Buffer, obj.Buffer);
    std::swap(this->Allocated, obj.Allocated);
    std::swap(this->Count, obj.Count);
}


CharacterBuffer::~CharacterBuffer(void)
{
    //LOG_INFO("DTOR for %p [Buffer=%p,Count=%d,Allocated=%d]", this, Buffer, (int) Count, (int) Allocated);
    Destroy();
}



void CharacterBuffer::Destroy()
{
    //LOG_INFO("Destroy Character Buffer from (%p) with [Buffer = %p, Count = %d, Allocated = %d]",this, Buffer, (int)Count, (int)Allocated);
    if (Buffer)
        delete []Buffer;
    Buffer = nullptr;
    Count = Allocated = 0;
}
bool CharacterBuffer::Grow(size_t newSize)
{
    newSize = ((newSize | 15) + 1) & 0x7FFFFFFF;
    if (newSize <= (Allocated & 0x7FFFFFFF))
        return true;
    Character* temp = new Character[newSize];
    CHECK(temp, false, "Failed to allocate: %d characters", newSize);
    if (Buffer)
    {
        memcpy(temp, Buffer, sizeof(Character) * this->Count);
        delete []Buffer;
    }
    Buffer    = temp;
    Allocated = newSize;
    return true;
}

bool CharacterBuffer::Set(const CharacterBuffer& obj)
{
    this->Count = 0; // we overwrite the buffer anyway - don't need to recopy it if the allocated space is too small
    CHECK(Grow(obj.Count), false, "Fail to allocate %d bytes ", (int) obj.Count);
    if (obj.Count>0)
    {
        memcpy(this->Buffer, obj.Buffer, sizeof(Character) * obj.Count);   
    }
    this->Count = obj.Count;
    return true;
}
bool CharacterBuffer::SetWithHotKey(const std::string_view text, unsigned int& hotKeyCharacterPosition, const ColorPair color)
{
    hotKeyCharacterPosition = 0xFFFFFFFF;
    CHECK(text.data(), false, "Expecting a valid (non-null) text");
    VALIDATE_ALLOCATED_SPACE(text.size() + this->Count, false);
    Character* ch              = this->Buffer;
    const unsigned char* p     = (const unsigned char*) text.data();
    auto textSize              = text.size();

    while (textSize > 0)
    {
        if ((hotKeyCharacterPosition == 0xFFFFFFFF) && ((*p) == '&') && (textSize > 1 /* not the last character*/))
        {
            char tmp = p[1] | 0x20;
            if (((tmp >= 'a') && (tmp <= 'z')) || ((tmp >= '0') && (tmp <= '9')))
            {
                hotKeyCharacterPosition = (unsigned int) (p - (const unsigned char*) text.data());
                p++;
                textSize--;
                continue;
            }
        }
        ch->Color = color;
        ch->Code  = *p;
        ch++;
        p++;
        textSize--;
    }
    this->Count = (unsigned int) (ch - this->Buffer);
    return true;
}
bool CharacterBuffer::SetWithHotKey(const std::u8string_view text, unsigned int& hotKeyCharacterPosition, const ColorPair color)
{
    hotKeyCharacterPosition = 0xFFFFFFFF;
    CHECK(text.data(), false, "Expecting a valid (non-null) text");
    VALIDATE_ALLOCATED_SPACE(text.size() + this->Count, false);
    Character* ch              = this->Buffer;
    const char8_t* p           = text.data();
    const char8_t* p_end       = p + text.size();
    auto textSize              = text.size();
    UnicodeChar uc;


    while (textSize > 0)
    {
        if ((hotKeyCharacterPosition == 0xFFFFFFFF) && ((*p) == '&') && (textSize > 1 /* not the last character*/))
        {
            char8_t tmp = p[1] | 0x20;
            if (((tmp >= 'a') && (tmp <= 'z')) || ((tmp >= '0') && (tmp <= '9')))
            {
                hotKeyCharacterPosition = (unsigned int) (ch - this->Buffer);
                p++;
                textSize--;
                continue;
            }
        }
        ch->Color = color;
        if ((*p) < 0x80)
        {
            ch->Code = *p;
            p++;
            ch++;
            textSize--;
        }
        else
        {
            // unicode encoding
            CHECK(ConvertUTF8CharToUnicodeChar(p, p_end, uc), false, "Fail to convert to unicode !");
            ch->Code = uc.Value;
            textSize -= uc.Length;
            p += uc.Length;
            ch++;
        }
    }
    this->Count = (unsigned int) (ch - this->Buffer);
    
    return true;
}

int  CharacterBuffer::FindAscii(const std::string_view & text, bool ignoreCase) const
{
    const unsigned char* p     = (const unsigned char*) text.data();
    const unsigned char* p_end = p + text.size();
    Character* ch              = this->Buffer;
    Character* ch_end          = this->Buffer + this->Count;
    CHECK(p, -1, "Expecting a valid (non_null) text !");
    CHECK(ch, -1, "Invalid buffer (not set)");
    if (p == p_end)
        return 0; // am empty string is always found at the first position
    if (text.size() > this->Count)
        return -1;
    Character* ch_max_search = ch_end - text.size();
    // the actual search    
    if (ignoreCase)
    {
        while (ch <= ch_max_search)
        {
            const unsigned char* s = p;
            Character* c           = ch;
            while ((s < p_end) && (c < ch_end) && (IgnoreCaseEquals(*s,c->Code)))
            {
                c++;
                s++;
            }
            if (s >= p_end)
                return (int)(ch - this->Buffer);
            ch++;
        }
    }
    else
    {
        while (ch <= ch_max_search)
        {
            const unsigned char* s = p;
            Character* c           = ch;
            while ((s < p_end) && (c < ch_end) && ((*s) == c->Code))
            {
                c++;
                s++;
            }
            if (s >= p_end)
                return (int)(ch - this->Buffer);
            ch++;
        }
    }
    return -1;
}
int  CharacterBuffer::FindUTF8(const std::u8string_view& text, bool ignoreCase) const
{
    const char8_t* p           = (const char8_t*) text.data();
    const char8_t* p_end       = p + text.size();
    Character* ch              = this->Buffer;
    Character* ch_end          = this->Buffer + this->Count;
    CHECK(p, -1, "Expecting a valid (non_null) text !");
    CHECK(ch, -1, "Invalid buffer (not set)");
    if (p == p_end)
        return 0; // am empty string is always found at the first position
    if (text.size() > this->Count)
        return -1;
    UnicodeChar uc;
    // the actual search
    while (ch < ch_end)
    {
        const char8_t* s = p;
        Character* c     = ch;
        while ((s < p_end) && (c < ch_end))
        {
            if ((*s) < 0x80)
            {
                uc.Value = *s;
                uc.Length = 1;
            }
            else
            {
                if (!ConvertUTF8CharToUnicodeChar(s, p_end, uc))
                    break;
            }
            if (ignoreCase)
            {
                if (!IgnoreCaseEquals(uc.Value, c->Code))
                    break;
            }
            else
            {
                if (uc.Value != c->Code)
                    break;
            }
            c++;
            s += uc.Length;
        }
        if (s >= p_end)
            return (int) (ch - this->Buffer);
        ch++;
    }
    return -1;
}
bool CharacterBuffer::Add(const AppCUI::Utils::ConstString& text, const ColorPair color)
{
    AppCUI::Utils::ConstStringObject textObj(text);
    LocalUnicodeStringBuilder<1024> ub;
    
    CHECK(textObj.Data, false, "Expecting a valid (non-null) string");
    if (textObj.Length == 0)
        return true; // nothing to do
    CHECK(Grow(this->Count + textObj.Length),
          false,
          "Fail to allocate space for the character buffer: %z",
          this->Count + textObj.Length);
    switch (textObj.Type)
    {
    case StringViewType::Ascii:
        CopyStringToCharBuffer<char>(this->Buffer + this->Count, (const char*) textObj.Data, textObj.Length, color);
        this->Count += textObj.Length;
        break;
    case StringViewType::CharacterBuffer:
        CopyStringToCharBuffer<Character>(this->Buffer + this->Count, (const Character*) textObj.Data, textObj.Length, color);
        this->Count += textObj.Length;
        break;
    case StringViewType::Unicode16:
        CopyStringToCharBuffer<char16_t>(this->Buffer + this->Count, (const char16_t*) textObj.Data, textObj.Length, color);
        this->Count += textObj.Length;
        break;
    case StringViewType::UTF8:
        CHECK(ub.Set(text), false, "Fail to convert UTF-8 to current internal format !");
        CopyStringToCharBuffer<char16_t>(this->Buffer + this->Count, ub.GetString(), ub.Len(), color);
        this->Count += ub.Len();
        break;
    default:
        RETURNERROR(false, "Unknwon string view type: %d", textObj.Type);
    }
    return true;
}
bool CharacterBuffer::Set(const AppCUI::Utils::ConstString& text, const ColorPair color)
{
    this->Count = 0;
    return Add(text, color);
}
bool CharacterBuffer::SetWithHotKey(const AppCUI::Utils::ConstString& text, unsigned int& hotKeyCharacterPosition,const ColorPair color)
{
    if (std::holds_alternative<std::u8string_view>(text))
    {
        return SetWithHotKey(std::get<std::u8string_view>(text), hotKeyCharacterPosition, color);
    }
    else
    {
        return SetWithHotKey(std::get<std::string_view>(text), hotKeyCharacterPosition, color);
    }
}

void CharacterBuffer::Clear()
{
    this->Count = 0;
}
bool CharacterBuffer::Delete(unsigned int start, unsigned int end)
{
    CHECK(end <= this->Count, false, "Invalid delete offset: %d (should be between 0 and %d)", end, this->Count);
    CHECK(start < end, false, "Start parameter (%d) should be smaller than End parameter (%d)", start, end);
    if (end == this->Count)
    {
        this->Count = start;
    }
    else
    {
        memmove(this->Buffer + start, this->Buffer + end, (this->Count - end) * sizeof(Character));
        this->Count -= (end - start);
    }
    return true;
}
bool CharacterBuffer::DeleteChar(unsigned int position)
{
    CHECK(position < this->Count,
          false,
          "Invalid delete offset: %d (should be between 0 and %d)",
          position,
          this->Count - 1);
    if ((position + 1) < this->Count)
    {
        memmove(
              this->Buffer + position, this->Buffer + position + 1, (this->Count - (position + 1)) * sizeof(Character));
    }
    this->Count--;
    return true;
}
bool CharacterBuffer::Insert(const char* text, unsigned int position, const ColorPair color, unsigned int textSize)
{
    CHECK(text, false, "Expecting a valid (non-null) text");
    CHECK(position <= this->Count,
          false,
          "Invalid insert offset: %d (should be between 0 and %d)",
          position,
          this->Count);
    COMPUTE_TEXT_SIZE(text, textSize);
    if (textSize == 0)
        return true; // nothing to insert
    VALIDATE_ALLOCATED_SPACE(textSize + this->Count, false);
    if (position < this->Count)
    {
        memmove(
              this->Buffer + position + textSize,
              this->Buffer + position,
              (this->Count - position) * sizeof(Character));
    }
    auto c = this->Buffer + position;
    this->Count += textSize;
    while (textSize > 0)
    {
        c->Code  = *text;
        c->Color = color;
        text++;
        c++;
        textSize--;
    }
    return true;
}
bool CharacterBuffer::Insert(const AppCUI::Utils::String& text, unsigned int position, const ColorPair color)
{
    return Insert(text.GetText(), position, color, text.Len());
}
bool CharacterBuffer::InsertChar(unsigned short characterCode, unsigned int position, const ColorPair color)
{
    VALIDATE_ALLOCATED_SPACE(this->Count + 1, false);
    CHECK(position <= this->Count,
          false,
          "Invalid insert offset: %d (should be between 0 and %d)",
          position,
          this->Count);
    if (position < this->Count)
    {
        memmove(this->Buffer + position + 1, this->Buffer + position, (this->Count - position) * sizeof(Character));
    }
    auto c   = this->Buffer + position;
    c->Code  = characterCode;
    c->Color = color;
    this->Count++;
    return true;
}
bool CharacterBuffer::SetColor(unsigned int start, unsigned int end, const ColorPair color)
{
    if (end > this->Count)
        end = this->Count;
    CHECK(start <= end, false, "Expecting a valid parameter for start (%d) --> should be smaller than %d", start, end);
    Character* ch   = this->Buffer + start;
    unsigned int sz = end - start;
    while (sz)
    {
        ch->Color = color;
        sz--;
        ch++;
    }
    return true;
}
void CharacterBuffer::SetColor(const ColorPair color)
{
    Character* ch = this->Buffer;
    size_t sz     = this->Count;
    while (sz)
    {
        ch->Color = color;
        sz--;
        ch++;
    }
}
bool CharacterBuffer::CopyString(AppCUI::Utils::String& text, unsigned int start, unsigned int end)
{
    CHECK(start < end, false, "Start position (%d) should be smaller than the end position (%d)", start, end);
    CHECK(end <= Count,
          false,
          "Invalid end position (%d), should be smaller or equal to maximum characters in the buffer (%d)",
          end,
          Count);
    CHECK(text.Realloc(end - start), false, "Fail to allocate %d character to be copied", end - start);

    Character* s = this->Buffer + start;
    Character* e = s + (end - start);
    text.Clear();
    while (s < e)
    {
        CHECK(text.AddChar((char) s->Code), false, "Fail to copy character with code: %d to string", s->Code);
        s++;
    }
    return true;
}
bool CharacterBuffer::CopyString(AppCUI::Utils::String& text)
{
    return CopyString(text, 0, this->Count);
}
int  CharacterBuffer::Find(const AppCUI::Utils::ConstString& text, bool ignoreCase) const
{
    if (std::holds_alternative<std::u8string_view>(text))
    {
        return FindUTF8(std::get<std::u8string_view>(text), ignoreCase);
    }
    else
    {
        return FindAscii(std::get<std::string_view>(text), ignoreCase);
    }
}
int CharacterBuffer::CompareWith(const CharacterBuffer& obj, bool ignoreCase) const
{
    Character* s = this->Buffer;
    Character* s_end = s + this->Count;
    Character* d     = obj.Buffer;
    Character* d_end = d + obj.Count;

    // null check
    if ((s) && (!d))
        return 1;
    if ((!s) && (d))
        return -1;
    if ((!s) && (!d))
        return 0;


    if (!ignoreCase)
    {
        while ((s<s_end) && (d<d_end))    
        {
            if (s->Code < d->Code)
                return -1;
            if (s->Code > d->Code)
                return 1;
            s++;
            d++;
        }
    }
    else
    {
        char16_t s_c, d_c;
        while ((s < s_end) && (d < d_end))
        {
            s_c = s->Code;
            d_c = d->Code;
            if ((s_c >= 'A') && (s_c <= 'Z'))
                s_c |= 0x20;
            if ((d_c >= 'A') && (d_c <= 'Z'))
                d_c |= 0x20;
            if (s_c < d_c)
                return -1;
            if (s_c > d_c)
                return 1;
            s++;
            d++;
        }  
    }
    if ((s == s_end) && (d < d_end))
        return -1;
    if ((s < s_end) && (d == d_end))
        return 1;
    return 0;
}


bool CharacterBuffer::ToString(std::string& output) const
{
    CHECK(this->Buffer, false, "");
    const Character* p = this->Buffer;
    const Character* e = p + this->Count;
    output.reserve(this->Count);
    output = "";
    while (p<e)
    {
        if (p->Code<256)
            output.append(1, (char) p->Code);
        else
            output.append(1, '?');
        p++;
    }
    return true;
}
bool CharacterBuffer::ToString(std::u16string& output) const
{
    CHECK(this->Buffer, false, "");
    const Character* p = this->Buffer;
    const Character* e = p + this->Count;
    output.reserve(this->Count);
    char16_t tmp[1] = { 0 };
    output          = tmp;
    while (p < e)
    {
        output.append(1, p->Code);
        p++;
    }
    return true;
}
bool CharacterBuffer::ToPath(std::filesystem::path& output) const
{
    CHECK(this->Buffer, false, "");
    const Character* p = this->Buffer;
    const Character* e = p + this->Count;
    output             = "";
    while (p < e)
    {
        output += p->Code;
        p++;
    }
    return true;
}