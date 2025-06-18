// delete.C — Tuple Deletion Implementation
// Defines deletion logic for removing records from a relation's heapfile.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "catalog.h"
#include "error.h"
#include "heapfile.h"
#include "query.h"

/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string& relation, const string& attrName,
                       const Operator op, const Datatype type,
                       const char* attrValue) {
    if (relation.empty()) {
        return BADCATPARM;
    }

    Status opStatus;
    AttrDesc attrDesc;
    RID rid;
    HeapFileScan* heapScanner = nullptr;

    try {
        // Create a heap file scanner
        heapScanner = new HeapFileScan(relation, opStatus);
        if (opStatus != OK) {
            delete heapScanner;
            return opStatus;
        }

        if (attrName.empty()) {
            // Start scanning without any specific attribute
            opStatus = heapScanner->startScan(0, 0, type, nullptr, op);
        } else {
            // Retrieve attribute details
            opStatus = attrCat->getInfo(relation, attrName, attrDesc);
            if (opStatus != OK) {
                delete heapScanner;
                return opStatus;
            }

            // Start scanning based on attribute type
            switch (type) {
                case INTEGER: {
                    int intVal = atoi(attrValue);
                    opStatus = heapScanner->startScan(
                        attrDesc.attrOffset, attrDesc.attrLen, INTEGER,
                        reinterpret_cast<const char*>(&intVal), op);
                    break;
                }
                case FLOAT: {
                    float floatVal = atof(attrValue);
                    opStatus = heapScanner->startScan(
                        attrDesc.attrOffset, attrDesc.attrLen, FLOAT,
                        reinterpret_cast<const char*>(&floatVal), op);
                    break;
                }
                case STRING:
                default:
                    opStatus = heapScanner->startScan(attrDesc.attrOffset,
                                                      attrDesc.attrLen, STRING,
                                                      attrValue, op);
            }
        }

        if (opStatus != OK) {
            delete heapScanner;
            return opStatus;
        }

        // Delete records that match the scan
        while ((opStatus = heapScanner->scanNext(rid)) == OK) {
            opStatus = heapScanner->deleteRecord();
            if (opStatus != OK) {
                delete heapScanner;
                return opStatus;
            }
        }

        opStatus = heapScanner->endScan();

    } catch (const std::exception& e) {
        cerr << "Exception encountered: " << e.what() << endl;
        delete heapScanner;
        return opStatus;
    }

    delete heapScanner;
    return (opStatus == FILEEOF) ? OK : opStatus;
}
