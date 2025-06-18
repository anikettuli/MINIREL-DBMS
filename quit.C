// quit.C — Exit Command Implementation
// Defines QU_Quit to cleanly shut down the DBMS and free resources.

#include <iostream>

#include "buf.h"
#include "catalog.h"
#include "page.h"
#include "query.h"
#include "utility.h"

extern BufMgr* bufMgr;
extern RelCatalog* relCat;
extern AttrCatalog* attrCat;

//
// Closes the catalog files in preparation for shutdown.
//
// No return value.
//

void UT_Quit(void) {
    // close relcat and attrcat

    delete relCat;
    delete attrCat;

    // delete bufMgr to flush out all dirty pages

    delete bufMgr;

    exit(1);
}
