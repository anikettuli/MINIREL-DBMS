// joinHT.C — Block Nested-Loop Join Hash Table Helper
// Implements joinHashTbl: build and probe hash buckets for join operations.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "catalog.h"
#include "joinHT.h"
#include "query.h"

// joinHashTbl constructor: initialize hash table of given size and attribute
// descriptor
joinHashTbl::joinHashTbl(int size, const AttrDesc& attr) {
    HTSIZE = size;
    joinAttr = attr;
    ht = new HTentry[HTSIZE];  // allocate the hash table
    for (int i = 0; i < HTSIZE; i++) {
        ht[i].chain = NULL;
        ht[i].bucketCnt = 0;
    }
}

// joinHashTbl destructor: clean up chained buckets and allocated memory
joinHashTbl::~joinHashTbl() {
    joinhashBucket* tmpBuf;

    for (int i = 0; i < HTSIZE; i++) {
        while (ht[i].chain) {
            tmpBuf = ht[i].chain;
            if (joinAttr.attrType == STRING) delete tmpBuf->attrValue.sValue;
            ht[i].chain = ht[i].chain->next;
            delete tmpBuf;
        }
    }
    delete[] ht;
}

// hash: compute hash value for attribute pointer based on its type
int joinHashTbl::hash(const char* attrPtr, int attrType) {
    int value = 0;

    switch (attrType) {
        case INTEGER:
            value = (*(int*)attrPtr) * HTSIZE * 31;
            break;
        case FLOAT:
            value = (int)((*(float*)attrPtr) * HTSIZE * 31);
            break;
        case STRING:
            // must be a null terminated string
            while (*attrPtr++) value = 31 * value + (int)*attrPtr;
            break;
        default:
            printf("illegal type in joinHT hash\n");
            break;
    }

    value = abs(value % HTSIZE);
    return value;
}

// build: insert a record into the hash table
void joinHashTbl::build(const Record& rec, const RID& rid) {
    joinhashBucket* tmpBuc;
    char* joinAttrPtr;

    joinAttrPtr = (char*)tuple + joinAttr.attrOffset;
    int index = hash(joinAttrPtr, joinAttr.attrType);

    tmpBuc = new joinhashBucket;
    if (!tmpBuc) return HASHTBLERROR;
    tmpBuc->next = ht[index].chain;
    ht[index].chain = tmpBuc;
    ht[index].bucketCnt++;  // keep track of how many buckets on this chain

    tmpBuc->rid = newRid;
    switch (joinAttr.attrType) {
        case INTEGER:
            tmpBuc->attrValue.iValue = *((int*)joinAttrPtr);
            break;
        case FLOAT:
            tmpBuc->attrValue.fValue = *((float*)joinAttrPtr);
            break;
        case STRING:
            tmpBuc->attrValue.sValue = new char[joinAttr.attrLen];
            memcpy((void*)tmpBuc->attrValue.sValue, (void*)joinAttrPtr,
                   joinAttr.attrLen);
            break;
        default:
            printf("illegal type in joinHT insert\n");
            break;
    }
}

// probe: find matching records for given attribute value
void joinHashTbl::probe(const char* keyPtr, int attrType) {
    joinhashBucket* tmpBuc;
    ridCnt = 0;

    int index = hash(innerJoinAttrPtr, joinAttr.attrType);
    tmpBuc = ht[index].chain;

    // allocate an array of RIDs.  This array may be slightly too big in the
    // case of "collisions" in which different join attribute values of the
    // outer hash to the same chain
    outRids = new RID[ht[index].bucketCnt];

    while (tmpBuc != NULL) {
        // scan hash chain looking for matches
        switch (joinAttr.attrType) {
            case INTEGER:
                if (tmpBuc->attrValue.iValue == *((int*)innerJoinAttrPtr)) {
                    outRids[ridCnt] = tmpBuc->rid;
                    ridCnt++;
                }
                break;
            case FLOAT:
                if (tmpBuc->attrValue.fValue == *((float*)innerJoinAttrPtr)) {
                    outRids[ridCnt] = tmpBuc->rid;
                    ridCnt++;
                }
                break;
            case STRING:
                if (strncmp(tmpBuc->attrValue.sValue, innerJoinAttrPtr,
                            joinAttr.attrLen) == 0) {
                    outRids[ridCnt] = tmpBuc->rid;
                    ridCnt++;
                }
                break;
            default:
                printf("illegal type in joinHT lookup\n");
                break;
        }
        tmpBuc = tmpBuc->next;
    }
    return OK;
}
