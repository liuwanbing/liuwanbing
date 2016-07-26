//-----------------------------------------------------------------------------
// File: mp.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_wrap.h"

#include <ERRNO.H>
extern int errno;

#ifndef TEST
#ifdef MAGIC_3D
#pragma comment(lib, "magic3d.lib")
#else
#pragma comment(lib, "magic.lib")
#endif
#endif

#pragma warning ( disable : 4996)

#ifdef MAGIC_3D
#include <math.h>

// ru: Инверс? кватернион?
// en: Quaternion inversion
void MagicQuaternionInversion(MAGIC_DIRECTION* q)
{
	float N=(float)(sqrt(q->x*q->x+q->y*q->y+q->z*q->z+q->w*q->w));
	q->x=-q->x/N;
	q->y=-q->y/N;
	q->z=-q->z/N;
	q->w=q->w/N;
}

// ru: Перемножение кватернионов
// en: Quaternion multiplication
void MagicQuaternionMul(MAGIC_DIRECTION *res, const MAGIC_DIRECTION *q1, const MAGIC_DIRECTION *q2)
{
	float A, B, C, D, E, F, G, H;

	A = (q1->w + q1->x) * (q2->w + q2->x);
	B = (q1->z - q1->y) * (q2->y - q2->z);
	C = (q1->x - q1->w) * (q2->y + q2->z);
	D = (q1->y + q1->z) * (q2->x - q2->w);
	E = (q1->x + q1->z) * (q2->x + q2->y);
	F = (q1->x - q1->z) * (q2->x - q2->y);
	G = (q1->w + q1->y) * (q2->w - q2->z);
	H = (q1->w - q1->y) * (q2->w + q2->z);

	res->w = B + (-E - F + G + H) * 0.5f;
	res->x = A - ( E + F + G + H) * 0.5f; 
	res->y =-C + ( E - F + G - H) * 0.5f;
	res->z =-D + ( E - F - G + H) * 0.5f;
}

#endif

// ------------------------------------------------------------------------------------------

MP_Platform::MP_Platform()
{
}

MP_Platform::~MP_Platform()
{
}

// ru: Конвертация wchar_t ?utf8
// en: Conversion of wchar_t in utf8
const char* MP_Platform::wchar_to_utf8(const wchar_t* str)
{
	return (const char*)str;
}

// ru: Конвертация utf8 ?wchar_t
// en: Conversion of utf8 in wchar_t
const wchar_t* MP_Platform::utf8_to_wchar(const char* str)
{
	return (const wchar_t*)str;
}

// ru: Удаление файл?(аналог "remove")
// en: Deleting of file (analogue "remove")
int MP_Platform::RemoveFile(const wchar_t* file)
{
	return _wremove(file);
}

// ------------------------------------------------------------------------------------------

MP_Device_WRAP* MP_Manager::device=NULL;

// eng: Class that is used as storage for Magic Particles emitters
// rus: Клас? которы?являет? хранилищем эмиттеро?Magic Particles
MP_Manager::MP_Manager()
{
	MP_Device::MP=this;
	MP_Atlas::MP=this;
	MP_Copy::MP=this;

	platform=NULL;

	k_emitter=0;
	max_emitter=10;
	m_emitter=new MP_Emitter*[max_emitter];
	m_descriptor=new int[max_emitter];
	for (int i=0;i<max_emitter;i++)
	{
		m_emitter[i]=NULL;
		m_descriptor[i]=0;
	}

	k_atlas=0;
	m_atlas=NULL;

	k_copy=0;
	m_copy=NULL;

	interpolation=MAGIC_INTERPOLATION_ENABLE;
	loop=MAGIC_LOOP;
	position_mode=MAGIC_CHANGE_EMITTER_DEFAULT;

	atlas_width=atlas_height=1024;
	atlas_frame_step=1;
	atlas_scale_step=0.01f;

	next_descriptor=0;
	next_index=-1;

	is_new_atlas=false;
}

MP_Manager& MP_Manager::GetInstance()
{
	static MP_Manager mp;
	return mp;
}

// eng: Cleaning up
// rus: Очистк?
void MP_Manager::Destroy()
{
	int i;
	for (i=0;i<max_emitter;i++)
	{
		if (m_emitter[i])
		{
			delete m_emitter[i];
			m_emitter[i]=NULL;
		}
	}

	if (m_emitter)
	{
		delete []m_emitter;
		m_emitter=NULL;
	}

	if (m_descriptor)
	{
		delete []m_descriptor;
		m_descriptor=NULL;
	}

	k_emitter=0;
	max_emitter=0;

	for (i=0;i<k_atlas;i++)
	{
		m_atlas[i]->Destroy();
		delete m_atlas[i];
		m_atlas[i]=NULL;
	}

	if (m_atlas)
	{
		delete []m_atlas;
		m_atlas=NULL;
	}

	k_atlas=0;

	for (i=0;i<k_copy;i++)
	{
		delete m_copy[i];
		m_copy[i]=NULL;
	}

	if (m_copy)
	{
		delete []m_copy;
		m_copy=NULL;
	}

	k_copy=0;

	MP_Device::MP=NULL;
	MP_Atlas::MP=NULL;
	MP_Copy::MP=NULL;

	if (platform)
	{
		delete platform;
		platform=NULL;
	}
}

