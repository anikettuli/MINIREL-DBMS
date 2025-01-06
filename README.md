# MINIREL-DBMS

MINIREL-DBMS is a simplified, single-user relational database management system (DBMS) developed for educational purposes. It implements various logical and physical layers of a DBMS, providing hands-on insights into how database systems work internally.

## Key Components

- **Buffer Manager (`buf.C`, `buf.h`)**  
  Manages efficient reading and writing of data pages between disk and memory, optimizing data retrieval and storage operations.

- **Heap File Manager (`heapfile.C`, `heapfile.h`)**  
  Handles the organization and storage of data within heap files, enabling efficient data insertion, deletion, and scanning.

- **Catalog Manager (`catalog.C`, `catalog.h`)**  
  Maintains metadata about database objects such as tables and schemas, facilitating data definition and management.

- **Query Processor (`query.h` and related files)**  
  Interprets and executes SQL-like queries, performing operations such as selection, projection, insertion, deletion, and joins.

- **Join Algorithms (`join.C`, `joinHT.C`, `joinHT.h`)**  
  Implements strategies for combining data from multiple tables, including hash-based and nested-loop joins.

## Directory Overview

- **`data` and `uttestdata`:**  
  Contains sample datasets used for testing and validation of DBMS functionalities.

- **`testqueries` and `uttestqueries`:**  
  Provides a collection of test SQL queries to verify the correctness of query processing and execution.

- **`parser`:**  
  Houses components responsible for parsing SQL-like commands, translating them into executable operations.

## Build and Execution

To build the project, ensure you have `make` installed on your system. Then, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/anikettuli/MINIREL-DBMS-
   cd MINIREL-DBMS-
   ```

2. Build the project:
   ```bash
   make
   ```

   This will compile the source files and generate the executable for the Minirel DBMS.

3. Run the executable:
   ```bash
   ./minirel
   ```

   The system accepts SQL-like commands for database operations.

## Usage

- Sample datasets for testing are located in the `data` and `uttestdata` directories.
- Test SQL queries are available in the `testqueries` and `uttestqueries` directories.  
  Use these to validate various functionalities of the DBMS.

## Contributors

The project was developed by **Aniket Tuli** and collaborators. Refer to the `members.txt` file in the repository for additional details about contributors.

## Notes

This project is intended for educational purposes, offering a practical approach to understanding relational DBMS implementation. It serves as a valuable resource for students and database enthusiasts.

## Repository Link

For more information and to access the source code, visit the [MINIREL-DBMS GitHub Repository](https://github.com/anikettuli/MINIREL-DBMS-).
