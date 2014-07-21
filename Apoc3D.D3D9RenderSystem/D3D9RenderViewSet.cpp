
#include "D3D9RenderViewSet.h"

#include "Enumeration.h"
#include "D3D9RenderDevice.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9RenderViewSet::D3D9RenderViewSet(IDirect3D9* d3d9)
				: m_currentSetting(0),m_device(0), m_ignoreSizeChanges(false),
				m_deviceLost(false),
				m_direct3D9(d3d9)
			{

			}


			D3D9RenderViewSet::~D3D9RenderViewSet(void)
			{
				if (m_currentSetting)
					delete m_currentSetting;
			}


			void D3D9RenderViewSet::PropogateSettings()
			{
				m_currentSetting->BackBufferCount = m_currentSetting->D3D9.PresentParameters.BackBufferCount;
				m_currentSetting->BackBufferWidth = m_currentSetting->D3D9.PresentParameters.BackBufferWidth;
				m_currentSetting->BackBufferHeight = m_currentSetting->D3D9.PresentParameters.BackBufferHeight;
				m_currentSetting->BackBufferFormat = m_currentSetting->D3D9.PresentParameters.BackBufferFormat;
				m_currentSetting->DepthStencilFormat = m_currentSetting->D3D9.PresentParameters.AutoDepthStencilFormat;
				m_currentSetting->DeviceType = m_currentSetting->D3D9.DeviceType;
				m_currentSetting->MultiSampleQuality = m_currentSetting->D3D9.PresentParameters.MultiSampleQuality;
				m_currentSetting->MultiSampleType = m_currentSetting->D3D9.PresentParameters.MultiSampleType;
				m_currentSetting->RefreshRate = m_currentSetting->D3D9.PresentParameters.FullScreen_RefreshRateInHz;
				m_currentSetting->Windowed = m_currentSetting->D3D9.PresentParameters.Windowed ? true : false;

			}
			bool D3D9RenderViewSet::CanDeviceBeReset(const DeviceSettings* const oldset,
				const DeviceSettings* const newset) const
			{
				if (!oldset)
					return false;

				return m_device &&
					oldset->D3D9.AdapterOrdinal == newset->D3D9.AdapterOrdinal &&
					oldset->D3D9.DeviceType == newset->D3D9.DeviceType &&
					oldset->D3D9.CreationFlags == newset->D3D9.CreationFlags;
			}

			void D3D9RenderViewSet::ChangeDevice(const DeviceSettings& settings, const DeviceSettings* minimumSettings)
			{
				if (minimumSettings)
				{
					Enumeration::setMinimumSettings(*minimumSettings);
				}
				else
				{
					Enumeration::ClearMinimumSetting();
				}
				DeviceSettings validSettings;
				Enumeration::FindValidSettings(m_direct3D9, settings, validSettings);
				validSettings.D3D9.PresentParameters.hDeviceWindow = 0;
				CreateDevice(validSettings);
			}
			void D3D9RenderViewSet::CreateDevice(const DeviceSettings& settings)
			{
				DeviceSettings* oldSettings = m_currentSetting;
				m_currentSetting = new DeviceSettings(settings);

				m_ignoreSizeChanges = true;

				// check if the device can be reset, or if we need to completely recreate it

				int64 result = 0;
				bool canReset = CanDeviceBeReset(oldSettings, m_currentSetting);
				if (canReset)
					result = ResetDevice();

				if (result == D3DERR_DEVICELOST)
					m_deviceLost = true;
				else if (!canReset || result != D3D_OK)
				{
					if (oldSettings)
						ReleaseDevice();

					InitializeDevice();
				}


				m_ignoreSizeChanges = false;

				if (oldSettings)
					delete oldSettings;
			}

			int32 D3D9RenderViewSet::GetAdapterOrdinal(HMONITOR mon)
			{
				const AdapterInfo* adapter = 0;
				const List<AdapterInfo*> adInfo = Enumeration::getAdapters();
				for (int32 i=0;i<adInfo.getCount();i++)
				{
					if (m_direct3D9->GetAdapterMonitor(adInfo[i]->AdapterOrdinal) == mon)
					{
						adapter = adInfo[i];
						break;
					}
				}

				if (adapter)
				{
					return adapter->AdapterOrdinal;
				}

				return -1;
			}

			void D3D9RenderViewSet::Control_BeginPaint(bool* cancel)
			{
				if (!m_device)
					return;

				if (m_deviceLost)
					Sleep(50);

				if (m_deviceLost)
				{
					HRESULT hr = m_device->TestCooperativeLevel();
					if (hr == D3DERR_DEVICELOST)
					{
						*cancel = true;
						return;
					}

					if (m_currentSetting->Windowed)
					{
						D3DDISPLAYMODE mode;
						HRESULT hr = m_direct3D9->GetAdapterDisplayMode(m_currentSetting->D3D9.AdapterOrdinal, &mode);
						assert(SUCCEEDED(hr));

						if (m_currentSetting->D3D9.AdapterFormat != mode.Format)
						{
							DeviceSettings newSettings = *m_currentSetting;
							ChangeDevice(newSettings, 0);
							*cancel = true;
							return;
						}
					}

					hr = ResetDevice();
					if (hr != D3D_OK)
					{
						*cancel = true;
						return;
					}
					m_deviceLost = false;
				}
			}
			void D3D9RenderViewSet::Control_EndPaint()
			{
			//	HRESULT hr = m_device->Present(NULL, NULL, NULL, NULL);
			//	if (hr == D3DERR_DEVICELOST)
			//	{
			//		m_deviceLost = true;
			//	}
			}
			void D3D9RenderViewSet::Control_UserResized()
			{
			//	if (m_ignoreSizeChanges || !EnsureDevice() ||
			//		(m_currentSetting->Windowed))
			//		return;

			//	DeviceSettings newSettings = *m_currentSetting;

			//	RECT rect;
			//	GetClientRect(m_game->getWindow()->getHandle(), &rect);

			//	int32 width = rect.right - rect.left;
			//	int32 height = rect.bottom - rect.top;

			//	if (width != newSettings.BackBufferWidth || height != newSettings.BackBufferHeight)
			//	{
			//		newSettings.BackBufferWidth = 0;
			//		newSettings.BackBufferHeight = 0;
			//		CreateDevice(newSettings);
			//	}
			}
			void D3D9RenderViewSet::Control_MonitorChanged()
			{
				if (!EnsureDevice() || !m_currentSetting->Windowed || m_ignoreSizeChanges)
					return;

				HMONITOR windowMonitor = MonitorFromWindow(0, MONITOR_DEFAULTTOPRIMARY);

				DeviceSettings newSettings = *m_currentSetting;
				int adapterOrdinal = GetAdapterOrdinal(windowMonitor);

				if (adapterOrdinal == -1)
					return;

				newSettings.D3D9.AdapterOrdinal = adapterOrdinal;

				CreateDevice(newSettings);
			}

			void D3D9RenderViewSet::InitializeDevice()
			{
				HWND sss = 0;
				HRESULT result = m_direct3D9->CreateDevice(m_currentSetting->D3D9.AdapterOrdinal,
					m_currentSetting->D3D9.DeviceType, sss ,
					m_currentSetting->D3D9.CreationFlags, &m_currentSetting->D3D9.PresentParameters, &m_device);

				if (result == D3DERR_DEVICELOST)
				{
					m_deviceLost = true;
				}
				else
				{
					assert(SUCCEEDED(result));

					if (FAILED(result))
					{
						MessageBox(0, L"Unable to create Direct3D Device", L"Error", MB_OK | MB_APPLMODAL | MB_ICONERROR);

						exit(0);
						return;
					}				
				}
				// UpdateDeviceStats();

			}

			HRESULT D3D9RenderViewSet::ResetDevice()
			{
				m_apiDevice->OnDeviceLost();

				HRESULT hr = m_device->Reset(&m_currentSetting->D3D9.PresentParameters);

				m_apiDevice->OnDeviceReset();
				return hr;
			}
			void D3D9RenderViewSet::ReleaseDevice()
			{
				if (!m_device)
					return;

				m_device->Release();
				m_device = 0;
			}
			


		}
	}
}