#include "D3D11Enumeration.h"
#include "Exception.h"

using namespace rex;
using namespace os;

D3D11Enumeration* D3D11Enumeration::GetInstance()
{
	static D3D11Enumeration instance;
	return &instance;
}

D3D11Enumeration::D3D11Enumeration()
	: m_hasEnumerated(false), m_hasMinimumSettings(false)
{

}
D3D11Enumeration::~D3D11Enumeration()
{
	ClearAdapterInfoList();
}

HRESULT D3D11Enumeration::FindValidSettings(IDXGIFactory1* fac, const D3D11DeviceSettings& preferredSettings, D3D11DeviceSettings& result)
{
	Enumerate(fac);

	D3D11EnumAdapterInfo* pAdapterInfo = NULL; 
	os::Vector<D3D11EnumAdapterInfo*>* pAdapterList = GetAdapterInfoList();
	D3D11EnumAdapterInfo* tempAdapterInfo = pAdapterList->operator[](0);

	for(uint iAdapter = 0; iAdapter < pAdapterList->Size(); iAdapter++)
	{
		tempAdapterInfo = pAdapterList->operator[](iAdapter);
		if (tempAdapterInfo->AdapterOrdinal == preferredSettings.AdapterOrdinal)
			pAdapterInfo = tempAdapterInfo;
	}

	if (pAdapterInfo == NULL) return E_FAIL; // no adapters found.
	D3D11EnumDeviceSettingsCombo* pDeviceSettingsCombo = NULL;
	float biggestScore = 0;

	int bestModeIndex=0;
	int bestMSAAIndex=0;

	int combo = 0;
	for(uint iDeviceCombo = 0; iDeviceCombo < pAdapterInfo->deviceSettingsComboList.Size(); iDeviceCombo++)
	{
		D3D11EnumDeviceSettingsCombo* tempDeviceSettingsCombo = pAdapterInfo->deviceSettingsComboList[iDeviceCombo];

		int bestMode;
		int bestMSAA;
		float score = RankSettingsCombo(tempDeviceSettingsCombo, &preferredSettings, bestMode, bestMSAA );
		if (score > biggestScore) 
		{
			combo = iDeviceCombo;
			biggestScore = score;
			pDeviceSettingsCombo = tempDeviceSettingsCombo;
			bestModeIndex = bestMode;
			bestMSAAIndex = bestMSAA;
		}
	}
	if (NULL == pDeviceSettingsCombo ) 
	{
		return E_FAIL; // no settigns found.
	}

	result = preferredSettings;
	result.AdapterOrdinal = pDeviceSettingsCombo->AdapterOrdinal;
	result.DriverType = pDeviceSettingsCombo->DeviceType;
	result.Output = pDeviceSettingsCombo->Output;

	result.sd.Windowed = pDeviceSettingsCombo->Windowed;
	if( GetSystemMetrics(0x1000) != 0 ) {// SM_REMOTESESSION
		result.sd.Windowed = 1;
	}   
	if (pDeviceSettingsCombo->pOutputInfo != NULL) {
		DXGI_MODE_DESC bestDisplayMode;
		bestDisplayMode = pDeviceSettingsCombo->pOutputInfo->displayModeList[bestModeIndex];
		if (!pDeviceSettingsCombo->Windowed) {

			result.sd.BufferDesc.Height = bestDisplayMode.Height;
			result.sd.BufferDesc.Width = bestDisplayMode.Width;
			result.sd.BufferDesc.RefreshRate.Numerator = bestDisplayMode.RefreshRate.Numerator;
			result.sd.BufferDesc.RefreshRate.Denominator = bestDisplayMode.RefreshRate.Denominator;
			result.sd.BufferDesc.Scaling = bestDisplayMode.Scaling;
			result.sd.BufferDesc.ScanlineOrdering = bestDisplayMode.ScanlineOrdering;
		}
	}
	if (result.DeviceFeatureLevel == 0)
		result.DeviceFeatureLevel = pDeviceSettingsCombo->pDeviceInfo->SelectedLevel;

	result.sd.SampleDesc.Count = pDeviceSettingsCombo->multiSampleCountList[bestMSAAIndex];
	if (result.sd.SampleDesc.Quality > pDeviceSettingsCombo->multiSampleQualityList[bestMSAAIndex] - 1)
		result.sd.SampleDesc.Quality = pDeviceSettingsCombo->multiSampleQualityList[bestMSAAIndex] - 1;

	result.sd.BufferDesc.Format = pDeviceSettingsCombo->BackBufferFormat;

	return S_OK;
}

