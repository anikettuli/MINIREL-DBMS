// buf.C — Buffer Manager Implementation
// Manages a fixed-size buffer pool using a clock replacement algorithm.
// Supports page allocation, reading, pinning/unpinning, and flushing dirty
// pages.

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>    // for errno
#include <cstdio>    // for printf, fprintf
#include <cstdlib>   // for exit, malloc
#include <cstring>   // for memset
#include <iostream>  // for std::cerr

#include "buf.h"
#include "page.h"

#define ASSERT(c)                                                  \
    {                                                              \
        if (!(c)) {                                                \
            cerr << "At line " << __LINE__ << ":" << endl << "  "; \
            cerr << "This condition should hold: " #c << endl;     \
            exit(1);                                               \
        }                                                          \
    }

//----------------------------------------
// Constructor of the class BufMgr
//----------------------------------------

// Constructor: initialize the buffer manager with 'bufs' frames
BufMgr::BufMgr(const int bufs) {
    numBufs = bufs;

    bufTable = new BufDesc[bufs];
    memset(bufTable, 0, bufs * sizeof(BufDesc));
    for (int i = 0; i < bufs; i++) {
        bufTable[i].frameNo = i;
        bufTable[i].valid = false;
    }

    bufPool = new Page[bufs];
    memset(bufPool, 0, bufs * sizeof(Page));

    int htsize = ((((int)(bufs * 1.2)) * 2) / 2) + 1;
    hashTable = new BufHashTbl(htsize);  // allocate the buffer hash table

    clockHand = bufs - 1;
}

BufMgr::~BufMgr() {
    // Destructor: flush dirty pages to disk and free resources
    // flush out all unwritten pages
    for (int i = 0; i < numBufs; i++) {
        BufDesc* tmpbuf = &bufTable[i];
        if (tmpbuf->valid == true && tmpbuf->dirty == true) {
#ifdef DEBUGBUF
            cout << "flushing page " << tmpbuf->pageNo << " from frame " << i
                 << endl;
#endif

            tmpbuf->file->writePage(tmpbuf->pageNo, &(bufPool[i]));
        }
    }

    delete[] bufTable;
    delete[] bufPool;
    delete hashTable;
}

// allocBuf: find or free a buffer frame using the clock algorithm
const Status BufMgr::allocBuf(int& frame) {
    // perform first part of clock algorithm to search for
    // open buffer frame
    // Assumes non-concurrent access to buffer manager
    Status status = OK;
    int numScanned = 0;
    bool found = 0;
    while (numScanned < 2 * numBufs) {
        // advance the clock
        advanceClock();
        numScanned++;

        // if invalid, use frame
        if (!bufTable[clockHand].valid) {
            break;
        }

        // is valid, check referenced bit
        if (!bufTable[clockHand].refbit) {
            // check to see if someone has it pinned
            if (bufTable[clockHand].pinCnt == 0) {
                // hasn't been referenced and is not pinned, use it

                // remove previous entry from hash table
                status = hashTable->remove(bufTable[clockHand].file,
                                           bufTable[clockHand].pageNo);
                found = true;
                // if (status != OK) return status;
                break;
            }
        } else {
            // has been referenced, clear the bit
            bufStats.accesses++;
            bufTable[clockHand].refbit = false;
        }
    }

    // check for full buffer pool
    if (!found && numScanned >= 2 * numBufs) {
        return BUFFEREXCEEDED;
    }

    // flush any existing changes to disk if necessary
    if (bufTable[clockHand].dirty) {
        bufStats.diskwrites++;

        status = bufTable[clockHand].file->writePage(bufTable[clockHand].pageNo,
                                                     &bufPool[clockHand]);
        if (status != OK) return status;
    }

    // return new frame number
    frame = clockHand;

    return OK;
}  // end allocBuf