// eng: Initialization
// rus: Инициализация
void MP_Manager::Initialization(MP_Platform* platform, int interpolation, int loop, int position_mode, int atlas_width, int atlas_height, int atlas_frame_step, float atlas_starting_scale, float atlas_scale_step, bool copy_mode)
{
	this->platform=platform;

	Magic_SetCleverModeForAtlas(true);
	Magic_SetStartingScaleForAtlas(atlas_starting_scale);

	this->interpolation=interpolation;
	this->loop=loop;
	this->position_mode=position_mode;
	this->sorting_mode=sorting_mode;

	this->atlas_width=atlas_width;
	this->atlas_height=atlas_height;
	this->atlas_frame_step=atlas_frame_step;
	this->atlas_scale_step=atlas_scale_step;

	this->copy_mode=copy_mode;
}

// eng: Returning descriptor of first emitter. 
// rus: Получени?дескриптор?первог?эмиттера
HM_EMITTER MP_Manager::GetFirstEmitter()
{
	next_descriptor=0;
	next_index=-1;

	if (k_emitter)
	{
		next_descriptor=m_descriptor[0];
		next_index=0;
	}

	return next_descriptor;
}

/// eng: Returning descriptor of next emitter. 
// rus: Получени?дескриптор?следующего эмиттера
HM_EMITTER MP_Manager::GetNextEmitter(HM_EMITTER hmEmitter)
{
	if (next_index==-1 || hmEmitter!=next_descriptor)
	{
		next_index=-1;
		for (int i=0;i<k_emitter;i++)
		{
			if (m_descriptor[i]==hmEmitter)
			{
				next_index=i;
				break;
			}
		}
	}

	next_descriptor=0;

	if (next_index!=-1)
	{
		next_index++;
		if (next_index<k_emitter)
		{
			next_descriptor=m_descriptor[next_index];
		}
		else
			next_index=-1;
	}

	return next_descriptor;
}

// eng: Returning the emitter by its descriptor
// rus: Возвращени?эмиттера по дескриптор?
MP_Emitter* MP_Manager::GetEmitter(HM_EMITTER hmEmitter)
{
	if (hmEmitter>=0 && hmEmitter<max_emitter)
		return m_emitter[hmEmitter];
	return NULL;
}

// eng: Returning the emitter by name
// rus: Возвращани?эмиттера по имен?
MP_Emitter* MP_Manager::GetEmitterByName(const char* name)
{
	HM_EMITTER hmEmitter=GetFirstEmitter();
	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		const char* emitter_name=emitter->GetEmitterName();
		if (!strcmp(name,emitter_name))
		{
			// eng: name coincides
			// rus: имя совпадае?
			return emitter;
		}
		hmEmitter=GetNextEmitter(hmEmitter);
	}

	return NULL;
}

// eng: Loading all emitters from emitters folder
// rus: Загрузка всех эмиттеро?из всех файлов
void MP_Manager::LoadAllEmitters()
{
	const wchar_t* file=platform->GetFirstFile();
	while (file)
	{
		LoadEmittersFromFile(file);
		file=platform->GetNextFile();
	}
	RefreshAtlas();
}

// eng: Loading all the emitters and animated folders from the file specified
// rus: Загрузка всех эмиттеро?из указанного файл? Загружаются эмиттеры ?анимированны?папк?
HM_FILE MP_Manager::LoadEmittersFromFile(const wchar_t* file)
{
	MP_StringW ptc_file=GetPathToPTC();
	ptc_file+=file;
	const char* file_utf8=wchar_to_utf8(ptc_file.w_str());

	HM_FILE mf=Magic_OpenFile(file_utf8);
	if (mf>0)
	{
		// eng: file was opened
		// rus: файл успешн?открыт
		LoadFolder(mf,"");
		return mf;
	}

	return 0;
}

HM_FILE MP_Manager::LoadEmittersFromFileInMemory(const char* address)
{
	HM_FILE mf=Magic_OpenFileInMemory(address);
	if (mf)
	{
		// eng: file was opened
		// rus: файл успешн?открыт
		LoadFolder(mf,"");
		return mf;
	}

	return 0;
}

// eng: Closing file
// rus: Выгрузка одного файл?
int MP_Manager::CloseFile(HM_FILE hmFile)
{
	RefreshAtlas();
	return Magic_CloseFile(hmFile);
}

// eng: Closing all files
// rus: Выгрузка всех файлов
void MP_Manager::CloseFiles()
{
	RefreshAtlas();
	Magic_CloseAllFiles();
}

// eng: Duplicating specified emitter
// rus: Дублирование указанного эмиттера
HM_EMITTER MP_Manager::DuplicateEmitter(HM_EMITTER hmEmitter)
{
	MP_Emitter* from=GetEmitter(hmEmitter);
	if (from)
	{
		if (Magic_IsInterval1(hmEmitter) && (!from->copy))
		{
			// eng: it is necessary firstly to create particles copy
			// rus: необходимо сначал?создат?копи?частиц
			from->Restart();
		}

		MP_Emitter* emitter=new MP_Emitter(0,this);
		*emitter=*from;
		AddEmitter(emitter);
		return emitter->GetEmitter();
	}
	return 0;
}

// eng: Updating emitters taking into account the passed time
// rus: Обновление эмиттеро?по таймер?
int MP_Manager::UpdateByTimer()
{
	static int fps=0;
	static int fps_counter=0;
	static unsigned long old_time=0;

	static unsigned long last_time=0;
	double rate=0.01;
	unsigned long new_time=GetTick();
	if (new_time>last_time)
	{
		rate=new_time-last_time;
		last_time=new_time;
		if (rate>500)
			rate=0.01;
	}

	unsigned long rtime=new_time-old_time;
	if (rtime>1000)
	{
		// eng: more than 1 second passed
		// rus: прошло больше секунд?
		double percent=((double)rtime)/10.f;
		fps=(int)(percent*fps_counter/100);
		fps_counter=0;
		old_time=new_time;
	}

	fps_counter++;

	Update(rate);

	return fps;
}