float D3D11Enumeration::RankSettingsCombo(D3D11EnumDeviceSettingsCombo* pDeviceSettingsCombo, const D3D11DeviceSettings* pOptimalDeviceSettings,
						int &bestModeIndex, int &bestMSAAIndex)
{
	float fCurRanking = 0.0f;

	// Arbitrary weights.  Gives preference to the ordinal, device type, and windowed
	const float fAdapterOrdinalWeight   = 1000.0f;
	const float fAdapterOutputWeight    = 500.0f;
	const float fDeviceTypeWeight       = 100.0f;
	const float fWARPOverRefWeight       = 80.0f;

	const float fWindowWeight           = 10.0f;
	const float fResolutionWeight       = 1.0f;
	const float fBackBufferFormatWeight = 1.0f;
	const float fMultiSampleWeight      = 1.0f;
	const float fRefreshRateWeight      = 1.0f;

	//---------------------
	// Adapter ordinal
	//---------------------
	if( pDeviceSettingsCombo->AdapterOrdinal == pOptimalDeviceSettings->AdapterOrdinal )
		fCurRanking += fAdapterOrdinalWeight;

	//---------------------
	// Adapter ordinal
	//---------------------
	if( pDeviceSettingsCombo->Output == pOptimalDeviceSettings->Output )
		fCurRanking += fAdapterOutputWeight;

	//---------------------
	// Device type
	//---------------------
	if( pDeviceSettingsCombo->DeviceType == pOptimalDeviceSettings->DriverType )
		fCurRanking += fDeviceTypeWeight;
	else if (pDeviceSettingsCombo->DeviceType == D3D_DRIVER_TYPE_WARP && pOptimalDeviceSettings->DriverType == D3D_DRIVER_TYPE_HARDWARE) {
		fCurRanking += fWARPOverRefWeight;
	}

	// Slightly prefer HAL 
	if( pDeviceSettingsCombo->DeviceType == D3D_DRIVER_TYPE_HARDWARE )
		fCurRanking += 0.1f;

	//---------------------
	// Windowed
	//---------------------
	if( pDeviceSettingsCombo->Windowed == pOptimalDeviceSettings->sd.Windowed )
		fCurRanking += fWindowWeight;

	//---------------------
	// Resolution
	//---------------------
	bool bResolutionFound = false;
	unsigned int best = 0xffffffff;
	bestModeIndex=0;
	for(uint idm = 0; pDeviceSettingsCombo->pOutputInfo != NULL && idm < pDeviceSettingsCombo->pOutputInfo->displayModeList.Size() && !bResolutionFound; idm++)
	{
		DXGI_MODE_DESC displayMode = pDeviceSettingsCombo->pOutputInfo->displayModeList[ idm ];
		if( displayMode.Width == pOptimalDeviceSettings->sd.BufferDesc.Width &&
			displayMode.Height == pOptimalDeviceSettings->sd.BufferDesc.Height )
			bResolutionFound = true;

		unsigned int current = 
			(UINT) abs ((int)displayMode.Width  - (int)pOptimalDeviceSettings->sd.BufferDesc.Width) + 
			(UINT) abs ((int)displayMode.Height - (int)pOptimalDeviceSettings->sd.BufferDesc.Height );

		if (current < best) {
			best = current;
			bestModeIndex= idm;

		}

	}
	if( bResolutionFound )
		fCurRanking += fResolutionWeight;

	//---------------------
	// Back buffer format
	//---------------------
	if( pDeviceSettingsCombo->BackBufferFormat == pOptimalDeviceSettings->sd.BufferDesc.Format )
	{
		fCurRanking += fBackBufferFormatWeight;
	}
	else
	{
		int nBitDepthDelta = abs( ( long )GetDXGIColorChannelBits( pDeviceSettingsCombo->BackBufferFormat ) -
			( long )GetDXGIColorChannelBits(
			pOptimalDeviceSettings->sd.BufferDesc.Format ) );
		float fScale = __max( 0.9f - ( float )nBitDepthDelta * 0.2f, 0.0f );
		fCurRanking += fScale * fBackBufferFormatWeight;
	}

	//---------------------
	// Back buffer count
	//---------------------
	// No caps for the back buffer count

	//---------------------
	// Multisample
	//---------------------
	bool bMultiSampleFound = false;
	bestMSAAIndex = 0;
	for( uint i = 0; i < pDeviceSettingsCombo->multiSampleCountList.Size(); i++ )
	{
		UINT Count = pDeviceSettingsCombo->multiSampleCountList[i];

		if( Count == pOptimalDeviceSettings->sd.SampleDesc.Count  )
		{
			bestMSAAIndex = i;
			bMultiSampleFound = true;
			break;
		}
	}
	if( bMultiSampleFound )
		fCurRanking += fMultiSampleWeight;

	//---------------------
	// Swap effect
	//---------------------
	// No caps for swap effects

	//---------------------
	// Depth stencil 
	//---------------------
	// No caps for swap effects

	//---------------------
	// Present flags
	//---------------------
	// No caps for the present flags

	//---------------------
	// Refresh rate
	//---------------------
	bool bRefreshFound = false;
	for( uint idm = 0; pDeviceSettingsCombo->pOutputInfo != NULL && idm < pDeviceSettingsCombo->pOutputInfo->displayModeList.Size(); idm++ )
	{
		DXGI_MODE_DESC displayMode = pDeviceSettingsCombo->pOutputInfo->displayModeList[idm];
		if( fabs( float( displayMode.RefreshRate.Numerator ) / displayMode.RefreshRate.Denominator -
			float( pOptimalDeviceSettings->sd.BufferDesc.RefreshRate.Numerator ) /
			pOptimalDeviceSettings->sd.BufferDesc.RefreshRate.Denominator ) < 0.1f )
			bRefreshFound = true;
	}
	if( bRefreshFound )
		fCurRanking += fRefreshRateWeight;

	//---------------------
	// Present interval
	//---------------------
	// No caps for the present flags

	return fCurRanking;
}

