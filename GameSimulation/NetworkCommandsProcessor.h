#pragma once
#include "NetworkCommandBase.h"
#include "GameTime.h"
#include "Player_id_type.h"
#include <memory>
#include <cereal\cereal.hpp>

namespace GEM::GameSim
{
	class NetworkCommand;


	/**!
	Recives, sends and processes NetworkComands.

	Every system, that need network exchange should instantiate it's networkExchangeProcessor
	and it commands for it
	*/

	class NetworkExchangeProcessor
	{
	private:
		/**!
		Signals which player's command is gonna be processed.
		Dispatcher must guarantee, that for every call to other methods, this variable is set correctly.
		*/
		PLAYER_ID_TYPE m_currentPlayer;
	public:

		inline void setCurrentPlayer(PLAYER_ID_TYPE currPlayerID) { m_currentPlayer = currPlayerID; }
		
		inline PLAYER_ID_TYPE getCurrentPlayer() { return m_currentPlayer; }

		/**!
		Applies a command. This method can be called only once for every command just when it is recived.
		\param[in] Command A command, that should be performed.
		\note It is const for a reason. Whe uses this system with history and confirmation
		so events can be rollback'd or re-applied untill it's ither confirmed or thrown away.
		This means, every call should be the same.
		\returns Returns true, if command applied successfuly, false otherwise
		*/
		virtual bool ApplyCommand(const NetworkCommand* Command, GameTime PacketTime) { return true; }

		/**!
		Undoes a command. This method is called, when command is still not confirmed,
		and history is changing, so all the commands in it must be rolled back and then
		commands that assumed to stay in a history will be re-applied
		*/
		virtual void RollbackCommand(const NetworkCommand* Command) {}

		/**!
		Re-applies command. This method is called for command, that was rolled back during history
		change and then assumed to still be potentially valid, so it is re-applied.
		\returns Returns true if command was reapplied successfully, false otherwise.
		*/
		virtual bool ReapplyCommand(const NetworkCommand* Command) {return true; }

		/**!
		This method is called for every command, that is confirmed.
		This is a final call for a particular command, so no method would be
		called with a command, that was confirmed or Rejected.
		*/
		virtual void ConfirmCommand(const NetworkCommand* Command) {}

		/**!
		This method is called for commands, that are rejected. This call is final for a
		particular Command.
		*/
		virtual void RejectCommand(const NetworkCommand* Command) {}

		/**!
		Only Processor knows it commands and how to serialize them. That's why this method is called
		when command need serialization.

		\note header of command will be written right before this method is called, so it's like an onion.
		*/
		virtual void SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command) = 0;
		/**!
		Deserializes one command from archive.
		\returns Command, that was deserialized.

		\note Archive is a stateful thing, so be careful and construct your commands in such a way
		that when you would deserialize them, you would not accedentally demage other things still in archve
		*/
		virtual std::unique_ptr<NetworkCommand> deserializeCommand(cereal::BinaryInputArchive & ar) = 0;
		/**!
		this method is called right after all recived events are processed and history is reconsidered.
		*/
		virtual void EndNetworkProcessing() {};

		/**!
		Every processor MUST have unique ID. That history/Dispacher can quickly find processor to which a command belongs
		*/
		virtual uint8_t getIdOfProcessor() const = 0;

		

	};

	/**!
	This class provide simple way to create processor, that supposed to work with only one class of commands,
	which is a the most common case.
	*/
	template<class CommandName>
	class NetworkExchangeProcessorSingleCommandSerialization : public NetworkExchangeProcessor
	{
		virtual inline void SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command)
		{
			auto CommandRecast = static_cast<const CommandName*>(Command);
			ar((*CommandRecast));
		}

		virtual inline std::unique_ptr<NetworkCommand> deserializeCommand(cereal::BinaryInputArchive & ar)
		{
			auto NewCommand = std::make_unique<CommandName>();
			ar(*NewCommand);
			return NewCommand;
		}
	};


}