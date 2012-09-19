// Application2.h: interface for the Application2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATION2_H__3B7CF41C_7810_4B2A_97FA_A33970420449__INCLUDED_)
#define AFX_APPLICATION2_H__3B7CF41C_7810_4B2A_97FA_A33970420449__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Application.h"

class Application2 : public Application  
{
public:
	Application2();
	virtual ~Application2();

	virtual int Render(); // Pass user input data and call renderer
};

#endif // !defined(AFX_APPLICATION2_H__3B7CF41C_7810_4B2A_97FA_A33970420449__INCLUDED_)
