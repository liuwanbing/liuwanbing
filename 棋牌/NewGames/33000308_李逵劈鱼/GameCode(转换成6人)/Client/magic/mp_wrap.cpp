//-----------------------------------------------------------------------------
// File: mp_wrap.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_wrap.h"


// eng: Class storing the textural atlas
// rus: Клас? которы?хранит текстурный атла?
MP_Atlas_WRAP::MP_Atlas_WRAP(int width, int height, const char* file) : MP_Atlas(width, height, file)
{
	if (file_name.empty())
	{
		HGE* hge=device->GetHGE();
		texture=hge->Texture_Create(width,height);
	}
	else
	{
		texture=device->LoadTextureFromFile(file_name.c_str());
	}

	quad.tex=texture;
	quad.v[0].z=0.f;
	quad.v[1].z=0.f;
	quad.v[2].z=0.f;
	quad.v[3].z=0.f;
}

// eng: Destroy atlas texture
// rus: Уничтожить текстуру атласа
void MP_Atlas_WRAP::Destroy()
{
	if (texture)
	{
		HGE* hge=device->GetHGE();
		hge->Texture_Free(texture);
		texture=NULL;
	}
}

// eng: Loading of frame texture
// rus: Загрузка текстурног?кадр?
void MP_Atlas_WRAP::LoadTexture(const MAGIC_CHANGE_ATLAS* c)
{
	HGE* hge=device->GetHGE();

	HTEXTURE texture_from;
	if (c->data)
		texture_from=hge->Texture_Load(c->data,c->length,false);
	else
	{
		texture_from=device->LoadTextureFromFile(c->file);
	}

	int texture_from_width=hge->Texture_GetWidth(texture_from,true);
	int texture_from_height=hge->Texture_GetHeight(texture_from,true);
	hgeSprite* spr_from=new hgeSprite(texture_from,0.f,0.f,(float)texture_from_width,(float)texture_from_height);

	int frame_width=c->width;
	int frame_height=c->height;

	float frame_from_width=spr_from->GetWidth();
	float frame_from_height=spr_from->GetHeight();

	float scale_x=((float)frame_width)/frame_from_width;
	float scale_y=((float)frame_height)/frame_from_height;

	int x=c->x;
	int y=c->y;

	int pitch_to=hge->Texture_GetWidth(texture);
	int pitch_from=hge->Texture_GetWidth(texture_from);

	unsigned long* to=hge->Texture_Lock(texture, false, x, y, frame_width, frame_height);
	unsigned long* from=hge->Texture_Lock(texture_from, true, 0, 0, (int)frame_from_width, (int)frame_from_height);

	for (int i=0;i<frame_width;i++)
	{
		for (int j=0;j<frame_height;j++)
		{
			int i2=(int)(((float)i)/scale_x);
			int j2=(int)(((float)j)/scale_y);

			unsigned long color=from[j2*pitch_from+i2];
			to[j*pitch_to+i]=color;
		}
	}

	hge->Texture_Unlock(texture_from);
	hge->Texture_Unlock(texture);

	if (texture_from)
	{
		hge->Texture_Free(texture_from);
		texture_from=NULL;
	}

	if (spr_from)
	{
		delete spr_from;
		spr_from=NULL;
	}
}

// eng: Cleaning up of rectangle
// rus: Очистк?прямоугольник?
void MP_Atlas_WRAP::CleanRectangle(const MAGIC_CHANGE_ATLAS* c)
{
	HGE* hge=device->GetHGE();

	unsigned long color=0x0;

	HTEXTURE t=quad.tex;
	quad.tex=0;

	float left=(float)c->x;
	float top=(float)c->y;
	float right=left+(float)(c->width-1);
	float bottom=top+(float)(c->height-1);

	hgeVertex* v0=&(quad.v[0]);
	v0->x=left;
	v0->y=top;
	v0->tx=0.f;
	v0->ty=0.f;
	v0->col=color;

	hgeVertex* v1=&(quad.v[1]);
	v1->x=right;
	v1->y=top;
	v1->tx=0.f;
	v1->ty=0.f;
	v1->col=color;

	hgeVertex* v2=&(quad.v[2]);
	v2->x=right;
	v2->y=bottom;
	v2->tx=0.f;
	v2->ty=0.f;
	v2->col=color;

	hgeVertex* v3=&(quad.v[3]);
	v3->x=left;
	v3->y=bottom;
	v3->tx=0.f;
	v3->ty=0.f;
	v3->col=color;

	hge->Gfx_RenderQuad(&quad);

	quad.tex=t;
}

// eng: Particle drawing
// rus: Отрисовк?частиц?
void MP_Atlas_WRAP::Draw(MAGIC_PARTICLE_VERTEXES* vertexes)
{
	unsigned long color=vertexes->color;

	hgeVertex* v0=&(quad.v[0]);
	v0->x=vertexes->vertex1.x;
	v0->y=vertexes->vertex1.y;
	v0->tx=vertexes->u1;
	v0->ty=vertexes->v1;
	v0->col=color;

	hgeVertex* v1=&(quad.v[1]);
	v1->x=vertexes->vertex2.x;
	v1->y=vertexes->vertex2.y;
	v1->tx=vertexes->u2;
	v1->ty=vertexes->v2;
	v1->col=color;

	hgeVertex* v2=&(quad.v[2]);
	v2->x=vertexes->vertex3.x;
	v2->y=vertexes->vertex3.y;
	v2->tx=vertexes->u3;
	v2->ty=vertexes->v3;
	v2->col=color;

	hgeVertex* v3=&(quad.v[3]);
	v3->x=vertexes->vertex4.x;
	v3->y=vertexes->vertex4.y;
	v3->tx=vertexes->u4;
	v3->ty=vertexes->v4;
	v3->col=color;

	HGE* hge=device->GetHGE();
	hge->Gfx_RenderQuad(&quad);
}

// eng: Setting of intense
// rus: Установить интенсивност?
void MP_Atlas_WRAP::SetIntense(bool intense)
{
	if (intense)
		quad.blend=(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
	else
		quad.blend=(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
}

// --------------------------------------------------------------------------------

// eng: Class controlling drawing
// rus: Клас? которы?управляет рисованием
MP_Device_WRAP::MP_Device_WRAP(int width, int height, HGE* hge) : MP_Device(width, height)
{
	this->hge=hge;
	is_lost=false;
}

// eng: Beginning of scene drawing
// rus: Начало отрисовк?сцен?
void MP_Device_WRAP::BeginScene()
{
	MP_Device::BeginScene();

	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
}

// eng: End of scene drawing
// rus: Коне?отрисовк?сцен?
void MP_Device_WRAP::EndScene()
{
	hge->Gfx_EndScene();
}

// eng: Indicates that device is lost
// rus: Проверка на потерю устройства рисования
bool MP_Device_WRAP::IsLost()
{
	if (is_lost)
	{
		is_lost=false;
		return true;
	}
	return false;
}

// eng: Loading texture from file
// rus: Загрузка текстуры из файл?
HTEXTURE MP_Device_WRAP::LoadTextureFromFile(const char* file_name)
{
	MP_StringW texture_file=MP->GetPathToTexture();
	texture_file+=file_name;

	MP_StringW mode="rb";

	FILE* file=_wfopen(texture_file.w_str(), mode.w_str());
	fseek(file,0,SEEK_END);
	long len=ftell(file);
	fseek(file,0,SEEK_SET);

	char* data=new char[len];

	fread(data,1,len,file);
	fclose(file);

	HTEXTURE texture=hge->Texture_Load(data,len,false);

	delete []data;

	return texture;
}

