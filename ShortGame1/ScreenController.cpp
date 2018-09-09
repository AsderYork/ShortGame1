#include "stdafx.h"
#include "ScreenController.h"

#include <algorithm>


namespace GEM
{

	void ScreenService::ActivateScreen(int32_t screenId)
	{
		Screen* Ptr = nullptr;

		for (auto& RequestedScreen : m_registeredScreens)
		{
			if (RequestedScreen.first == screenId) {
				Ptr = RequestedScreen.second.get();
				break;
			}
		}
		if (Ptr == nullptr)
		{
			LOGCATEGORY("ScreenService/ActivateScreen").error("Can't activate screen with id %i. There is no such screen.", screenId);
			throw std::exception("Can't activate screen with id %i. There is no such screen.", screenId);
		}

		Screen* OldTop = nullptr;
		if (!m_stack.empty())	{
			OldTop = m_stack.back();
		}

		auto RemoveIter = std::remove(m_stack.begin(), m_stack.end(), Ptr);
		if (RemoveIter < m_stack.end()) {
			(*RemoveIter) = Ptr;
		}
		else {
			m_stack.push_back(Ptr);
		}


		if (m_isInitted && OldTop != nullptr && (OldTop != m_stack.back()))
		{
			m_stack.back()->OnTop();
			if (!m_stack.back()->m_isTransparent)
			{
				for (auto it = m_stack.rbegin() + 1; it != m_stack.rend(); it++)
				{
					(*it)->NoLongerOnTop();
					if (!(*it)->m_isTransparent) { break; }
				}
			}
		}
	}


	Service::ActionResult ScreenService::initialize()
	{
		for (auto& RegisteredScreen : m_registeredScreens)
		{
			if (!RegisteredScreen.second->Init())
			{
				LOGCATEGORY("ScreenService/initialize").error("Screen #%i return false on it's init!", RegisteredScreen.first);
				return ActionResult::AR_ERROR;
			}
		}
		m_isInitted = true;

		if (!m_stack.empty())
		{
			m_stack.back()->OnTop();
		}

		return ActionResult::AR_OK;
	}
	void ScreenService::shutdown()
	{
		m_stack.clear();
		m_registeredScreens.clear();
	}

	Service::ActionResult ScreenService::preFrame(float timeDelta)
	{
		bool Continue = true;
		for (auto Iter = m_stack.rbegin(); Iter != m_stack.rend(); Iter++)
		{
			if (!Continue) { break; }
			(*Iter)->PreFrame(timeDelta);
			Continue = (*Iter)->m_isTransparent;
		}
		return ActionResult::AR_OK;
	}
	Service::ActionResult ScreenService::frame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}
	Service::ActionResult ScreenService::postFrame(float timeDelta)
	{
		bool Continue = true;
		//Stack is copied to protect against cases, when Screens add other screen in a queue on this step
		auto LocalExecStack = m_stack;
		for (auto Iter = LocalExecStack.rbegin(); Iter != LocalExecStack.rend(); Iter++)
		{
			if (!Continue) { break; }
			(*Iter)->PostFrame(timeDelta);
			Continue = (*Iter)->m_isTransparent;
		}

		//Removal
		std::vector<Screen*> NewStack;
		auto OldTop = m_stack.back();
		bool NeedVisibilityRechek = !OldTop->m_isTransparent;
		for (auto& Scr : m_stack)
		{
			if (Scr->m_markedForRemoval) { Scr->LeaveStack(); }
			else { NewStack.push_back(Scr); }
		}
		if (NewStack.size() != m_stack.size())
		{
			m_stack.swap(NewStack);
		}

		if (OldTop != m_stack.back() && NeedVisibilityRechek)
		{
			for (auto it = m_stack.rbegin() + 1; it != m_stack.rend(); it++)
			{
				if (!(*it)->m_isTransparent) { break; }
				(*it)->NoLongerOnTop();
			}
		}

		return ActionResult::AR_OK;
	}
	void Screen::ActivateAnotherScreen(int32_t id)
	{
		if (m_service == nullptr) {
			LOGCATEGORY("Screen/ActivateAnotherScreen").error("Can't Activate another screen. This screen doesn't belong to any ScreenServive!");
			throw std::exception("Cant' Activate screen from unregistered screen!");
		}
		m_service->ActivateScreen(id);
	}
}