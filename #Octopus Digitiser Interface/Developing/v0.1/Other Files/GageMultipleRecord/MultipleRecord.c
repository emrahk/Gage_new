/////////////////////////////////////////////////////////////////////////
//
// GageMultipleRecord
//
// See the official site at www.gage-applied.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 1994-2005 by Gage Applied Technologies.
// All rights reserved.
//
// This code is free for personal and commercial use, providing this
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// No warranty of any kind, expressed or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
/////////////////////////////////////////////////////////////////////////
//
// GageMultipleRecord demonstrates how to initialize, setup, acquire
// and transfer data using a CompuScope data acquisition system in multiple
// record mode. Data from each specified segment for all channels are then
// saved to seperate ASCII files as voltages.



//
///////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#include "CsPrototypes.h"
#include "CsAppSupport.h"
#include "CsTchar.h"
#include "CsSdkMisc.h"

//my libs
#include "time.h"

int32 TransferTimeStamp(CSHANDLE hSystem, uInt32 u32SegmentStart, uInt32 u32SegmentCount, void* pTimeStamp);

int _tmain()
{
	int32						i32Status = CS_SUCCESS;
	uInt32						u32ChannelNumber;
	TCHAR						szFileName[MAX_PATH];
	TCHAR						szFormatString[MAX_PATH];
	int64						i64Depth = 0;
	void*						pBuffer = NULL;
	float*						pVBuffer = NULL;
	int64*						pTimeStamp = NULL;
	double*						pDoubleData;
	uInt32						u32Count;
	int32						i32TickFrequency = 0;
	uInt32						u32Mode;
	CSHANDLE					hSystem = 0;
	IN_PARAMS_TRANSFERDATA		InData = {0};
	OUT_PARAMS_TRANSFERDATA		OutData = {0};
	CSSYSTEMINFO				CsSysInfo = {0};
	CSAPPLICATIONDATA			CsAppData = {0};
	FileHeaderStruct			stHeader = {0};
	CSACQUISITIONCONFIG			CsAcqCfg = {0};
	CSCHANNELCONFIG				CsChanCfg = {0};
	LPCTSTR						szIniFile = _T("MultipleRecord.ini");
	uInt32						u32ChannelIndexIncrement;
	int							nMaxSegmentNumber;
	int							nMaxChannelNumber;


//Timing things

	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );


/*
	Initializes the CompuScope boards found in the system. If the
	system is not found a message with the error code will appear.
	Otherwise i32Status will contain the number of systems found.
*/
	i32Status = CsInitialize();

	if (CS_FAILED(i32Status))
	{
		DisplayErrorString(i32Status);
		return (-1);
	}
/*
	Get System. This sample program only supports one system. If
	2 systems or more are found, the first system that is found
	will be the system that will be used. hSystem will hold a unique
	system identifier that is used when referencing the system.
*/
	i32Status = CsGetSystem(&hSystem, 0, 0, 0, 0);

	if (CS_FAILED(i32Status))
	{
		DisplayErrorString(i32Status);
		return (-1);
	}
/*
	Get System information. The u32Size field must be filled in
	 prior to calling CsGetSystemInfo
*/
	CsSysInfo.u32Size = sizeof(CSSYSTEMINFO);
	i32Status = CsGetSystemInfo(hSystem, &CsSysInfo);

/*
	Display the system name from the driver
*/
	_ftprintf(stdout, _T("\nBoard Name: %s"), CsSysInfo.strBoardName);

	i32Status = CsAs_ConfigureSystem(hSystem, (int)CsSysInfo.u32ChannelCount, 1, (LPCTSTR)szIniFile, &u32Mode);
	
	if (CS_FAILED(i32Status))
	{
		if (CS_INVALID_FILENAME == i32Status)
		{
/*
			Display message but continue on using defaults.
*/
			_ftprintf(stdout, _T("\nCannot find %s - using default parameters."), szIniFile);
		}
		else
		{	
/*
			Otherwise the call failed.  If the call did fail we should free the CompuScope
			system so it's available for another application
*/
			DisplayErrorString(i32Status);
			CsFreeSystem(hSystem);
			return(-1);
		}
	}
