// utility.h — Utility functions and constants used across the DBMS

#ifndef UTILITY_H
#define UTILITY_H

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <functional>
#include <iostream>

#include "error.h"
using namespace std;
#include "error.h"

// define if debug output wanted

//
// Prototypes for utility layer functions
//

const Status UT_Load(const string& relation, const string& fileName);

const Status UT_Print(string relation);

void UT_Quit(void);

#endif  // UTILITY_H