// eng: Updating emitters
// rus: Обновление эмиттеро?
void MP_Manager::Update(double time)
{
	HM_EMITTER hmEmitter=GetFirstEmitter();

	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		int state=emitter->GetState();
		if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
			emitter->Update(time);
		
		hmEmitter=GetNextEmitter(hmEmitter);
	}
}

// eng: Rendering all emitters
// rus: Рисовани?эмиттеро?
int MP_Manager::Render()
{
	RefreshAtlas();

	device->BeginScene();

	// eng: visualisation of all emitters
	// rus: визуализац? всех эмиттеро?
	int k_particles=0;

	HM_EMITTER hmEmitter=GetFirstEmitter();

	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		k_particles+=emitter->Render();
		hmEmitter=GetNextEmitter(hmEmitter);
	}

	device->EndScene();

	return k_particles;
}

// eng: Stopping all the emitters
// rus: Остановк?эмиттеро?
void MP_Manager::Stop()
{
	HM_EMITTER hmEmitter=GetFirstEmitter();

	while (hmEmitter)
	{
		MP_Emitter* emitter=GetEmitter(hmEmitter);
		emitter->SetState(MAGIC_STATE_STOP);
		hmEmitter=GetNextEmitter(hmEmitter);
	}
}

// eng: Loading folder
// rus: Загрузка папк?
void MP_Manager::LoadFolder(HM_FILE file, const char* path)
{
	Magic_SetCurrentFolder(file, path);

	MAGIC_FIND_DATA find;
	const char* name=Magic_FindFirst(file,&find,MAGIC_FOLDER | MAGIC_EMITTER);
	while (name)
	{
		if (find.animate)
		{
			LoadEmitter(file,name);
		}
		else
			LoadFolder(file,name);

		name=Magic_FindNext(file,&find);
	}

	Magic_SetCurrentFolder(file,"..");
}

// eng: Loading emitter
// rus: Загрузка конкретног?эмиттера
MP_Emitter* MP_Manager::LoadEmitter(HM_FILE file, const char* path)
{
	// eng: it is necessary to load emitter from file
	// rus: нужн?извлеч?эмитте?из файл?
	MP_Emitter* em=NULL;
	HM_EMITTER emitter=Magic_LoadEmitter(file,path);
	if (emitter)
	{
		em=new MP_Emitter(emitter,this);
		const char* ptc=Magic_GetFileName(file);
		if (ptc && Magic_HasTextures(file))
			em->restore_file=platform->utf8_to_wchar(ptc);

		AddEmitter(em);
		// eng: initialization of emitter by default values
		// rus: инициализация эмиттера значен?ми по умолчани?
		if (interpolation!=MAGIC_INTERPOLATION_DEFAULT)
		{
			bool _interpolation=false;
			if (interpolation==MAGIC_INTERPOLATION_ENABLE)
				_interpolation=true;
			Magic_SetInterpolationMode(emitter,_interpolation);
		}
		Magic_SetLoopMode(emitter,loop);

		switch (position_mode)
		{
		case MAGIC_CHANGE_EMITTER_ONLY:
			Magic_SetEmitterPositionMode(emitter,false);
			Magic_SetEmitterDirectionMode(emitter,false);
			break;

		case MAGIC_CHANGE_EMITTER_AND_PARTICLES:
			Magic_SetEmitterPositionMode(emitter,true);
			Magic_SetEmitterDirectionMode(emitter,true);
			break;
		}

		if (Magic_GetStaticAtlasCount(file))
			em->is_atlas=true;
		else
			is_new_atlas=true;
	}

	return em;
}

// eng: Adding new emitter into array
// rus: Добавление нового эмиттера ?массив
void MP_Manager::AddEmitter(MP_Emitter* emitter)
{
	int i;

	next_descriptor=0;
	next_index=-1;

	int index=(int)emitter->GetEmitter();

	while (index>=max_emitter)
	{
		int new_max_emitter=max_emitter+10;

		MP_Emitter** vm_emitter=new MP_Emitter*[new_max_emitter];
		for (i=0;i<max_emitter;i++)
			vm_emitter[i]=m_emitter[i];
		delete []m_emitter;
		m_emitter=vm_emitter;
		
		int* vm_descriptor=new int[new_max_emitter];
		for (i=0;i<max_emitter;i++)
			vm_descriptor[i]=m_descriptor[i];
		delete []m_descriptor;
		m_descriptor=vm_descriptor;

		for (i=max_emitter;i<new_max_emitter;i++)
		{
			m_emitter[i]=NULL;
			m_descriptor[i]=0;
		}

		max_emitter=new_max_emitter;
	}

	m_emitter[index]=emitter;
	m_descriptor[k_emitter]=index;
	k_emitter++;
}

