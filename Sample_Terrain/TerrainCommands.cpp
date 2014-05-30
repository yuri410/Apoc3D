#include "TerrainCommands.h"
#include "Terrain.h"
#include "GameCamera.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Core/CommandInterpreter.h"

using namespace Apoc3D::Utility;

namespace SampleTerrain
{
	void GenCommand(CommandArgsConstRef args)
	{
		LogManager::getSingleton().Write(LOG_CommandResponse, L"Use gen get/set.", LOGLVL_Infomation);
	}
	void GenGetCommand(CommandArgsConstRef args)
	{
		PerlinNoise& pn = Terrain::GetNoiseGenerator();
		LogManager::getSingleton().Write(LOG_CommandResponse, L"persistence = " + StringUtils::SingleToString((float)pn.Persistence(), 4), LOGLVL_Infomation);
		LogManager::getSingleton().Write(LOG_CommandResponse, L"frequency = " + StringUtils::SingleToString((float)pn.Frequency(), 4), LOGLVL_Infomation);
		LogManager::getSingleton().Write(LOG_CommandResponse, L"octaves = " + StringUtils::IntToString(pn.Octaves()), LOGLVL_Infomation);
		LogManager::getSingleton().Write(LOG_CommandResponse, L"seed = " + StringUtils::IntToString(pn.RandomSeed()), LOGLVL_Infomation);
	}
	void GenSetCommand(CommandArgsConstRef args)
	{
		PerlinNoise& pn = Terrain::GetNoiseGenerator();

		double persistence = StringUtils::ParseDouble(args[0]);
		double frequency = StringUtils::ParseDouble(args[1]);
		int octaves = StringUtils::ParseInt32(args[2]);
		int seed = StringUtils::ParseInt32(args[3]);

		pn.Set(persistence, frequency, pn.Amplitude(), octaves, seed);
	}
	void JumpHeightCommand(CommandArgsConstRef args)
	{
		GameCamera::JumpVelocity = StringUtils::ParseSingle(args[0]);
	}
	void FlyCommand(CommandArgsConstRef args)
	{
		GameCamera::Flying = StringUtils::ParseBool(args[0]);
		GameCamera::FlyingAlt = StringUtils::ParseSingle(args[1]);
	}

	void RegisterTerrainCommands()
	{
		{
			CommandDescription desc(L"gen", 0, GenCommand, L"Noise Generator", L"");

			desc.SubCommands.PushBack(CommandDescription(L"get", 0, GenGetCommand, L"Noise Generator Get params. [persistence, frequency, octaves, seed]", L""));
			desc.SubCommands.PushBack(CommandDescription(L"set", 4, GenSetCommand, L"Noise Generator Set params. [persistence, frequency, octaves, seed]", L""));

			CommandInterpreter::getSingleton().RegisterCommand(desc);
		}

		CommandInterpreter::getSingleton().RegisterCommand(CommandDescription(L"jumpvel", 1, JumpHeightCommand, L"Jump Velocity", L""));
		CommandInterpreter::getSingleton().RegisterCommand(CommandDescription(L"fly", 2, FlyCommand, L"Fly mode", L""));
	}
}