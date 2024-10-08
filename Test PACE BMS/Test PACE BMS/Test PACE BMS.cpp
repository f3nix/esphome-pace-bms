// Test PACE BMS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <sstream>
#include "..\..\components\pace_bms\pace_bms_protocol_v25.h"


std::ostringstream error;
std::ostringstream warning;
std::ostringstream info;
std::ostringstream debug;
std::ostringstream verbose;
std::ostringstream veryVerbose;

void ErrorLogFunc(std::string message)
{
	std::cout << "    Error: " << message << std::endl;
	error << message << std::endl;
}

void WarningLogFunc(std::string message)
{
	std::cout << "    Warning: " << message << std::endl;
	warning << message << std::endl;
}

void InfoLogFunc(std::string message)
{
	std::cout << "    Info: " << message << std::endl;
	info << message << std::endl;
}

void DebugLogFunc(std::string message)
{
	std::cout << "    Debug: " << message << std::endl;
	debug << message << std::endl;
}

void VerboseLogFunc(std::string message)
{
	std::cout << "    Verbose: " << message << std::endl;
	verbose << message << std::endl;
}

void VeryVerboseLogFunc(std::string message)
{
	std::cout << "    VeryVerbose: " << message << std::endl;
	veryVerbose << message << std::endl;
}

