/*
 * helpers.h
 *
 *  Created on: May 20, 2015
 *      Author: mzohner
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include "socket.h"
#include "typedefs.h"

#include "thread.h"
#include <glib.h>
#include <iostream>

struct snd_ctx
{
	uint8_t *snd_buf;
	uint32_t snd_bytes;
	CSocket *sock;
};

static uint32_t exchange_information(uint32_t myneles, uint32_t mybytelen, uint32_t mysecparam, uint32_t mynthreads, CSocket &sock)
{

	uint32_t pneles, pbytelen, psecparam, pnthreads, pprotocol;

	// Send own values
	sock.Send(&myneles, sizeof(uint32_t));
	sock.Send(&mybytelen, sizeof(uint32_t));
	sock.Send(&mysecparam, sizeof(uint32_t));
	sock.Send(&mynthreads, sizeof(uint32_t));

	// Receive partner values
	sock.Receive(&pneles, sizeof(uint32_t));
	sock.Receive(&pbytelen, sizeof(uint32_t));
	sock.Receive(&psecparam, sizeof(uint32_t));
	sock.Receive(&pnthreads, sizeof(uint32_t));

	// Assert
	assert(mybytelen == pbytelen);
	assert(mysecparam == psecparam);
	assert(mynthreads == pnthreads);

	return pneles;
}

static void create_result_from_matches_var_bitlen(uint8_t ***result, uint32_t **resbytelens, uint32_t *inbytelens,
												  uint8_t **inputs, uint32_t *matches, uint32_t intersect_size)
{
	uint32_t i;

	*result = (uint8_t **)malloc(sizeof(uint8_t *) * intersect_size);
	*resbytelens = (uint32_t *)malloc(sizeof(uint32_t) * intersect_size);

	std::sort(matches, matches + intersect_size);

	for (i = 0; i < intersect_size; i++)
	{
		// cout << "matches[" << i << "]: " << matches[i]  << endl;
		(*resbytelens)[i] = inbytelens[matches[i]];
		(*result)[i] = (uint8_t *)malloc((*resbytelens)[i]);
		memcpy((*result)[i], inputs[matches[i]], (*resbytelens)[i]);
	}
}

static void *send_data(void *context)
{
	snd_ctx *ctx = (snd_ctx *)context;
	ctx->sock->Send(ctx->snd_buf, ctx->snd_bytes);
	return 0;
}

static void snd_and_rcv(uint8_t *snd_buf, uint32_t snd_bytes, uint8_t *rcv_buf, uint32_t rcv_bytes, CSocket *sock)
{
	pthread_t snd_task;
	bool created, joined;
	snd_ctx ctx;

	// Start new sender thread
	ctx.sock = sock;
	ctx.snd_buf = snd_buf;
	ctx.snd_bytes = snd_bytes;
	created = !pthread_create(&snd_task, NULL, send_data, (void *)&(ctx));

	// receive
	sock->Receive(rcv_buf, rcv_bytes);
	assert(created);

	joined = !pthread_join(snd_task, NULL);
	assert(joined);
}
static void snd_and_rcv(uint8_t *snd_buf, uint32_t snd_bytes, uint8_t *rcv_buf, uint32_t rcv_bytes, CSocket *sock, role_type role)
{
	pthread_t snd_task;
	bool created, joined;
	snd_ctx ctx;

	if (CLIENT == role)
	{ // recv
		sock->Receive(rcv_buf, rcv_bytes);
	}
	else
	{
		// Start new sender thread
		ctx.sock = sock;
		ctx.snd_buf = snd_buf;
		ctx.snd_bytes = snd_bytes;
		created = !pthread_create(&snd_task, NULL, send_data, (void *)&(ctx));
		assert(created);

		joined = !pthread_join(snd_task, NULL);
		assert(joined);
	}
}

static uint32_t find_intersection(uint8_t *hashes, uint32_t neles, uint8_t *phashes, uint32_t pneles,
								  uint32_t hashbytelen, uint32_t *perm, uint32_t *matches)
{

	uint32_t *invperm = (uint32_t *)malloc(sizeof(uint32_t) * neles);
	uint64_t *tmpval, tmpkey = 0;
	uint32_t mapbytelen = min((uint32_t)hashbytelen, (uint32_t)sizeof(uint64_t));
	uint32_t size_intersect, i, intersect_ctr;
	for (i = 0; i < neles; i++)
	{
		invperm[perm[i]] = i;
	}
	GHashTable *map = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, NULL);
	for (i = 0; i < neles; i++)
	{
		memcpy(&tmpkey, hashes + i * hashbytelen, mapbytelen);
		g_hash_table_insert(map, (void *)&tmpkey, &(invperm[i]));
	}
	for (i = 0, intersect_ctr = 0; i < pneles; i++)
	{
		memcpy(&tmpkey, phashes + i * hashbytelen, mapbytelen);
		if (g_hash_table_lookup_extended(map, (void *)&tmpkey, NULL, (void **)&tmpval))
		{
			matches[intersect_ctr] = tmpval[0];
			intersect_ctr++;
			assert(intersect_ctr <= min(neles, pneles));
		}
	}
	size_intersect = intersect_ctr;

	free(invperm);
	return size_intersect;
}

#endif /* HELPERS_H_ */
