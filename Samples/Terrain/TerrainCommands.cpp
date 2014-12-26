#include "TerrainCommands.h"
#include "Terrain.h"
#include "GameCamera.h"

namespace SampleTerrain
{
	void GenGetCommand(CommandArgsConstRef args)
	{
		PerlinNoise& pn = Terrain::GetNoiseGenerator();
		LogManager::getSingleton().Write(LOG_CommandResponse, L"persistence = " + StringUtils::DoubleToString(pn.Persistence(), StrFmt::fp<4>::val), LOGLVL_Infomation);
		LogManager::getSingleton().Write(LOG_CommandResponse, L"frequency = " + StringUtils::DoubleToString(pn.Frequency(), StrFmt::fp<4>::val), LOGLVL_Infomation);
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

		pn.Set(persistence, frequency, octaves, seed);
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
		CommandInterpreter::getSingleton().RegisterCommand(
		{
			L"gen", 0, nullptr, L"Noise Generator",
			{
				{ L"get", 0, GenGetCommand, L"Noise Generator Get params. [persistence, frequency, octaves, seed]" },
				{ L"set", 4, GenSetCommand, L"Noise Generator Set params. [persistence, frequency, octaves, seed]" }
			}
		});

		CommandInterpreter::getSingleton().RegisterCommand(CommandDescription(L"jumpvel", 1, JumpHeightCommand, L"Jump Velocity", L""));
		CommandInterpreter::getSingleton().RegisterCommand(CommandDescription(L"fly", 2, FlyCommand, L"Fly mode", L""));
	}
}