void BasicTests()
{
	PaceBmsProtocolV25* paceBms = new PaceBmsProtocolV25(PaceBmsProtocolV25::CID1_LithiumIron, &ErrorLogFunc, &WarningLogFunc, &InfoLogFunc, &DebugLogFunc, &VerboseLogFunc, &VeryVerboseLogFunc);
	std::vector<uint8_t> buffer;
	bool res;

	// ============================================================================
	// 
	// Main "Realtime Monitoring" tab of PBmsTools 2.4
	// These are the commands sent in a loop to fill out the display
	// 
	// ============================================================================

	// ==== Read Analog Information
	// 0 Responding Bus Id
	// 1 Cell Count (this example has 16 cells)
	// 2 Cell Voltage (repeated Cell Count times) - stored as v * 1000, so 56 is 56000
	// 3 Temperature Count (this example has 6 temperatures)
	// 4 Temperature (repeated Temperature Count times) - stored as (value * 10) + 2730, to decode (value - 2730) / 10.0 = value
	// 5 Current - stored as value * 100
	// 6 Total Voltage - stored as value * 1000
	// 7 Remaining Capacity - stored as value * 100
	// 8 [Constant] = 03
	// 9 Full Capacity - stored as value * 100
	// 0 Cycle Count
	// 1 Design Capacity - stored as value * 100
	// req:   ~25014642E00201FD30.
	// resp:  ~25014600F07A0001100CC70CC80CC70CC70CC70CC50CC60CC70CC70CC60CC70CC60CC60CC70CC60CC7060B9B0B990B990B990BB30BBCFF1FCCCD12D303286A008C2710E1E4.
	//                     00001122222222222222222222222222222222222222222222222222222222222222223344444444444444444444444455556666777788999900001111

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	int exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadAnalogInformationRequestV25);
	paceBms->CreateReadAnalogInformationRequest(1, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadAnalogInformationRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadAnalogInformationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadAnalogInformationRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadAnalogInformationRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadAnalogInformationResponseV25);
	PaceBmsProtocolV25::AnalogInformation analogInfo;
	res = paceBms->ProcessReadAnalogInformationResponse(
		1,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadAnalogInformationResponseV25,
			PaceBmsProtocolV25::exampleReadAnalogInformationResponseV25 + exlen),
		analogInfo);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadAnalogInformationResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadAnalogInformationResponse returned false" << std::endl;
	}
	else if (
		analogInfo.cellCount != 16 ||
		analogInfo.cellVoltagesMillivolts[0] != 3271 ||
		analogInfo.cellVoltagesMillivolts[1] != 3272 ||
		analogInfo.cellVoltagesMillivolts[2] != 3271 ||
		analogInfo.cellVoltagesMillivolts[3] != 3271 ||
		analogInfo.cellVoltagesMillivolts[4] != 3271 ||
		analogInfo.cellVoltagesMillivolts[5] != 3269 ||
		analogInfo.cellVoltagesMillivolts[6] != 3270 ||
		analogInfo.cellVoltagesMillivolts[7] != 3271 ||
		analogInfo.cellVoltagesMillivolts[8] != 3271 ||
		analogInfo.cellVoltagesMillivolts[9] != 3270 ||
		analogInfo.cellVoltagesMillivolts[10] != 3271 ||
		analogInfo.cellVoltagesMillivolts[11] != 3270 ||
		analogInfo.cellVoltagesMillivolts[12] != 3270 ||
		analogInfo.cellVoltagesMillivolts[13] != 3271 ||
		analogInfo.cellVoltagesMillivolts[14] != 3270 ||
		analogInfo.cellVoltagesMillivolts[15] != 3271 ||
		analogInfo.temperatureCount != 6 ||
		analogInfo.temperaturesTenthsCelcius[0] != 241 ||
		analogInfo.temperaturesTenthsCelcius[1] != 239 ||
		analogInfo.temperaturesTenthsCelcius[2] != 239 ||
		analogInfo.temperaturesTenthsCelcius[3] != 239 ||
		analogInfo.temperaturesTenthsCelcius[4] != 265 ||
		analogInfo.temperaturesTenthsCelcius[5] != 274 ||
		analogInfo.currentMilliamps != -2250 ||
		analogInfo.totalVoltageMillivolts != 52429 ||
		analogInfo.remainingCapacityMilliampHours != 48190 ||
		analogInfo.fullCapacityMilliampHours != 103460 ||
		analogInfo.cycleCount != 140 ||
		analogInfo.designCapacityMilliampHours != 100000 ||
		analogInfo.SoC != 46.5783882f ||
		analogInfo.SoH != 100.0f ||
		analogInfo.powerWatts != -117.965248f ||
		analogInfo.minCellVoltageMillivolts != 3269 ||
		analogInfo.maxCellVoltageMillivolts != 3272 ||
		analogInfo.avgCellVoltageMillivolts != 3270 ||
		analogInfo.maxCellDifferentialMillivolts != 3
		)
	{
		std::cout << "FAIL: ProcessReadAnalogInformationResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadAnalogInformationResponse" << std::endl;
	}

	// ==== Read Status Information
	// 0 Responding Bus Id
	// 1 Cell Count (this example has 16 cells)
	// 2 Cell Warning (repeated Cell Count times) see: DecodeWarningValue
	// 3 Temperature Count (this example has 6 temperatures)
	// 4 Temperature Warning (repeated Temperature Count times) see: DecodeWarningValue
	// 5 Charge Current Warning see: DecodeWarningValue
	// 6 Total Voltage Warning see: DecodeWarningValue
	// 7 Discharge Current Warning see: DecodeWarningValue
	// 8 Protection Status 1 see: DecodeProtectionStatus1Value
	// 9 Protection Status 2 see: DecodeProtectionStatus2Value
	// 0 System Status see: DecodeSystemStatusValue
	// 1 Configuration Status see: DecodeConfigurationStatusValue
	// 2 Fault Status see: DecodeFaultStatusValue
	// 3 Balance Status (high byte) set bits indicate those cells are balancing
	// 4 Balance Status (low byte) set bits indicate those cells are balancing
	// 5 Warning Status 1 see: DecodeWarningStatus1Value
	// 6 Warning Status 2 see: DecodeWarningStatus2Value
	// req:   ~25014644E00201FD2E.
	// resp:  ~25014600004C000110000000000000000000000000000000000600000000000000000000000E000000000000EF3A.
	//                     0000112222222222222222222222222222222233444444444444556677889900112233445566

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadStatusInformationRequestV25);
	paceBms->CreateReadStatusInformationRequest(1, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadStatusInformationRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadStatusInformationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadStatusInformationRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadStatusInformationRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadStatusInformationResponseV25);
	PaceBmsProtocolV25::StatusInformation statusInformation;
	res = paceBms->ProcessReadStatusInformationResponse(
		1,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadStatusInformationResponseV25,
			PaceBmsProtocolV25::exampleReadStatusInformationResponseV25 + exlen),
		statusInformation);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadStatusInformationResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadStatusInformationResponse returned false" << std::endl;
	}
	else if (statusInformation.warningText.length() != 0 || statusInformation.balancingText.length() != 0 || statusInformation.systemText.compare("Discharging; Discharge MOSFET On; Charge MOSFET On") != 0 || statusInformation.configurationText.length() != 0 || statusInformation.protectionText.length() != 0 || statusInformation.faultText.length() != 0)
	{
		std::cout << "FAIL: ProcessReadStatusInformationResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadStatusInformationResponse" << std::endl;
	}

	// ==== Read Hardware Version
	// 1 Hardware Version string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 'actual character' bytes (40 ASCII hex chars)
	// req:   ~250146C10000FD9A.
	// resp:  ~25014600602850313653313030412D313831322D312E30302000F58E.
	//                     1111111111111111111111111111111111111111

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadHardwareVersionRequestV25);
	paceBms->CreateReadHardwareVersionRequest(1, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadHardwareVersionRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadHardwareVersionRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadHardwareVersionRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadHardwareVersionRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadHardwareVersionResponseV25);
	std::string versionText;
	res = paceBms->ProcessReadHardwareVersionResponse(
		1,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadHardwareVersionResponseV25,
			PaceBmsProtocolV25::exampleReadHardwareVersionResponseV25 + exlen),
		versionText);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadHardwareVersionResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadHardwareVersionResponse returned false" << std::endl;
	}
	else if (versionText.compare("P16S100A-1812-1.00") != 0)
	{
		std::cout << "FAIL: ProcessReadHardwareVersionResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadHardwareVersionResponse" << std::endl;
	}

	// ==== Read Serial Number
	// 1 Serial Number string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 or 40 'actual character' bytes (40 or 80 ASCII hex chars)
	// req:   ~250146C20000FD99.
	// resp:  ~25014600B05031383132313031333830333039442020202020202020202020202020202020202020202020202020EE0F.
	//                     11111111111111111111111111111111111111111111111111111111111111111111111111111111

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadSerialNumberRequestV25);
	paceBms->CreateReadSerialNumberRequest(1, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadSerialNumberRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadSerialNumberRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadSerialNumberRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadSerialNumberRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadSerialNumberResponseV25);
	std::string serialText;
	res = paceBms->ProcessReadSerialNumberResponse(
		1,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadSerialNumberResponseV25,
			PaceBmsProtocolV25::exampleReadSerialNumberResponseV25 + exlen),
		serialText);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadSerialNumberResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadSerialNumberResponse returned false" << std::endl;
	}
	else if (serialText.compare("1812101380309D") != 0)
	{
		std::cout << "FAIL: ProcessReadSerialNumberResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadSerialNumberResponse" << std::endl;
	}

	// ============================================================================
	// 
	// Main "Realtime Monitoring" tab of PBmsTools 2.4
	// These are in the "Switch Control" section
	// 
	// ============================================================================

	// ==== Sound Alarm Switch
	// 1: The "on/off" switch command, see: enum SwitchCommand
	// open:  ~25004699E0020DFD12.
	//                     11
	// resp:  ~25004600C0040D01FCC3.
	//                     11??
	// close: ~25004699E0020CFD13.
	//                     11
	// resp:  ~25004600C0040C00FCC5.
	//                     11??

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	PaceBmsProtocolV25::SwitchCommand command = PaceBmsProtocolV25::SC_DisableBuzzer;
	std::string commandAsText = "SC_DisableBuzzer";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDisableBuzzerSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteDisableBuzzerSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDisableBuzzerSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteDisableBuzzerSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteDisableBuzzerSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	command = PaceBmsProtocolV25::SC_EnableBuzzer;
	commandAsText = "SC_EnableBuzzer";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteEnableBuzzerSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteEnableBuzzerSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteEnableBuzzerSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteEnableBuzzerSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteEnableBuzzerSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	// ==== LED Alarm Switch
	// 1: The "on/off" switch command, see: enum SwitchCommand
	// open:  ~25004699E00206FD20.
	//                     11
	// resp:  ~25004600C0040602FCD0.
	//                     11??
	// close: ~25004699E00207FD1F.
	//                     11
	// resp:  ~25004600C0040722FCCD.
	//                     11??

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	command = PaceBmsProtocolV25::SC_DisableLedWarning;
	commandAsText = "SC_DisableLedWarning";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDisableLedWarningSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteDisableLedWarningSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDisableLedWarningSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteDisableLedWarningSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteDisableLedWarningSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	command = PaceBmsProtocolV25::SC_EnableLedWarning;
	commandAsText = "SC_EnableLedWarning";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteEnableLedWarningSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteEnableLedWarningSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteEnableLedWarningSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteEnableLedWarningSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteEnableLedWarningSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	// ==== Charge Current Limiter Switch
	// 1: The "on/off" switch command, see: enum SwitchCommand
	// open:  ~25004699E0020AFD15.
	//                     11
	// resp:  ~25004600C0040A22FCC3.
	//                     11??
	// close: ~25004699E0020BFD14.
	//                     11
	// resp:  ~25004600C0040B32FCC1.
	//                     11??

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	command = PaceBmsProtocolV25::SC_DisableChargeCurrentLimiter;
	commandAsText = "SC_DisableChargeCurrentLimiter";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	command = PaceBmsProtocolV25::SC_EnableChargeCurrentLimiter;
	commandAsText = "SC_EnableChargeCurrentLimiter";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	// note: this is actually in the "System Configuration" section of PBmsTools 2.4 but logically belongs here and uses the same CID2 as the other switch commands
	// ==== Charge Current Limiter Current Limit Gear Switch
	// 1: The "low/high" switch command, see: enum SwitchCommand
	// low:   ~25004699E00209FD1D.
	//                     11
	// resp:  ~25004600C0040938FCC4.
	//                     11??
	// high:  ~25004699E00208FD1E.
	//                     11
	// resp:  ~25004600C0040830FCCD.
	//                     11??

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	command = PaceBmsProtocolV25::SC_SetChargeCurrentLimiterCurrentLimitHighGear;
	commandAsText = "SC_SetChargeCurrentLimiterCurrentLimitHighGear";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	command = PaceBmsProtocolV25::SC_SetChargeCurrentLimiterCurrentLimitLowGear;
	commandAsText = "SC_SetChargeCurrentLimiterCurrentLimitLowGear";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandRequestV25);
	paceBms->CreateWriteSwitchCommandRequest(0, command, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSwitchCommandRequest (" + commandAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSwitchCommandRequest (" + commandAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandResponseV25);
	res = paceBms->ProcessWriteSwitchCommandResponse(
		0,
		command,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSwitchCommandResponse (" + commandAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSwitchCommandResponse (" + commandAsText + ")" << std::endl;
	}

	// ==== Charge MOSFET Switch
	// 1: The "on/off" state, see: enum MosfetState
	// open:  ~2500469AE00200FD1E.
	//                     11
	// resp:  ~25004600E00226FD30.
	//                     ??
	// close: ~2500469AE00201FD1D.
	//                     11
	// resp:  ~25004600E00224FD32.
	//                     ??

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	PaceBmsProtocolV25::MosfetType type = PaceBmsProtocolV25::MT_Charge;
	std::string typeAsText = "MT_Charge";
	PaceBmsProtocolV25::MosfetState state = PaceBmsProtocolV25::MS_Open;
	std::string stateAsText = "MS_Open";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetChargeOpenSwitchCommandRequestV25);
	paceBms->CreateWriteMosfetSwitchCommandRequest(0, type, state, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteMosfetChargeOpenSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetChargeOpenSwitchCommandResponseV25);
	res = paceBms->ProcessWriteMosfetSwitchCommandResponse(
		0,
		type,
		state,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteMosfetChargeOpenSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteMosfetChargeOpenSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	type = PaceBmsProtocolV25::MT_Charge;
	typeAsText = "MT_Charge";
	state = PaceBmsProtocolV25::MS_Close;
	stateAsText = "MS_Close";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetChargeCloseSwitchCommandRequestV25);
	paceBms->CreateWriteMosfetSwitchCommandRequest(0, type, state, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteMosfetChargeCloseSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetChargeCloseSwitchCommandResponseV25);
	res = paceBms->ProcessWriteMosfetSwitchCommandResponse(
		0,
		type,
		state,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteMosfetChargeCloseSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteMosfetChargeCloseSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	// ==== Discharge MOSFET Switch
	// 1: The "on/off" state, see: enum MosfetState
	// open:  ~2500469BE00200FD1D.
	//                     11
	// resp:  ~25004600E00204FD34.
	//                     ??
	// close: ~2500469BE00201FD1C.
	//                     11
	// resp:  ~25004609E00204FD2B.
	//                     ??

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	type = PaceBmsProtocolV25::MT_Discharge;
	typeAsText = "MT_Charge";
	state = PaceBmsProtocolV25::MS_Open;
	stateAsText = "MS_Open";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetDischargeOpenSwitchCommandRequestV25);
	paceBms->CreateWriteMosfetSwitchCommandRequest(0, type, state, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteMosfetDischargeOpenSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetDischargeOpenSwitchCommandResponseV25);
	res = paceBms->ProcessWriteMosfetSwitchCommandResponse(
		0,
		type,
		state,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteMosfetDischargeOpenSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteMosfetDischargeOpenSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	type = PaceBmsProtocolV25::MT_Discharge;
	typeAsText = "MT_Charge";
	state = PaceBmsProtocolV25::MS_Close;
	stateAsText = "MS_Close";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetDischargeCloseSwitchCommandRequestV25);
	paceBms->CreateWriteMosfetSwitchCommandRequest(0, type, state, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteMosfetDischargeCloseSwitchCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteMosfetSwitchCommandRequest (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteMosfetDischargeCloseSwitchCommandResponseV25);
	res = paceBms->ProcessWriteMosfetSwitchCommandResponse(
		0,
		type,
		state,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteMosfetDischargeCloseSwitchCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteMosfetDischargeCloseSwitchCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteMosfetSwitchCommandResponse (" + typeAsText + "/" + stateAsText + ")" << std::endl;
	}

	// ==== Reboot (labeled as "Shutdown" in PBmsTools, but it actually causes a reboot in my experience)
	// x: unknown payload, this may be a command code and there may be more but I'm not going to test that due to potentially unknown consequences
	// write: ~2500469CE00201FD1B.
	//                     xx
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteRebootCommandRequestV25);
	paceBms->CreateWriteShutdownCommandRequest(0, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteRebootCommandRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteRebootCommandRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteRebootCommandRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteRebootCommandRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteRebootCommandResponseV25);
	res = paceBms->ProcessWriteShutdownCommandResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteRebootCommandResponseV25,
			PaceBmsProtocolV25::exampleWriteRebootCommandResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteRebootCommandResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteRebootCommandResponse returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteRebootCommandResponse" << std::endl;
	}

	// ============================================================================
	// 
	// "Memory Information" tab of PBmsTools 2.4 
	// 
	// ============================================================================

	// ==== Read Log History
	// This appears to be a "history" table
	// I'm not sure what prompts the battery to create a "history record" entry - the number of entries per day varies from 2-6 at a glance and there is sometimes a week or two missing between records
	// My battery contained 400 records (and it's been on for over a year continuous, so I believe this is the limit)
	// The last 4 (ASCII hex digits) request payload digits are a "count up" starting at 0000 and ending at 0x0190 = 400 dec, record index is zero-based with newest first (lowest payload value)
	// I haven't decoded the response yet, but it contains
	//         Date/Time
	//         Pack Amps (-in/out)
	//         Pack Voltage
	//         Remaing Capacity (Ah)
	//         Full Capacity (Ah)
	//         MaxVolt (cell) (mV)
	//         MinVolt (cell) (mV)
	//         Alarm Type
	//         Protect Type
	//         Fault Type
	//         Cell Voltage 1-16
	//         Temperatures 1-6
	// req:   ~250046A1C004018FFCA7.
	// resp:  ~25004600709018021D020038100D970D990D9A0D990D990D970D990D980D990D980D800D980D980D990D980D98060B740B750B770B760B710B79FF6ED9D7286A286A0000000000060043FFFFFFFFDDE3.
	//            the values in this response:  
	//                2024-2-29 2:00:56 - 1.460	
	//                55.767	
	//                103.460	
	//                103.460	
	//                3482	
	//                3456				
	//                3479	3481	3482	3481	3481	3479	3481	3480	3481	3480	3456	3480	3480	3481	3480	3480	
	//                20.2	20.3	20.5	20.4	19.9	20.7
	// resp:  ~250046000000FDAF.
	//            this means "no more records available"


	// -------- NOT IMPLEMENTED --------



	// ==== System Time
	// 1 Year:   read: 2024 write: 2024 (add 2000)
	// 2 Month:  read: 08   write: 08
	// 3 Day:    read: 21   write: 20 
	// 4 Hour:   read: 05   write: 14
	// 5 Minute: read: 29   write: 15
	// 6 Second: read: 31   write: 37
	// read:  ~250046B10000FD9C.
	// resp:  ~25004600400C180815051D1FFB10.
	//                     112233445566
	// write: ~250046B2400C1808140E0F25FAFC.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadSystemTimeRequestV25);
	paceBms->CreateReadSystemDateTimeRequest(0, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadSystemTimeRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadSystemTimeRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadSystemTimeRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadSystemTimeRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadSystemTimeResponseV25);
	PaceBmsProtocolV25::DateTime dateTime;
	res = paceBms->ProcessReadSystemDateTimeResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadSystemTimeResponseV25,
			PaceBmsProtocolV25::exampleReadSystemTimeResponseV25 + exlen),
		dateTime);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadSystemTimeResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadSystemTimeResponse returned false" << std::endl;
	}
	else if (dateTime.Year != 2024 || dateTime.Month != 8 || dateTime.Day != 21 || dateTime.Hour != 5 || dateTime.Minute != 29 || dateTime.Second != 31)
	{
		std::cout << "FAIL: ProcessReadSystemTimeResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadSystemTimeResponse" << std::endl;
	}



	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSystemTimeRequestV25);
	dateTime.Year = 2024;
	dateTime.Month = 8;
	dateTime.Day = 20;
	dateTime.Hour = 14;
	dateTime.Minute = 15;
	dateTime.Second = 37;
	paceBms->CreateWriteSystemDateTimeRequest(0, dateTime, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteSystemTimeRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteSystemTimeRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteSystemTimeRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteSystemTimeRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSystemTimeResponseV25);
	res = paceBms->ProcessWriteSystemDateTimeResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteSystemTimeResponseV25,
			PaceBmsProtocolV25::exampleWriteSystemTimeResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteSystemTimeResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteSystemTimeResponse returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteSystemTimeResponse" << std::endl;
	}


	// ============================================================================
	// 
	// "Parameter Setting" tab of PBmsTools 2.4 with DIP set to address 00 
	// PBmsTools 2.4 is broken and can't address packs other than 00 for configuration
	// 
	// ============================================================================






	/*
	enum ReadConfigurationType {
		 = CID2_ReadCellOverVoltageConfiguration,
		RC_PackOverVoltage = CID2_ReadPackOverVoltageConfiguration,
		RC_CellUnderVoltage = CID2_ReadCellUnderVoltageConfiguration,
		RC_PackUnderVoltage = CID2_ReadPackUnderVoltageConfiguration,
		RC_ChargeOverCurrent = CID2_ReadChargeOverCurrentConfiguration,
		RC_DischargeSlowOverCurrent = CID2_ReadDischargeSlowOverCurrentConfiguration,
		RC_DischargeFastOverCurrent = CID2_ReadDischargeFastOverCurrentConfiguration,
		RC_ShortCircuitProtection = CID2_ReadShortCircuitProtectionConfiguration,
		RC_CellBalancing = CID2_ReadCellBalancingConfiguration,
		RC_Sleep = CID2_ReadSleepConfiguration,
		RC_FullChargeLowCharge = CID2_ReadFullChargeLowChargeConfiguration,
		RC_ChargeAndDischargeOverTemperature = CID2_ReadChargeAndDischargeOverTemperatureConfiguration,
		RC_ChargeAndDischargeUnderTemperature = CID2_ReadChargeAndDischargeUnderTemperatureConfiguration,
	};
	*/






	// ==== Cell Over Voltage Configuration
	// 1 Cell OV Alarm (V): 3.60 - stored as v * 1000, so 3.6 is 3600 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 2 Cell OV Protect (V): 3.70 - stored as v * 1000, so 3.7 is 3700 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 3 Cell OVP Release (V): 3.38  - stored as v * 1000, so 3.38 is 3380 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 4 Cell OVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
	// read:  ~250046D10000FD9A.
	// resp:  ~25004600F010010E100E740D340AFA35.
	//                     ??11112222333344 
	// write: ~250046D0F010010E100E740D340AFA21.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	PaceBmsProtocolV25::ReadConfigurationType configType = PaceBmsProtocolV25::RC_CellOverVoltage;
	std::string configTypeString = "RC_CellOverVoltage";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadCellOverVoltageConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadCellOverVoltageConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadCellOverVoltageConfigurationResponseV25);
	PaceBmsProtocolV25::CellOverVoltageConfiguration cellOverVoltageConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadCellOverVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadCellOverVoltageConfigurationResponseV25 + exlen),
		cellOverVoltageConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (cellOverVoltageConfig.AlarmMillivolts != 3600 ||
		cellOverVoltageConfig.ProtectionMillivolts != 3700 ||
		cellOverVoltageConfig.ProtectionReleaseMillivolts != 3380 ||
		cellOverVoltageConfig.ProtectionDelayMilliseconds != 1000)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationRequestV25);
	cellOverVoltageConfig.AlarmMillivolts = 3600;
	cellOverVoltageConfig.ProtectionMillivolts = 3700;
	cellOverVoltageConfig.ProtectionReleaseMillivolts = 3380;
	cellOverVoltageConfig.ProtectionDelayMilliseconds = 1000;
	paceBms->CreateWriteConfigurationRequest(0, cellOverVoltageConfig, buffer);
	std::string fixed_exampleWriteCellOverVoltageConfigurationRequestV25 = std::string(PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationRequestV25, PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationRequestV25 + exlen);
	//// "on the wire" the length checksum is calculated incorrectly, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteCellOverVoltageConfigurationRequestV25[9] = '0';
	// "on the wire" the frame checksum is calculated incorrectly as a knock-on effect of the length checksum being wrong, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteCellOverVoltageConfigurationRequestV25[31] = '3';
	//fixed_exampleWriteCellOverVoltageConfigurationRequestV25[32] = '7';
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), fixed_exampleWriteCellOverVoltageConfigurationRequestV25.data(), exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	// ==== Pack Over Voltage Configuration
	// 1 Pack OV Alarm (V): 57.6 (write: 57.61) - stored as v * 100, so 57.6 is 57600 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 2 Pack OV Protect (V): 59.2 - stored as v * 100, so 59.2 is 59200 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 3 Pack OVP Release (V): 54.0 - stored as v * 100, so 54.0 is 54000 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 4 Pack OVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
	// read:  ~250046D50000FD96.
	// resp:  ~25004600F01001E100E740D2F00AFA24.
	//                     ??11112222333344
	// write: ~250046D4F01001E10AE740D2F00AF9FB.
	//        ~250046D4001000E100E740D2F00AFA23
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_PackOverVoltage;
	configTypeString = "RC_PackOverVoltage";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadPackOverVoltageConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadPackOverVoltageConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadPackOverVoltageConfigurationResponseV25);
	PaceBmsProtocolV25::PackOverVoltageConfiguration packOverVoltageConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadPackOverVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadPackOverVoltageConfigurationResponseV25 + exlen),
		packOverVoltageConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (packOverVoltageConfig.AlarmMillivolts != 57600 ||
		packOverVoltageConfig.ProtectionMillivolts != 59200 ||
		packOverVoltageConfig.ProtectionReleaseMillivolts != 54000 ||
		packOverVoltageConfig.ProtectionDelayMilliseconds != 1000)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationRequestV25);
	packOverVoltageConfig.AlarmMillivolts = 57610;
	packOverVoltageConfig.ProtectionMillivolts = 59200;
	packOverVoltageConfig.ProtectionReleaseMillivolts = 54000;
	packOverVoltageConfig.ProtectionDelayMilliseconds = 1000;
	paceBms->CreateWriteConfigurationRequest(0, packOverVoltageConfig, buffer);
	std::string fixed_exampleWritePackOverVoltageConfigurationRequestV25 = std::string(PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationRequestV25, PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationRequestV25 + exlen);
	//// "on the wire" the length checksum is calculated incorrectly, fix it so it matches what our (correct) code generates
	//fixed_exampleWritePackOverVoltageConfigurationRequestV25[9] = '0';
	// "on the wire" the frame checksum is calculated incorrectly as a knock-on effect of the length checksum being wrong, fix it so it matches what our (correct) code generates
	//fixed_exampleWritePackOverVoltageConfigurationRequestV25[30] = 'A';
	//fixed_exampleWritePackOverVoltageConfigurationRequestV25[31] = '1';
	//fixed_exampleWritePackOverVoltageConfigurationRequestV25[32] = '1';
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), fixed_exampleWritePackOverVoltageConfigurationRequestV25.data(), exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}


	// ==== Cell Under Voltage Configuration
	// 1 Cell UV Alarm (V): 2.8 - stored as v * 100, so 2.8 is 2800 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
	// 2 Cell UV Protect (V): 2.5 - stored as v * 100, so 2.5 is 2500 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
	// 3 Cell UVP Release (V): 2.9 - stored as v * 100, so 2.9 is 2900 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
	// 4 Cell UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
	// read:  ~250046D30000FD98.
	// resp:  ~25004600F010010AF009C40B540AFA24.
	//                     ??11112222333344
	// write: ~250046D2F010010AF009C40B540AFA0E.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_CellUnderVoltage;
	configTypeString = "RC_CellUnderVoltage";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadCellUnderVoltageConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadCellUnderVoltageConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadCellUnderVoltageConfigurationResponseV25);
	PaceBmsProtocolV25::CellUnderVoltageConfiguration cellUnderVoltageConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadCellUnderVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadCellUnderVoltageConfigurationResponseV25 + exlen),
		cellUnderVoltageConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (cellUnderVoltageConfig.AlarmMillivolts != 2800 ||
		cellUnderVoltageConfig.ProtectionMillivolts != 2500 ||
		cellUnderVoltageConfig.ProtectionReleaseMillivolts != 2900 ||
		cellUnderVoltageConfig.ProtectionDelayMilliseconds != 1000)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationRequestV25);
	cellUnderVoltageConfig.AlarmMillivolts = 2800;
	cellUnderVoltageConfig.ProtectionMillivolts = 2500;
	cellUnderVoltageConfig.ProtectionReleaseMillivolts = 2900;
	cellUnderVoltageConfig.ProtectionDelayMilliseconds = 1000;
	paceBms->CreateWriteConfigurationRequest(0, cellUnderVoltageConfig, buffer);
	std::string fixed_exampleWriteCellUnderVoltageConfigurationRequestV25 = std::string(PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationRequestV25, PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationRequestV25 + exlen);
	//// "on the wire" the length checksum is calculated incorrectly, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteCellUnderVoltageConfigurationRequestV25[9] = '0';
	// "on the wire" the frame checksum is calculated incorrectly as a knock-on effect of the length checksum being wrong, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteCellUnderVoltageConfigurationRequestV25[31] = '2';
	//fixed_exampleWriteCellUnderVoltageConfigurationRequestV25[32] = '4';
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), fixed_exampleWriteCellUnderVoltageConfigurationRequestV25.data(), exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}


	// ==== Pack Under Voltage Configuration
	// 1 Pack UV Alarm (V): 44.8 - stored as v * 100, so 44.8 is 44800 - valid range reported by PBmsTools as 15-50 in steps of 0.01
	// 2 Pack UV Protect (V): 40.0 - stored as v * 100, so 40.0 is 40000 - valid range reported by PBmsTools as 15-50 in steps of 0.01
	// 3 Pack UVP Release (V): 46.4 - stored as v * 100, so 46.4 is 46400 - valid range reported by PBmsTools as 15-50 in steps of 0.01
	// 4 Pack UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
	// read:  ~250046D70000FD94.
	// resp:  ~25004600F01001AF009C40B5400AFA24.
	//                     ??11112222333344
	// write: ~250046D6F01001AF009C40B5400AFA0A.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_PackUnderVoltage;
	configTypeString = "RC_PackUnderVoltage";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadPackUnderVoltageConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadPackUnderVoltageConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadPackUnderVoltageConfigurationResponseV25);
	PaceBmsProtocolV25::PackUnderVoltageConfiguration packUnderVoltageConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadPackUnderVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadPackUnderVoltageConfigurationResponseV25 + exlen),
		packUnderVoltageConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (packUnderVoltageConfig.AlarmMillivolts != 44800 ||
		packUnderVoltageConfig.ProtectionMillivolts != 40000 ||
		packUnderVoltageConfig.ProtectionReleaseMillivolts != 46400 ||
		packUnderVoltageConfig.ProtectionDelayMilliseconds != 1000)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationRequestV25);
	packUnderVoltageConfig.AlarmMillivolts = 44800;
	packUnderVoltageConfig.ProtectionMillivolts = 40000;
	packUnderVoltageConfig.ProtectionReleaseMillivolts = 46400;
	packUnderVoltageConfig.ProtectionDelayMilliseconds = 1000;
	paceBms->CreateWriteConfigurationRequest(0, packUnderVoltageConfig, buffer);
	std::string fixed_exampleWritePackUnderVoltageConfigurationRequestV25 = std::string(PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationRequestV25, PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationRequestV25 + exlen);
	//// "on the wire" the length checksum is calculated incorrectly, fix it so it matches what our (correct) code generates
	//fixed_exampleWritePackUnderVoltageConfigurationRequestV25[9] = '0';
	// "on the wire" the frame checksum is calculated incorrectly as a knock-on effect of the length checksum being wrong, fix it so it matches what our (correct) code generates
	//fixed_exampleWritePackUnderVoltageConfigurationRequestV25[31] = '2';
	//fixed_exampleWritePackUnderVoltageConfigurationRequestV25[32] = '0';
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), fixed_exampleWritePackUnderVoltageConfigurationRequestV25.data(), exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}





	// ==== Charge Over Current Configuration
	// 1 Charge OC Alarm (A): 104 - stored directly in amps - valid range reported by PBmsTools as 1-150
	// 2 Charge OC Protect (A): 110 - stored directly in amps - valid range reported by PBmsTools as 1-150
	// 3 Charge OCP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 500 to 10000 in steps of 500
	// read:  ~250046D90000FD92.
	// resp:  ~25004600400C010068006E0AFB1D.
	//                     ??1111222233
	// write: ~250046D8400C010068006E0AFB01.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_ChargeOverCurrent;
	configTypeString = "RC_ChargeOverCurrent";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeOverCurrentConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadChargeOverCurrentConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeOverCurrentConfigurationResponseV25);
	PaceBmsProtocolV25::ChargeOverCurrentConfiguration chargeOverCurrentConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadChargeOverCurrentConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadChargeOverCurrentConfigurationResponseV25 + exlen),
		chargeOverCurrentConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (chargeOverCurrentConfig.AlarmAmperage != 104 ||
		chargeOverCurrentConfig.ProtectionAmperage != 110 ||
		chargeOverCurrentConfig.ProtectionDelayMilliseconds != 1000)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeOverCurrentConfigurationRequestV25);
	chargeOverCurrentConfig.AlarmAmperage = 104;
	chargeOverCurrentConfig.ProtectionAmperage = 110;
	chargeOverCurrentConfig.ProtectionDelayMilliseconds = 1000;
	paceBms->CreateWriteConfigurationRequest(0, chargeOverCurrentConfig, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteChargeOverCurrentConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeOverCurrentConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteChargeOverCurrentConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteChargeOverCurrentConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}


	// ==== Discharge SLOW Over Current Configuration
	// 1 Discharge OC Alarm (A): 105 - stored as negative two's complement in amps***, -105 is FF97 - valid range reported by PBmsTools as 1-150
	// 2 Discharge OC 1 Protect (A): 110 - stored as negative two's complement in amps***, -110 is FF92 - valid range reported by PBmsTools as 1-150
	// 3 Discharge OC 1 Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 500 to 10000 in steps of 500
	// ********* important *********: this is returned as the negative two's complement, but is STORED (written back) as the normal positive value!
	// read:  ~250046DB0000FD89.
	// resp:  ~25004600400C01FF97FF920AFAD3.
	//                     ??1111222233
	// write: ~250046DA400C010069006E0AFAF7.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_DischargeOverCurrent1;
	configTypeString = "RC_DischargeSlowOverCurrent";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadDishargeOverCurrent1ConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadDishargeOverCurrent1ConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadDishargeOverCurrent1ConfigurationResponseV25);
	PaceBmsProtocolV25::DischargeOverCurrent1Configuration dischargeSlowOverCurrentConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadDishargeOverCurrent1ConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadDishargeOverCurrent1ConfigurationResponseV25 + exlen),
		dischargeSlowOverCurrentConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (dischargeSlowOverCurrentConfig.AlarmAmperage != 105 ||
		dischargeSlowOverCurrentConfig.ProtectionAmperage != 110 ||
		dischargeSlowOverCurrentConfig.ProtectionDelayMilliseconds != 1000)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDishargeOverCurrent1ConfigurationRequestV25);
	dischargeSlowOverCurrentConfig.AlarmAmperage = 105;
	dischargeSlowOverCurrentConfig.ProtectionAmperage = 110;
	dischargeSlowOverCurrentConfig.ProtectionDelayMilliseconds = 1000;
	paceBms->CreateWriteConfigurationRequest(0, dischargeSlowOverCurrentConfig, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteDishargeOverCurrent1ConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDishargeOverCurrent1ConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteDishargeOverCurrent1ConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteDishargeOverCurrent1ConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	// ==== Dicharge FAST Over Current Configuration
	// 1 Discharge OC 2 Protect: 150 - stored directly in amps - valid range reported by PBmsTools as 5-300 in steps of 5, but since this is an 8 bit store location, the actual max is 255????????
	// 2 Discharge OC 2 Delay Time (ms): 100 - stored in 25ms steps, so 100 is 4 (4x25=100), 400 is 16 (16x25=400) - valid range reported by PBmsTools as 100-2000 in steps of 100
	// x = apparently, garbage written by the firmware - it's not included in the PBmsTools write
	// read:  ~250046E30000FD97.
	// resp:  ~25004600400C009604009604FB32.
	//                     ??1122xxxxxx
	// write: ~250046E2A006009604FC4E.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_DischargeOverCurrent2;
	configTypeString = "RC_DischargeFastOverCurrent";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadDishargeOverCurrent2ConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadDishargeOverCurrent2ConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadDishargeOverCurrent2ConfigurationResponseV25);
	PaceBmsProtocolV25::DischargeOverCurrent2Configuration dischargeFastOverCurrentConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadDishargeOverCurrent2ConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadDishargeOverCurrent2ConfigurationResponseV25 + exlen),
		dischargeFastOverCurrentConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (dischargeFastOverCurrentConfig.ProtectionAmperage != 150 ||
		dischargeFastOverCurrentConfig.ProtectionDelayMilliseconds != 100)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDishargeOverCurrent2ConfigurationRequestV25);
	dischargeFastOverCurrentConfig.ProtectionAmperage = 150;
	dischargeFastOverCurrentConfig.ProtectionDelayMilliseconds = 100;
	paceBms->CreateWriteConfigurationRequest(0, dischargeFastOverCurrentConfig, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteDishargeOverCurrent2ConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteDishargeOverCurrent2ConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteDishargeOverCurrent2ConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteDishargeOverCurrent2ConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	// ==== Short Circuit Protection Configuration
	// 1 Delay Time (us): 300 - stored in 25 microsecond steps, 300 is 12 - valid range reported by PBmsTools as as 100-500 in steps of 50
	// read:  ~250046E50000FD95.
	// resp:  ~25004600E0020CFD25.
	//                     11
	// write: ~250046E4E0020CFD0C.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_ShortCircuitProtection;
	configTypeString = "RC_ShortCircuitProtection";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadShortCircuitProtectionConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadShortCircuitProtectionConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadShortCircuitProtectionConfigurationResponseV25);
	PaceBmsProtocolV25::ShortCircuitProtectionConfiguration shortCircuitProtectionConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadShortCircuitProtectionConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadShortCircuitProtectionConfigurationResponseV25 + exlen),
		shortCircuitProtectionConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (shortCircuitProtectionConfig.ProtectionDelayMicroseconds != 300)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteShortCircuitProtectionConfigurationRequestV25);
	shortCircuitProtectionConfig.ProtectionDelayMicroseconds = 300;
	paceBms->CreateWriteConfigurationRequest(0, shortCircuitProtectionConfig, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteShortCircuitProtectionConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteShortCircuitProtectionConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteShortCircuitProtectionConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteShortCircuitProtectionConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}


	// ==== Cell Balancing Configuration
	// 1 Balance Threshold (V): 3.4 - stored as v * 100, so 3.4 is 3400 - valid range reported by PBmsTools as 3.3-4.5 in steps of 0.01
	// 2 Balance Delta Cell (mv): 30 - stored directly, so 30 is 30 - valid range reported by PBmsTools as 20-500 in steps of 5
	// read:  ~250046B60000FD97.
	// resp:  ~2500460080080D48001EFBE9.
	//                     11112222
	// write: ~250046B580080D48001EFBD2.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_CellBalancing;
	configTypeString = "RC_CellBalancing";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadCellBalancingConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadCellBalancingConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadCellBalancingConfigurationResponseV25);
	PaceBmsProtocolV25::CellBalancingConfiguration cellBalancingConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadCellBalancingConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadCellBalancingConfigurationResponseV25 + exlen),
		cellBalancingConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (cellBalancingConfig.ThresholdMillivolts != 3400 ||
		cellBalancingConfig.DeltaCellMillivolts != 30)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteCellBalancingConfigurationRequestV25);
	cellBalancingConfig.ThresholdMillivolts = 3400;
	cellBalancingConfig.DeltaCellMillivolts = 30;
	paceBms->CreateWriteConfigurationRequest(0, cellBalancingConfig, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteCellBalancingConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteCellBalancingConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteCellBalancingConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteCellBalancingConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	// ==== Sleep Configuration
	// 1 Sleep v-cell: 3.1 - stored as v * 100, so 3.1 is 3100 - valid range reported by PBmsTools as 2-4 in steps of 0.01
	// 2 Delay Time (minute): 5 - stored directly - valid range reported by PBmsTools as 1-120
	// read:  ~250046A00000FD9E.
	// resp:  ~2500460080080C1C0005FBF3.
	//                     1111??22
	// write: ~250046A880080C1C0005FBDA.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_Sleep;
	configTypeString = "RC_Sleep";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadSleepConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadSleepConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadSleepConfigurationResponseV25);
	PaceBmsProtocolV25::SleepConfiguration sleepConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadSleepConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadSleepConfigurationResponseV25 + exlen),
		sleepConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (sleepConfig.CellMillivolts != 3100 ||
		sleepConfig.DelayMinutes != 5)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSleepConfigurationRequestV25);
	sleepConfig.CellMillivolts = 3100;
	sleepConfig.DelayMinutes = 5;
	paceBms->CreateWriteConfigurationRequest(0, sleepConfig, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteSleepConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteSleepConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteSleepConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteSleepConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	// ==== Full Charge and Low Charge
	// 1 Pack Full Charge Voltage: 56.0 - stored as v * 1000, so 56 is 56000 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 2 Pack Full Charge Current (ma): 2000 - stored directly in ma - valid range reported by PBmsTools as 500-5000 in steps of 500
	// 3 State of Charge Low Alarm (%): 5 - stored directly - valid range reported by PBmsTools as 1-100
	// read:  ~250046AF0000FD88.
	// resp:  ~25004600600ADAC007D005FB60.
	//                     1111222233
	// write: ~250046AE600ADAC007D005FB3A.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_FullChargeLowCharge;
	configTypeString = "RC_FullChargeLowCharge";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadFullChargeLowChargeConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadFullChargeLowChargeConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadFullChargeLowChargeConfigurationResponseV25);
	PaceBmsProtocolV25::FullChargeLowChargeConfiguration fullChargeLowChargeConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadFullChargeLowChargeConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadFullChargeLowChargeConfigurationResponseV25 + exlen),
		fullChargeLowChargeConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (fullChargeLowChargeConfig.FullChargeMillivolts != 56000 ||
		fullChargeLowChargeConfig.FullChargeMilliamps != 2000 ||
		fullChargeLowChargeConfig.LowChargeAlarmPercent != 5)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteFullChargeLowChargeConfigurationRequestV25);
	fullChargeLowChargeConfig.FullChargeMillivolts = 56000;
	fullChargeLowChargeConfig.FullChargeMilliamps = 2000;
	fullChargeLowChargeConfig.LowChargeAlarmPercent = 5;
	paceBms->CreateWriteConfigurationRequest(0, fullChargeLowChargeConfig, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteFullChargeLowChargeConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteFullChargeLowChargeConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteFullChargeLowChargeConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteFullChargeLowChargeConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	// ==== Charge / Discharge Over Temperature Protection Configuration
	// 1 Charge Over Temperature Alarm: 51 - stored as (value * 10) + 2730 = 3240, to decode (value - 2730) / 10.0 = 51 - valid range reported by PBmsTools as 20-100
	// 2 Charge Over Temperature Protect: 55 - stored as (value * 10) + 2730 = 3280, to decode (value - 2730) / 10.0 = 55 - valid range reported by PBmsTools as 20-100
	// 3 Charge Over Temperature Protection Release: 50 - stored as (value * 10) + 2730 = 3230, to decode (value - 2730) / 10.0 = 50 - valid range reported by PBmsTools as 20-100
	// 4 Discharge Over Temperature Alarm: 56 - stored as (value * 10) + 2730 = 3290, to decode (value - 2730) / 10.0 = 56 - valid range reported by PBmsTools as 20-100
	// 5 Discharge Over Temperature Protect: 60 - stored as (value * 10) + 2730 = 3330, to decode (value - 2730) / 10.0 = 60 - valid range reported by PBmsTools as 20-100
	// 6 Discharge Over Temperature Protect Release: 55 - stored as (value * 10) + 2730 = 3280, to decode (value - 2730) / 10.0 = 55 - valid range reported by PBmsTools as 20-100
	// read:  ~250046DD0000FD87.
	// resp:  ~25004600501A010CA80CD00C9E0CDA0D020CD0F7BE.
	//                     ??111122223333444455556666
	// write: ~250046DC501A010CA80CD00C9E0CDA0D020CD0F797.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_ChargeAndDischargeOverTemperature;
	configTypeString = "RC_ChargeAndDischargeOverTemperature";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeAndDischargeOverTemperatureConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadChargeAndDischargeOverTemperatureConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeAndDischargeOverTemperatureConfigurationResponseV25);
	PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration chargeAndDischargeOverTemperatureConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadChargeAndDischargeOverTemperatureConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadChargeAndDischargeOverTemperatureConfigurationResponseV25 + exlen),
		chargeAndDischargeOverTemperatureConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (chargeAndDischargeOverTemperatureConfig.ChargeAlarm != 51 ||
		chargeAndDischargeOverTemperatureConfig.ChargeProtection != 55 ||
		chargeAndDischargeOverTemperatureConfig.ChargeProtectionRelease != 50 ||
		chargeAndDischargeOverTemperatureConfig.DischargeAlarm != 56 ||
		chargeAndDischargeOverTemperatureConfig.DischargeProtection != 60 ||
		chargeAndDischargeOverTemperatureConfig.DischargeProtectionRelease != 55)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25);
	chargeAndDischargeOverTemperatureConfig.ChargeAlarm = 51;
	chargeAndDischargeOverTemperatureConfig.ChargeProtection = 55;
	chargeAndDischargeOverTemperatureConfig.ChargeProtectionRelease = 50;
	chargeAndDischargeOverTemperatureConfig.DischargeAlarm = 56;
	chargeAndDischargeOverTemperatureConfig.DischargeProtection = 60;
	chargeAndDischargeOverTemperatureConfig.DischargeProtectionRelease = 55;
	paceBms->CreateWriteConfigurationRequest(0, chargeAndDischargeOverTemperatureConfig, buffer);
	std::string fixed_exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25 = std::string(PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25, PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25 + exlen);
	//// "on the wire" the length checksum is calculated incorrectly, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25[9] = '6';
	// "on the wire" the frame checksum is calculated incorrectly as a knock-on effect of the length checksum being wrong, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25[42] = '6';
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), fixed_exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25.data(), exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	// ==== Charge / Discharge Under Temperature Protection Configuration   
	// 1 Charge Under Temperature Alarm: 0 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 2 Charge Under Temperature Protection: (-5) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 3 Charge Under Temperature Release: 0 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 4 Discharge Under Temperature Alarm: (-15) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 5 Discharge Under Temperature Protect: (-20) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 5 Discharge Under Temperature Release: (-15) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// read:  ~250046DF0000FD85.
	// resp:  ~25004600501A010AAA0A780AAA0A1409E20A14F7E5.
	//                     ??111122223333444455556666
	// write: ~250046DE501A010AAA0A780AAA0A1409E20A14F7BC.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	configType = PaceBmsProtocolV25::RC_ChargeAndDischargeUnderTemperature;
	configTypeString = "RC_ChargeAndDischargeUnderTemperature";

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationRequestV25);
	paceBms->CreateReadConfigurationRequest(0, configType, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationResponseV25);
	PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration chargeAndDischargeUnderTemperatureConfig;
	res = paceBms->ProcessReadConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationResponseV25,
			PaceBmsProtocolV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationResponseV25 + exlen),
		chargeAndDischargeUnderTemperatureConfig);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else if (chargeAndDischargeUnderTemperatureConfig.ChargeAlarm != 0 ||
		chargeAndDischargeUnderTemperatureConfig.ChargeProtection != -5 ||
		chargeAndDischargeUnderTemperatureConfig.ChargeProtectionRelease != 0 ||
		chargeAndDischargeUnderTemperatureConfig.DischargeAlarm != -15 ||
		chargeAndDischargeUnderTemperatureConfig.DischargeProtection != -20 ||
		chargeAndDischargeUnderTemperatureConfig.DischargeProtectionRelease != -15)
	{
		std::cout << "FAIL: ProcessReadConfigurationResponse (" + configTypeString + ") did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadConfigurationResponse (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25);
	chargeAndDischargeUnderTemperatureConfig.ChargeAlarm = 0;
	chargeAndDischargeUnderTemperatureConfig.ChargeProtection = -5;
	chargeAndDischargeUnderTemperatureConfig.ChargeProtectionRelease = 0;
	chargeAndDischargeUnderTemperatureConfig.DischargeAlarm = -15;
	chargeAndDischargeUnderTemperatureConfig.DischargeProtection = -20;
	chargeAndDischargeUnderTemperatureConfig.DischargeProtectionRelease = -15;
	paceBms->CreateWriteConfigurationRequest(0, chargeAndDischargeUnderTemperatureConfig, buffer);
	std::string fixed_exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25 = std::string(PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25, PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25 + exlen);
	//// "on the wire" the length checksum is calculated incorrectly, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25[9] = '6';
	// "on the wire" the frame checksum is calculated incorrectly as a knock-on effect of the length checksum being wrong, fix it so it matches what our (correct) code generates
	//fixed_exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25[42] = 'B';
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), fixed_exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25.data(), exlen))
	{
		std::cout << "FAIL: CreateWriteConfigurationRequest (" + configTypeString + ") created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteConfigurationRequest (" + configTypeString + ")" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationResponseV25);
	res = paceBms->ProcessWriteConfigurationResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationResponseV25,
			PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteConfigurationResponse (" + configTypeString + ") returned false" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteConfigurationResponse (" + configTypeString + ")" << std::endl;
	}




	// ????????? mystery ????????? this is sent during "Parameter Setting" tab Read/Write All but does not correspond to ANY value that I can find (and certainly not in the Parameter Setting tab) displayed in PBmsTools
	// read:  ~250046E10000FD99.
	// resp:  ~25004600200E010E2E0EF60DFCFA5D.
	//                     ??????????????
	// write: ~250046E0200E010E2E0EF60DFCFA48.
	// resp:  ~250046000000FDAF.

	// ????????? mystery ????????? this is sent during "Parameter Setting" tab Read/Write All but does not correspond to ANY value that I can find (and certainly not in the Parameter Setting tab) displayed in PBmsTools
	// read:  ~250046E70000FD93.
	// resp:  ~25004600501A0109E209B009E20D340D660D34F806.
	//                     ??????????????????????????
	// write: ~250046E6501A0109E209B009E20D340D660D34F7EB.
	// resp:  ~250046000000FDAF.


	// ============================================================================
	// 
	// "System Configuration" tab of PBmsTools 2.4
	// 
	// ============================================================================

	// note: "Charge Current Limiter Current Limit Gear Switch" is in this page in PBmsTools but I moved it to the SwitchCommand section above because it uses the same CID2 and fits in nicely with that code

	// "The charge limiter limits the charge current if it goes > 100A to 10A, this is useful for a multi-pack setup where the packs are not balanced"

	// ==== Charge Current Limiter Start Current 
	// 1 Charge Current Limiter Start Current: 100 - stored directly - valid range reported by PBmsTools as 5-150 
	// read:  ~250046ED0000FD86.
	// resp:  ~25004600C0040064FCCE.
	//                     ??11
	// write: ~250046EEC0040064FCA4.
	// resp:  ~250046000000FDAF.

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeCurrentLimiterStartCurrentRequestV25);
	paceBms->CreateReadChargeCurrentLimiterStartCurrentRequest(0, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadChargeCurrentLimiterStartCurrentRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadChargeCurrentLimiterStartCurrentRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadChargeCurrentLimiterStartCurrentRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadChargeCurrentLimiterStartCurrentRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadChargeCurrentLimiterStartCurrentResponseV25);
	uint8_t current;
	res = paceBms->ProcessReadChargeCurrentLimiterStartCurrentResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadChargeCurrentLimiterStartCurrentResponseV25,
			PaceBmsProtocolV25::exampleReadChargeCurrentLimiterStartCurrentResponseV25 + exlen),
		current);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadChargeCurrentLimiterStartCurrentResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadChargeCurrentLimiterStartCurrentResponse returned false" << std::endl;
	}
	else if (current != 100)
	{
		std::cout << "FAIL: ProcessReadChargeCurrentLimiterStartCurrentResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadChargeCurrentLimiterStartCurrentResponse" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeCurrentLimiterStartCurrentRequestV25);
	current = 100;
	paceBms->CreateWriteChargeCurrentLimiterStartCurrentRequest(0, current, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateWriteChargeCurrentLimiterStartCurrentRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleWriteChargeCurrentLimiterStartCurrentRequestV25, exlen))
	{
		std::cout << "FAIL: CreateWriteChargeCurrentLimiterStartCurrentRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateWriteChargeCurrentLimiterStartCurrentRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleWriteChargeCurrentLimiterStartCurrentResponseV25);
	res = paceBms->ProcessWriteChargeCurrentLimiterStartCurrentResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleWriteChargeCurrentLimiterStartCurrentResponseV25,
			PaceBmsProtocolV25::exampleWriteChargeCurrentLimiterStartCurrentResponseV25 + exlen));
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessWriteChargeCurrentLimiterStartCurrentResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessWriteChargeCurrentLimiterStartCurrentResponse returned false" << std::endl;
	}
	else if (current != 100)
	{
		std::cout << "FAIL: ProcessWriteChargeCurrentLimiterStartCurrentResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessWriteChargeCurrentLimiterStartCurrentResponse" << std::endl;
	}


	// ==== Read Remaining Capacity
	// 1 Remaining Capacity (mAh): 62040 - stored in 10mAh hours, so 62040 is 6204
	// 2 Actual Capacity (mAh): 103460 - stored in 10mAh hours, so 103460 is 10346
	// 3 Design Capacity (mAh): 100000 - stored in 10mAh hours, so 100000 is 10000
	// read:  ~250046A60000FD98.
	// resp:  ~25004600400C183C286A2710FB0E.
	//                     111122223333

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadRemainingCapacityRequestV25);
	paceBms->CreateReadRemainingCapacityRequest(0, buffer);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: CreateReadRemainingCapacityRequest logged something above verbose" << std::endl;
	}
	else if (buffer.size() != exlen || 0 != memcmp(buffer.data(), PaceBmsProtocolV25::exampleReadRemainingCapacityRequestV25, exlen))
	{
		std::cout << "FAIL: CreateReadRemainingCapacityRequest created a different request than the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: CreateReadRemainingCapacityRequest" << std::endl;
	}

	error.str("");
	warning.str("");
	info.str("");
	debug.str("");
	verbose.str("");
	veryVerbose.str("");

	exlen = (int)strlen((char*)PaceBmsProtocolV25::exampleReadRemainingCapacityResponseV25);
	uint32_t remaining;
	uint32_t actual;
	uint32_t design;
	res = paceBms->ProcessReadRemainingCapacityResponse(
		0,
		std::vector<uint8_t>(
			PaceBmsProtocolV25::exampleReadRemainingCapacityResponseV25,
			PaceBmsProtocolV25::exampleReadRemainingCapacityResponseV25 + exlen),
		remaining,
		actual,
		design);
	if (error.str().length() != 0 || warning.str().length() != 0 || info.str().length() != 0)
	{
		std::cout << "FAIL: ProcessReadRemainingCapacityResponse logged something above verbose" << std::endl;
	}
	else if (res != true)
	{
		std::cout << "FAIL: ProcessReadRemainingCapacityResponse returned false" << std::endl;
	}
	else if (remaining != 62040 || actual != 103460 || design != 100000)
	{
		std::cout << "FAIL: ProcessReadRemainingCapacityResponse did not accurately decode the known good example" << std::endl;
	}
	else
	{
		std::cout << "PASS: ProcessReadRemainingCapacityResponse" << std::endl;
	}

	// There are many other settings in "System Configuration" that can be written and/or calibrated here, 
	// none of which I am exposing because it would be a Very Bad Idea to mess with them
}

