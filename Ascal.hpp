/*
 * Ascal.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#ifndef ASCAL_HPP_
#define ASCAL_HPP_
#include "AscalExecutor.hpp"
#include "keywords/PrintBoolSetting.hpp"
#include "keywords/TimeBoolSetting.hpp"
#include "keywords/ApproxIntAction.hpp"
#include "keywords/ApproxIntAction.hpp"
#include "keywords/ArcCosAction.hpp"
#include "keywords/ArcSinAction.hpp"
#include "keywords/ArcTanAction.hpp"
#include "keywords/ArrGetValAction.hpp"
#include "keywords/ArrPushValAction.hpp"
#include "keywords/ArrSetValAction.hpp"
#include "keywords/CosAction.hpp"
#include "keywords/DeleteObject.hpp"
#include "keywords/DerivefnAction.hpp"
#include "keywords/ElseAction.hpp"
#include "keywords/ExistsAction.hpp"
#include "keywords/FlushOutAction.hpp"
#include "keywords/ForRangeAction.hpp"
#include "keywords/ifAction.hpp"
#include "keywords/ImportAction.hpp"
#include "keywords/InputAction.hpp"
#include "keywords/JumpZF.hpp"
#include "keywords/JumpNZ.hpp"
#include "keywords/EvalSetZF.hpp"
#include "keywords/LenAction.hpp"
#include "keywords/LoadStrAction.hpp"
#include "keywords/MemoizeOperations.hpp"
#include "keywords/NewGlobalFunction.hpp"
#include "keywords/NewGlobalVarAction.hpp"
#include "keywords/NewLocalVar.hpp"
#include "keywords/NewLocalVarAction.hpp"
#include "keywords/PauseAction.h"
#include "keywords/PlotAction.hpp"
#include "keywords/PlotGUIAction.hpp"
#include "keywords/PrintCalculation.hpp"
#include "keywords/PrintCommand.hpp"
#include "keywords/PrintCharAction.hpp"
#include "keywords/PrintDefaultAction.hpp"
#include "keywords/PrintString.hpp"
#include "keywords/PrintStringAction.hpp"
#include "keywords/PrintVarAction.hpp"
#include "keywords/QuitAction.hpp"
#include "keywords/RandomAction.hpp"
#include "keywords/RedoAction.hpp"
#include "keywords/returnAction.hpp"
#include "keywords/RunAction.hpp"
#include "keywords/SetAction.hpp"
#include "keywords/ShowOpBoolSetting.hpp"
#include "keywords/SimplifyFunctionAction.hpp"
#include "keywords/SinAction.hpp"
#include "keywords/SleepAction.hpp"
#include "keywords/SplitStrAction.hpp"
#include "keywords/SRandomAction.hpp"
#include "keywords/TanAction.hpp"
#include "keywords/TryAction.hpp"
#include "keywords/UndoAction.hpp"
#include "keywords/WhenAction.hpp"
#include "keywords/ManualAction.hpp"
#include "keywords/WhileAction.h"
#include "Keyword.hpp"
#include <boost/pool/object_pool.hpp>
class Ascal {
private:
	AscalExecutor runtime;
public:
	template <typename keyword>
	void addKeyWord()
	{
		keyword *key = new keyword(runtime);
		runtime.addKeyWord((Keyword*) key);
	}
	Ascal(char** argv, int argc, int index):runtime(argv, argc, index, std::cin.rdbuf())
	{
		initKeywords();
	}
	Ascal(char** argv, int argc, int index, std::streambuf* inputStream):runtime(argv, argc, index, inputStream)
	{
		initKeywords();
	}
	void initKeywords()
	{
		addKeyWord<ShowOpBoolSetting>();
		addKeyWord<ManualAction>();
			addKeyWord<MemoizeOperations>();
			addKeyWord<PrintBoolSetting>();
			addKeyWord<TimeBoolSetting>();
			addKeyWord<LenAction>();
			addKeyWord<ApproxIntAction>();
			addKeyWord<ArcCosAction>();
			addKeyWord<ArcSinAction>();
			addKeyWord<ArcTanAction>();
			addKeyWord<CosAction>();
			addKeyWord<SinAction>();
			addKeyWord<TanAction>();
			addKeyWord<ArrGetValAction>();
			addKeyWord<ArrPushValAction>();
			addKeyWord<ArrSetValAction>();
			addKeyWord<DeleteObject>();
			addKeyWord<DerivefnAction>();
			addKeyWord<ElseAction>();
			addKeyWord<ExistsAction>();
			addKeyWord<ForRangeAction>();
			addKeyWord<FlushOutAction>();
			addKeyWord<IfAction>();
			addKeyWord<ImportAction>();
			addKeyWord<InputAction>();
			addKeyWord<LoadStrAction>();
			addKeyWord<NewGlobalFunction>();
			addKeyWord<NewGlobalVarAction>();
			addKeyWord<NewLocalVarAction>();
			addKeyWord<NewLocalVar>();
			addKeyWord<PauseAction>();
			addKeyWord<PlotAction>();
			addKeyWord<PlotGUIAction>();
			addKeyWord<PrintCalculation>();
			addKeyWord<PrintCommand>();
			addKeyWord<PrintCharAction>();
			addKeyWord<PrintDefaultAction>();
			addKeyWord<PrintString>();
			addKeyWord<PrintStringAction>();
			addKeyWord<PrintVarAction>();
			addKeyWord<QuitAction>();
			addKeyWord<RandomAction>();
			addKeyWord<RedoAction>();
			addKeyWord<returnAction>();
			addKeyWord<RunAction>();
			addKeyWord<SetAction>();
			addKeyWord<SimplifyFunctionAction>();
			addKeyWord<SleepAction>();
			addKeyWord<SplitStrAction>();
			addKeyWord<SRandomAction>();
			addKeyWord<TryAction>();
			addKeyWord<UndoAction>();
			addKeyWord<WhenAction>();
			addKeyWord<WhileAction>();
			addKeyWord<JumpZF>();
			addKeyWord<JumpNZ>();
			addKeyWord<EvalSetZF>();
	}
	double execExpression(std::string &&exp);
	double execExpression(std::string &exp);
	CommandLineParams& getCLParams()
	{
		return runtime.getCLParams();
	}
};

#endif /* ASCAL_HPP_ */
