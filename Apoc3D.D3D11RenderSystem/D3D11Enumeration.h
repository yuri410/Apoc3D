#pragma once

#include "D3D11DeviceSettings.h"

namespace rex
{
	class D3D11EnumAdapterInfo;
	class D3D11EnumOutputInfo;
	class D3D11EnumDeviceInfo;
	struct D3D11EnumDeviceSettingsCombo;

	//--------------------------------------------------------------------------------------
	// Enumerates available Direct3D11 adapters, devices, modes, etc.
	//--------------------------------------------------------------------------------------
	class D3D11Enumeration
	{
	public:
		static D3D11Enumeration* GetInstance();

		// Call Enumerate() to enumerate available D3D11 adapters, devices, modes, etc.
		bool HasEnumerated() const { return m_hasEnumerated; }
		HRESULT Enumerate(IDXGIFactory1* factory);

		void SetMinimumSettings(const D3D11DeviceSettings& settings) { m_minimumSettings = settings; }
		void ClearMinimumSettings() { m_hasMinimumSettings = false; }

		// These should be called after Enumerate() is called
		os::Vector<D3D11EnumAdapterInfo*>*  GetAdapterInfoList();
		D3D11EnumAdapterInfo*               GetAdapterInfo( UINT AdapterOrdinal );
		D3D11EnumDeviceInfo*                GetDeviceInfo( UINT AdapterOrdinal, D3D_DRIVER_TYPE DeviceType );
		D3D11EnumOutputInfo*                GetOutputInfo( UINT AdapterOrdinal, UINT Output );
		D3D11EnumDeviceSettingsCombo*       GetDeviceSettingsCombo( D3D11DeviceSettings* pDeviceSettings ) { return GetDeviceSettingsCombo( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DriverType, pDeviceSettings->Output, pDeviceSettings->sd.BufferDesc.Format, pDeviceSettings->sd.Windowed ); }
		D3D11EnumDeviceSettingsCombo*       GetDeviceSettingsCombo( UINT AdapterOrdinal, D3D_DRIVER_TYPE DeviceType, UINT Output, DXGI_FORMAT BackBufferFormat, BOOL Windowed );

		HRESULT FindValidSettings(IDXGIFactory1* fac, const D3D11DeviceSettings& settings, D3D11DeviceSettings& result);

		~D3D11Enumeration();
	private:
		D3D11Enumeration();

		bool m_hasEnumerated;
		bool m_hasMinimumSettings;
		D3D11DeviceSettings m_minimumSettings;
		os::Vector<D3D11EnumAdapterInfo*> m_AdapterInfoList;

		HRESULT EnumerateOutputs(D3D11EnumAdapterInfo *pAdapterInfo);
		HRESULT EnumerateDevices(D3D11EnumAdapterInfo *pAdapterInfo);
		HRESULT EnumerateDeviceCombos(IDXGIFactory1 *pFactory, D3D11EnumAdapterInfo* pAdapterInfo);
		HRESULT EnumerateDeviceCombosNoAdapter(D3D11EnumAdapterInfo* pAdapterInfo);

		HRESULT EnumerateDisplayModes(D3D11EnumOutputInfo *pOutputInfo);
		void BuildMultiSampleQualityList(DXGI_FORMAT fmt, D3D11EnumDeviceSettingsCombo* pDeviceCombo);
		void ClearAdapterInfoList();

		float RankSettingsCombo(D3D11EnumDeviceSettingsCombo* pDeviceSettingsCombo, const D3D11DeviceSettings* pOptimalDeviceSettings,
			int &bestModeIndex, int &bestMSAAIndex);

		UINT GetDXGIColorChannelBits( DXGI_FORMAT fmt );
	};

	//--------------------------------------------------------------------------------------
	// A class describing an adapter which contains a unique adapter ordinal 
	// that is installed on the system
	//--------------------------------------------------------------------------------------
	class D3D11EnumAdapterInfo
	{
	public:
		~D3D11EnumAdapterInfo();

		UINT AdapterOrdinal;
		DXGI_ADAPTER_DESC AdapterDesc;
		String UniqueDescription;
		IDXGIAdapter *m_pAdapter;
		bool bAdapterUnavailable;

		os::Vector<D3D11EnumOutputInfo*> outputInfoList;
		os::Vector<D3D11EnumDeviceInfo*> deviceInfoList;

		// List of D3D11EnumDeviceSettingsCombo* with a unique set 
		// of BackBufferFormat, and Windowed
		os::Vector<D3D11EnumDeviceSettingsCombo*> deviceSettingsComboList;
	private:
		const D3D11EnumAdapterInfo &operator=(const D3D11EnumAdapterInfo &rhs);
	};

	class D3D11EnumOutputInfo
	{
	public:
		~D3D11EnumOutputInfo();

		UINT AdapterOrdinal;
		UINT Output;
		IDXGIOutput* m_pOutput;
		DXGI_OUTPUT_DESC Desc;

		os::Vector<DXGI_MODE_DESC> displayModeList;
	private:
		const D3D11EnumOutputInfo &operator=(const D3D11EnumOutputInfo &rhs);
	};

	class D3D11EnumDeviceInfo
	{
	public:
		~D3D11EnumDeviceInfo();

		UINT AdapterOrdinal;
		D3D_DRIVER_TYPE DeviceType;
		D3D_FEATURE_LEVEL SelectedLevel;
		D3D_FEATURE_LEVEL MaxLevel;
		BOOL ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;
	private:
		const D3D11EnumDeviceInfo& operator=(const D3D11EnumDeviceInfo& rhs);
	};

	struct D3D11EnumDeviceSettingsCombo
	{
		UINT AdapterOrdinal;
		D3D_DRIVER_TYPE DeviceType;
		DXGI_FORMAT BackBufferFormat;
		BOOL Windowed;
		UINT Output;

		os::Vector<UINT> multiSampleCountList;
		os::Vector<UINT> multiSampleQualityList;

		D3D11EnumAdapterInfo* pAdapterInfo;
		D3D11EnumDeviceInfo* pDeviceInfo;
		D3D11EnumOutputInfo* pOutputInfo;
	};
}