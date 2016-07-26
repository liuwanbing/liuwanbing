#include "TypeFont.h"

#define MAX_STRINGBUFFER 10240

FontSprite* FontSprite::CreateEx(const char* lpsTTFile, float nFontSize)
{
	TypeFont* pTTFont = new TypeFont;
	if (NULL == (pTTFont)) return NULL;
	if(pTTFont->Attach(lpsTTFile,nFontSize))
		return (FontSprite*)pTTFont;
	DELETE_OBJECT(pTTFont);
	return NULL;
}

////////////////////////////////////////////////////////////////////////////
// TTF字体

TypeFont::TypeFont()
{
	m_pHGE = hgeCreate(HGE_VERSION);

	m_FTLibrary = NULL;
	m_FTFace = NULL;
	m_pFontBuffer = NULL;

	m_nKerningWidth = 0;
	m_nKerningHeight = 0;
	m_nFontSize = 0;

	m_pSprite = new hgeSprite( 0, 0, 0, 0, 0 );
	m_pSprite->SetColor( ARGB( 255, 255, 255, 255 ) );
}

TypeFont::~TypeFont()
{
	DELETE_OBJECTARRAY(m_pFontBuffer);

	if (m_FTFace)
	{
		FT_Done_Face(m_FTFace);
		m_FTFace = NULL;
	}
	if (m_FTLibrary)
	{
		FT_Done_FreeType(m_FTLibrary);
		m_FTLibrary = NULL;
	}

	size_t size = m_Glyphs.size();
	for (size_t i = 0;i < size;i++) m_Glyphs[i].Release(m_pHGE);
	m_Glyphs.clear();

	DELETE_OBJECT(m_pSprite);
	RELEASE_OBJECT(m_pHGE);
}

void TypeFont::Release()
{
	DELETE_SELF(this);
}

bool TypeFont::Attach(const char* lpsTTFile, float nFontSize)
{
	if ((m_FTFace))
		return false;

	if ((m_FTLibrary))
		return false;

	if (FT_Init_FreeType( &m_FTLibrary ))
		return false;

	DWORD nFileSize = 0;
	m_pHGE->System_SetState(HGE_RESENCRYPT, false);
	void* lpdata = m_pHGE->Resource_Load(lpsTTFile,&nFileSize);
	m_pHGE->System_SetState(HGE_RESENCRYPT, true);
	if (lpdata)
	{
		if (nFileSize > 0)
		{
			m_pFontBuffer = new unsigned char[nFileSize];
			if (NULL == (m_pFontBuffer))
				return false;

			memcpy(m_pFontBuffer,lpdata,nFileSize);

			if (FT_New_Memory_Face( m_FTLibrary,m_pFontBuffer,nFileSize,0,&m_FTFace ))
				return false;

			m_pHGE->Resource_Free(lpdata);
		}
		else
			return false;
	}
	else
	{
		if (FT_New_Face( m_FTLibrary,lpsTTFile,0,&m_FTFace ))
			return false;
	}

	m_Glyphs.resize( m_FTFace->num_glyphs );
	size_t size = m_Glyphs.size();
	for (size_t i = 0;i < size;i++) m_Glyphs[i].Reset();

	m_nFontSize = nFontSize;

	return	true;
}

unsigned int TypeFont::GetGlyphByCharacter(wchar_t c)
{
	unsigned int idx = FT_Get_Char_Index( m_FTFace, c );

	if (idx && NULL == (m_Glyphs[idx - 1].hTexture)) CacheCharacter(idx);

	return	idx;
}

SIZE TypeFont::GetTextSize(const wchar_t* text)
{
	SIZE dim = {0, static_cast<LONG>(m_nFontSize)};
	float nRowWidth = 0;

	while(*text)
	{
		if (*text == L'\n' || *text == L'\r')
		{
			dim.cy += static_cast<LONG>(m_nFontSize + m_nKerningHeight);
			if (dim.cx < static_cast<LONG>(nRowWidth))
				dim.cx = static_cast<LONG>(nRowWidth);
			nRowWidth = 0;
		}
		else
			nRowWidth += (GetWidthFromCharacter(*text) + m_nKerningWidth);
		++text;
	}

	if (dim.cx < static_cast<LONG>(nRowWidth))
		dim.cx = static_cast<LONG>(nRowWidth);

	return dim;
}

inline float TypeFont::GetWidthFromCharacter(wchar_t c)
{
	unsigned int n = GetGlyphByCharacter(c);
	if ( n > 0)
	{
		float w = m_Glyphs[n - 1].w;
		float x = m_Glyphs[n - 1].x;
		if (w + x > 0) return w + x;
	}
	if (c >= 0x2000)
	{
		return	m_nFontSize;
	}
	else
	{
		return	_floor(m_nFontSize / 2);
	}
}

