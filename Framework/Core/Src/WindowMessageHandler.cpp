#include"Precompiled.h"
#include"WindowMessageHandler.h"
#include"DebugUtil.h"

using namespace DgEngine;
using namespace DgEngine::Core;

void WindowMessageHandler::Hook(HWND window, Callback cb)
{
	mWindow=window;
	mPreviousCallback=(Callback)GetWindowLongPtrA(window, GWLP_WNDPROC);
	SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)cb);
}

void WindowMessageHandler::Unhook()
{
	SetWindowLongPtrA(mWindow, GWLP_WNDPROC, (LONG_PTR)mPreviousCallback);
	mWindow=nullptr;
}

LRESULT WindowMessageHandler::ForwardMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT(mPreviousCallback!=nullptr, "wINDOWMessageHandler: no callbak=ck is hooked");
	return CallWindowProcA((WNDPROC)mPreviousCallback, window, message, wParam, lParam);
}
