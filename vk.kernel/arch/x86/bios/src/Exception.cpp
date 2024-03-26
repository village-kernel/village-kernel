//###########################################################################
// Exception.cpp
// Definitions of the functions that manage exception
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Exception.h"
#include "Hardware.h"


/// @brief Constructor
Exception::Exception()
	:debug(NULL),
	interrupt(NULL)
{
}


/// @brief Destructor
Exception::~Exception()
{
}


/// @brief Exception Setup
void Exception::Setup()
{
	//Gets the debug pointer
	debug = (Debug*)&kernel->debug;

	//Gets the interrupt pointer
	interrupt = (Interrupt*)&kernel->interrupt;

	//Set interrupt service
	interrupt->AppendISR(IRQ_Division_By_Zero,            (Method)&Exception::DivisionByZeroHandler,            this);
	interrupt->AppendISR(IRQ_Debug,                       (Method)&Exception::DebugHandler,                     this);
	interrupt->AppendISR(IRQ_Non_Maskable_Interrupt,      (Method)&Exception::NonMaskableInterruptHandler,      this);
	interrupt->AppendISR(IRQ_Breakpoint,                  (Method)&Exception::BreakpointHandler,                this);
	interrupt->AppendISR(IRQ_Into_Detected_Overflow,      (Method)&Exception::IntoDetectedOverflowHandler,      this);
	interrupt->AppendISR(IRQ_Out_Of_Bounds,               (Method)&Exception::OutOfBoundsHandler,               this);
	interrupt->AppendISR(IRQ_Invalid_Opcode,              (Method)&Exception::InvalidOpcodeHandler,             this);
	interrupt->AppendISR(IRQ_No_Coprocessor,              (Method)&Exception::NoCoprocessorHandler,             this);
	interrupt->AppendISR(IRQ_Doule_Fault,                 (Method)&Exception::DouleFaultHandler,                this);
	interrupt->AppendISR(IRQ_Coprocessor_Segment_Overrun, (Method)&Exception::CoprocessorSegmentOverrunHandler, this);
	interrupt->AppendISR(IRQ_Bad_TSS,                     (Method)&Exception::BadTSSHandler,                    this);
	interrupt->AppendISR(IRQ_Segment_Not_Present,         (Method)&Exception::SegmentNotPresentHandler,         this);
	interrupt->AppendISR(IRQ_Stack_Fault,                 (Method)&Exception::StackFaultHandler,                this);
	interrupt->AppendISR(IRQ_General_Protection_Fault,    (Method)&Exception::GeneralProtectionFaultHandler,    this);
	interrupt->AppendISR(IRQ_Page_Fault,                  (Method)&Exception::PageFaultHandler,                 this);
	interrupt->AppendISR(IRQ_Unknown_Interrupt,           (Method)&Exception::UnknownInterruptHandler,          this);
	interrupt->AppendISR(IRQ_Coprocessor_Fault,           (Method)&Exception::CoprocessorFaultHandler,          this);
	interrupt->AppendISR(IRQ_Alignment_Check,             (Method)&Exception::AlignmentCheckHandler,            this);
	interrupt->AppendISR(IRQ_Machine_Check,               (Method)&Exception::MachineCheckHandler,              this);
}


/// @brief Exception Exit
void Exception::Exit()
{

}


/// @brief DivisionByZeroHandler
void Exception::DivisionByZeroHandler()
{
	debug->Error("Division By Zero");
	while(1) {}
}


/// @brief DebugHandler
void Exception::DebugHandler()
{
	debug->Error("Debug");
	while(1) {}
}


/// @brief NonMaskableInterruptHandler
void Exception::NonMaskableInterruptHandler()
{
	debug->Error("Non Maskable Interrupt");
	while(1) {}
}


/// @brief BreakpointHandler
void Exception::BreakpointHandler()
{
	debug->Error("Breakpoint");
	while(1) {}
}


/// @brief IntoDetectedOverflowHandler
void Exception::IntoDetectedOverflowHandler()
{
	debug->Error("Into Detected Overflow");
	while(1) {}
}


/// @brief OutOfBoundsHandler
void Exception::OutOfBoundsHandler()
{
	debug->Error("Out Of Bounds");
	while(1) {}
}


/// @brief InvalidOpcodeHandler
void Exception::InvalidOpcodeHandler()
{
	debug->Error("Invalid Opcode");
	while(1) {}
}


/// @brief NoCoprocessorHandler
void Exception::NoCoprocessorHandler()
{
	debug->Error("No Coprocessor");
	while(1) {}
}


/// @brief DouleFaultHandler
void Exception::DouleFaultHandler()
{
	debug->Error("Doule Fault");
	while(1) {}
}


/// @brief CoprocessorSegmentOverrunHandler
void Exception::CoprocessorSegmentOverrunHandler()
{
	debug->Error("Coprocessor Segment Overrun");
	while(1) {}
}


/// @brief BadTSSHandler
void Exception::BadTSSHandler()
{
	debug->Error("Bad TSS");
	while(1) {}
}


/// @brief SegmentNotPresentHandler
void Exception::SegmentNotPresentHandler()
{
	debug->Error("Segment Not Present");
	while(1) {}
}


/// @brief StackFaultHandler
void Exception::StackFaultHandler()
{
	debug->Error("Stack Fault");
	while(1) {}
}


/// @brief GeneralProtectionFaultHandler
void Exception::GeneralProtectionFaultHandler()
{
	debug->Error("General Protection Fault");
	while(1) {}
}


/// @brief PageFaultHandler
void Exception::PageFaultHandler()
{
	debug->Error("Page Fault");
	while(1) {}
}


/// @brief UnknownInterruptHandler
void Exception::UnknownInterruptHandler()
{
	debug->Error("Unknown Interrupt");
	while(1) {}
}


/// @brief CoprocessorFaultHandler
void Exception::CoprocessorFaultHandler()
{
	debug->Error("Coprocessor Fault");
	while(1) {}
}


/// @brief AlignmentCheckHandler
void Exception::AlignmentCheckHandler()
{
	debug->Error("Alignment Check");
	while(1) {}
}


/// @brief MachineCheckHandler
void Exception::MachineCheckHandler()
{
	debug->Error("Machine Check");
	while(1) {}
}
