#pragma once

#include "OpenGL.h"
#include "GLTexture.h"
#include "OpenglEngineHead.h"

#define	MAX_PARTICLES	500					// 定义最大的粒子数
struct	stuParticles						// 创建粒子数据结构
{
	GLboolean	active;						// 是否激活
	GLfloat		life;						// 粒子生命
	GLfloat		fade;						// 衰减速度

	GLfloat		r;							// 红色值
	GLfloat		g;							// 绿色值
	GLfloat		b;							// 蓝色值

	GLfloat		x;							// X 位置
	GLfloat		y;							// Y 位置

	GLfloat		xi;							// X 方向
	GLfloat		yi;							// Y 方向

	GLfloat		xg;							// X 方向重力加速度
	GLfloat		yg;							// Y 方向重力加速度

};											// 粒子数据结构



class OPENGL_ENGINE_CLASS CParticles
{

	GLboolean		m_Rainbow;							// 是否为彩虹模式
	GLfloat			m_Slowdown;							// 减速粒子
	GLfloat			m_XSpeed;							// X方向的速度
	GLfloat			m_YSpeed;							// Y方向的速度
	GLuint			m_Col;								// 当前的颜色
	GLuint			m_Delay;							// 彩虹效果延迟
	stuParticles	m_Particle[MAX_PARTICLES];			// 保存1000个粒子的数组

public:
	static CGLTexture	m_ImageParticle;				// 粒子


public:
	CParticles(void);
	~CParticles(void);


public:
	GLvoid			DrawParticles(CGLDevice* pOpenGL, INT nXBegan, INT nYBegan, float nXEnd, float nYEnd);
};