/*
	If the return value is greater than  1, then either the application, 
	acquisition, some of the Channel and / or some of the Trigger sections
	were missing from the ini file and the default parameters were used. 
*/
	if (CS_USING_DEFAULT_ACQ_DATA & i32Status)
		_ftprintf(stdout, _T("\nNo ini entry for acquisition. Using defaults."));

	if (CS_USING_DEFAULT_CHANNEL_DATA & i32Status)
		_ftprintf(stdout, _T("\nNo ini entry for one or more Channels. Using defaults for missing items."));

	if (CS_USING_DEFAULT_TRIGGER_DATA & i32Status)
		_ftprintf(stdout, _T("\nNo ini entry for one or more Triggers. Using defaults for missing items."));

	i32Status = CsAs_LoadConfiguration(hSystem, szIniFile, APPLICATION_DATA, &CsAppData);

	if (CS_FAILED(i32Status))
	{
		if (CS_INVALID_FILENAME == i32Status)
			_ftprintf(stdout, _T("\nUsing default application parameters."));
		else
		{
			DisplayErrorString(i32Status);
			CsFreeSystem(hSystem);
			return -1;
		}
	}
	else if (CS_USING_DEFAULT_APP_DATA & i32Status)
	{
/*
		If the return value is CS_USING_DEFAULT_APP_DATA (defined in ConfigSystem.h) 
		then there was no entry in the ini file for Application and we will use
		the application default values, which were set in CsAs_LoadConfiguration.
*/
		_ftprintf(stdout, _T("\nNo ini entry for application data. Using defaults"));
	}
/*
	Send the acqcuisition, channel and trigger parameters that we've read
	from the ini file to the hardware.
*/
/*
	Commit the values to the driver.  This is where the values get sent to the
	hardware.  Any invalid parameters will be caught here and an error returned.
*/
	i32Status = CsDo(hSystem, ACTION_COMMIT);
	if (CS_FAILED(i32Status))
	{
		DisplayErrorString(i32Status);
		CsFreeSystem(hSystem);
		return (-1);
	}
/*
	Get the current sample size, resolution and offset parameters from the driver
	by calling CsGet for the ACQUISTIONCONFIG structure. These values are used
	when saving the file.
*/
	CsAcqCfg.u32Size = sizeof(CsAcqCfg);
	i32Status = CsGet(hSystem, CS_ACQUISITION, CS_ACQUISITION_CONFIGURATION, &CsAcqCfg);
	if (CS_FAILED(i32Status))
	{
		DisplayErrorString(i32Status);
		CsFreeSystem(hSystem);
		return (-1);
	}
/*
	We need to allocate a buffer
	for transferring the data
*/
	pBuffer  = VirtualAlloc(NULL, (size_t)(CsAppData.i64TransferLength * CsAcqCfg.u32SampleSize), MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pBuffer)
	{
		_ftprintf (stderr, _T("\nUnable to allocate memory\n"));
		CsFreeSystem(hSystem);
		return (-1);
	}
/*
	We also need to allocate a buffer for transferring the timestamp
*/
	pTimeStamp = (int64 *)VirtualAlloc(NULL, (size_t)(CsAppData.u32TransferSegmentCount * sizeof(int64)), MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pTimeStamp)
	{
		_ftprintf (stderr, _T("\nUnable to allocate memory\n"));
		if (NULL != pBuffer)
			VirtualFree(pBuffer, 0, MEM_RELEASE);
		return (-1);
	}
	if (TYPE_FLOAT == CsAppData.i32SaveFormat)
	{
/*
		Allocate another buffer to pass the data that is going to be converted
		into voltages
*/
		pVBuffer  = (float *)VirtualAlloc(NULL, (size_t)(CsAppData.i64TransferLength * sizeof(float)), MEM_COMMIT, PAGE_READWRITE);
		if (NULL == pVBuffer)
		{
			_ftprintf (stderr, _T("\nUnable to allocate memory\n"));
			CsFreeSystem(hSystem);
			if (NULL != pBuffer)
				VirtualFree(pBuffer, 0, MEM_RELEASE);
			if (NULL != pTimeStamp)
				VirtualFree(pTimeStamp, 0, MEM_RELEASE);
			return (-1);
		}
	}
	pDoubleData = (double *)VirtualAlloc(NULL, (size_t)(CsAppData.u32TransferSegmentCount * sizeof(double)), MEM_COMMIT, PAGE_READWRITE);

/*
	Start the data Acquisition
*/
	i32Status = CsDo(hSystem, ACTION_START);
	if (CS_FAILED(i32Status))
	{
		DisplayErrorString(i32Status);
		CsFreeSystem(hSystem);
		return (-1);
	}
/*
	DataCaptureComplete queries the system to see when
	the capture is complete
*/
	if (!DataCaptureComplete(hSystem))
	{
		CsFreeSystem(hSystem);
		return (-1);
	}
/*
	Acquisition is now complete.
	Call the function TransferTimeStamp. This function is used to transfer the timestamp
	data. The i32TickFrequency, which is returned from this fuction, is the clock rate
	of the counter used to acquire the timestamp data.
*/
	i32TickFrequency = TransferTimeStamp(hSystem, CsAppData.u32TransferSegmentStart, CsAppData.u32TransferSegmentCount, pTimeStamp);

