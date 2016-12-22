/*
 * dfu.h
 * Functions for handling idevices in normal mode
 *
 * Copyright (c) 2010-2013 Martin Szulecki. All Rights Reserved.
 * Copyright (c) 2012 Nikias Bassen. All Rights Reserved.
 * Copyright (c) 2010 Joshua Hill. All Rights Reserved.
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

#ifndef IDEVICERESTORE_DFU_H
#define IDEVICERESTORE_DFU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libirecovery.h>
#include "common.h"

struct dfu_client_t {
	irecv_client_t client;
	const char* ipsw;
	plist_t tss;
};

int dfu_client_new(struct idevicerestore_client_t* client);
void dfu_client_free(struct idevicerestore_client_t* client);
int dfu_check_mode(struct idevicerestore_client_t* client, int* mode);
int dfu_check_device(struct idevicerestore_client_t* client);
int dfu_send_buffer(struct idevicerestore_client_t* client, char* buffer, uint32_t size);
int dfu_send_component(struct idevicerestore_client_t* client, plist_t build_identity, const char* component);
int dfu_get_cpid(struct idevicerestore_client_t* client, unsigned int* cpid);
int dfu_get_nonce(struct idevicerestore_client_t* client, unsigned char** nonce, int* nonce_size);
int dfu_enter_recovery(struct idevicerestore_client_t* client, plist_t build_identity);


#ifdef __cplusplus
}
#endif

#endif