// readPage: fetch a page into the buffer pool and pin it
const Status BufMgr::readPage(File* file, const int PageNo, Page*& page) {
    // check to see if it is already in the buffer pool
    // cout << "readPage called on file.page " << file << "." << PageNo << endl;
    int frameNo = 0;
    Status status = hashTable->lookup(file, PageNo, frameNo);
    if (status == OK) {
        // set the referenced bit
        bufTable[frameNo].refbit = true;
        bufTable[frameNo].pinCnt++;
        page = &bufPool[frameNo];
    } else  // not in the buffer pool, must allocate a new page
    {
        // alloc a new frame
        status = allocBuf(frameNo);
        if (status != OK) return status;

        // read the page into the new frame
        bufStats.diskreads++;
        status = file->readPage(PageNo, &bufPool[frameNo]);
        if (status != OK) return status;

        // set up the entry properly
        bufTable[frameNo].Set(file, PageNo);
        page = &bufPool[frameNo];

        // insert in the hash table
        status = hashTable->insert(file, PageNo, frameNo);
        if (status != OK) {
            return status;
        }
    }

    return OK;
}

// unPinPage: unpin a page, marking it dirty if modified
const Status BufMgr::unPinPage(File* file, const int PageNo, const bool dirty) {
    // lookup in hashtable
    Status status = OK;
    int frameNo = 0;
    status = hashTable->lookup(file, PageNo, frameNo);
    if (status != OK) return status;
    /*
    if (status != OK) {cout << "lookup failed in unpinpage\n"; return status;}
    cout << "unpinning (file.page) " << file << "." << PageNo << " with dirty
    flag = " << dirty << endl; cout << "\t page is in frame " << frameNo << "
    pinCnt is " << bufTable[frameNo].pinCnt  << endl;
    */

    if (dirty == true) bufTable[frameNo].dirty = dirty;

    // make sure the page is actually pinned
    if (bufTable[frameNo].pinCnt == 0) {
        return PAGENOTPINNED;
    } else
        bufTable[frameNo].pinCnt--;
    return OK;
}

// flushFile: write all dirty pages for a given file back to disk
const Status BufMgr::flushFile(const File* file) {
    Status status;

    for (int i = 0; i < numBufs; i++) {
        BufDesc* tmpbuf = &(bufTable[i]);
        if (tmpbuf->valid == true && tmpbuf->file == file) {
            if (tmpbuf->pinCnt > 0) return PAGEPINNED;

            if (tmpbuf->dirty == true) {
#ifdef DEBUGBUF
                cout << "flushing page " << tmpbuf->pageNo << " from frame "
                     << i << endl;
#endif
                if ((status = tmpbuf->file->writePage(tmpbuf->pageNo,
                                                      &(bufPool[i]))) != OK)
                    return status;

                tmpbuf->dirty = false;
            }

            hashTable->remove(file, tmpbuf->pageNo);

            tmpbuf->file = NULL;
            tmpbuf->pageNo = -1;
            tmpbuf->valid = false;
        }

        else if (tmpbuf->valid == false && tmpbuf->file == file)
            return BADBUFFER;
    }

    return OK;
}

const Status BufMgr::disposePage(File* file, const int pageNo) {
    // see if it is in the buffer pool
    Status status = OK;
    int frameNo = 0;
    status = hashTable->lookup(file, pageNo, frameNo);
    if (status == OK) {
        // clear the page
        bufTable[frameNo].Clear();
    }
    status = hashTable->remove(file, pageNo);

    // deallocate it in the file
    return file->disposePage(pageNo);
}

const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page) {
    int frameNo;

    // allocate a new page in the file
    Status status = file->allocatePage(pageNo);
    if (status != OK) return status;

    // alloc a new frame
    status = allocBuf(frameNo);
    if (status != OK) return status;

    // set up the entry properly
    bufTable[frameNo].Set(file, pageNo);
    page = &bufPool[frameNo];

    // insert in thehash table
    status = hashTable->insert(file, pageNo, frameNo);
    if (status != OK) {
        return status;
    }
    // cout << "allocated page " << pageNo <<  " to file " << file << "frame is:
    // " << frameNo  << endl;
    return OK;
}

void BufMgr::printSelf(void) {
    BufDesc* tmpbuf;

    cout << endl << "Print buffer...\n";
    for (int i = 0; i < numBufs; i++) {
        tmpbuf = &(bufTable[i]);
        cout << i << "\t" << (char*)(&bufPool[i])
             << "\tpinCnt: " << tmpbuf->pinCnt;

        if (tmpbuf->valid == true) cout << "\tvalid\n";
        cout << endl;
    };
}
