#pragma once
#include <string>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <optional>
#include <list>
#include "ConsoleParserHelper.h"

namespace GEM
{
	class LexicalParser;
	class LogicalParser;
	class Evaluator;

	class LexicalParser
	{
	private:
		friend class Evaluator;
		friend class LogicalParser;
		struct Lexema
		{
			/**!
			Not all marks can have a meaning. There's a vector of ones that do
			*/
			static std::vector<std::string> AllowedMarks;
			enum LEXEME_TYPE {
				NONE,
				NAME,//"A_name","Name34", "Ultra name"; Everything, that is a name of something. Or just a string of text
				NUMBER,//"5"
				NUMBER_DOTTED,
				MARK,
				LITERAL
			} Type;

			std::string str;
			Lexema(LEXEME_TYPE Type, std::string& _str) : Type(Type), str(_str) {}
			Lexema() : Type(NONE), str() {}
		};

		using RetType = std::vector<std::vector<Lexema>>;
		void PushTmpLexema(RetType& TmpRet, Lexema& TmpLexema);
		void ParseOneCharacter(char& c, RetType& TmpRet, Lexema& TmpLexema);
		std::string ParseOneCommand(std::string& line, RetType& TmpRet, Lexema& TmpLexema);
	public:
		std::vector<std::vector<Lexema>> Parse(std::string line);
	};

	/**LOGICAL PARSER THINGS*/
	class LogicalParser
	{
	private:
		friend class Evaluator;
		//Pre-declaration;
		struct VariableName;
		struct NumericalValue;
		struct Literal;
		struct Function;
		struct Operator;
		enum class UnitType { VAR, NUM, FUNC, OP, LIETARL, NONE };//Variable, Numerical, Function, Operator, ...
		using LogicalUnit = std::pair<std::variant<VariableName, NumericalValue, Literal, Function, Operator>, UnitType>;


		struct Object {
			enum class NAME_TYPE {
				STR,
				NUM,
				FUNC,
			} Subtype = NAME_TYPE::STR;
			std::unordered_map<std::string, Object> Subobjects;
			std::string Value;
		};




		struct VariableName {
			/**
			<[NAME][.]...>[NAME] - A variable name
			*/
			std::vector<std::string> Hierarchy;
		};
		struct NumericalValue {
			std::string Value;
			bool isDotted = false;
		};
		struct Literal {
			std::string LiteralValue;
		};
		struct Operator {
			std::string Op;
			int getPriority() const;
			bool isActionOperator() const;
		};
		//Complex units; They can be deduced only after all previous units. They can remove units from m_units and use them inside!
		struct Function {
			//[[VAR][(](VAR/NUMERIC/EVAL_NUM/EVAL_STR/LITERAL/FUNC)[)] / [VAR][(][VAR/NUMERIC/EVAL_NUM/EVAL_STR/LITERAL/FUNC]<[,][VAR/NUMERIC/EVAL_NUM/EVAL_STR/LITERAL/FUNC]>[)]
			int Arity = 0;
			VariableName Name;
		};

		/*Checks if Buffer contains a LogicalUnit of a given type. If it does,
		It will create an instance of it and return it. Otherwise nothing will be returned
		The second value will be in (From,Lexemas.size()]. It's a place of a last lexema, used to construct A Varname.
		If no VarName returned, then this value will be -1;		*/
		std::pair<std::optional<VariableName>, int> IsThereVariableName(std::vector<LexicalParser::Lexema>& Lexemas, int From);
		//Check if Lexema[From] Is numerical. If it is builds and returns it
		std::optional<NumericalValue> IsThereNumerical(std::vector<LexicalParser::Lexema>& Lexemas, int From);
		//Check if Lexema[From] Is literal. If it is builds and returns it
		std::optional<Literal> IsThereLiteral(std::vector<LexicalParser::Lexema>& Lexemas, int From);
		//Check if Lexema[From] Is operator. If it is builds and returns it
		std::optional<Operator> IsThereOperator(std::vector<LexicalParser::Lexema>& Lexemas, int From);

		std::list<LogicalUnit> ParseCommand(std::vector<LexicalParser::Lexema>& Lexemas);

		//Collapse [VAR][(] in to [FUNCTION]
		void CollapseFunctions(std::list<LogicalUnit>& Units);
		std::list<LogicalUnit> ToPostfix(std::list<LogicalUnit>& Units);


	public:

		std::vector<std::list<LogicalUnit>> Parse(std::vector<std::vector<LexicalParser::Lexema>> Lexemics);
		void Print(std::vector<std::list<LogicalUnit>> Input);

	};

	class Evaluator
	{
		/**!Evaluation rules
		[VAR]
		Shows variable value; Variable must presist in a ScopeStack;

		[NUM/LITERAL]
		Shows a value of NUM or Literal;

		[VAR]=[NUM/LITERAL/VAR]
		Assign value of right part to the Variable on the left.
		Type of Variable on the right must match type of something on the right
		All variables in this command must presist in a ScopeStack;

		[{]<COMMAND>[}]
		A block. Creates it's own scope. Can be used as a single COMMAND

		[if][(][NUM][)][COMMAND]
		Process next command only if NUM != 0; Yes, there is no bool type.

		[while][(][NUM][)][COMMAND]
		Process next command while NUM == 0; Please, don't do infinite loop! There is no way to check for it. It's Turing-complete!

		[FUNC][(]([VAR/LITARAL/NUM]<[,][VAR/LITEARL/NUM]>)[)]
		Calls a function. Function can be provided from somewhe. If there is no function with specified name, check str Variables.
		If there is str Variable wth name of a specified function, evaluate it. But they should not have any input! They also can't have output!

		There was a rule on how to create a variable. Well, no more! Now only using sys.createStr([NAME],[VAL])/sys.createNum([NAME],[VAL]);
		*/
	public:
		enum class OBJTYPE { STR, NUM, NUM_DOTTED, UNDECLARED, VAR };//VAR - can be used only as a return type for Functions!
	private:

