/* CatenaStm32L0_LoRaWAN_commands.cpp	Fri Oct 13 2017 15:19:30 chwon */

/*

Module:  CatenaStm32L0_LoRaWAN_commands.cpp

Function:
	The command engine for lorawan commands on the Catena STM32 L0 platform

Version:
	V0.6.0	Fri Oct 13 2017 15:19:30 chwon	Edit level 1

Copyright notice:
	This file copyright (C) 2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.

	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.

Author:
	ChaeHee Won, MCCI Corporation	October 2017

Revision history:
   0.6.0  Fri Oct 13 2017 15:19:30  chwon
	Module created.

*/

#ifdef ARDUINO_ARCH_STM32

#include "CatenaStm32L0.h"

#include "Catena_Log.h"

#include <cstring>

using namespace McciCatena;

/****************************************************************************\
|
|	The command table
|
\****************************************************************************/

static cCommandStream::CommandFn doConfigure;

static const cCommandStream::cEntry sDispatchEntries[] =
	{
	{ "configure", doConfigure },
	};

static cCommandStream::cDispatch
sDispatch(sDispatchEntries, sizeof(sDispatchEntries), "lorawan");


struct KeyMap
	{
	const char *pName;
	cFramStorage::StandardKeys uKey;
	};

static KeyMap sKeyMap[] =
	{
	{ "deveui", cFramStorage::StandardKeys::kDevEUI },
	{ "appeui", cFramStorage::StandardKeys::kAppEUI },
	{ "appkey", cFramStorage::StandardKeys::kAppKey, },
	{ "nwkskey", cFramStorage::StandardKeys::kNwkSKey, },
	{ "appskey", cFramStorage::StandardKeys::kAppSKey, },
	{ "devaddr", cFramStorage::StandardKeys::kDevAddr, },
	{ "netid", cFramStorage::StandardKeys::kNetID, },
	{ "fcntup", cFramStorage::StandardKeys::kFCntUp, },
	{ "fcntdown", cFramStorage::StandardKeys::kFCntDown, },
	{ "join", cFramStorage::StandardKeys::kJoin, },
	};

/*

Name:	CatenaStm32L0::LoRaWAN::addCommands()

Function:
	Add the lorawan commands to the Catena command table.

Definition:
	private: bool CatenaStm32L0::LoRaWAN::addCommands();

Description:
	All the commands are added to the system command table.

Returns:
	true for success.

*/

bool
CatenaStm32L0::LoRaWAN::addCommands()
	{
	gLog.printf(gLog.kTrace, "CatenaStm32L0::LoRaWAN::addCommands(): adding\n");
	this->m_pCatena->addCommands(
		sDispatch, static_cast<void *>(this)
		);
	}

/*

Name:	doConfigure()

Function:
	Implement the LoRaWAN value set/get commands

Definition:
	static cCommandStream::CommandFn doConfigure;

	static cCommandStream::CommandStatus
		doConfigure(
			cCommandStream *pThis,
			void *pContext,
			int argc,
			char **argv
			);

Description:
	This function dispatches the various commands, parsing the input
	parameters if any to set the corresponding value, or displaying
	the values.

	The parsed syntax:

	lorawan configure [ {param} [ {value} ] ]

Returns:
	Command status

*/


static cCommandStream::CommandStatus
doConfigure(
	cCommandStream *pThis,
	void *pContext,
	int argc,
	char **argv
	)
	{
	CatenaStm32L0::LoRaWAN * const pLoRaWAN =
		static_cast<CatenaStm32L0::LoRaWAN *>(pContext);
	CatenaStm32L0 * const pCatena = pLoRaWAN->getCatena();
	uint8_t databuf[16];

	if (argc < 2)
		{
		// TODO(tmm@mcci.com) display values
		return cCommandStream::CommandStatus::kInvalidParameter;
		}

	const char * const pName = argv[1];
	cFram::Cursor cursor(pCatena->getFram());

	for (auto const & p : sKeyMap)
		{
		if (strcasecmp(p.pName, pName) == 0)
			{
			// matched!
			cursor.locate(p.uKey);
			}
		}

	if (! cursor.isbound())
		{
		pThis->printf(
			"%s: unknown\n",
			__func__,
			pName
			);
		return cCommandStream::CommandStatus::kInvalidParameter;
		}

	size_t size;

	size = cursor.getitemsize();
	if (size > sizeof(databuf))
		size = sizeof(databuf);

	// display
	if (argc <= 2)
		{
		if (! cursor.islocated())
			{
			pThis->printf("%s: not initialized\n", pName);
			return cCommandStream::CommandStatus::kNotInitialized;
			}
		else
			{
			char strbuf[64];

			if (! cursor.get(databuf, size))
				{
				pThis->printf("%s: read error\n", pName);
				return cCommandStream::CommandStatus::kReadError;
				}

			cursor.formatvalue(
				strbuf, sizeof(strbuf), 0,
				databuf, size
				);

			pThis->printf("%s\n", strbuf);
			return cCommandStream::CommandStatus::kSuccess;
			}
		}
	else
		{
		const char * const pValue = argv[2];

		// parse the argument according to the cursor (which
		// specifies what will receive it
		if (! cursor.parsevalue(
				pValue,
				databuf,
				size
				))
			{
			pThis->printf("%s: invalid parameter: %s\n",
				pName, pValue
				);
			return cCommandStream::CommandStatus::kInvalidParameter;
			}
		else if (! cursor.create())
			{
			pThis->printf("%s: could not create entry\n",
				pName
				);
			return cCommandStream::CommandStatus::kCreateError;
			}
		else
			{
			return cursor.put(databuf, size)
				? cCommandStream::CommandStatus::kSuccess
				: cCommandStream::CommandStatus::kWriteError
				;
			}
		}
	}

#endif // ARDUINO_ARCH_STM32

/**** end of CatenaStm32L0_LoRaWAN_commands.cpp ****/