// eng: Refreshing textural atlases
// rus: Построение текстурног?атласа
void MP_Manager::RefreshAtlas()
{
	int i;

	if (is_new_atlas)
	{
		// eng: new emitters were added, it is necessary to create new atlases for them
		// rus: были добавлен?новы?эмиттеры, необходимо создат?для ни?атласы
		is_new_atlas=false;

		int k=GetEmitterCount();
		if (k)
		{
			HM_EMITTER* hm_emitter=new HM_EMITTER[k];

			k=0;

			HM_EMITTER hmEmitter=GetFirstEmitter();
			while (hmEmitter)
			{
				MP_Emitter* emitter=GetEmitter(hmEmitter);
				if (!emitter->is_atlas)
				{
					emitter->is_atlas=true;
					hm_emitter[k]=hmEmitter;
					k++;
				}
				hmEmitter=GetNextEmitter(hmEmitter);
			}

			if (k)
				Magic_CreateAtlasesForEmitters(atlas_width,atlas_height,k,hm_emitter,atlas_frame_step,atlas_scale_step);

			delete []hm_emitter;
			hm_emitter=NULL;
		}
	}

	MAGIC_CHANGE_ATLAS c;
	while (Magic_GetNextAtlasChange(&c)==MAGIC_SUCCESS)
	{
		switch (c.type)
		{
		case MAGIC_CHANGE_ATLAS_LOAD:
			// eng: loading of frame in atlas
			// rus: загрузка кадр??атла?
			m_atlas[c.index]->LoadTexture(&c);
			break;

		case MAGIC_CHANGE_ATLAS_CLEAN:
			// eng: cleaning up of rectangle in atlas
			// rus: очистк?прямоугольник??атласе
			m_atlas[c.index]->CleanRectangle(&c);
			break;

		case MAGIC_CHANGE_ATLAS_CREATE:
			// eng: creating of atlas
			// rus: создание атласа
			if (m_atlas)
			{
				// eng: broadening of atlas array
				// rus: расширение массив атласо?
				MP_Atlas** vm_atlas=new MP_Atlas*[k_atlas+1];
				for (i=0;i<k_atlas;i++)
					vm_atlas[i]=m_atlas[i];
				delete []m_atlas;
				m_atlas=vm_atlas;
			}
			else
			{
				m_atlas=new MP_Atlas*[1];
			}

			m_atlas[k_atlas]=device->NewAtlas(c.width, c.height, c.file);
			k_atlas++;

			break;

		case MAGIC_CHANGE_ATLAS_DELETE:
			// eng: Deleting of atlas
			// rus: удаление атласа
			m_atlas[c.index]->Destroy();
			delete m_atlas[c.index];

			if (k_atlas==1)
			{
				delete []m_atlas;
				m_atlas=NULL;
			}
			else
			{
				MP_Atlas** vm_atlas=new MP_Atlas*[k_atlas-1];
				for (i=0;i<c.index;i++)
					vm_atlas[i]=m_atlas[i];
				for (i=c.index+1;i<k_atlas;i++)
					vm_atlas[i-1]=m_atlas[i];
				delete []m_atlas;
				m_atlas=vm_atlas;
			}
			k_atlas--;
		}
	}
}

// eng: Restoring textural atlas in cases of loosing textures
// rus: Восстановление текстурног?атласа ?случае потери тексту?
void MP_Manager::RestoreAtlas()
{
	if (k_emitter)
	{
		int i;

		// eng: Recreating of static atlas
		// rus: Перестраивае?статически?атласы
		for (i=0;i<k_atlas;i++)
		{
			MP_Atlas* atlas=m_atlas[i];
			MP_String file=atlas->GetFile();
			if (!(file.empty()))
			{
				// eng: it is necessary to reload texture
				// rus: необходимо перегрузит?текстуру
				int width, height;
				atlas->GetSize(width, height);

				atlas->Destroy();
				delete atlas;
				m_atlas[i]=device->NewAtlas(width,height,file.c_str());
			}
		}

		// eng: Recreating of dynamic atlas
		// rus: Перестраивае?динамические атласы
		int k_restore_file=0;
		MP_StringW** m_restore_file=new MP_StringW*[k_emitter];

		HM_EMITTER hmEmitter=GetFirstEmitter();
		while (hmEmitter)
		{
			MP_Emitter* emitter=GetEmitter(hmEmitter);

			bool add=true;

			if (emitter->restore_file.empty())
				add=false;
			else
			{
				for (int j=0;j<k_restore_file;j++)
				{
					if (*(m_restore_file[j])==emitter->restore_file)
					{
						add=false;
						break;
					}
				}
			}

			if (add)
			{
				m_restore_file[k_restore_file]=&(emitter->restore_file);
				k_restore_file++;
			}

			hmEmitter=GetNextEmitter(hmEmitter);
		}

		HM_FILE* m_opened_file=NULL;
		if (k_restore_file)
		{
			// rus: loading all found files with textures
			// rus: загружае?вс?отобранные файл??текстурами
			m_opened_file=new HM_FILE[k_restore_file];
			for (i=0;i<k_restore_file;i++)
			{
				MP_StringW path=GetPathToPTC();
				path+=*(m_restore_file[i]);
				const char* path_utf8=wchar_to_utf8(path.w_str());
				m_opened_file[i]=Magic_OpenFile(path_utf8);
			}
		}

		// eng: Recreating of dynamic atlas
		// rus: Перестраивае?динамические атласы
		Magic_CreateAtlases(atlas_width,atlas_height,atlas_frame_step,atlas_scale_step);

		RefreshAtlas();

		if (k_restore_file)
		{
			// eng: Unload ptc-file with textures
			// rus: выгружае?ptc-файл??текстурами
			for (i=0;i<k_restore_file;i++)
				Magic_CloseFile(m_opened_file[i]);

			delete []m_opened_file;
			m_opened_file=NULL;
		}

		delete []m_restore_file;
		m_restore_file=NULL;
		k_restore_file=0;
	}
}

