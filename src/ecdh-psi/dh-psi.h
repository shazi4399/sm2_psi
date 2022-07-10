/*
 * dh-psi.h
 *
 *  Created on: Jun 28, 2022
 *      Author: tcoole
 */

#ifndef DH_PSI_H_
#define DH_PSI_H_

#include "../util/typedefs.h"
#include "../util/connection.h"
#include <glib.h>
#include "../util/helpers.h"
#include <ippcp.h> /* ipp library */
#include "../util/gmCrypto/crypto.h"

void echo_hello();
uint32_t dh_psi(role_type role, uint32_t neles, uint32_t pneles, uint32_t *elebytelens, uint8_t **elements,
				uint8_t ***result, uint32_t **resbytelens, CSocket *sock, uint32_t user_id);
#endif /* DH_PSI_H_ */
