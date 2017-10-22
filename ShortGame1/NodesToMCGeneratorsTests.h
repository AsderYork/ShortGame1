#pragma once
#include "NodesToMCGenerator.h"
#include <chrono>
#include <vector>
#include <memory>
#include <tuple>

namespace GEM
{

	template<class Type, class... T>
	int AddInVector(std::vector<std::unique_ptr<NTMCG_Base>> &vec, T... args)
	{
		vec.push_back(std::make_unique<Type>(args...));
		return 0;
	}

	template<class... T>
	void DoNothing(T... args)
	{
	}

	template<class ... Types>
	std::vector< std::vector<std::vector<float>>> TestSpeed(int NewMapToGenerate, int GenerationsForEveryMap)
	{
		typedef std::chrono::duration<float> fsec;
		//Preparation
		//Get all needed chunks
		GEM::ChunkLoader<GEM::NodeChunk> m_loader("Test", "test");

		auto chunkCentre = m_loader.getChunk(0, 0);

		auto chunkFront = m_loader.getChunk(0, 1);
		auto chunkRight = m_loader.getChunk(1, 0);
		auto chunkFrontRight = m_loader.getChunk(1, 1);



		//[Generator][Map][Try]
		std::vector<std::vector<std::vector<float>>> Result;
		Result.resize(sizeof...(Types));
		for (auto& Gen : Result)
		{
			Gen.resize(NewMapToGenerate);
			for (auto& Map : Gen)
			{
				Map.resize(GenerationsForEveryMap);
			}

		}

		for (int NewMaps = 0; NewMaps < NewMapToGenerate; NewMaps++)
		{
			//Change CenreChunk
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
					for (int z = 0; z < CHUNK_SIZE; z++)
					{
						chunkCentre->NodeMap[x][y][z].Value = rand() % 256;
					}
				}
			}
			//Change Front
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
					chunkFront->NodeMap[x][y][0].Value = rand() % 256;
				}
			}
			//Change Right
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
					chunkRight->NodeMap[0][y][z].Value = rand() % 256;
				}
			}
			//Change Edge
			for (int y = 0; y < CHUNK_SIZE; y++)
			{
				chunkFrontRight->NodeMap[0][y][0].Value = rand() % 256;
			}



			std::vector<std::unique_ptr<NTMCG_Base>> Generators;
			
			DoNothing(AddInVector<Types>(Generators, chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, 0, 0)...);

			
			for (int GenNum = 0; GenNum < Generators.size(); GenNum++)
			{
				for (int Try = 0; Try < GenerationsForEveryMap; Try++)
				{				
						auto t0 = std::chrono::high_resolution_clock::now();
						Generators[GenNum]->Generate();
						auto t1 = std::chrono::high_resolution_clock::now();
						Result[GenNum][NewMaps][Try] = std::chrono::duration_cast<fsec>(t1 - t0).count();
				}
			}
			
		}
	
			return Result;
	}

	template<class ... Types>
	std::vector< std::vector<std::vector<float>>> TestSpeedUpdate(int NewMapToGenerate, int GenerationsForEveryMap)
	{
		typedef std::chrono::duration<float> fsec;
		//Preparation
		//Get all needed chunks
		GEM::ChunkLoader<GEM::NodeChunk> m_loader("Test", "test");

		auto chunkCentre = m_loader.getChunk(0, 0);

		auto chunkFront = m_loader.getChunk(0, 1);
		auto chunkRight = m_loader.getChunk(1, 0);
		auto chunkFrontRight = m_loader.getChunk(1, 1);



		//[Generator][Map][Try]
		std::vector<std::vector<std::vector<float>>> Result;
		Result.resize(sizeof...(Types));
		for (auto& Gen : Result)
		{
			Gen.resize(NewMapToGenerate);
			for (auto& Map : Gen)
			{
				Map.resize(GenerationsForEveryMap);
			}

		}

		for (int NewMaps = 0; NewMaps < NewMapToGenerate; NewMaps++)
		{
			//Change CenreChunk
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
					for (int z = 0; z < CHUNK_SIZE; z++)
					{
						chunkCentre->NodeMap[x][y][z].Value = rand() % 256;
					}
				}
			}
			//Change Front
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
					chunkFront->NodeMap[x][y][0].Value = rand() % 256;
				}
			}
			//Change Right
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				for (int y = 0; y < CHUNK_HEIGHT; y++)
				{
					chunkRight->NodeMap[0][y][z].Value = rand() % 256;
				}
			}
			//Change Edge
			for (int y = 0; y < CHUNK_SIZE; y++)
			{
				chunkFrontRight->NodeMap[0][y][0].Value = rand() % 256;
			}



			std::vector<std::unique_ptr<NTMCG_Base>> Generators;

			DoNothing(AddInVector<Types>(Generators, chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, 0, 0)...);


			for (int GenNum = 0; GenNum < Generators.size(); GenNum++)
			{
				//Do one generate for every map;
				Generators[GenNum]->Generate();
				for (int Try = 0; Try < GenerationsForEveryMap; Try++)
				{
					//Change random node
					Generators[GenNum]->ChangeNode(std::rand() % CHUNK_SIZE, std::rand() % CHUNK_HEIGHT, std::rand() % CHUNK_SIZE);
					auto t0 = std::chrono::high_resolution_clock::now();
					Generators[GenNum]->Update();
					auto t1 = std::chrono::high_resolution_clock::now();
					Result[GenNum][NewMaps][Try] = std::chrono::duration_cast<fsec>(t1 - t0).count();
				}

			}

		}

		return Result;
	}


	void InterprateResults(std::vector< std::vector<std::vector<float>>> &Results)
	{
		std::vector<std::vector<float>> AvgForTry;
		std::vector<std::vector<float>> DeviationForTry;
		
		
		AvgForTry.resize(Results.size());
		DeviationForTry.resize(Results.size());

		for (int Gen = 0; Gen < Results.size(); Gen++)
		{
			AvgForTry[Gen].resize(Results[Gen].size());
			DeviationForTry[Gen].resize(Results[Gen].size());

			for (int Map = 0; Map < Results[Gen].size(); Map++)
			{
				float Avg = 0;
				for (int Try = 0; Try < Results[Gen][Map].size(); Try++)
				{
					Avg += Results[Gen][Map][Try];
				}
				AvgForTry[Gen][Map] = Avg / Results[Gen][Map].size();
				float Deviation = 0;
				for (int Try = 0; Try < Results[Gen][Map].size(); Try++)
				{
					Deviation += pow(Avg - Results[Gen][Map][Try],2);
				}
				DeviationForTry[Gen][Map] = Deviation / Results[Gen][Map].size();

			}
		}

		printf("MC Generators spped test results\n");
		printf("Map#     |Try#     |");
		for (int i = 0; i < Results.size(); i++)
		{
			printf("Generator %-3i|",i);
		}
		printf("\n");
		for (int Map = 0; Map < Results[0].size(); Map++)
		{
			for (int Try = 0; Try < Results[0][Map].size(); Try++)
			{
				printf("%-9i|%-9i|", Map, Try);
				for (int i = 0; i < Results.size(); i++)
				{
					printf("%13.10f|", Results[i][Map][Try]);
				}
				printf("\n");
			}
			printf("<><><><><><><><><><>");
			for (int Try = 0; Try < Results.size(); Try++)
			{
				printf("<><><><><><><>");
			}
			printf("\n");
		}








		printf("\n");
		printf("Deviation and Average\n");
		printf("Map#     |");
		for (int i = 0; i < Results.size(); i++)
		{
			printf("Generator %-3i          |", i);
		}
		printf("\n");
		printf("_________|");
		for (int i = 0; i < Results.size(); i++)
		{
			printf("Avg:      |Dev:        |", i);
		}
		printf("\n");
		for (int Map = 0; Map < Results[0].size(); Map++)
		{
			printf("%-9i|", Map);
			for (int i = 0; i < Results.size(); i++)
			{
				printf("%10.6f|%12.5f|", AvgForTry[i][Map], DeviationForTry[i][Map]);
			}
			printf("\n");		
		}
	}
}