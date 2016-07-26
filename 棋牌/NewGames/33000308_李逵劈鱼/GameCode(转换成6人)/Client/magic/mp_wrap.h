//-----------------------------------------------------------------------------
// File: mp_wrap.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_WRAPPER
#define MAGIC_PARTICLES_WRAPPER

#include "mp.h"
#include "hge.h"
#include "hgefont.h"

// eng: Class storing the textural atlas
// rus: Клас? которы?хранит текстурный атла?
class MP_Atlas_WRAP : public MP_Atlas
{
protected:
	HTEXTURE texture;

	hgeQuad quad;
	
public:
	MP_Atlas_WRAP(int width, int height, const char* file);
	virtual ~MP_Atlas_WRAP(){}

	// eng: Returns texture
	// rus: Возвращает текстуру
	HTEXTURE GetTexture(){return texture;}

	// eng: Destroy atlas texture
	// rus: Уничтожить текстуру атласа
	virtual void Destroy();

	// eng: Loading of frame texture
	// rus: Загрузка текстурног?кадр?
	virtual void LoadTexture(const MAGIC_CHANGE_ATLAS* c);

	// eng: Cleaning up of rectangle
	// rus: Очистк?прямоугольник?
	virtual void CleanRectangle(const MAGIC_CHANGE_ATLAS* c);

	// eng: Particle drawing
	// rus: Отрисовк?частиц?
	virtual void Draw(MAGIC_PARTICLE_VERTEXES* vertexes);

	// eng: Setting of intense
	// rus: Установить интенсивност?
	virtual void SetIntense(bool intense);
};

// eng: Class controlling drawing
// rus: Клас? которы?управляет рисованием
class MP_Device_WRAP : public MP_Device
{
public:
	HGE* hge;
	bool is_lost;

public:

	MP_Device_WRAP(int width, int height, HGE* hge);

	HGE* GetHGE(){return hge;}

	// eng: Creating
	// rus: Создание
	virtual bool Create(){return true;}

	// eng: Destroying
	// rus: Уничтожени?
	virtual void Destroy(){}

	// eng: Beginning of scene drawing
	// rus: Начало отрисовк?сцен?
	virtual void BeginScene();

	// eng: End of scene drawing
	// rus: Коне?отрисовк?сцен?
	virtual void EndScene();

	// eng: Indicates that device is lost
	// rus: Проверка на потерю устройства рисования
	virtual bool IsLost();

	// eng: Loading texture from file
	// rus: Загрузка текстуры из файл?
	HTEXTURE LoadTextureFromFile(const char* file);
};


#endif