// eng: Deleting specified emitter 
// rus: Удаление указанного эмиттера
int MP_Manager::DeleteEmitter(HM_EMITTER hmEmitter)
{
	int result=MAGIC_ERROR;
	next_descriptor=0;
	next_index=-1;

	for (int j=0;j<k_emitter;j++)
	{
		HM_EMITTER hme=m_descriptor[j];
		if (hme==hmEmitter)
		{
			// it is necessary to delete this element from index array
			// нужн?удалит?данный элемен?из индексного массив?
			for (int k=j+1;k<k_emitter;k++)
			{
				m_descriptor[k-1]=m_descriptor[k];
			}

			k_emitter--;

			m_descriptor[k_emitter]=0;

			delete m_emitter[hmEmitter];
			m_emitter[hmEmitter]=NULL;

			result=MAGIC_SUCCESS;

			break;
		}
	}

	return result;
}

// eng: Adding file with particles copy
// rus: Добавление файл??копией частиц
MP_Copy* MP_Manager::AddCopy(MP_Emitter* emitter)
{
	if (m_copy)
	{
		MP_Copy** vm_copy=new MP_Copy*[k_copy+1];
		for (int i=0;i<k_copy;i++)
			vm_copy[i]=m_copy[i];
		delete []m_copy;
		m_copy=vm_copy;
	}
	else
		m_copy=new MP_Copy*[1];

	MP_Copy* copy=new MP_Copy(emitter);
	m_copy[k_copy]=copy;
	k_copy++;

	return copy;
}

// eng: Deleting file with particles copy
// rus: Удаление файл??копией частиц
void MP_Manager::DeleteCopy(MP_Copy* copy)
{
	// eng: it is necessary to delete copy
	// rus: надо удалит?копи?
	int i;
	
	int index=-1;
	for (i=0;i<k_copy;i++)
	{
		if (m_copy[i]==copy)
		{
			index=i;
			break;
		}
	}

	delete m_copy[index];

	if (k_copy==1)
	{
		delete []m_copy;
		m_copy=NULL;
	}
	else
	{
		MP_Copy** vm_copy=new MP_Copy*[k_copy-1];
		for (i=0;i<index;i++)
			vm_copy[i]=m_copy[i];
		for (i=index+1;i<k_copy;i++)
			vm_copy[i-1]=m_copy[i];
		delete []m_copy;
		m_copy=vm_copy;
	}

	k_copy--;
}

// eng: Searching among files containing particle copies by specified emitter id
// rus: Поис?сред?файлов копи?частиц соответствующего указанному идентификатору эмиттера
MP_Copy* MP_Manager::FindCopy(unsigned int emitter_id)
{
	if (GetCopyMode())
	{
		for (int i=0;i<k_copy;i++)
		{
			MP_Copy* copy=m_copy[i];
			if (copy->GetEmitterID()==emitter_id)
				return copy;
		}
	}
	return NULL;
}

// ------------------------------------------------------------------------------------------------

// eng: Class, specialized for work with the emitters
// rus: Клас? которы?хранит загруженны?эмиттеры

MP_Device_WRAP* MP_Emitter::device=NULL;

MP_Emitter::MP_Emitter(HM_EMITTER emitter, MP_Manager* owner)
{
	this->emitter=emitter;
	this->owner=owner;

	z=0.f;

	first_restart=true;

	copy=NULL;

	restore_file="";

	state=MAGIC_STATE_UPDATE;

	is_atlas=false;
}

MP_Emitter::~MP_Emitter()
{
	if (copy)
	{
		copy->DecReference();
		copy=NULL;
	}

	Magic_UnloadEmitter(emitter);
}

MP_Emitter& MP_Emitter::operator=(const MP_Emitter& from)
{
	if (copy)
	{
		copy->DecReference();
		copy=NULL;
	}

	state=from.state;

	is_atlas=from.is_atlas;

	z=from.z;

	if (from.copy)
	{
		copy=from.copy;
		copy->IncReference(this);
	}

	restore_file=from.restore_file;

	emitter=Magic_DuplicateEmitter(from.emitter);
	
	return *this;
}

// eng: Returning the name of the emitter
// rus: Возвращени?имен?эмиттера
const char* MP_Emitter::GetEmitterName()
{
	return Magic_GetEmitterName(emitter);
}

// eng: Restarting of emitter
// rus: Установк?эмиттера на стартову?позици?
void MP_Emitter::Restart()
{
	if (Magic_IsInterval1(emitter))
	{
		// eng: animation starts not from beginning
		// rus: анимац? начинает? не ?начала
		if (!copy)
		{
			copy=owner->FindCopy(Magic_GetEmitterID(emitter));
			if (!copy)
				copy=owner->AddCopy(this);
		}

		copy->LoadParticles(this);
	}
	else
	{
		Magic_Restart(emitter);
	}

	first_restart=false;
}

// eng: Position of emitter
// rus: Позиция эмиттера
void MP_Emitter::GetPosition(MP_POSITION& position)
{
	MAGIC_POSITION pos;
	Magic_GetEmitterPosition(emitter,&pos);
	position.x=pos.x;
	position.y=pos.y;
	#ifdef MAGIC_3D
	position.z=pos.z;
	#else
	position.z=z;
	#endif
}

void MP_Emitter::SetPosition(MP_POSITION& position)
{
	MAGIC_POSITION pos;
	pos.x=position.x;
	pos.y=position.y;

	#ifdef MAGIC_3D
	pos.z=position.z;
	#else
	z=position.z;
	#endif
	Magic_SetEmitterPosition(emitter,&pos);
}

