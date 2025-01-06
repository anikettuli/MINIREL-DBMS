#include "catalog.h"
#include "query.h"

// forward declaration
const Status ScanSelect(const string &result,
                        const int projCnt,
                        const AttrDesc projNames[],
                        const AttrDesc *attrDesc,
                        const Operator op,
                        const char *filter,
                        const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string &result,
                       const int projCnt,
                       const attrInfo projNames[],
                       const attrInfo *attr,
                       const Operator op,
                       const char *attrValue)
{
  // Qu_Select sets up things and then calls ScanSelect to do the actual work
  cout << "Doing QU_Select " << endl;

  Status status;
  AttrDesc attrDesc;
  AttrDesc *projDescs = new AttrDesc[projCnt];
  int reclen = 0;
  Operator currOp;
  const char *filterVal;

  for (int i = 0; i < projCnt; i++)
  {
    status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, projDescs[i]);
    reclen += projDescs[i].attrLen;
    if (status != OK)
      return status;
  }

  if (attr != NULL)
  {
    status = attrCat->getInfo(attr->relName, attr->attrName, attrDesc);
    if (status != OK)
      return status;

    int tmp1;
    float tmp2;

    switch (attr->attrType)
    {
    case STRING:
      filterVal = attrValue;
      break;

    case INTEGER:
      tmp1 = atoi(attrValue);
      filterVal = (char *)&tmp1;
      break;

    case FLOAT:
      tmp2 = atof(attrValue);
      filterVal = (char *)&tmp2;
      break;
    }
    currOp = op;
  }
  else
  {
    strcpy(attrDesc.relName, projNames[0].relName);
    strcpy(attrDesc.attrName, projNames[0].attrName);

    attrDesc.attrOffset = 0;
    attrDesc.attrLen = 0;

    attrDesc.attrType = STRING;
    filterVal = NULL;
    currOp = EQ;
  }

  status = ScanSelect(result, projCnt, projDescs, &attrDesc, currOp, filterVal, reclen);
  if (status != OK)
    return status;

  return OK;
}

const Status ScanSelect(const string &result,
#include "stdio.h"
#include "stdlib.h"
                        const int projCnt,
                        const AttrDesc projNames[],
                        const AttrDesc *attrDesc,
                        const Operator op,
                        const char *filter,
                        const int reclen)
{
  cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;

  Status status;
  Record outRecord;           // output record
  char outRecordData[reclen]; // output record data

  InsertFileScan resultTable(result, status);
  if (status != OK)
    return status;

  HeapFileScan heapScan(attrDesc->relName, status);
  if (status != OK)
    return status;

  outRecord.data = (void *)outRecordData;
  outRecord.length = reclen;

  status = heapScan.startScan(attrDesc->attrOffset, attrDesc->attrLen, (Datatype)attrDesc->attrType, filter, op);
  if (status != OK)
    return status;

  Record currRecord;
  RID currRid;

  while (heapScan.scanNext(currRid) == OK)
  {
    status = heapScan.getRecord(currRecord);
    if (status != OK)
      return status;

    int outOffset = 0;
    for (int i = 0; i < projCnt; i++)
    {
      memcpy(outRecordData + outOffset, (char *)currRecord.data + projNames[i].attrOffset, projNames[i].attrLen);
      outOffset += projNames[i].attrLen;
    }

    RID newRid;
    status = resultTable.insertRecord(outRecord, newRid);
    if (status != OK)
      return status;
  }

  status = heapScan.endScan();
  if (status != OK)
    return status;

  return OK;
}