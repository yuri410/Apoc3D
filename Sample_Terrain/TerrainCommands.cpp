#include "TerrainCommands.h"
#include "Terrain.h"
#include "GameCamera.h"

#include "Apoc3D/Core/Logging.h"
#include "Apoc3D/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace SampleTerrain
{
	GenCommand::GenCommand()
	{
		m_desc.Name = L"Noise Generator";
		m_desc.CommandName = L"gen";
		m_desc.NumOfParameters = 0;
		m_desc.SubCommands.Add(new GenGetCommand());
		m_desc.SubCommands.Add(new GenSetCommand());
	}
	void GenCommand::Execute(const Apoc3D::Collections::List<String>& args)
	{
		LogManager::getSingleton().Write(LOG_CommandResponse, L"Use gen get/set.", LOGLVL_Infomation);
	}

	//////////////////////////////////////////////////////////////////////////

	GenGetCommand::GenGetCommand()
	{
		m_desc.Name = L"Noise Generator Get params. [persistence, frequency, octaves, seed]";
		m_desc.CommandName = L"get";
		m_desc.NumOfParameters = 0;
	}
	void GenGetCommand::Execute(const Apoc3D::Collections::List<String>& args)
	{
		PerlinNoise& pn = Terrain::GetNoiseGenerator();
		LogManager::getSingleton().Write(LOG_CommandResponse, L"persistence = " + StringUtils::ToString((float)pn.Persistence(), 4), LOGLVL_Infomation);
		LogManager::getSingleton().Write(LOG_CommandResponse, L"frequency = " + StringUtils::ToString((float)pn.Frequency(), 4), LOGLVL_Infomation);
		LogManager::getSingleton().Write(LOG_CommandResponse, L"octaves = " + StringUtils::ToString(pn.Octaves()), LOGLVL_Infomation);
		LogManager::getSingleton().Write(LOG_CommandResponse, L"seed = " + StringUtils::ToString(pn.RandomSeed()), LOGLVL_Infomation);
	}
	
	//////////////////////////////////////////////////////////////////////////

	GenSetCommand::GenSetCommand()
	{
		m_desc.Name = L"Noise Generator Set params. [persistence, frequency, octaves, seed]";
		m_desc.CommandName = L"set";
		m_desc.NumOfParameters = 4;
	}

	void GenSetCommand::Execute(const Apoc3D::Collections::List<String>& args)
	{
		PerlinNoise& pn = Terrain::GetNoiseGenerator();

		double persistence = StringUtils::ParseDouble(args[0]);
		double frequency = StringUtils::ParseDouble(args[1]);
		int octaves = StringUtils::ParseInt32(args[2]);
		int seed = StringUtils::ParseInt32(args[3]);

		pn.Set(persistence, frequency, pn.Amplitude(), octaves, seed);
	}

	//////////////////////////////////////////////////////////////////////////

	JumpHeightCommand::JumpHeightCommand()
	{
		m_desc.Name = L"Jump Velocity";
		m_desc.CommandName = L"jumpvel";
		m_desc.NumOfParameters = 1;
	}
	void JumpHeightCommand::Execute(const Apoc3D::Collections::List<String>& args)
	{
		GameCamera::JumpVelocity = StringUtils::ParseSingle(args[0]);
	}
}