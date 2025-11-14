#include <Render/pch.h>
#include <Render/Header/Window.h>

#include <Render/Header/GraphicEngine.h>
#include <Render/Header/Render.h>
#include <Render/Header/RenderResources.h>

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

		if (width < 8)
		{
			width = 8;
		}
		if (height < 8)
		{
			height = 8;
		}

		if (!pointer->IsOpen())
			break;
		
		pointer->OnSize(width, height);
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

Window::~Window()
{
	if (m_pGraphic)
	{
		delete m_pGraphic;
	}
}

Window::Window(HINSTANCE hInstance, uint32 width, uint32 height, LPCSTR windowTitle)
{
	m_hInstance = hInstance;
	m_width = width;
	m_height = height;
	m_windowTitle = windowTitle;
	Initialize();
	m_pGraphic = new GraphicEngine(this);	
}

bool Window::Initialize()
{
	
	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = "Sample Window Class";

	RegisterClass(&wc);

	m_hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		m_windowTitle,
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

bool Window::IsOpen() const
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

GraphicEngine* Window::GetGraphicEngine() const
{
	return m_pGraphic;
}


#pragma endregion

void Window::OnSize(uint32 width, uint32 height)
{
	m_pGraphic->GetRender()->GetRenderResources()->Resize(width, height);
}