inline void TypeFont::CacheCharacter(unsigned int idx)
{
	FT_Set_Pixel_Sizes(m_FTFace,0,static_cast<unsigned int>(m_nFontSize)); // 指定像素大小

	// 读取字体轮廓
	if (!FT_Load_Glyph(m_FTFace,idx,FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP))
	{
		FT_GlyphSlot glyph = m_FTFace->glyph;
		if (glyph->format == ft_glyph_format_outline)
		{
			if (!FT_Render_Glyph( glyph, FT_RENDER_MODE_NORMAL))
			{
				TTFONTGLYPH& fontgl = m_Glyphs[idx - 1];

				FT_Bitmap bits = glyph->bitmap;
				fontgl.x = static_cast<float>(glyph->bitmap_left);
				fontgl.y = static_cast<float>(glyph->bitmap_top);

				unsigned char *ttf_pixels = bits.buffer;
				fontgl.w = static_cast<float>(bits.width);
				fontgl.h = static_cast<float>(bits.rows);

				if ((fontgl.hTexture)) m_pHGE->Texture_Free( fontgl.hTexture );

				fontgl.hTexture = m_pHGE->Texture_Create( bits.width, bits.rows );
				int tex_w = m_pHGE->Texture_GetWidth(fontgl.hTexture);

				DWORD* tex_pixels = m_pHGE->Texture_Lock( fontgl.hTexture, false );
				unsigned int* texp = (unsigned int*)tex_pixels;

				bool bFlag = false;//DIRECT3D8;
				for (int y = 0;y < bits.rows;y++)
				{
					unsigned int *rowp = texp;
					for (int x = 0;x < bits.width;x++)
					{
						if (*ttf_pixels)
						{
							if (bFlag)
							{
								*rowp = *ttf_pixels;
								*rowp *= 0x01010101;
							}
							else
							{
								*rowp = *ttf_pixels << 24;
								*rowp |= 0xffffff;
							}
						}
						else
						{
							*rowp = 0;
						}
						ttf_pixels++;
						rowp++;
					}
					texp += tex_w;
				}
				m_pHGE->Texture_Unlock( fontgl.hTexture );
			}
		}
	}
}

DWORD TypeFont::GetColor( int i )
{
	return m_pSprite->GetColor(i);
}

void TypeFont::SetColor( DWORD dwColor, int i )
{
	m_pSprite->SetColor(dwColor,i);
}

void TypeFont::Printf( float x, float y, const wchar_t *format, ... )
{
	va_list l;
	va_start( l, format );

	wchar_t sText[MAX_STRINGBUFFER];
	//wvsprintfW( sText, format, l );
  vswprintf(sText, MAX_STRINGBUFFER - 1, format, l);

	Render( x, y, sText );
}

void TypeFont::Render( float x, float y, const wchar_t* text )
{
	float offsetX = x;
	float offsetY = y;

	unsigned int n;

	while(*text)
	{
		if (*text == L'\n' || *text == L'\r')
		{
			offsetX = x;
			offsetY += (m_nFontSize + m_nKerningHeight);
		}
		else
		{
			n = GetGlyphByCharacter(*text);
			if ( n > 0)
			{
				m_pSprite->SetTexture( m_Glyphs[n-1].hTexture );
				m_pSprite->SetTextureRect( 0, 0, m_Glyphs[n-1].w, m_Glyphs[n-1].h );
				m_pSprite->Render( offsetX + m_Glyphs[n-1].x, offsetY + m_nFontSize - m_Glyphs[n-1].y);

				offsetX += (GetWidthFromCharacter(*text) + m_nKerningWidth);
			} 
			else 
			{
				offsetX += (GetWidthFromCharacter(*text) + m_nKerningWidth);
			}
		}

		++text;
	}
}

void TypeFont::RenderEx( float x, float y, const wchar_t* text, float scale)
{
	float offsetX = x;
	float offsetY = y;

	unsigned int n;

	while(*text)
	{
		if (*text == L'\n' || *text == L'\r')
		{
			offsetX = x;
			offsetY += _floor((m_nFontSize + m_nKerningHeight) * scale);
		}
		else
		{
			n = GetGlyphByCharacter(*text);
			if ( n > 0)
			{
				m_pSprite->SetTexture( m_Glyphs[n-1].hTexture );
				m_pSprite->SetTextureRect( 0, 0, m_Glyphs[n-1].w, m_Glyphs[n-1].h );
				m_pSprite->RenderEx( offsetX + m_Glyphs[n-1].x, offsetY + m_nFontSize - m_Glyphs[n-1].y ,0.0f, scale );

				offsetX += _floor((GetWidthFromCharacter(*text) + m_nKerningWidth) * scale);
			} 
			else 
			{
				offsetX += _floor((GetWidthFromCharacter(*text) + m_nKerningWidth) * scale);
			}
		}

		++text;
	}
}

//! Calculates the index of the character in the text which is on a specific position.
wchar_t TypeFont::GetCharacterFromPos( const wchar_t* text, float pixel_x, float pixel_y )
{
	float x = 0;
	float y = 0;

	while (*text)
	{
		if (*text == L'\n' || *text == L'\r')
		{
			x = 0;
			y += (m_nFontSize+m_nKerningHeight);
			text++;

			if (!(*text))
				break;
		}

		float w = GetWidthFromCharacter(*text);
		if (pixel_x > x && pixel_x <= x + w + 1.0 &&
			pixel_y > y && pixel_y <= y + m_nFontSize + 1.0)
			return *text;

		x += (w+m_nKerningWidth);

		text++;
	}

	return L'\0';
}

void TypeFont::SetKerningWidth ( float kerning )
{
	m_nKerningWidth = kerning;
}

float TypeFont::GetKerningWidth()
{
	return m_nKerningWidth;
}

void TypeFont::SetKerningHeight ( float kerning )
{
	m_nKerningHeight = kerning;
}

float TypeFont::GetKerningHeight ()
{
	return m_nKerningHeight;
}

float TypeFont::GetFontSize()
{
	return m_nFontSize;
}