// eng: Moving the emitter to the position specified allowing restart. 
// rus: Перемещени?эмиттера ?указанну?позици??возможностью перезапуск? Пр?перемещени?вс?существующие частиц?перемещают? вместе ?эмиттеро?
void MP_Emitter::Move(MP_POSITION& position, bool restart)
{
	if (restart)
	{
		Restart();
		SetPosition(position);
	}
	else
	{
		bool mode=Magic_GetEmitterPositionMode(emitter);
		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: temporary setting mode of movement together with particles
			// rus: временно устанавливае?режи?перемещения вместе ?частицам?
			Magic_SetEmitterPositionMode(emitter,MAGIC_CHANGE_EMITTER_AND_PARTICLES);
		}

		SetPosition(position);

		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: restore previous mode of movement
			// rus: возвращаем на мест?старый режи?перемещения
			Magic_SetEmitterPositionMode(emitter,MAGIC_CHANGE_EMITTER_ONLY);
		}
	}
}

// eng: Offsetting the current emitter position by the value specified
// rus: Смещение текуще?позици?эмиттера на указанну?величину. Буде?использова?текущи?режи?перемещения эмиттера
void MP_Emitter::Offset(MP_POSITION& offset)
{
	MP_POSITION pos;
	GetPosition(pos);
	pos+=offset;
	SetPosition(pos);
}

// eng: Direction of emitter
// rus: Направлени?эмиттера
void MP_Emitter::SetDirection(MAGIC_DIRECTION* direction)
{
	Magic_SetEmitterDirection(emitter, direction);
}

void MP_Emitter::GetDirection(MAGIC_DIRECTION* direction)
{
	Magic_GetEmitterDirection(emitter, direction);
}

// eng: Setting the emitter direction to the specified value with the restart ability
// rus: Поворо?эмиттера ?указанно?направлени??возможностью перезапуск? Пр?повороте вс?существующие частиц?поворачивают? вместе ?эмиттеро?
void MP_Emitter::Direct(MAGIC_DIRECTION* direction, bool restart)
{
	if (restart)
	{
		Restart();
		SetDirection(direction);
	}
	else
	{
		bool mode=Magic_GetEmitterDirectionMode(emitter);
		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: temporary setting mode of movement together with particles
			// rus: временно устанавливае?режи?перемещения вместе ?частицам?
			Magic_SetEmitterDirectionMode(emitter,MAGIC_CHANGE_EMITTER_AND_PARTICLES);
		}

		SetDirection(direction);

		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: restore previous mode of movement
			// rus: возвращаем на мест?старый режи?перемещения
			Magic_SetEmitterDirectionMode(emitter,MAGIC_CHANGE_EMITTER_ONLY);
		}
	}
}

// eng: Rotating of the emitter by the specified value
// rus: Вращение эмиттера на указанну?величину. Буде?использова?текущи?режи?вращен? эмиттера
void MP_Emitter::Rotate(MAGIC_DIRECTION* offset)
{
	MAGIC_DIRECTION q;
	GetDirection(&q);
	#ifdef MAGIC_3D
	MagicQuaternionInversion(&q);
	MagicQuaternionMul(&q,&q,offset);
	#else
	q.angle+=offset->angle;
	#endif
	SetDirection(&q);
}

// eng: Setting the scale of the emitter
// rus: Установк?масштаба эмиттера
void MP_Emitter::SetScale(float scale)
{
	Magic_SetScale(emitter,scale);
}

float MP_Emitter::GetScale()
{
	return Magic_GetScale(emitter); 
}

// eng: Setting the state of the emitter
// rus: Установк?статус?эмиттера
void MP_Emitter::SetState(int state)
{
	if (this->state!=state)
	{
		if (state==MAGIC_STATE_UPDATE && Magic_IsInterrupt(emitter))
		{
			// eng: it is necessary to turn off interrupting of emitter work
			// rus: необходимо отключит?прерывание работы эмиттера
			Magic_SetInterrupt(emitter,false);
		}

		if (state==MAGIC_STATE_STOP && this->state!=MAGIC_STATE_INTERRUPT)
		{
			// eng: unload particles from memory
			// rus: выгружае?пространство частиц из па?ти
			Magic_Stop(emitter);
		}
		else
		{
			if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
			{
				// eng: start emitter
				// rus: стартуем эмитте?
				if (!first_restart)
				{
					if (this->state==MAGIC_STATE_STOP || (!Magic_InInterval(emitter)))
					{
						// rus: позиция анимации эмиттера не входит ?интервал видимост? необходимо осуществит?установк?на начало
						if (copy)
							copy->LoadParticles(this);
					}
				}

				if (state==MAGIC_STATE_INTERRUPT)
					Magic_SetInterrupt(emitter,true);
			}
		}

		this->state=state;
	}
}

// eng: Updating emitter
// rus: Обновление эмиттера
void MP_Emitter::Update(double time)
{
	if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
	{
		if (first_restart)
			Restart();

		if (!Magic_IsInterpolationMode(emitter))
		{
			// rus: without interpolation a fixing step is only possible
			// rus: бе?интерполяци?возможен только фиксированны?ша?
			time=Magic_GetUpdateTime(emitter);
		}

		if (!Magic_Update(emitter,time))
		{
			// eng: working of emitter is over
			// rus: выполнение эмиттера завершен?
			SetState(MAGIC_STATE_STOP);
		}
	}
}

