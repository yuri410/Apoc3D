#pragma once
/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#ifndef APOC3D_D3D9_ENUMERATION_H
#define APOC3D_D3D9_ENUMERATION_H


#include "D3D9Common.h"
#include "DeviceSettings.h"

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{ 
		namespace D3D9RenderSystem
		{
			class AdapterInfo;
			class DeviceInfo;

			class SettingsCombo
			{
			private:
				SettingsCombo(const SettingsCombo& another) { }
			public:
				SettingsCombo(){}
				int32 AdapterOrdinal;

				D3DDEVTYPE DeviceType;

				D3DFORMAT AdapterFormat;

				D3DFORMAT BackBufferFormat;

				bool Windowed;

				vector<D3DFORMAT> DepthStencilFormats;

				vector<D3DMULTISAMPLE_TYPE> MultisampleTypes;
				vector<int32> MultisampleQualities;
				vector<uint32> PresentIntervals;

				AdapterInfo* AdapterInfo;

				DeviceInfo* DeviceInfo;


			};
		
			class DeviceInfo
			{
			private:
				DeviceInfo(const DeviceInfo& another) { }
			public:
				DeviceInfo() {}
				D3DDEVTYPE DeviceType;

				D3DCAPS9 Capabilities;

				vector<SettingsCombo*> DeviceSettings;

			};
			class AdapterInfo
			{
			private:
				AdapterInfo(const AdapterInfo& another) { }
			public:
				AdapterInfo() {}
				int32 AdapterOrdinal;

				D3DADAPTER_IDENTIFIER9 Details;

				String Description;

				vector<D3DDISPLAYMODE> DisplayModes;

				vector<DeviceInfo*> Devices;

			};

			class Enumeration
			{
			private:
				

				static bool m_hasMinimumSettings;
				static DeviceSettings m_minimumSettings;
				static vector<AdapterInfo*> m_adapters;
				static bool m_hasEnumerated;


				
				static void EnumerateDevices(IDirect3D9* d3d9, AdapterInfo* info, vector<D3DFORMAT>& adapterFormats);

				static void EnumerateSettingsCombos(IDirect3D9* d3d9, AdapterInfo* adapterInfo, 
					DeviceInfo* deviceInfo, vector<D3DFORMAT>& adapterFormats);

				static void BuildDepthStencilFormatList(IDirect3D9* d3d9, SettingsCombo* combo);

				static void BuildMultisampleTypeList(IDirect3D9* d3d9, SettingsCombo* combo);

				static void BuildPresentIntervalList(IDirect3D9* d3d9, SettingsCombo* combo);



				static float RankSettingsCombo(const SettingsCombo* combo, const Direct3D9Settings& optimal, const D3DDISPLAYMODE& desktopMode);
				static void BuildValidSettings(const SettingsCombo* combo, const Direct3D9Settings& input, Direct3D9Settings& result);
				static D3DDISPLAYMODE FindValidResolution(const SettingsCombo* combo, const Direct3D9Settings& input);
				static void BuildOptimalSettings(IDirect3D9* d3d9, const DeviceSettings& settings, Direct3D9Settings& optimal);
			public:

				static const vector<AdapterInfo*>& getAdapters() { return m_adapters; }
				static bool getHasEnumerated()
				{
					return m_hasEnumerated;
				}

				static const DeviceSettings& getMiniumSettings()
				{
					return m_minimumSettings;
				}
				static void setMinimumSettings(const DeviceSettings& settings)
				{
					m_minimumSettings = settings;
					m_hasMinimumSettings = true;
				}
				static void ClearMinimumSetting() { m_hasMinimumSettings = false; }

				static void Enumerate(IDirect3D9* d3d9);

				
				static void FindValidSettings(IDirect3D9* d3d9, const DeviceSettings& settings, DeviceSettings& result);

			private:
				Enumeration(void){ }
				~Enumeration(void) { }
			};
		}
		
	};
};

#endif