		std::function<void(std::string)> m_printer = [](std::string s) {printf("%s\n", s.c_str());  };

		//Something, that can be accessed. It can recive
		struct Variable {
			OBJTYPE Type;
			std::string Value;
			//This methods can't be assigned in Language, but other code can use it to allow Language to manage it's variables as a regular variable
			std::function<void(std::string)>m_set;
			std::function<std::string()>m_get;

			void set(std::string Val) {
				if (m_set) { m_set(Val); }
				else { Value = Val; }
			}
			std::string get() {
				if (m_get) { return m_get(); }
				else { return Value; }
			}

		};
		struct Function {
			//Functions can't realy be set in language. But they can be accessed by other code!
			OBJTYPE RetType;//If function returns UNDECLARED then it returns nothing
			std::function<std::string(std::vector<std::string>&)>Eval;
			int Arity = 0;//Amount of values function recives. If this value is bigger, then was passed, missing value will be empty strings
		};
		struct Holder {
		private:
			std::variant<Variable, Function> Holding;
		public:
			bool canBeComplex = true;//True if new objects can be added to this one. False otherwise
			Holder(std::variant<Variable, Function>& Obj) { Holding = std::move(Obj); }
			Holder(Variable& Obj) : Holding(std::move(Obj)) {}
			Holder(Function& Obj) : Holding(std::move(Obj)) {}
			Holder() {}
			Holder(const Holder&& Obj) : Holding(std::move(Obj.Holding)) {}
			Holder& operator=(const Holder&& Obj) { Holding = std::move(Obj.Holding); return *this; }

			template<typename T>
			T& get() { return std::get<T>(Holding); }

			std::map<std::string, Holder> Sub;
			template<typename T>
			bool isType() const { return std::holds_alternative<T>(Holding); }

		};

		std::map<std::string, Holder> Sub;

		std::optional<std::map<std::string, Evaluator::Holder>::iterator> FindName(const std::vector<std::string>& Name);

		void ParseCommand(std::list<LogicalParser::LogicalUnit>& List);

		void CreateDefaultEnviroment();

		//Creates variable.
		bool CreateVariable(std::vector<std::string> VarName, OBJTYPE VarType, std::string defValue = "", bool CanBeAugmented = true, std::function<void(std::string)>setter = nullptr, std::function<std::string()>getter = nullptr);

		bool CreateFunction(std::vector<std::string> VarName, OBJTYPE ReturnType, std::function<std::string(std::vector<std::string>&)>Func, int Arity, bool CanBeAugmented = true);

		LexicalParser Lexical;
		LogicalParser Logical;

		std::optional<std::vector<std::string>> NameToHierarchy(std::string& name);

	public:

		void SetPrinter(std::function<void(std::string)> Printer);

		template<class ParamClass, typename T>
		bool RegisterObjectParameter(ParamClass* ObjPointer, std::string ParamName,
			const T(ParamClass::*getter)() const,
			void (ParamClass::*setter)(T),
			OBJTYPE Type,
			std::function<T(std::string&)> ValFromString = ParamToStr<T>,
			std::function<std::string(T)> ValToString = [](T val) {return ResultToStr(val); })
		{
			auto& Hierarchy = NameToHierarchy(ParamName);
			if (!Hierarchy) { return false; }

			auto& GetterFunc = std::bind(ValToString, std::bind(getter, ObjPointer));

			auto& SetterFunc = std::bind(setter, ObjPointer, std::bind(ValFromString, std::placeholders::_1));

			return CreateVariable((*Hierarchy), Type, "", true, SetterFunc, GetterFunc);

		};


		/**!
		Registers a function.
		\param[in] Funcname Name of a func, as it can be accessed.
		\param[in] ReturnType A type of result of this function call. UNDECLARED if function returns nothing. VAR cannot be returned.
		If it's a object function, then next parameter is a pointer to that object. Otherwise it's just omitted
		And finally, the function itself. Can be function of an object, free function, or std::function. Can't be std::bound result.
		If provided function use something instead of bool,int,float,std::string as a parameter, a specialization of ParamToStr in ConsoleParserHelper.h must be added
		The same goes for return parameter, but with ResultToStr.
		*/
		template<typename... Args>
		bool RegisterFunction(std::string Funcname, OBJTYPE ReturnType, Args... Functhigns)
		{
			auto& Hierarchy = NameToHierarchy(Funcname);
			if (!Hierarchy) { return false; }

			auto& Fnc = FomralizeFunction(Functhigns...);

			return 	CreateFunction((*Hierarchy), ReturnType, Fnc.first, static_cast<int>(Fnc.second));

		}

		Evaluator();
		void Parse(std::string Input);
	};
}