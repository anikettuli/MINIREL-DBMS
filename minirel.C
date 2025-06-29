// minirel.C — Main Program for MiniRel DBMS
// Initializes system components and enters the interactive command loop.

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "buf.h"
#include "catalog.h"
#include "error.h"
#include "query.h"

// global objects: database, manager, catalogs, error handler
DB db;
Error error;

BufMgr* bufMgr;
RelCatalog* relCat;
AttrCatalog* attrCat;

JoinType JoinMethod;

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " dbname" << endl;
        return 1;
    }

    if (chdir(argv[1]) < 0) {
        perror("chdir");
        exit(1);
    }

    JoinMethod = NLJoin;  // default join method
    if (argc == 3)        // alternative join method specified
    {
        if (strcmp(argv[2], "SM") == 0)
            JoinMethod = SMJoin;
        else if (strcmp(argv[2], "HJ") == 0)
            JoinMethod = HashJoin;
    }

    // create buffer manager
    bufMgr = new BufMgr(100);

    // open relation and attribute catalogs
    Status status;
    relCat = new RelCatalog(status);
    if (status == OK) attrCat = new AttrCatalog(status);
    if (status != OK) {
        error.print(status);
        exit(1);
    }

    cout << "Welcome to Minirel" << endl;
    cout << "    Using ";
    if (JoinMethod == NLJoin) {
        cout << "Nested Loops Join Method" << endl;
    } else if (JoinMethod == HashJoin) {
        cout << "Hash Join Method" << endl;
    } else {
        cout << "Sort Merge Join Method" << endl;
    }

    extern void parse();
    parse();

    return 0;
}
