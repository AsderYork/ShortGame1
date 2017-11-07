#pragma once
#include "CEGUI_Service.h"
#include "SDL2_Service.h"
#include "ConsoleParser.h"

namespace GEM
{
	class ConsoleLayout : public CEGUI_Layout, public SDL_KeyboardListener
	{
	private:
		CEGUI::Window* m_consoleWindow;
		CEGUI::Editbox* m_editbox;
		CEGUI::MultiLineEditbox* m_print;
		Evaluator Eval;

		bool ProcessInput(const CEGUI::EventArgs& e);
		void WriteText(std::string Text);
		void WriteText(CEGUI::String Text);
	public:
		ConsoleLayout(SDL_Controller* SDL_controll);

		inline Evaluator& getEvaluator() { return Eval; }

		// Унаследовано через CEGUI_Layout
		virtual bool Initialize() override;
		virtual void turnOn() override;
		virtual void turnOff() override;
		virtual void PreFrame(float delta) override;

		// Унаследовано через SDL_KeyboardListener
		virtual void textInput(const SDL_TextInputEvent & arg) override;
		virtual void keyPressed(const SDL_KeyboardEvent & arg) override;
		virtual void keyReleased(const SDL_KeyboardEvent & arg) override;
	};
}