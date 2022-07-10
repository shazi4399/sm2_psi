/*
 * demonstrator.h
 *
 *  Created on: May 20, 2015
 *      Author: mzohner
 */

#ifndef DEMONSTRATOR_H_
#define DEMONSTRATOR_H_

#include "../ecdh-psi/dh-psi.h"
#include <fstream>
#include <iostream>
#include <string>
#include "../util/parse_options.h"
#include "../util/helpers.h"

using namespace std;

//#define PRINT_INPUT_ELEMENTS

int32_t psi_demonstrator(int32_t argc, char **argv);

void read_elements(uint8_t ***elements, uint32_t **elebytelens, uint32_t *nelements, string filename, uint8_t col);

int32_t read_psi_demo_options(int32_t *argcp, char ***argvp, role_type *role, string *filename, string *address,
							  uint32_t *nelements, bool *detailed_timings, uint8_t *col, uint32_t *user_id);

#endif /* DEMONSTRATOR_H_ */