/*
	If TransferTimeStamp fails, i32TickFrequency will be negative,
	which represents an error code. If there is an error we'll set
	the time stamp info in pDoubleData to 0.
*/
	ZeroMemory(pDoubleData, CsAppData.u32TransferSegmentCount * sizeof(double));
	if (CS_SUCCEEDED(i32TickFrequency))
	{
	/*
		Allocate a buffer of doubles to store the the timestamp data after we have
		converted it to microseconds.
	*/
		for (u32Count = 0; u32Count < CsAppData.u32TransferSegmentCount; u32Count++)
		{
		/*
			The number of ticks that have ocurred / tick count(the number of ticks / second)
			= the number of seconds elapsed. Multiple by 1000000 to get the number of
			mircoseconds
		*/
			pDoubleData[u32Count] = (double)(* (pTimeStamp + u32Count)) * 1.e6 / (double)(i32TickFrequency);
		}
	}

/*
	Now transfer the actual acquired data for each desired multiple group.
	Fill in the InData structure for transferring the data
*/
	InData.u32Mode = TxMODE_DEFAULT;
	InData.i64StartAddress = CsAppData.i64TransferStartPosition;
	InData.i64Length =  CsAppData.i64TransferLength;
	InData.pDataBuffer = pBuffer;
	u32ChannelIndexIncrement = CsAs_CalculateChannelIndexIncrement(&CsAcqCfg, &CsSysInfo );

/*	
	format a string with the number of segments  and channels so all filenames will have
	the same number of characters.
*/
	_stprintf(szFormatString, _T("%d"), CsAppData.u32TransferSegmentStart + CsAppData.u32TransferSegmentCount - 1);
	nMaxSegmentNumber = (int)_tcslen(szFormatString);
	
	_stprintf(szFormatString, _T("%d"), CsSysInfo.u32ChannelCount);
	nMaxChannelNumber = (int)_tcslen(szFormatString);
	
	_stprintf(szFormatString, _T("%d_%%s_CH%%0%dd-%%0%dd.dat"), rawtime, nMaxChannelNumber, nMaxSegmentNumber);

	for	(u32ChannelNumber = 1; u32ChannelNumber <= CsSysInfo.u32ChannelCount; u32ChannelNumber += u32ChannelIndexIncrement)
	{
		int nMulRecGroup;
		int nTimeStampIndex;
/*
		Variable that will contain either raw data or data in Volts depending on requested format
*/
		void* pSrcBuffer = NULL;

		ZeroMemory(pBuffer,(size_t)(CsAppData.i64TransferLength * CsAcqCfg.u32SampleSize));
		InData.u16Channel = (uInt16)u32ChannelNumber;

/*
		This for loop transfers each multiple record segment to a seperate file. It also
		writes the time stamp information for the segment to the header of the file. Note
		that the timestamp array (pDoubleData) starts at index 0, even if the starting transfer
		segment is not 0. Note that the user is responsible for ensuring that the ini file
		has valid values and the segments that are being tranferred have been captured.
*/
		for (nMulRecGroup = CsAppData.u32TransferSegmentStart, nTimeStampIndex = 0; nMulRecGroup < (int)(CsAppData.u32TransferSegmentStart + CsAppData.u32TransferSegmentCount);
									nMulRecGroup++, nTimeStampIndex++)
		{
/*
			Transfer the captured data
*/
			InData.u32Segment = (uInt32)nMulRecGroup;
			i32Status = CsTransfer(hSystem, &InData, &OutData);
			if (CS_FAILED(i32Status))
			{
				DisplayErrorString(i32Status);
				if (NULL != pBuffer)
					VirtualFree(pBuffer, 0, MEM_RELEASE);
				if (NULL != pVBuffer)
					VirtualFree(pVBuffer, 0, MEM_RELEASE);
				if (NULL != pTimeStamp)
					VirtualFree(pTimeStamp, 0, MEM_RELEASE);
				if (NULL != pDoubleData)
					VirtualFree(pDoubleData, 0, MEM_RELEASE);
				CsFreeSystem(hSystem);
				return (-1);
			}
/*
		Note: to optimize the transfer loop, everything from
		this point on in the loop could be moved out and done
		after all the channels are transferred.
*/

/*
			Assign a file name for each channel that we want to save
*/
			_stprintf(szFileName, szFormatString, CsAppData.lpszSaveFileName, u32ChannelNumber, nMulRecGroup);

/*
			Gather up the information needed for the volt conversion and/or file header
*/
			CsChanCfg.u32Size = sizeof(CSCHANNELCONFIG);
			CsChanCfg.u32ChannelIndex = u32ChannelNumber;
			CsGet(hSystem, CS_CHANNEL, CS_ACQUISITION_CONFIGURATION, &CsChanCfg);

			if (TYPE_FLOAT == CsAppData.i32SaveFormat)
			{
/*
				Call the ConvertToVolts function. This function will convert the raw
				data to voltages. We pass the actual length, which will be converted
				from 0 to actual length.  Any invalid samples at the beginning are
				handled in the SaveFile routine.
*/
				i64Depth = OutData.i64ActualLength;

				i32Status = CsAs_ConvertToVolts(i64Depth, CsChanCfg.u32InputRange, CsAcqCfg.u32SampleSize,
												CsAcqCfg.i32SampleOffset, CsAcqCfg.i32SampleRes, 
												CsChanCfg.i32DcOffset, pBuffer, pVBuffer);
				if (CS_FAILED(i32Status))
				{
					DisplayErrorString(i32Status);
					continue;
				}

				pSrcBuffer = pVBuffer;
			}
			else
			{
				pSrcBuffer = pBuffer;
			}
/*
			The driver may have had to change the start address and length
			due to alignment issues, so we'll get the actual start and length
			from the driver.
*/

			stHeader.i64Start = OutData.i64ActualStart;
			stHeader.i64Length = OutData.i64ActualLength;
			stHeader.u32SampleSize = CsAcqCfg.u32SampleSize;
			stHeader.i32SampleRes = CsAcqCfg.i32SampleRes;
			stHeader.i32SampleOffset = CsAcqCfg.i32SampleOffset;
			stHeader.u32InputRange = CsChanCfg.u32InputRange;
			stHeader.i32DcOffset = CsChanCfg.i32DcOffset;
			stHeader.u32SegmentCount = CsAcqCfg.u32SegmentCount;
			stHeader.u32SegmentNumber = InData.u32Segment;
			stHeader.dTimeStamp = pDoubleData[nTimeStampIndex];

			i32Status = (int32)CsAs_SaveFile(szFileName, pSrcBuffer, CsAppData.i32SaveFormat, &stHeader);
			if ( 0 > i32Status )
			{
				if (CS_MISC_ERROR == i32Status)
				{
					_ftprintf(stderr, _T("\n"));
					_ftprintf(stderr, CsAs_GetLastFileError());
				}
				else
				{
					DisplayErrorString(i32Status);
				}
				continue;
			}
		}
	}
