#pragma once
#include "NetworkCommandBase.h"

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
	public:
		/**!
		Applies a command. This method can be called only once for every command just when it is recived.
		\param[in] Command A command, that should be performed.
		\note It is const for a reason. Whe uses this system with history and confirmation
		so events can be rollback'd or re-applied untill it's ither confirmed or thrown away.
		This means, every call should be the same.
		\returns Returns true, if command applied successfuly, false otherwise
		*/
		virtual bool ApplyCommand(const NetworkCommand* Command) = 0;

		/**!
		Undoes a command. This method is called, when command is still not confirmed,
		and history is changing, so all the commands in it must be rolled back and then
		commands that assumed to stay in a history will be re-applied
		*/
		virtual void RollbackCommand(const NetworkCommand* Command) = 0;

		/**!
		Re-applies command. This method is called for command, that was rolled back during history
		change and then assumed to still be potentially valid, so it is re-applied.
		\returns Returns true if command was reapplied successfully, false otherwise.
		*/
		virtual bool ReapplyCommand(const NetworkCommand* Command) = 0;

		/**!
		This method is called for every command, that is confirmed.
		This is a final call for a particular command, so no method would be
		called with a command, that was confirmed or Rejected.
		*/
		virtual void ConfirmCommand(const NetworkCommand* Command) = 0;

		/**!
		This method is called for commands, that are rejected. This call is final for a
		particular Command.
		*/
		virtual void RejectCommand(const NetworkCommand* Command) = 0;

		/**!
		this method is called right after all recived events are processed and history is reconsidered.
		*/
		virtual void EndNetworkProcessing() {};


	};

}