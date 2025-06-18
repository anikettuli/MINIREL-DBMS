// insert.C — Tuple Insertion Implementation
// Defines QU_Insert to insert records into a relation based on attribute list.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "catalog.h"
#include "error.h"
#include "heapfile.h"
#include "query.h"

/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string& relation, const int attrCnt,
                       const attrInfo attrList[]) {
    Status status;
    RID rid;
    AttrDesc* schemaAttr;
    int recSize = 0;
    int schemaAttrCount;
    char* recData;
    bool attrFlag;
    int intVal;
    float floatVal;

    status = attrCat->getRelInfo(relation, schemaAttrCount, schemaAttr);

    if (status != OK) {
        return status;
    }

    if (schemaAttrCount != attrCnt) {
        return BADCATPARM;
    }

    for (int i = 0; i < schemaAttrCount; i++) {
        recSize += schemaAttr[i].attrLen;
    }

    InsertFileScan ifs(relation, status);

    if (status != OK) {
        return UNIXERR;
    }

    recData = (char*)malloc(recSize);

    if (!recData) {
        return INSUFMEM;
    }

    memset(recData, 0, recSize);

    for (int ct1 = 0; ct1 < schemaAttrCount; ct1++) {
        attrFlag = false;
        for (int ct2 = 0; ct2 < attrCnt; ct2++) {
            if (strcmp(schemaAttr[ct1].attrName, attrList[ct2].attrName) == 0) {
                if (schemaAttr[ct1].attrType != attrList[ct2].attrType) {
                    free(recData);
                    return ATTRTYPEMISMATCH;
                }
                attrFlag = true;

                switch (attrList[ct2].attrType) {
                    case INTEGER:
                        intVal = atoi((char*)attrList[ct2].attrValue);
                        memcpy((char*)recData + schemaAttr[ct1].attrOffset,
                               (char*)&intVal, schemaAttr[ct1].attrLen);
                        break;

                    case FLOAT:
                        floatVal = atof((char*)attrList[ct2].attrValue);
                        memcpy((char*)recData + schemaAttr[ct1].attrOffset,
                               (char*)&floatVal, schemaAttr[ct1].attrLen);
                        break;

                    default:
                        memcpy((char*)recData + schemaAttr[ct1].attrOffset,
                               attrList[ct2].attrValue,
                               schemaAttr[ct1].attrLen);
                        break;
                }
            }
        }

        if (!attrFlag) {
            free(recData);
            return ATTRNOTFOUND;
        }
    }

    Record recToAdd = {recData, recSize};

    status = ifs.insertRecord(recToAdd, rid);
    free(recData);

    // part 6
    return OK;
}