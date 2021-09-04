#include "AppCUI.hpp"

using namespace AppCUI::Graphics;

#define COMPUTE_RGB(r, g, b)                                                                                           \
    ((((unsigned int) ((r) &0xFF)) << 16) | (((unsigned int) ((g) &0xFF)) << 8) | ((unsigned int) ((b) &0xFF)))
#define COMPUTE_ARGB(a, r, g, b)                                                                                       \
    ((((unsigned int) ((a) &0xFF)) << 24) | (((unsigned int) ((r) &0xFF)) << 16) |                                     \
     (((unsigned int) ((g) &0xFF)) << 8) | ((unsigned int) ((b) &0xFF)))

static const unsigned int _console_colors_[16] = {
    COMPUTE_RGB(0, 0, 0),       // Black
    COMPUTE_RGB(0, 0, 128),     // DarkBlue
    COMPUTE_RGB(0, 128, 0),     // DarkGreen
    COMPUTE_RGB(0, 128, 128),   // Teal
    COMPUTE_RGB(128, 0, 0),     // DarkRed
    COMPUTE_RGB(128, 0, 128),   // Purple
    COMPUTE_RGB(128, 128, 0),   // Brown
    COMPUTE_RGB(192, 192, 192), // LightGray
    COMPUTE_RGB(128, 128, 128), // DarkGray
    COMPUTE_RGB(0, 0, 255),     // Blue
    COMPUTE_RGB(0, 255, 0),     // Green
    COMPUTE_RGB(0, 255, 255),   // Aqua
    COMPUTE_RGB(255, 0, 0),     // Red
    COMPUTE_RGB(255, 0, 255),   // Pink
    COMPUTE_RGB(255, 255, 0),   // Yellow
    COMPUTE_RGB(255, 255, 255), // White
};

#define CHECK_INDEX(errorValue)                                                                                        \
    unsigned int* pixel;                                                                                               \
    CHECK(Pixels != nullptr, errorValue, "Image was not instantiated yet (have you called Create methods ?)");         \
    CHECK(x < Width,                                                                                                   \
          errorValue,                                                                                                  \
          "Invalid X (%d) value. It should be less than %d (the width of the image)",                                  \
          x,                                                                                                           \
          Width);                                                                                                      \
    CHECK(y < Height,                                                                                                  \
          errorValue,                                                                                                  \
          "Invalid Y (%d) value. It should be less than %d (the height of the image)",                                 \
          y,                                                                                                           \
          Height);                                                                                                     \
    pixel = &Pixels[y * Width + x];

#define VALIDATE_CONSOLE_INDEX                                                                                         \
    CHECK(((unsigned int) color) < 16, false, "Invalid console color index (should be between 0 and 15)");

Image::Image()
{
    Width = Height = 0;
    Pixels         = nullptr;
}
Image::~Image()
{
    if (Pixels)
        delete Pixels;
    Width = Height = 0;
    Pixels         = nullptr;
}
bool Image::Create(unsigned int width, unsigned int height)
{
    CHECK(width > 0, false, "Invalid 'width' parameter (should be bigger than 0)");
    CHECK(height > 0, false, "Invalid 'height' parameter (should be bigger than 0)");
    if (Pixels)
    {
        delete Pixels;
        Pixels = nullptr;
        Width = Height = 0;
    }
    Pixels = new unsigned int[width * height];
    CHECK(Pixels != nullptr, false, "Unable to alocate space for an %d x %d image", width, height);
    unsigned int* s = Pixels;
    unsigned int* e = s + width * height;
    while (s < e)
    {
        (*s) = 0;
        s++;
    }
    Width  = width;
    Height = height;
    return true;
}
bool Image::SetPixel(unsigned int x, unsigned int y, unsigned int color)
{
    CHECK_INDEX(false);
    *pixel = color;
    return true;
}

bool Image::SetPixel(unsigned int x, unsigned int y, const Color color)
{
    CHECK_INDEX(false);
    if (((unsigned int) color) < 16)
        *pixel = _console_colors_[(unsigned int) color];
    return true;
}

bool Image::Clear(unsigned int color)
{
    CHECK(Pixels != nullptr, false, "Image was not instantiated yet (have you called Create methods ?)");
    unsigned int* s = Pixels;
    unsigned int* e = s + (size_t)Width * (size_t)Height;
    while (s < e)
    {
        (*s) = color;
        s++;
    }
    return true;
}
bool Image::Clear(const Color color)
{
    VALIDATE_CONSOLE_INDEX;
    return Clear(_console_colors_[(unsigned int) color]);
}

bool Image::SetPixel(
      unsigned int x, unsigned int y, unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha)
{
    CHECK_INDEX(false);
    *pixel = COMPUTE_ARGB(Alpha, Red, Green, Blue);
    return true;
}
unsigned int Image::GetPixel(unsigned int x, unsigned int y, unsigned int invalidIndexValue) const
{
    CHECK_INDEX(invalidIndexValue);
    return *pixel;
}
bool Image::GetPixel(unsigned int x, unsigned int y, unsigned int& color) const
{
    CHECK_INDEX(false);
    color = (*pixel);
    return true;
}
unsigned int Image::ComputeSquareAverageColor(unsigned int x, unsigned int y, unsigned int sz)
{
    if ((x >= this->Width) || (y >= this->Height) || (sz==0))
        return 0;  // nothing to compute
    unsigned int e_x = x + sz;
    unsigned int e_y = y + sz;
    if (e_x >= this->Width)
        e_x = this->Width;
    if (e_y >= this->Height)
        e_y = this->Height;
    auto xSize         = e_x - x;
    auto ySize         = e_y - y;
    auto sPtr          = Pixels + (size_t) Width * (size_t) y + (size_t) x;
    unsigned int sum_r = 0;
    unsigned int sum_g = 0;
    unsigned int sum_b = 0;
    if ((xSize == 0) || (ySize == 0))
        return 0; // nothing to compute (sanity check)

    while (y<e_y)
    {
        unsigned int* p = sPtr;
        unsigned int* e = p + (size_t)xSize;
        while (p<e)
        {
            sum_r += ((*p) >> 16) & 0xFF;
            sum_g += ((*p) >> 8) & 0xFF;
            sum_b += (*p) & 0xFF;
            p++;
        }
        sPtr += Width; // move to next line
        y++;
    }
    const unsigned int sz = xSize * ySize;
    const auto result_r   = sum_r / sz;
    const auto result_g   = sum_g / sz;
    const auto result_b   = sum_b / sz;
    return COMPUTE_RGB(result_r, result_g, result_b);
}
#undef COMPUTE_RGB
#undef COMPUTE_ARGB