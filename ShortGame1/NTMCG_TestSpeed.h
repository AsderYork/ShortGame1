#pragma once
#include <vector>
#include "NTMCG_Naive.h"
#include <chrono>
#include "ChunkLoader.h"

inline std::vector<std::pair<std::vector<float>,float>> PerformNTMCG_TestSpeed(unsigned int Maps, unsigned int IteratiosOnMap, int MaxChanges=100, int MinChanges=0)
{
	
	typedef std::chrono::duration<float> fsec;

	auto MapStart = std::chrono::system_clock::now();
	auto MapEnd = std::chrono::system_clock::now();
	auto UpdateStart = std::chrono::system_clock::now();
	auto UpdateEnd = std::chrono::system_clock::now();

	std::vector<std::pair<std::vector<float>, float>> RetVec;

	for (unsigned int mapIt = 0; mapIt < Maps; mapIt++)
	{
		auto chunkCentre = std::make_shared<GEM::NodeChunk>();
		chunkCentre->generateNewChunk();

		auto chunkFront = std::make_shared<GEM::NodeChunk>();
		chunkFront->generateNewChunk();

		auto chunkRight = std::make_shared<GEM::NodeChunk>();
		chunkRight->generateNewChunk();

		auto chunkFrontRight = std::make_shared<GEM::NodeChunk>();
		chunkFrontRight->generateNewChunk();

		std::pair<std::vector<float>, float> TmpPair;
		MapStart = std::chrono::system_clock::now();
		GEM::NodeToMCGeneratorNaive Generator(chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, 0, 0);
		Generator.Generate();
		MapEnd = std::chrono::system_clock::now();
		TmpPair.second = std::chrono::duration_cast<fsec>(MapEnd - MapStart).count();

		for (unsigned int updateIt = 0; updateIt < Maps; updateIt++)
		{
			int ChangesNum = MinChanges;
			if (MaxChanges != MinChanges)
			{
				ChangesNum += rand() % (MaxChanges - MinChanges);
			}


			for (int i = 0; i < ChangesNum; i++)
			{
				int x = rand() % CHUNK_SIZE;
				int y = rand() % CHUNK_HEIGHT;
				int z = rand() % CHUNK_SIZE;

				auto& OldVal = (*chunkCentre).NodeMap[x][y][z];
				if(OldVal.Value == 0)
				{
					(*chunkCentre).NodeMap[x][y][z].Value = 255;
				}else{
					(*chunkCentre).NodeMap[x][y][z].Value = 0;
				}
				Generator.ChangeNode(x, y, z);
			}

			UpdateStart = std::chrono::system_clock::now();
			Generator.Update();
			UpdateEnd = std::chrono::system_clock::now();
			TmpPair.first.push_back(std::chrono::duration_cast<fsec>(UpdateEnd - UpdateStart).count());
		}

		RetVec.push_back(TmpPair);
	}
	return RetVec;
}

inline void PerformNTMCG_TestSpeed_Results(std::vector<std::pair<std::vector<float>, float>>& Results)
{
	float AvgGenerate = 0;
	float DevGenerate = 0;

	float OverallUpdateAvg = 0;


	for (auto& Map : Results)
	{
		AvgGenerate += Map.second;
	}
	AvgGenerate /= Results.size();

	for (auto& Map : Results)
	{
		DevGenerate += abs(pow(AvgGenerate - Map.second, 2));
	}
	DevGenerate /= Results.size();

	std::vector<float> AvgUpdate;
	std::vector<float> DevUpdate;

	for (auto& Map : Results)
	{
		float TmpAvg = 0;
		float TmpDev = 0;

		for (auto& Try : Map.first)
		{
			TmpAvg += Try;
		}
		TmpAvg /= Map.first.size();

		for (auto& Try : Map.first)
		{
			TmpDev += abs(pow(TmpAvg - Try, 2));
		}
		TmpDev /= Map.first.size();

		OverallUpdateAvg += TmpAvg;
		AvgUpdate.push_back(TmpAvg);
		DevUpdate.push_back(TmpDev);
	}

	OverallUpdateAvg /= Results.size();

	//Printing
	printf("AllData\n---------------------\n");
	for (auto& Entry : Results)
	{
		printf("Generate:%f\n", Entry.second);

		for (auto& Try : Entry.first)
		{
			printf("Update:%f\n", Try);
		}

		printf("\n---------------------\n");
	}

	printf("Sub Avgs\n---------------------\n");	
	printf("Generate Avg:%10.6f\n", AvgGenerate);
	printf("Generate Dev:%10.6f\n", DevGenerate);

	for (int i = 0; i < AvgUpdate.size(); i++)
	{
		printf("Update:Avg:%10.6f;%10.6f\n", AvgUpdate[i], DevUpdate[i]);
	}

	printf("\n---------------------\n");

	printf("Overall AVG for try:%10.6f\n", OverallUpdateAvg);
}
