# MINIREL-DBMS: A Lightweight Relational Database Management System

MINIREL-DBMS is an educational, single-user relational database management system (DBMS). It's designed to provide a hands-on understanding of the internal workings of a database system, covering key logical and physical layers. This project was developed by Aniket Tuli, Sreyas Srivastava, and Yajurva Shrotriya.

## What it Does

Minirel implements a subset of functionalities found in commercial DBMS:
- **Data Definition Language (DDL)**: Supports creating and destroying databases and tables.
- **Data Manipulation Language (DML)**: Supports inserting, deleting, and selecting data.
- **Query Processing**: Includes a parser for SQL-like commands and an execution engine.
- **Join Operations**: Implements Nested Loop Join, Sort-Merge Join, and Hash Join.
- **Buffer Management**: Efficiently manages data transfer between memory and disk.
- **Heap File Management**: Organizes and stores records in heap files.
- **Catalog Management**: Maintains metadata (schema information) about relations and attributes.

## Why it Does It

The primary goal of Minirel is educational. It aims to:
- Demystify the internal operations of a DBMS.
- Provide a practical platform for learning about database concepts like query optimization, data storage, indexing (though not explicitly implemented in this version, it's a natural extension), and transaction management (also a potential extension).
- Serve as a foundation for students and enthusiasts to experiment with and extend database functionalities.

## How it Does It (Architecture and Key Components)

Minirel is built with a modular C++ architecture. Here's a breakdown of its core components:

- **Main Program (`minirel.C`)**:
    - Initializes the system, including the buffer manager and catalogs.
    - Takes the database name as a command-line argument.
    - Allows specifying the join method (Nested Loop, Sort-Merge, or Hash Join).
    - Invokes the parser to process user commands.

- **Parser (`parser/`)**:
    - Uses `lex` (or `flex`) for lexical analysis (`scan.l`) and `yacc` (or `bison`) for parsing (`parse.y`).
    - Translates SQL-like commands into an internal representation (parse tree/nodes).
    - `interp.C` likely handles the interpretation or execution of these parsed commands by calling relevant modules.

- **Buffer Manager (`buf.C`, `buf.h`, `bufHash.C`)**:
    - Manages a buffer pool in memory to cache disk pages.
    - Implements a page replacement policy (details would be in `buf.C`) to decide which page to evict when the buffer is full.
    - Uses a hash table (`bufHash.C`) for quick lookup of pages in the buffer pool.
    - Handles pinning/unpinning of pages and marking pages as dirty.

- **Page and Record ID (`page.h`, `page.C`, `rid.h`)**:
    - `page.h` and `page.C` define the structure of a disk page and provide functions to manage records within a page (e.g., inserting, deleting, iterating records).
    - `rid.h` defines the structure of a Record ID (RID), which uniquely identifies a record (typically by page number and slot number within the page).

- **Heap File Manager (`heapfile.C`, `heapfile.h`)**:
    - Implements heap files, which are unordered collections of records.
    - Provides an API to create and delete heap files, insert records, delete records (identified by RID), retrieve records, and scan all records in a file.
    - Interacts with the Buffer Manager to read/write pages.

- **Catalog Manager (`catalog.C`, `catalog.h`)**:
    - Manages system catalogs, which are special tables that store metadata.
    - `relCat` (Relation Catalog): Stores information about tables (relations), such as table name, number of attributes, file name where data is stored, etc.
    - `attrCat` (Attribute Catalog): Stores information about attributes (columns) of each table, such as attribute name, type, length, and the relation it belongs to.
    - These catalogs are themselves stored as heap files.

- **Database Operations (`db.C`, `db.h`, `dbcreate.C`, `dbdestroy.C`)**:
    - `db.C`, `db.h`: Likely provide core database functionalities or utilities.
    - `dbcreate.C`: Handles the creation of a new database (which might involve creating initial catalog files).
    - `dbdestroy.C`: Handles the deletion of an entire database.

- **Query Processing and DML Commands**:
    - **`query.h`**: Likely contains declarations for functions involved in query execution.
    - **`create.C`**: Implements the `CREATE TABLE` command. Interacts with the catalog to add new table and attribute information.
    - **`destroy.C`**: Implements the `DROP TABLE` (or `DESTROY TABLE`) command. Removes table and attribute information from the catalog and deletes the heap file.
    - **`load.C`**: Implements the `LOAD` command, used to bulk-load data from an external file into a table.
    - **`insert.C`**: Implements the `INSERT` command. Adds a new record to a table's heap file and updates any relevant catalog information if needed (e.g., record count, though this might be dynamic).
    - **`delete.C`**: Implements the `DELETE` command. Removes records from a table's heap file based on a condition.
    - **`select.C`**: Implements the `SELECT` command. Retrieves records from one or more tables based on specified conditions and projections.
    - **`print.C`**: Implements the `PRINT` command, likely used to display the contents of a relation or schema information.
    - **`help.C`**: Implements the `HELP` command, providing usage information.
    - **`quit.C`**: Implements the `QUIT` command to exit Minirel.

- **Join Algorithms**:
    - **`join.C`**: May contain common join logic or a specific join implementation (e.g., a simple nested loop join if `joinHT.C` and `sort.C` handle more advanced ones).
    - **`joinHT.C`, `joinHT.h`**: Implements a Hash Join algorithm. This typically involves building a hash table on one relation and probing it with records from the other.
    - **`sort.C`, `sort.h`**: Provides sorting utilities, essential for the Sort-Merge Join algorithm.
    - **`partition.C`, `partition.h`**: Likely related to partitioning data, which can be a step in some hash join or sort-merge join algorithms, especially for external memory operations.

- **Error Handling (`error.C`, `error.h`)**:
    - Provides a centralized way to manage and report errors that occur during DBMS operations.

- **Utility (`utility.h`)**:
    - Contains miscellaneous helper functions and definitions used across the project.

## Files and Directories Overview

- **`*.C`, `*.h` (root directory)**: Core C++ source and header files for the DBMS components.
- **`Makefile`**: Build script for compiling the project.
- **`minirel.C`**: Main entry point for the Minirel DBMS.
- **`dbcreate.C`**: Utility to create a new Minirel database.
- **`dbdestroy.C`**: Utility to destroy an existing Minirel database.
- **`members.txt`**: Lists project contributors and their roles.
- **`README.md`**: This file.
- **`data/`**: Contains sample data files (e.g., `.data` files) used for populating tables.
    - `gen*`: Likely scripts or source code for generating test data.
- **`parser/`**: Contains the source code for the SQL parser.
    - `parse.y`: Yacc/Bison grammar file.
    - `scan.l`: Lex/Flex scanner file.
- **`testqueries/`**: Contains sample query files (`qu.*`) for testing the DBMS.
- **`uttest*` (e.g., `uttest`, `uttestdata/`, `uttestqueries/`)**: Likely related to unit testing infrastructure and specific unit test cases.
- **`qutest*`**: Executables or scripts, possibly for specific query test scenarios (e.g., `qutestHJ` for Hash Join tests).

## Steps to Build and Run

**Prerequisites:**
- A C++ compiler (like g++)
- `make` utility
- `lex` (or `flex`) and `yacc` (or `bison`) for the parser (usually handled by `make` in the `parser` directory).

**Build Instructions:**

1.  **Clone the Repository (if you haven't already):**
    ```bash
    git clone <your-repository-url>
    cd MINIREL-DBMS--main
    ```

2.  **Compile the Project:**
    Open a terminal in the project's root directory (`MINIREL-DBMS--main`) and run:
    ```bash
    make
    ```
    This will:
    - Compile the parser in the `parser/` directory.
    - Compile all the `.C` files in the root directory.
    - Link the object files to create the executables: `minirel`, `dbcreate`, and `dbdestroy`.

**Running Minirel:**

1.  **Create a Database:**
    Before running `minirel`, you need a database directory. Let's say you want to create a database named `mydb`.
    ```bash
    mkdir mydb
    ./dbcreate mydb
    ```
    This initializes the necessary catalog files within the `mydb` directory.

2.  **Run the Minirel Interactive Shell:**
    To interact with your database:
    ```bash
    ./minirel mydb
    ```
    If you want to specify a join algorithm (e.g., Hash Join):
    ```bash
    ./minirel mydb HJ
    ```
    Supported join methods (specified as the third argument):
    - `NL` (Nested Loop Join - default if not specified)
    - `SM` (Sort-Merge Join)
    - `HJ` (Hash Join)

    Once inside the Minirel shell, you can type SQL-like commands. For example:
    ```
    CREATE TABLE Sailors (sid INTEGER, sname CHAR(20), rating INTEGER, age REAL);
    LOAD Sailors "sailors.dat"; // Assuming sailors.dat is in the 'mydb' directory or a path accessible
    SELECT * FROM Sailors WHERE rating > 7;
    HELP;
    QUIT;
    ```
    *(Note: The exact syntax and available commands can be verified by looking at `parser/parse.y` and the `HELP` command output.)*

3.  **Destroy a Database (Optional):**
    To remove a database and its files:
    ```bash
    ./dbdestroy mydb
    ```
    **Caution**: This will delete all data in `mydb`.

**Cleaning Up Compiled Files:**

To remove all compiled object files, executables, and backup files:
```bash
make clean
```