// eng: Emitter visualization
// rus: Отрисовк?эмиттера. Возвращает? количество нарисованных частиц
int MP_Emitter::Render()
{
	int count=0;
	if (state!=MAGIC_STATE_STOP)
	{
		HM_EMITTER emitter=GetEmitter();
		if (Magic_InInterval(emitter))
		{
			MAGIC_RENDERING rendering;
			Magic_CreateFirstRenderedParticlesList(emitter, &rendering);
			int particles_count=rendering.count;
			if (particles_count)
			{
				MP_Atlas* atlas=owner->GetAtlas(rendering.texture_id);

				atlas->BeginDrawEmitter(this);

				while (particles_count)
				{
					atlas->BeginDrawParticles();

					if (rendering.intense)
						atlas->SetIntense(true);
					else
						atlas->SetIntense(false);

					// eng: particles drawing
					// rus: рисовани?частиц
					for (int i=0;i<particles_count;i++)
					{
						MAGIC_PARTICLE_VERTEXES vertexes;
						Magic_GetNextParticleVertexes(&vertexes);
						atlas->Draw(&vertexes);
					}

					atlas->EndDrawParticles();

					count+=particles_count;
					Magic_CreateNextRenderedParticlesList(&rendering);
					particles_count=rendering.count;
				}

				atlas->EndDrawEmitter(this);
			}
		}
	}
	return count;
}

// ------------------------------------------------------------------------------------------

// eng: Class storing the files with particles copies
// rus: Клас? которы?хранит файл??копиями частиц
int MP_Copy::file_count=0;
MP_StringW MP_Copy::file_name="mp";
MP_Manager* MP_Copy::MP=NULL;

MP_Copy::MP_Copy(MP_Emitter* emitter)
{
	emitter_id=Magic_GetEmitterID(emitter->GetEmitter());
	reference=0;
	file="";
	IncReference(emitter);
}

MP_Copy::~MP_Copy()
{
	Clear();
}

// eng: Cleaning up
// rus: Очистк?
void MP_Copy::Clear()
{
	if (!file.empty())
	{
		MP->RemoveFile(file.w_str());
		file="";
	}
	reference=0;
}

// eng: Increasing of reference count
// rus: Увеличение числ?ссылок на файл
void MP_Copy::IncReference(MP_Emitter* emitter)
{
	if (!reference)
	{
		// eng: temporary folder exists
		// rus: временная папк?существует
		srand(MP->GetTick());
		bool repeat_select;
		do
		{
			repeat_select=false;
			file=MP->GetPathToTemp();
			file+=file_name;
			static char s[20];
			sprintf(s,"%d",file_count);
			file+=s;
			if (MP->RemoveFile(file.w_str())==-1)
			{
				if (errno!=ENOENT)
				{
					// eng: temporary existing was not deleted, it is necessary to choose another one
					// rus: существующий временны?файл не бы?удален, надо выбрат?другой файл
					repeat_select=true;

					// eng: select random digit from 0 to 9
					// rus: выбираем случайно?числ?от 0 до 9
					int r=rand();
					r=r % 10;
					// eng: convert it to symbol
					// rus: превращаем ег??символ
					char v=(char)r;
					v+='0';
					file_name+=v;
				}
			}
		}
		while (repeat_select);
		file_count++;

		LoadParticles(emitter);

		reference++;
	}
	else
	{
		if (!MP->GetCopyMode())
			reference++;
	}
}

// eng: Decreasing of reference count
// rus: Уменьшение числ?ссылок на файл
void MP_Copy::DecReference()
{
	if (!MP->GetCopyMode())
	{
		reference--;
		if (!reference)
			MP->DeleteCopy(this);
	}
}

// eng: Loading of particles from file to emitter
// rus: Загрузка частиц из файл??эмитте?
void MP_Copy::LoadParticles(MP_Emitter* emitter)
{
	const char* temp_file=NULL;
	if (!file.empty())
	{
		temp_file=MP->wchar_to_utf8(file.w_str());
	}

	Magic_EmitterToInterval1(emitter->GetEmitter(), 1.f, temp_file);
}

// ------------------------------------------------------------------------------------------

// eng: Class storing the textural atlas. This class will be abstract
// rus: Клас? которы?хранит текстурный атла? Этот клас?буде?абстрактны?

MP_Manager* MP_Atlas::MP=NULL;
MP_Device_WRAP* MP_Atlas::device=NULL;

MP_Atlas::MP_Atlas(int width, int height, const char* file)
{
	atlas_width=width;
	atlas_height=height;
	file_name="";
	if (file)
		file_name=file;
}

// ------------------------------------------------------------------------------------------

#ifndef TEST
#ifdef MAGIC_3D

MP_VECTOR3::MP_VECTOR3(float x, float y, float z)
{
    this->x=x;
    this->y=y;
    this->z=z;
}

MP_VECTOR3 MP_VECTOR3::operator+(const MP_VECTOR3& v ) const
{
    return MP_VECTOR3(x + v.x, y + v.y, z + v.z);
}

MP_VECTOR3 MP_VECTOR3::operator-(const MP_VECTOR3& v ) const
{
    return MP_VECTOR3(x - v.x, y - v.y, z - v.z);
}

MP_VECTOR3 MP_VECTOR3::operator*(float f) const
{
    return MP_VECTOR3(x * f, y * f, z * f);
}

MP_VECTOR3 MP_VECTOR3::operator/(float f) const
{
    float fInv = 1.0f/f;
    return MP_VECTOR3(x * fInv, y * fInv, z * fInv);
}
#endif
#endif

