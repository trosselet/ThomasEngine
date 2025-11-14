#ifndef WINDOW_INCLUDE__H
#define WINDOW_INCLUDE__H

class GraphicEngine;

class Window
{
public:
	~Window();

	Window(HINSTANCE hInstance=nullptr, uint32 width=900, uint32 height=600, const wchar_t* windowTitle=L"Base Window");

	bool IsOpen();

	HINSTANCE GetHInstance() const;
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	HWND GetHWND() const;
	GraphicEngine* GetGraphicEngine() const;

	void OnSize(uint32 width, uint32 height);

	void Update();
	void Close();
	void OnDestroy();

private:
	bool Initialize();

private:
	HINSTANCE m_hInstance = nullptr;
	uint32 m_width = 0;
	uint32 m_height = 0;
	const wchar_t* m_windowTitle = nullptr;
	HWND m_hwnd = nullptr;

	bool m_isOpen = false;

private:
	GraphicEngine* m_pGraphic = nullptr;

};


#endif // !WINDOW_INCLUDE__H