UINT D3D11Enumeration::GetDXGIColorChannelBits( DXGI_FORMAT fmt )
{
	switch( fmt )
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 32;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
		return 16;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
		return 10;

	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return 8;

	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 5;

	default:
		return 0;
	}
}

HRESULT D3D11Enumeration::Enumerate(IDXGIFactory1* pFactory)
{
	if (m_hasEnumerated)
	{
		ClearAdapterInfoList();
		m_hasEnumerated = false;
	}

	HRESULT hr;
	m_hasEnumerated = true;

	ClearAdapterInfoList();

	for( int index = 0; ; ++index )
	{
		IDXGIAdapter* pAdapter = NULL;
		hr = pFactory->EnumAdapters( index, &pAdapter );
		if( FAILED( hr ) ) // DXGIERR_NOT_FOUND is expected when the end of the list is hit
			break;

		D3D11EnumAdapterInfo* pAdapterInfo = new D3D11EnumAdapterInfo;
		if( !pAdapterInfo )
		{
			SAFE_RELEASE( pAdapter );
			return E_OUTOFMEMORY;
		}
		ZeroMemory( pAdapterInfo, sizeof( D3D11EnumAdapterInfo ) );
		pAdapterInfo->AdapterOrdinal = index;
		pAdapter->GetDesc( &pAdapterInfo->AdapterDesc );
		pAdapterInfo->m_pAdapter = pAdapter;

		// Enumerate the device driver types on the adapter.
		hr = EnumerateDevices( pAdapterInfo );
		if( FAILED( hr ) )
		{
			delete pAdapterInfo;
			continue;
		}

		hr = EnumerateOutputs( pAdapterInfo );
		if( FAILED( hr ) || pAdapterInfo->outputInfoList.Size() <= 0 )
		{
			delete pAdapterInfo;
			continue;
		}

		// Get info for each devicecombo on this device
		if( FAILED( hr = EnumerateDeviceCombos( pFactory, pAdapterInfo ) ) )
		{
			delete pAdapterInfo;
			continue;
		}

		m_AdapterInfoList.PushBack( pAdapterInfo );
	}


	//  If we did not get an adapter then we should still enumerate WARP and Ref.
	if (m_AdapterInfoList.Size() == 0) 
	{
		D3D11EnumAdapterInfo* pAdapterInfo = new D3D11EnumAdapterInfo;
		if( !pAdapterInfo )
		{
			return E_OUTOFMEMORY;
		}
		ZeroMemory( pAdapterInfo, sizeof( D3D11EnumAdapterInfo ) );
		pAdapterInfo->bAdapterUnavailable = true;

		hr = EnumerateDevices( pAdapterInfo );

		// Get info for each devicecombo on this device
		if( FAILED( hr = EnumerateDeviceCombosNoAdapter(pAdapterInfo ) ) )
		{
			delete pAdapterInfo;
		}

		m_AdapterInfoList.PushBack( pAdapterInfo );
	}

	//
	// Check for 2 or more adapters with the same name. Append the name
	// with some instance number if that's the case to help distinguish
	// them.
	//
	bool bUniqueDesc = true;
	D3D11EnumAdapterInfo* pAdapterInfo;
	for( uint32 i = 0; i < m_AdapterInfoList.Size(); i++ )
	{
		D3D11EnumAdapterInfo* pAdapterInfo1 = m_AdapterInfoList[i];

		for( uint32 j = i + 1; j < m_AdapterInfoList.Size(); j++ )
		{
			D3D11EnumAdapterInfo* pAdapterInfo2 = m_AdapterInfoList[j];
			if( wcsncmp( pAdapterInfo1->AdapterDesc.Description,
				pAdapterInfo2->AdapterDesc.Description, sizeof(pAdapterInfo1->AdapterDesc.Description)/sizeof(WCHAR) ) == 0 )
			{
				bUniqueDesc = false;
				break;
			}
		}

		if( !bUniqueDesc )
			break;
	}

	for( uint32 i = 0; i < m_AdapterInfoList.Size(); i++ )
	{
		pAdapterInfo = m_AdapterInfoList[i];
		pAdapterInfo->UniqueDescription = pAdapterInfo->AdapterDesc.Description;

		if( !bUniqueDesc )
		{
			pAdapterInfo->UniqueDescription.append(L" (");
			pAdapterInfo->UniqueDescription.append(StringUtils::ToString(pAdapterInfo->AdapterOrdinal));
			pAdapterInfo->UniqueDescription.append(L")");
		}
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT D3D11Enumeration::EnumerateOutputs(D3D11EnumAdapterInfo* pAdapterInfo)
{
	HRESULT hr;
	IDXGIOutput* pOutput;

	for( int iOutput = 0; ; ++iOutput )
	{
		pOutput = NULL;
		hr = pAdapterInfo->m_pAdapter->EnumOutputs( iOutput, &pOutput );
		if( DXGI_ERROR_NOT_FOUND == hr )
		{
			return S_OK;
		}
		else if( FAILED( hr ) )
		{
			return hr;	//Something bad happened.
		}
		else //Success!
		{
			D3D11EnumOutputInfo* pOutputInfo = new D3D11EnumOutputInfo;
			if( !pOutputInfo )
			{
				SAFE_RELEASE( pOutput );
				return E_OUTOFMEMORY;
			}
			ZeroMemory( pOutputInfo, sizeof( D3D11EnumOutputInfo ) );
			pOutput->GetDesc( &pOutputInfo->Desc );
			pOutputInfo->Output = iOutput;
			pOutputInfo->m_pOutput = pOutput;

			EnumerateDisplayModes( pOutputInfo );
			if( pOutputInfo->displayModeList.Size() <= 0 )
			{
				// If this output has no valid display mode, do not save it.
				delete pOutputInfo;
				continue;
			}

			pAdapterInfo->outputInfoList.PushBack(pOutputInfo);
		}
	}
}

//--------------------------------------------------------------------------------------
HRESULT D3D11Enumeration::EnumerateDisplayModes( D3D11EnumOutputInfo* pOutputInfo )
{
	float refreshRateOfMinimum;
	if (m_minimumSettings.sd.BufferDesc.RefreshRate.Denominator == 0)
		refreshRateOfMinimum = 0;
	else 
		refreshRateOfMinimum = (float)m_minimumSettings.sd.BufferDesc.RefreshRate.Numerator / m_minimumSettings.sd.BufferDesc.RefreshRate.Denominator;

	HRESULT hr = S_OK;
	DXGI_FORMAT allowedAdapterFormatArray[] =
	{
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,     //This is DXUT's preferred mode

		DXGI_FORMAT_R8G8B8A8_UNORM,			
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R10G10B10A2_UNORM
	};
	int allowedAdapterFormatArrayCount = sizeof( allowedAdapterFormatArray ) / sizeof( allowedAdapterFormatArray[0] );

	// Swap perferred modes for apps running in linear space
	DXGI_FORMAT RemoteMode = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	for( int f = 0; f < allowedAdapterFormatArrayCount; ++f )
	{
		// Fast-path: Try to grab at least 512 modes.
		//			  This is to avoid calling GetDisplayModeList more times than necessary.
		//			  GetDisplayModeList is an expensive call.
		UINT NumModes = 512;
		DXGI_MODE_DESC* pDesc = new DXGI_MODE_DESC[ NumModes ];
		assert( pDesc );
		if( !pDesc )
			return E_OUTOFMEMORY;

		hr = pOutputInfo->m_pOutput->GetDisplayModeList( allowedAdapterFormatArray[f],
			DXGI_ENUM_MODES_SCALING,
			&NumModes,
			pDesc );
		if( DXGI_ERROR_NOT_FOUND == hr )
		{
			SAFE_DELETE_ARRAY( pDesc );
			NumModes = 0;
			break;
		}
		else if( MAKE_DXGI_HRESULT( 34 ) == hr && RemoteMode == allowedAdapterFormatArray[f] )
		{
			// DXGI cannot enumerate display modes over a remote session.  Therefore, create a fake display
			// mode for the current screen resolution for the remote session.
			if( 0 != GetSystemMetrics( 0x1000 ) ) // SM_REMOTESESSION
			{
				DEVMODE DevMode;
				DevMode.dmSize = sizeof( DEVMODE );
				if( EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &DevMode ) )
				{
					NumModes = 1;
					pDesc[0].Width = DevMode.dmPelsWidth;
					pDesc[0].Height = DevMode.dmPelsHeight;
					pDesc[0].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					pDesc[0].RefreshRate.Numerator = 60;
					pDesc[0].RefreshRate.Denominator = 1;
					pDesc[0].ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
					pDesc[0].Scaling = DXGI_MODE_SCALING_CENTERED;
					hr = S_OK;
				}
			}
		}
		else if( DXGI_ERROR_MORE_DATA == hr )
		{
			// Slow path.  There were more than 512 modes.
			SAFE_DELETE_ARRAY( pDesc );
			hr = pOutputInfo->m_pOutput->GetDisplayModeList( allowedAdapterFormatArray[f],
				DXGI_ENUM_MODES_SCALING,
				&NumModes,
				NULL );
			if( FAILED( hr ) )
			{
				NumModes = 0;
				break;
			}

			pDesc = new DXGI_MODE_DESC[ NumModes ];
			assert( pDesc );
			if( !pDesc )
				return E_OUTOFMEMORY;

			hr = pOutputInfo->m_pOutput->GetDisplayModeList( allowedAdapterFormatArray[f],
				DXGI_ENUM_MODES_SCALING,
				&NumModes,
				pDesc );
			if( FAILED( hr ) )
			{
				SAFE_DELETE_ARRAY( pDesc );
				NumModes = 0;
				break;
			}

		}

		if( 0 == NumModes && 0 == f )
		{
			// No R8G8B8A8_UNORM_SRGB modes!
			// Abort the fast-path if we're on it
			allowedAdapterFormatArrayCount = sizeof( allowedAdapterFormatArray ) / sizeof
				( allowedAdapterFormatArray[0] );
			SAFE_DELETE_ARRAY( pDesc );
			continue;
		}

		if( SUCCEEDED( hr ) )
		{
			for( UINT m = 0; m < NumModes; m++ )
			{
				const DXGI_MODE_DESC& mode = pDesc[m];

				float refreshRateOfMode = mode.RefreshRate.Denominator == 0 ? 0 : ((float)mode.RefreshRate.Numerator / mode.RefreshRate.Denominator);
				
				// check minimum settings
				if ((int)mode.Width < m_minimumSettings.sd.BufferDesc.Width ||
					(int)mode.Height < m_minimumSettings.sd.BufferDesc.Height ||
					(int)refreshRateOfMode < refreshRateOfMinimum)
				{
					continue;
				}

				pOutputInfo->displayModeList.PushBack(pDesc[m]);
			}
		}

		SAFE_DELETE_ARRAY( pDesc );
	}

	return hr;
}

//--------------------------------------------------------------------------------------
HRESULT D3D11Enumeration::EnumerateDevices(D3D11EnumAdapterInfo* pAdapterInfo)
{
	HRESULT hr;
	//D3D11DeviceSettings deviceSettings = D3D11DeviceSettings();
	const D3D_DRIVER_TYPE devTypeArray[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT devTypeArrayCount = sizeof( devTypeArray ) / sizeof( devTypeArray[0] );

	if (m_hasMinimumSettings)
	{
		for( UINT iDeviceType = 0; iDeviceType < devTypeArrayCount; iDeviceType++ )
		{
			if (m_minimumSettings.DriverType == devTypeArray[iDeviceType])
			{
				devTypeArrayCount = iDeviceType + 1;
				break;
			}
		}
	}

	// Enumerate each Direct3D device type
	for( UINT iDeviceType = 0; iDeviceType < devTypeArrayCount; iDeviceType++ )
	{
		D3D11EnumDeviceInfo* pDeviceInfo = new D3D11EnumDeviceInfo;
		if( pDeviceInfo == NULL )
			return E_OUTOFMEMORY;

		// Fill struct w/ AdapterOrdinal and D3D_DRIVER_TYPE
		pDeviceInfo->AdapterOrdinal = pAdapterInfo->AdapterOrdinal;
		pDeviceInfo->DeviceType = devTypeArray[iDeviceType];

		D3D_FEATURE_LEVEL FeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		UINT NumFeatureLevels = ARRAYSIZE( FeatureLevels );

		// Call D3D11CreateDevice to ensure that this is a D3D11 device.
		ID3D11Device* pd3dDevice = NULL;
		ID3D11DeviceContext* pd3dDeviceContext = NULL;
		IDXGIAdapter* pAdapter = NULL;
		//if( devTypeArray[iDeviceType] == D3D_DRIVER_TYPE_HARDWARE )
		//    pAdapter = pAdapterInfo->m_pAdapter;
		hr = D3D11CreateDevice( pAdapter,
			devTypeArray[iDeviceType],
			( HMODULE )0,
			0,
			FeatureLevels,
			NumFeatureLevels,
			D3D11_SDK_VERSION,
			&pd3dDevice,
			&pDeviceInfo->MaxLevel,
			&pd3dDeviceContext );

		if (m_hasMinimumSettings)
		{
			if( FAILED( hr ) || pDeviceInfo->MaxLevel < m_minimumSettings.DeviceFeatureLevel)
			{
				delete pDeviceInfo;
				continue;
			}
		}
		if( FAILED( hr ))
		{
			delete pDeviceInfo;
			continue;
		}

		pDeviceInfo->SelectedLevel = pDeviceInfo->MaxLevel;
		
		IDXGIDevice1* pDXGIDev = NULL;
		hr = pd3dDevice->QueryInterface( __uuidof( IDXGIDevice1 ), ( LPVOID* )&pDXGIDev );
		if( SUCCEEDED( hr ) && pDXGIDev )
		{
			SAFE_RELEASE( pAdapterInfo->m_pAdapter );
			pDXGIDev->GetAdapter( &pAdapterInfo->m_pAdapter );
		}
		SAFE_RELEASE( pDXGIDev );


		D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS ho;
		pd3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &ho, sizeof(ho));
		pDeviceInfo->ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x = ho.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x; 
		SAFE_RELEASE( pd3dDeviceContext );             
		SAFE_RELEASE( pd3dDevice );
		pAdapterInfo->deviceInfoList.PushBack( pDeviceInfo );
	}

	return S_OK;
}

HRESULT D3D11Enumeration::EnumerateDeviceCombosNoAdapter(D3D11EnumAdapterInfo* pAdapterInfo)
{
	// Iterate through each combination of device driver type, output,
	// adapter format, and backbuffer format to build the adapter's device combo list.
	//

	for( uint32 device = 0; device < pAdapterInfo->deviceInfoList.Size(); ++device )
	{
		D3D11EnumDeviceInfo* pDeviceInfo = pAdapterInfo->deviceInfoList[device];

		DXGI_FORMAT BufferFormatArray[] =
		{
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,   //This is DXUT's preferred mode

			DXGI_FORMAT_R8G8B8A8_UNORM,		
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			DXGI_FORMAT_R10G10B10A2_UNORM
		};
		const UINT BufferFormatArrayCount = sizeof( BufferFormatArray ) / sizeof
			( BufferFormatArray[0] );
				
		for( UINT iBufferFormat = 0; iBufferFormat < BufferFormatArrayCount; iBufferFormat++ )
		{
			DXGI_FORMAT BufferFormat = BufferFormatArray[iBufferFormat];

			// At this point, we have an adapter/device/backbufferformat/iswindowed
			// DeviceCombo that is supported by the system. We still 
			// need to find one or more suitable depth/stencil buffer format,
			// multisample type, and present interval.
			D3D11EnumDeviceSettingsCombo* pDeviceCombo = new D3D11EnumDeviceSettingsCombo;
			if( pDeviceCombo == NULL )
				return E_OUTOFMEMORY;

			pDeviceCombo->AdapterOrdinal = pDeviceInfo->AdapterOrdinal;
			pDeviceCombo->DeviceType = pDeviceInfo->DeviceType;
			pDeviceCombo->BackBufferFormat = BufferFormat;
			pDeviceCombo->Windowed = TRUE;
			pDeviceCombo->Output = 0;
			pDeviceCombo->pAdapterInfo = pAdapterInfo;
			pDeviceCombo->pDeviceInfo = pDeviceInfo;
			pDeviceCombo->pOutputInfo = NULL;

			BuildMultiSampleQualityList( BufferFormat, pDeviceCombo );

			pAdapterInfo->deviceSettingsComboList.PushBack( pDeviceCombo );
		}
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT D3D11Enumeration::EnumerateDeviceCombos( IDXGIFactory1* pFactory, D3D11EnumAdapterInfo* pAdapterInfo )
{
	// Iterate through each combination of device driver type, output,
	// adapter format, and backbuffer format to build the adapter's device combo list.
	//

	for( uint32 output = 0; output < pAdapterInfo->outputInfoList.Size(); ++output )
	{
		D3D11EnumOutputInfo* pOutputInfo = pAdapterInfo->outputInfoList[output];

		for( uint32 device = 0; device < pAdapterInfo->deviceInfoList.Size(); ++device )
		{
			D3D11EnumDeviceInfo* pDeviceInfo = pAdapterInfo->deviceInfoList[device];

			DXGI_FORMAT backBufferFormatArray[] =
			{
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,   //This is DXUT's preferred mode

				DXGI_FORMAT_R8G8B8A8_UNORM,		
				DXGI_FORMAT_R16G16B16A16_FLOAT,
				DXGI_FORMAT_R10G10B10A2_UNORM
			};
			const UINT backBufferFormatArrayCount = sizeof( backBufferFormatArray ) / sizeof
				( backBufferFormatArray[0] );

			for( UINT iBackBufferFormat = 0; iBackBufferFormat < backBufferFormatArrayCount; iBackBufferFormat++ )
			{
				if (m_hasMinimumSettings)
				{
					if (m_minimumSettings.sd.BufferDesc.Format != DXGI_FORMAT_UNKNOWN &&
						m_minimumSettings.sd.BufferDesc.Format != backBufferFormatArray[iBackBufferFormat])
					{
						continue;
					}
				}

				DXGI_FORMAT backBufferFormat = backBufferFormatArray[iBackBufferFormat];

				for( int nWindowed = 0; nWindowed < 2; nWindowed++ )
				{
					if( !nWindowed && pOutputInfo->displayModeList.Size() == 0 )
						continue;

					// determine if there are any modes for this particular format
					UINT iModes = 0;
					for( uint32 i = 0; i < pOutputInfo->displayModeList.Size(); i++ )
					{
						if( backBufferFormat == pOutputInfo->displayModeList[i].Format )
							iModes ++;
					}
					if( 0 == iModes )
						continue;

					// At this point, we have an adapter/device/backbufferformat/iswindowed
					// DeviceCombo that is supported by the system. We still 
					// need to find one or more suitable depth/stencil buffer format,
					// multisample type, and present interval.
					D3D11EnumDeviceSettingsCombo* pDeviceCombo = new D3D11EnumDeviceSettingsCombo;
					if( pDeviceCombo == NULL )
						return E_OUTOFMEMORY;

					pDeviceCombo->AdapterOrdinal = pDeviceInfo->AdapterOrdinal;
					pDeviceCombo->DeviceType = pDeviceInfo->DeviceType;
					pDeviceCombo->BackBufferFormat = backBufferFormat;
					pDeviceCombo->Windowed = ( nWindowed != 0 );
					pDeviceCombo->Output = pOutputInfo->Output;
					pDeviceCombo->pAdapterInfo = pAdapterInfo;
					pDeviceCombo->pDeviceInfo = pDeviceInfo;
					pDeviceCombo->pOutputInfo = pOutputInfo;

					BuildMultiSampleQualityList( backBufferFormat, pDeviceCombo );

					pAdapterInfo->deviceSettingsComboList.PushBack(pDeviceCombo);
				}
			}
		}
	}

	return S_OK;
}


void D3D11Enumeration::ClearAdapterInfoList()
{
	D3D11EnumAdapterInfo* pAdapterInfo;
	for( uint32 i = 0; i < m_AdapterInfoList.Size(); i++ )
	{
		pAdapterInfo = m_AdapterInfoList[i];
		delete pAdapterInfo;
	}

	m_AdapterInfoList.Clear();
}

//--------------------------------------------------------------------------------------
void D3D11Enumeration::BuildMultiSampleQualityList( DXGI_FORMAT fmt, D3D11EnumDeviceSettingsCombo* pDeviceCombo )
{
	ID3D11Device* pd3dDevice = NULL;
	ID3D11DeviceContext* pd3dDeviceContext = NULL;
	IDXGIAdapter* pAdapter = NULL;

	//if( pDeviceCombo->DeviceType == D3D_DRIVER_TYPE_HARDWARE )
	//    DXUTGetDXGIFactory()->EnumAdapters( pDeviceCombo->pAdapterInfo->AdapterOrdinal, &pAdapter );

	//DXGI_ADAPTER_DESC dad;
	//pAdapter->GetDesc(&dad);

	D3D_FEATURE_LEVEL *FeatureLevels = &(pDeviceCombo->pDeviceInfo->SelectedLevel);
	D3D_FEATURE_LEVEL returnedFeatureLevel;

	UINT NumFeatureLevels = 1;

	HRESULT hr = D3D11CreateDevice( pAdapter, 
		pDeviceCombo->DeviceType,
		( HMODULE )0,
		0,
		FeatureLevels,
		NumFeatureLevels,
		D3D11_SDK_VERSION,
		&pd3dDevice,
		&returnedFeatureLevel,
		&pd3dDeviceContext )  ;

	if( FAILED( hr))  return;

	if (returnedFeatureLevel != pDeviceCombo->pDeviceInfo->SelectedLevel) return;

	for( UINT i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++i )
	{
		UINT Quality;
		if( SUCCEEDED( pd3dDevice->CheckMultisampleQualityLevels( fmt, i, &Quality ) ) && Quality > 0 )
		{
			//From D3D10 docs: When multisampling a texture, the number of quality levels available for an adapter is dependent on the texture 
			//format used and the number of samples requested. The maximum sample count is defined by 
			//D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT in d3d10.h. If the returned value of pNumQualityLevels is 0, 
			//the format and sample count combination is not supported for the installed adapter.

			if (m_hasMinimumSettings)
			{
				if (i < m_minimumSettings.sd.SampleDesc.Count)
				{
					continue;
				}
			}

			if (Quality != 0) 
			{
				pDeviceCombo->multiSampleCountList.PushBack( i );
				pDeviceCombo->multiSampleQualityList.PushBack( Quality );
			}
		}
	}

	SAFE_RELEASE( pAdapter );
	SAFE_RELEASE( pd3dDevice );
	SAFE_RELEASE (pd3dDeviceContext);
}


os::Vector<D3D11EnumAdapterInfo*>* D3D11Enumeration::GetAdapterInfoList() { return &m_AdapterInfoList; }

//--------------------------------------------------------------------------------------
D3D11EnumAdapterInfo* D3D11Enumeration::GetAdapterInfo( UINT AdapterOrdinal )
{
	for( uint32 iAdapter = 0; iAdapter < m_AdapterInfoList.Size(); iAdapter++ )
	{
		D3D11EnumAdapterInfo* pAdapterInfo = m_AdapterInfoList[iAdapter];
		if( pAdapterInfo->AdapterOrdinal == AdapterOrdinal )
			return pAdapterInfo;
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
D3D11EnumDeviceInfo* D3D11Enumeration::GetDeviceInfo( UINT AdapterOrdinal, D3D_DRIVER_TYPE DeviceType )
{
	D3D11EnumAdapterInfo* pAdapterInfo = GetAdapterInfo( AdapterOrdinal );
	if( pAdapterInfo )
	{
		for( uint32 iDeviceInfo = 0; iDeviceInfo < pAdapterInfo->deviceInfoList.Size(); iDeviceInfo++ )
		{
			D3D11EnumDeviceInfo* pDeviceInfo = pAdapterInfo->deviceInfoList[iDeviceInfo];
			if( pDeviceInfo->DeviceType == DeviceType )
				return pDeviceInfo;
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
D3D11EnumOutputInfo* D3D11Enumeration::GetOutputInfo( UINT AdapterOrdinal, UINT Output )
{
	D3D11EnumAdapterInfo* pAdapterInfo = GetAdapterInfo( AdapterOrdinal );
	if( pAdapterInfo && pAdapterInfo->outputInfoList.Size() > int( Output ) )
	{
		return pAdapterInfo->outputInfoList[Output];
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
D3D11EnumDeviceSettingsCombo* D3D11Enumeration::GetDeviceSettingsCombo( UINT AdapterOrdinal,
																		 D3D_DRIVER_TYPE DeviceType, UINT Output,
																		 DXGI_FORMAT BackBufferFormat, BOOL Windowed )
{
	D3D11EnumAdapterInfo* pAdapterInfo = GetAdapterInfo( AdapterOrdinal );
	if( pAdapterInfo )
	{
		for( uint32 iDeviceCombo = 0; iDeviceCombo < pAdapterInfo->deviceSettingsComboList.Size(); iDeviceCombo++ )
		{
			D3D11EnumDeviceSettingsCombo* pDeviceSettingsCombo = pAdapterInfo->deviceSettingsComboList[iDeviceCombo];

			if( pDeviceSettingsCombo->BackBufferFormat == BackBufferFormat &&
				pDeviceSettingsCombo->Windowed == Windowed )
				return pDeviceSettingsCombo;
		}
	}

	return NULL;
}


//--------------------------------------------------------------------------------------
D3D11EnumOutputInfo::~D3D11EnumOutputInfo( void )
{
	SAFE_RELEASE( m_pOutput );
	displayModeList.Clear();
}


//--------------------------------------------------------------------------------------
D3D11EnumDeviceInfo::~D3D11EnumDeviceInfo()
{
}


//--------------------------------------------------------------------------------------
D3D11EnumAdapterInfo::~D3D11EnumAdapterInfo( void )
{
	for( uint32 i = 0; i < outputInfoList.Size(); i++ )
	{
		D3D11EnumOutputInfo* pOutputInfo = outputInfoList[i];
		delete pOutputInfo;
	}
	outputInfoList.Clear();

	for( uint32 i = 0; i < deviceInfoList.Size(); ++i )
	{
		D3D11EnumDeviceInfo* pDeviceInfo = deviceInfoList[i];
		delete pDeviceInfo;
	}
	deviceInfoList.Clear();

	for( uint32 i = 0; i < deviceSettingsComboList.Size(); ++i )
	{
		D3D11EnumDeviceSettingsCombo* pDeviceCombo = deviceSettingsComboList[i];
		delete pDeviceCombo;
	}
	deviceSettingsComboList.Clear();

	SAFE_RELEASE( m_pAdapter );
}
