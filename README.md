# Advanced Project Code Search Engine

## Overview

The **Advanced Project Code Search Engine** is a C++11 Windows application designed to efficiently search through large project or code folders. It supports multiple file types and implements advanced features such as Boolean search, phrase search, and TF-IDF ranking to provide highly relevant search results.

This project demonstrates key concepts in **OOP, data structures, and algorithm design**, making it both practical and a strong showcase for interviews.

---

## Features

* **Supported File Types:** `.txt`, `.cpp`, `.h`, `.py`, `.java`, `.js`, `.cs`
* **Recursive Folder Scan:** Automatically scans all subfolders of a project.
* **Boolean Search:** Supports `AND` and `OR` queries.
* **Phrase Search:** Exact multi-word searches where words appear together.
* **TF-IDF Ranking:** Returns results ranked by relevance.
* **Polymorphism:** `FileParser` base class with derived parsers for each file type.
* **Encapsulation & Modular Design:** Includes `Indexer`, `QueryEngine`, `Document`, and parser classes.
* **Command-Line Interface (CLI):** Interactive query input and ranked results display.

---

## Getting Started

### Prerequisites

* Windows OS
* C++11 compatible compiler (e.g., `g++` or Visual Studio)

### Compilation

```bash
g++ main.cpp -o project
```

### Running

```bash
./project
```

1. Enter the root folder path of your project.
2. Enter a search query ( also  supports Boolean operators AND/OR and phrase search).
3. View results ranked by TF-IDF relevance.
4. Type `exit` to quit.

---

## Usage Example

```
Enter root folder path of your project: C:\Users\Nitish\Desktop\SampleProject
Enter search query: main AND function
Results (ranked by TF-IDF):
SampleProject\file1.cpp | frequency: 3 | TF-IDF: 0.123
SampleProject\file2.h   | frequency: 2 | TF-IDF: 0.098
```

---

## Project Structure

```
main.cpp             # Main program
Document             # Represents each file
FileParser           # Abstract base class
  TxtParser          # Derived class for .txt files
  CodeParser         # Derived class for code files
Indexer              # Builds inverted index
QueryEngine          # Handles search, boolean logic, TF-IDF ranking
FileScanner          # Recursively scans folders
```

---

## Key Concepts Demonstrated

* **OOP:** Polymorphism, encapsulation, abstraction, single responsibility.
* **Data Structures:** `map`, `vector`, `set`.
* **Algorithms:** TF-IDF calculation, Boolean logic, phrase search.
* **Software Design:** Modular, maintainable, and extendable architecture.

---

## Goals & Use Cases

* Efficiently search large project/code folders.
* Rank files by relevance for faster identification of important files.
* Useful for developers looking for functions, variables, or code references.
* Can be extended to include snippet highlighting, wildcard search, or GUI interface.


---

## Future Enhancements

* Add multi-line phrase search.
* Support for additional file types and formats.
* Add snippet highlighting and context preview.
* Implement multi-threaded indexing for large projects.
* Optionally create a GUI for more user-friendly interaction.

---

## Author

**Nitish**

---

## License

This project is open-source and available under the MIT License.
