/*
 * Ascal.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#ifndef ASCAL_HPP_
#define ASCAL_HPP_
#ifndef debug
#define NDEBUG
#endif
#include <cassert>
#include "AscalExecutor.hpp"
#include "keywords/PrintBoolSetting.hpp"
#include "keywords/TimeBoolSetting.hpp"
//#include "keywords/ApproxIntAction.hpp"
//#include "keywords/ApproxIntAction.hpp"
#include "keywords/ArcCosAction.hpp"
#include "keywords/ArcSinAction.hpp"
#include "keywords/ArcTanAction.hpp"
#include "keywords/ArrErase.hpp"
#include "keywords/ArrGetValAction.hpp"
#include "keywords/ArrPushValAction.hpp"
#include "keywords/ArrSetValAction.hpp"
#include "keywords/CosAction.hpp"
#include "keywords/DeleteObject.hpp"
//#include "keywords/DerivefnAction.hpp"
//#include "keywords/ElseAction.hpp"
//#include "keywords/ExistsAction.hpp"
//#include "keywords/FlushOutAction.hpp"
#include "keywords/ForRangeAction.hpp"
//#include "keywords/ifAction.hpp"
//#include "keywords/ImportAction.hpp"
//#include "keywords/InputAction.hpp"
//#include "keywords/JumpZF.hpp"
//#include "keywords/JumpNZ.hpp"
//#include "keywords/EvalSetZF.hpp"
#include "keywords/LenAction.hpp"
#include "keywords/IncAction.hpp"
//#include "keywords/LoadStrAction.hpp"
#include "keywords/MemoizeOperations.hpp"
#include "keywords/NewGlobalFunction.hpp"
//#include "keywords/NewGlobalVarAction.hpp"
#include "keywords/NewLocalVar.hpp"
#include "keywords/NewLocalVarAction.hpp"
#include "keywords/PauseAction.h"
//#include "keywords/PlotAction.hpp"
#include "keywords/PlotGUIAction.hpp"
//#include "keywords/PrintCalculation.hpp"
//#include "keywords/PrintCommand.hpp"
//#include "keywords/PrintCharAction.hpp"
//#include "keywords/PrintDefaultAction.hpp"
//#include "keywords/PrintString.hpp"
//#include "keywords/PrintStringAction.hpp"
//#include "keywords/PrintVarAction.hpp"
#include "keywords/QuitAction.hpp"
#include "keywords/RandomAction.hpp"
#include "keywords/WhenAction.hpp"
//#include "keywords/RedoAction.hpp"
//#include "keywords/returnAction.hpp"
//#include "keywords/RunAction.hpp"
#include "keywords/SetAction.hpp"
#include "keywords/ShowOpBoolSetting.hpp"
//#include "keywords/SimplifyFunctionAction.hpp"
#include "keywords/SinAction.hpp"
#include "keywords/SleepAction.hpp"
//#include "keywords/SplitStrAction.hpp"
#include "keywords/SRandomAction.hpp"
#include "keywords/TanAction.hpp"
//#include "keywords/TryAction.hpp"
//#include "keywords/UndoAction.hpp"
//#include "keywords/WhenAction.hpp"
//#include "keywords/ManualAction.hpp"
#include "keywords/WhileAction.h"
//#include "keywords/OperateAction.hpp"
#include "keywords/OperatePushDoubleAction.hpp"
#include "Keyword.hpp"
class Ascal {
private:
	AscalExecutor runtime;
public:
	template <typename keyword>
	void addKeyWord()
	{
		keyword *key = new keyword(runtime);//constructor of keyword uses runtime.keywordsLookupTable.size() + 1 for opCode
		runtime.addKeyWord(reinterpret_cast<Keyword*>(key));
	}
	~Ascal() {}
	Ascal(char** argv, int argc, int index): runtime(argv, argc, index, std::cin.rdbuf())
	{
		initKeywords();
        runtime.loadInitialFunctions();
	}
	Ascal(char** argv, int argc, int index, std::streambuf* inputStream): runtime(argv, argc, index, inputStream)
	{
		initKeywords();
        runtime.loadInitialFunctions();
	}
	void initKeywords()
	{
		addKeyWord<OperatePushDoubleAction>();
		//addKeyWord<OperateAction>();
		addKeyWord<ShowOpBoolSetting>();
		//addKeyWord<ManualAction>();
			addKeyWord<MemoizeOperations>();
			addKeyWord<PrintBoolSetting>();
			addKeyWord<TimeBoolSetting>();
			addKeyWord<ArcCosAction>();
			addKeyWord<ArcSinAction>();
			addKeyWord<ArcTanAction>();
			addKeyWord<IncAction>();
			addKeyWord<CosAction>();
			addKeyWord<SinAction>();
			addKeyWord<TanAction>();
			addKeyWord<LenAction>();
			addKeyWord<ArrEraseValAction>();
			addKeyWord<DeleteObject>();
			addKeyWord<ForRangeAction>();
			addKeyWord<NewLocalVarAction>();
			addKeyWord<PauseAction>();
			addKeyWord<QuitAction>();
			addKeyWord<RandomAction>();
			addKeyWord<SleepAction>();
			addKeyWord<WhileAction>();
			addKeyWord<SetAction>();
			addKeyWord<NewLocalVar>();
			addKeyWord<NewGlobalFunction>();
			addKeyWord<WhenAction>();
			addKeyWord<PlotGUIAction>();
			addKeyWord<ArrGetValAction>();
			addKeyWord<ArrPushValAction>();
			addKeyWord<ArrSetValAction>();
/*
			addKeyWord<DerivefnAction>();
			addKeyWord<ApproxIntAction>();
			addKeyWord<ElseAction>();
			addKeyWord<ExistsAction>();
			addKeyWord<FlushOutAction>();
			addKeyWord<IfAction>();
			addKeyWord<ImportAction>();
			addKeyWord<InputAction>();
			addKeyWord<LoadStrAction>();
			addKeyWord<NewGlobalVarAction>();
			addKeyWord<PlotAction>();
			addKeyWord<PrintCalculation>();
			addKeyWord<PrintCommand>();
			addKeyWord<PrintCharAction>();
			addKeyWord<PrintDefaultAction>();
			addKeyWord<PrintString>();
			addKeyWord<PrintStringAction>();
			addKeyWord<PrintVarAction>();
			addKeyWord<RedoAction>();
			addKeyWord<returnAction>();
			addKeyWord<RunAction>();
			addKeyWord<SimplifyFunctionAction>();
			addKeyWord<SplitStrAction>();
			addKeyWord<SRandomAction>();
			addKeyWord<TryAction>();
			addKeyWord<UndoAction>();
			addKeyWord<JumpZF>();
			addKeyWord<JumpNZ>();
			addKeyWord<EvalSetZF>();
			*/
	}
	double execExpression(string_view exp, AscalFrame<double>*& current);
	double execExpression(string_view exp);
	CommandLineParams& getCLParams()
	{
		return runtime.getCLParams();
	}
};

#endif /* ASCAL_HPP_ */
