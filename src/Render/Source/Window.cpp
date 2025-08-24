#include "pch.h"
#include "Header/Window.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lParam);
		Window* pointer = reinterpret_cast<Window*>(param->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pointer));
		break;
	}

	case WM_SIZE:
	{
		Window* pointer = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		uint32 width = LOWORD(lParam);
		uint32 height = HIWORD(lParam);
		
		pointer->OnSize(hwnd, (UINT)wParam, width, height);
		break;
	}
	case WM_CLOSE:
	{
		Window* pointer = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		pointer->Close();
		break;
	}
	case WM_DESTROY:
	{
		Window* pointer = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		pointer->OnDestroy();
		break;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Window::Window(HINSTANCE hInstance, uint32 width, uint32 height, const wchar_t* windowTitle)
{
	m_hInstance = hInstance;
	m_width = width;
	m_height = height;
	m_windowTitle = windowTitle;
	Initialize();
}

bool Window::Initialize()
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	
	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	m_hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		(LPCWSTR)m_windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_width,
		m_height,
		nullptr,
		nullptr,
		m_hInstance,
		this
	);

	if (m_hwnd == nullptr)
	{
		return 0;
	}

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	m_isOpen = true;
	return 1;
}

void Window::Update()
{

	MSG msg = { nullptr };

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			return;
		}
		DispatchMessage(&msg);
		return;
	}

}

void Window::Close()
{
}

void Window::OnDestroy()
{
	m_isOpen = false;
}


#pragma region GetWindowInformation

bool Window::IsOpen()
{
	return m_isOpen;
}

HINSTANCE Window::GetHInstance() const
{
	return m_hInstance;
}

uint32 Window::GetWidth() const
{
	return m_width;
}

uint32 Window::GetHeight() const
{
	return m_height;
}

HWND Window::GetHWND() const
{
	return m_hwnd;
}


#pragma endregion

void Window::OnSize(HWND hwnd, UINT wParam, uint32 width, uint32 height)
{
}