/*
	Free any buffers that have been allocated
*/

	if( NULL != pTimeStamp)
	{
		VirtualFree(pTimeStamp, 0, MEM_RELEASE);
		pTimeStamp = NULL;
	}

	if( NULL != pDoubleData )
	{
		VirtualFree(pDoubleData, 0, MEM_RELEASE);
		pDoubleData = NULL;
	}

	if ( NULL != pVBuffer )
	{
		VirtualFree(pVBuffer, 0, MEM_RELEASE);
		pVBuffer = NULL;
	}

	if ( NULL != pBuffer)
	{
		VirtualFree(pBuffer, 0, MEM_RELEASE);
		pBuffer = NULL;
	}
/*
	Free the CompuScope system and any resources it's been using
*/
	i32Status = CsFreeSystem(hSystem);

	DisplayFinishString();
	return 0;
}

int32 TransferTimeStamp(CSHANDLE hSystem, uInt32 u32SegmentStart, uInt32 u32SegmentCount, void* pTimeStamp)
{
	IN_PARAMS_TRANSFERDATA		InTSData = {0};
	OUT_PARAMS_TRANSFERDATA		OutTSData = {0};
	int32						i32Status = CS_SUCCESS;

	InTSData.u16Channel = 1;
	InTSData.u32Mode = TxMODE_TIMESTAMP;
	InTSData.i64StartAddress = 1;
	InTSData.i64Length = (int64)u32SegmentCount;
	InTSData.u32Segment = u32SegmentStart;

	ZeroMemory(pTimeStamp,(size_t)(u32SegmentCount * sizeof(int64)));
	InTSData.pDataBuffer = pTimeStamp;

	i32Status = CsTransfer(hSystem, &InTSData, &OutTSData);
	if (CS_FAILED(i32Status))
	{
/*
		if the error is INVALID_TRANSFER_MODE it just means that this systems
		doesn't support time stamp. We can continue on with the program.
*/
		if (CS_INVALID_TRANSFER_MODE == i32Status)
			_ftprintf (stderr, _T("\nTime stamp is not supported in this system.\n"));		
		else
			DisplayErrorString(i32Status);

		VirtualFree(pTimeStamp, 0, MEM_RELEASE);
		pTimeStamp = NULL;
		return (i32Status);
	}
/*
	Tick count is stored in OutTSData.i32LowPart.
*/
	return OutTSData.i32LowPart;
}
