/**
 * Test application class for homework assignment #1
 * USC csci 580 *
*/

// Application1.h: interface for the Application1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATION1_H__C6586E06_8B31_44F6_84BB_11344FED0C14__INCLUDED_)
#define AFX_APPLICATION1_H__C6586E06_8B31_44F6_84BB_11344FED0C14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Application.h"

class Application1 : public Application
{
public:
	Application1();
	virtual ~Application1();

	virtual int Render(); // Pass user input data and call renderer
}; 

#endif // !defined(AFX_APPLICATION1_H__C6586E06_8B31_44F6_84BB_11344FED0C14__INCLUDED_)
