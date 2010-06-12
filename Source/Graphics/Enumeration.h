
#ifndef ENUMERATION_H
#define ENUMERATION_H

#pragma once
#include "..\Common.h"
#include "DeviceSettings.h"
#include <list>

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{ 
		struct AdapterInfo
		{
			int32 AdapterOrdinal;

			D3DADAPTER_IDENTIFIER9 Details;

			String Description;

			list<D3DDISPLAYMODE> DisplayModes;

			list<DeviceInfo> Devices;
		};

		struct DeviceInfo
		{
			DeviceType DeviceType;

			D3DCAPS9 Capabilities;

			list<SettingsCombo> DeviceSettings;

		};

		struct SettingsCombo
		{
			int32 AdapterOrdinal;

			DeviceType DeviceType;

			Format AdapterFormat;

			Format BackBufferFormat;

			bool Windowed;

			list<Format> DepthStencilFormats;

			list<MSAAType> MultisampleTypes;
			list<int32> MultisampleQualities;
			list<uint32> PresentIntervals;
			
			AdapterInfo AdapterInfo;

			DeviceInfo DeviceInfo;


		};

		class Enumeration
		{
		private:
			DeviceSettings* m_minimumSettings;
			list<AdapterInfo>* m_adapters;
			bool m_hasEnumerated;

			bool getHasEnumerated() const
			{
				return m_hasEnumerated;
			}
			
			DeviceSettings* getDeviceSettings() const
			{
				return m_minimumSettings;
			}
			void setMinimumSettings(const DeviceSettings* settings)
			{
				m_minimumSettings = settings;
			}

		public:
			void Enumerate(IDirect3D9* const d3d9)
			{
				m_hasEnumerated = true;
				m_adapters = new list<AdapterInfo>();

				list<Format> adapterFormats;
				Format allowedAdapterFormats[4] = {
					D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_A2R10G10B10 };

				UINT count = d3d9->GetAdapterCount();
				for (UINT i=0;i<count;i++)
				{
					D3DADAPTER_IDENTIFIER9 dai;

					d3d9->GetAdapterIdentifier(i,0, &dai);

					adapterFormats.clear();

					AdapterInfo ai;
					ai.AdapterOrdinal = i;
					ai.Details = dai;

					for (int j=0;j<4;j++)
					{
						UINT modeCount = d3d9->GetAdapterModeCount();
						for (UINT k=0;k<modeCount;k++)
						{
							D3DDISPLAYMODE mode;
							d3d9->get
							if (m_minimumSettings)
							{

							}
						}

					}
				}	
			}

		private:
			
			void EnumerateDevices(const AdapterInfo &info, const list<Format> &adapterFormats);


			void EnumerateSettingsCombos(const AdapterInfo &adapterInfo, 
				const DeviceInfo &deviceInfo, const list<Format> &adapterFormats);

			void BuildDepthStencilFormatList(const SettingsCombo &combo);

			void BuildMultisampleTypeList(const SettingsCombo &combo);

			void BuildPresentIntervalList(const SettingsCombo &combo);

		public:
			Enumeration(void){ }
			~Enumeration(void) { }
		};
	};
};

#endif