#pragma once

namespace DgEngine::Graphics::DebugUI
{
	enum class Theme
	{
		Classic,
		Light,
		Dark,
	};

	void StaticInitialize(HWND window, bool docking = false, bool multiViewport = false);
	void StaticTerminate();
	void SetTheme(Theme theme);

	void BeginRender();
	void EndRender();

}