bool WriteSerial(HANDLE hComPort, unsigned char* buffer, int bufferLen)
{
	DWORD dwBytesWritten;
	bool success = WriteFile(hComPort, buffer, bufferLen, &dwBytesWritten, NULL);

	if (dwBytesWritten != bufferLen)
		return false;

	return true;
}

int ReadSerialUntilTerminator(HANDLE hComPort, unsigned char* buffer, int bufferLen, char terminator)
{
	int offset = 0;
	while (buffer[offset - 1] != terminator && bufferLen - offset > 0)
	{
		DWORD dwBytesRead;
		bool success = ReadFile(hComPort, buffer + offset, 1, &dwBytesRead, NULL);
		if (!success)
			return -1;
		if (dwBytesRead == 0)
			return -1;

		offset += dwBytesRead;
	}

	return offset;
}

// use with a USB to RS485 adapter
// quick and dirty, no error checks because it's throwaway
void ComPortTests(int portNum, int rs485_address)
{
	HANDLE serialHandle;
	std::string comName = std::string("\\\\.\\COM") + std::to_string(portNum);
	std::wstring comNameW = std::wstring(comName.begin(), comName.end());
	serialHandle = CreateFile(comNameW.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);
	serialParams.BaudRate = CBR_9600;
	serialParams.fBinary = TRUE;
	serialParams.Parity = FALSE;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.fRtsControl = RTS_CONTROL_TOGGLE;
	SetCommState(serialHandle, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 5;
	timeout.ReadTotalTimeoutConstant = 5;
	timeout.ReadTotalTimeoutMultiplier = 5;
	timeout.WriteTotalTimeoutConstant = 5;
	timeout.WriteTotalTimeoutMultiplier = 1;
	SetCommTimeouts(serialHandle, &timeout);

	SetCommMask(serialHandle, EV_RXCHAR);


	PaceBmsProtocolV25* paceBms = new PaceBmsProtocolV25(PaceBmsProtocolV25::CID1_LithiumIron, &ErrorLogFunc, &WarningLogFunc, &InfoLogFunc, &DebugLogFunc, &VerboseLogFunc, &VeryVerboseLogFunc);

	/*
	ZeroMemory(buffer, bufferLen);
	signed short srv = paceBms->CreateAnalogInformationRequest(rs485_address, buffer, bufferLen);
	buffer[srv] = 0;
	if (!WriteSerial(serialHandle, buffer, (int)strlen((char*)buffer)))
	{
		std::cout << "Unable to write to serial" << std::endl;
		return;
	}

	ZeroMemory(buffer, bufferLen);
	int bytesRead = 0;
	bytesRead = ReadSerialUntilTerminator(serialHandle, buffer, bufferLen, '\r');
	if(bytesRead <= 0)
	{
		std::cout << "Unable to read from serial" << std::endl;
		return;
	}

	buffer[bytesRead] = 0;
	PaceBms::TheAnalogInformation analogInfo;
	bool brv = paceBms->ProcessAnalogInformationResponse(rs485_address, buffer, bytesRead, &analogInfo);
	if (brv == false)
	{
		std::cout << "ProcessAnalogInformationResponse returned false" << std::endl;
	}

	std::cout << "Create/ProcessAnalogInformation over COM port OK" << std::endl;


	ZeroMemory(buffer, bufferLen);
	srv = paceBms->CreateStatusInformationRequest(rs485_address, buffer, bufferLen);
	buffer[srv] = 0;
	if (!WriteSerial(serialHandle, buffer, (int)strlen((char*)buffer)))
	{
		std::cout << "Unable to write to serial" << std::endl;
		return;
	}

	ZeroMemory(buffer, bufferLen);
	bytesRead = 0;
	bytesRead = ReadSerialUntilTerminator(serialHandle, buffer, bufferLen, '\r');
	if (bytesRead <= 0)
	{
		std::cout << "Unable to read from serial" << std::endl;
		return;
	}

	std::string warningText;
	std::string balancingText;
	std::string systemText;
	std::string configurationText;
	std::string protectionText;
	std::string faultText;
	brv = paceBms->ProcessStatusInformationResponse(rs485_address, buffer, bytesRead, warningText, balancingText, systemText, configurationText, protectionText, faultText);
	if (brv == false)
	{
		std::cout << "ProcessWarningInformationResponse returned false" << std::endl;
	}

	std::cout << "live warning status text: " << warningText << std::endl;
	std::cout << "live balancing status text: " << balancingText << std::endl;
	std::cout << "live system status text: " << systemText << std::endl;
	std::cout << "live configuration status text: " << configurationText << std::endl;
	std::cout << "live protection status text: " << protectionText << std::endl;
	std::cout << "live fault status text: " << faultText << std::endl;
	std::cout << "Create/ProcessWarningInformation over COM port OK" << std::endl;


	ZeroMemory(buffer, bufferLen);
	srv = paceBms->CreateHardwareVersionRequest(rs485_address, buffer, bufferLen);
	buffer[srv] = 0;
	if (!WriteSerial(serialHandle, buffer, (int)strlen((char*)buffer)))
	{
		std::cout << "Unable to write to serial" << std::endl;
		return;
	}

	ZeroMemory(buffer, bufferLen);
	bytesRead = 0;
	bytesRead = ReadSerialUntilTerminator(serialHandle, buffer, bufferLen, '\r');
	if (bytesRead <= 0)
	{
		std::cout << "Unable to read from serial" << std::endl;
		return;
	}

	std::string hardwareVersion;
	brv = paceBms->ProcessHardwareVersionResponse(rs485_address, buffer, bytesRead, hardwareVersion);
	if (brv == false)
	{
		std::cout << "ProcessHardwareVersionResponse returned false" << std::endl;
	}

	std::cout << "live hardware version: " << hardwareVersion << std::endl;
	std::cout << "Create/ProcessHardwareVersion over COM port OK" << std::endl;


	ZeroMemory(buffer, bufferLen);
	srv = paceBms->CreateSerialNumberRequest(rs485_address, buffer, bufferLen);
	buffer[srv] = 0;
	if (!WriteSerial(serialHandle, buffer, (int)strlen((char*)buffer)))
	{
		std::cout << "Unable to write to serial" << std::endl;
		return;
	}

	ZeroMemory(buffer, bufferLen);
	bytesRead = 0;
	bytesRead = ReadSerialUntilTerminator(serialHandle, buffer, bufferLen, '\r');
	if (bytesRead <= 0)
	{
		std::cout << "Unable to read from serial" << std::endl;
		return;
	}

	std::string productInformation;
	brv = paceBms->ProcessSerialNumberResponse(rs485_address, buffer, bytesRead, productInformation);
	if (brv == false)
	{
		std::cout << "ProcessSerialNumberResponse returned false" << std::endl;
	}

	std::cout << "live serial number: " << productInformation << std::endl;
	std::cout << "Create/ProcessSerialNumber over COM port OK" << std::endl;
	*/





	//ZeroMemory(buffer, bufferLen);
	//srv = paceBms->Create_unknown1_Request(rs485_address, buffer, bufferLen);
	//buffer[srv] = 0;
	//if (!WriteSerial(serialHandle, buffer, (int)strlen((char*)buffer)))
	//{
	//    std::cout << "Unable to write to serial" << std::endl;
	//    return;
	//}

	//ZeroMemory(buffer, bufferLen);
	//bytesRead = 0;
	//bytesRead = ReadSerialUntilTerminator(serialHandle, buffer, bufferLen, '\r');
	//if (bytesRead <= 0)
	//{
	//    std::cout << "Unable to read from serial" << std::endl;
	//    return;
	//}


	//ZeroMemory(buffer, bufferLen);
	//srv = paceBms->Create_unknown2_Request(rs485_address, buffer, bufferLen);
	//buffer[srv] = 0;
	//if (!WriteSerial(serialHandle, buffer, (int)strlen((char*)buffer)))
	//{
	//    std::cout << "Unable to write to serial" << std::endl;
	//    return;
	//}

	//ZeroMemory(buffer, bufferLen);
	//bytesRead = 0;
	//bytesRead = ReadSerialUntilTerminator(serialHandle, buffer, bufferLen, '\r');
	//if (bytesRead <= 0)
	//{
	//    std::cout << "Unable to read from serial" << std::endl;
	//    return;
	//}





	CloseHandle(serialHandle);
}

// temp code for forcing PBmsTools to give up it's secrets, used with a software NULL serial port loopback emulator, but I guess you could use a physical loopback too
/*void EmulatePaceBms(int portNum, int rs485_address)
{
	HANDLE serialHandle;
	std::string comName = std::string("\\\\.\\COM") + std::to_string(portNum);
	std::wstring comNameW = std::wstring(comName.begin(), comName.end());
	serialHandle = CreateFile(comNameW.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);
	serialParams.BaudRate = CBR_9600;
	serialParams.fBinary = TRUE;
	serialParams.Parity = FALSE;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.fRtsControl = RTS_CONTROL_TOGGLE;
	SetCommState(serialHandle, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 5;
	timeout.ReadTotalTimeoutConstant = 5;
	timeout.ReadTotalTimeoutMultiplier = 5;
	timeout.WriteTotalTimeoutConstant = 5;
	timeout.WriteTotalTimeoutMultiplier = 1;
	SetCommTimeouts(serialHandle, &timeout);

	SetCommMask(serialHandle, EV_RXCHAR);

	unsigned char buf[1024];
	while (true)
	{
		int read = ReadSerialUntilTerminator(serialHandle, buf, 1024, '\r');
		if (read == -1)
			continue;
		buf[read] = 0;
		printf("read:  %s\n", buf);

		// read protocol
		if (buf[7] == '9' && buf[8] == '9' ||
			buf[7] == 'E' && buf[8] == 'B')
		{
			std::string test_response("~25004600A006131400FD11\r");

			std::vector<uint8_t> vec(test_response.c_str(), test_response.c_str() + test_response.length());
			uint16_t cs = PaceBmsProtocolV25::CalculateRequestOrResponseChecksum(vec);
			uint16_t offset = 19;
			PaceBmsProtocolV25::WriteHexEncodedUShort(vec, offset, cs);
			test_response = std::string(vec.data(), vec.data() + test_response.length());

			printf("write: %s\n", test_response.c_str());
			WriteSerial(serialHandle, (unsigned char*)(test_response.c_str()), (int)test_response.length());
		}
		else
		{
			// generic command error
			printf("werr:  ~250046040000FDAB\n");
			WriteSerial(serialHandle, (unsigned char*)"~250046040000FDAB\r", 18);
		}
	}


	CloseHandle(serialHandle);
}*/

int main()
{
	BasicTests();

	//ComPortTests(8, 1);

	//EmulatePaceBms(31, 1);
}
