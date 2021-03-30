#include "stdafx.h"
#include "ConsoleLayout.h"
#include <iostream>       // std::cout, std::hex
#include <string> 
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8
#include <cstdint>        // std::uint_least32_t

namespace GEM
{
	bool ConsoleLayout::ProcessInput(const CEGUI::EventArgs & e)
	{
		auto Text = m_editbox->getText();//Process it BEFORE rempval!
		printf("Enter!\n");
		WriteText(Text);
		std::wstring_convert<std::codecvt_utf8<__int32>, __int32> cv;
		std::string Str = cv.to_bytes((__int32*)Text.c_str());
		Eval.Parse(Str);
		m_editbox->setText("");
		return true;
	}
	void ConsoleLayout::WriteText(std::string Text)
	{
		auto TmpStr = m_print->getText();
		TmpStr += Text;
		m_print->setText(TmpStr);
		m_print->setCaretIndex(m_print->getText().size());
	}

	void ConsoleLayout::WriteText(CEGUI::String Text)
	{
		auto TmpStr = m_print->getText();
		TmpStr += Text;
		m_print->setText(TmpStr);
		m_print->setCaretIndex(m_print->getText().size());
	}

	ConsoleLayout::ConsoleLayout(SDL_Controller* SDL_controll)
	{
		SDL_controll->registerKeyboardListener(this);
	}
	bool ConsoleLayout::Initialize()
	{
		m_consoleWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("Console.layout");
		m_editbox = (CEGUI::Editbox*)m_consoleWindow->getChild("Input");
		m_print = (CEGUI::MultiLineEditbox*)m_consoleWindow->getChild("ConsoleText");
		auto DefWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
		DefWindow->addChild(m_consoleWindow);

		m_editbox->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ConsoleLayout::ProcessInput, this));
		turnOff();

		Eval.SetPrinter([this](std::string Str) {WriteText(Str); });
		return true;

	}

	void ConsoleLayout::turnOn()
	{
		m_consoleWindow->setActive(true);
		m_consoleWindow->setVisible(true);
		getCreator()->VoteForVisibleCursor();
		m_turnedOn = true;
	}

	void ConsoleLayout::turnOff()
	{
		m_consoleWindow->setActive(false);
		m_consoleWindow->setVisible(false);
		getCreator()->VoteAgainstVisibleCursor();
		m_turnedOn = false;
	}

	void ConsoleLayout::PreFrame(float delta)
	{
	}
	void ConsoleLayout::textInput(const SDL_TextInputEvent & arg)
	{		
		
	}
	void ConsoleLayout::keyPressed(const SDL_KeyboardEvent & arg)
	{
		if (arg.keysym.scancode == SDL_SCANCODE_GRAVE)
		{
			m_turnedOn ? turnOff() : turnOn();
		}
	}
	void ConsoleLayout::keyReleased(const SDL_KeyboardEvent & arg)
	{
	}
}