// ------------------------------------------------------------------------------------------

// eng: Class controlling drawing. This class will be abstract
// rus: Клас? которы?управляет рисованием. Этот клас?буде?абстрактны?

MP_Manager* MP_Device::MP=NULL;

MP_Device::MP_Device(int width, int height)
{
	MP_Manager::device=(MP_Device_WRAP*)this;
	MP_Emitter::device=(MP_Device_WRAP*)this;
	MP_Atlas::device=(MP_Device_WRAP*)this;

	window_width=width;
	window_height=height;
}

MP_Device::~MP_Device()
{
	MP_Manager::device=NULL;
	MP_Emitter::device=NULL;
	MP_Atlas::device=NULL;
}

// eng: Destroying
// rus: Уничтожени?
void MP_Device::Destroy()
{
	if (MP)
	{
		int k_atlas=MP->GetAtlasCount();
		for (int i=0;i<k_atlas;i++)
			MP->GetAtlas(i)->Destroy();
	}
}

// eng: Beginning of scene drawing
// rus: Начало отрисовк?сцен?
void MP_Device::BeginScene()
{
	if (IsLost())
	{
		Create();
		#ifdef MAGIC_3D
		// eng: setting view matrix
		// rus: устанавливае?матриц?вида
		SetScene3d();
		#endif
		MP->RestoreAtlas();
	}
}

// eng: Creating of atlas object
// rus: Создание объект?атласа
MP_Atlas* MP_Device::NewAtlas(int width, int height, const char* file)
{
	return new MP_Atlas_WRAP(width, height, file);
}

// ------------------------------------------------------------------------------------------

// eng: Getting identity matrix 
// rus: Возвращает единичну?матриц?
void MatrixIdentity(MAGIC_MATRIX* m)
{
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
			if (i==j)
				m->m[i][j]=1;
			else
				m->m[i][j]=0;
		}
	}
}

// eng: Calculation of perspective projection matrix (analogue of D3DXMatrixPerspectiveFovLH from Direct3D) 
// rus: Расчет матриц?перспективно?проекции (аналог D3DXMatrixPerspectiveFovLH из Direct3D)
void MatrixPerspectiveFovLH(MAGIC_MATRIX* m, float fovy, float aspect, float zn, float zf)
{
	float yScale=(float)(1/(tan(fovy/2.f)));
	float xScale=yScale/aspect;

	m->m[0][0]=xScale;
	m->m[1][0]=0.f;
	m->m[2][0]=0.f;
	m->m[3][0]=0.f;

	m->m[0][1]=0.f;
	m->m[1][1]=yScale;
	m->m[2][1]=0.f;
	m->m[3][1]=0.f;

	m->m[0][2]=0.f;
	m->m[1][2]=0.f;
	m->m[2][2]=zf/(zf-zn);
	m->m[3][2]=-zn*zf/(zf-zn);

	m->m[0][3]=0.f;
	m->m[1][3]=0.f;
	m->m[2][3]=1.f;
	m->m[3][3]=0.f;
}

// eng: Calculation of orthogonal projection (analogue of D3DXMatrixOrthoLH from Direct3D) 
// rus: Расчет матриц?ортогонально?проекции (аналог D3DXMatrixOrthoLH из Direct3D)
void MatrixOrthoLH(MAGIC_MATRIX* m, float width, float height, float zn, float zf)
{
	m->m[0][0]=2.f/width;
	m->m[1][0]=0.f;
	m->m[2][0]=0.f;
	m->m[3][0]=0.f;

	m->m[0][1]=0.f;
	m->m[1][1]=2.f/height;
	m->m[2][1]=0.f;
	m->m[3][1]=0.f;

	m->m[0][2]=0.f;
	m->m[1][2]=0.f;
	m->m[2][2]=1.f/(zf-zn);
	m->m[3][2]=zn/(zn-zf);

	m->m[0][3]=0.f;
	m->m[1][3]=0.f;
	m->m[2][3]=0.f;
	m->m[3][3]=1.f;
}

// eng: Calculation of view matrix (analogue of D3DXMatrixLookAtLH from Direct3D) 
// rus: Расчет матриц?вида (аналог D3DXMatrixLookAtLH из Direct3D)
void MatrixLookAtLH(MAGIC_MATRIX* m, MP_POSITION* peye, MP_POSITION* pat, MP_POSITION* pup)
{
	MP_POSITION right, rightn, up, upn, vec, vec2; 

	vec2=(*pat)-(*peye);
	
	vec=vec2;
	vec.Normalize();

	right.Cross(pup,&vec);
	up.Cross(&vec,&right);

	rightn=right;
	rightn.Normalize();

	upn=up;
	upn.Normalize();
    
	m->m[0][0] = rightn.x;
    m->m[1][0] = rightn.y;
    m->m[2][0] = rightn.z;
    m->m[3][0] = -peye->Dot(&rightn); 
    m->m[0][1] = upn.x;
    m->m[1][1] = upn.y;
    m->m[2][1] = upn.z;
    m->m[3][1] = -peye->Dot(&upn);
    m->m[0][2] = vec.x;
    m->m[1][2] = vec.y;
    m->m[2][2] = vec.z;
    m->m[3][2] = -peye->Dot(&vec);
    m->m[0][3] = 0.0f; 
    m->m[1][3] = 0.0f; 
    m->m[2][3] = 0.0f; 
    m->m[3][3] = 1.0f; 
}
