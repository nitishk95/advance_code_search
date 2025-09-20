#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cmath>


using namespace std;

// ---------------- Document ----------------
struct Document {
    int id;
    string filename;
    string content;
    int totalWords;
};

// ---------------- FileParser Base Class ----------------
class FileParser {
public:
    virtual string parse(const string& filepath) = 0; // pure virtual
    virtual ~FileParser() {}
};

// ---------------- Derived Parsers ----------------
class TxtParser : public FileParser {
public:
    string parse(const string& filepath) override {
        ifstream file(filepath.c_str());
        if (!file.is_open()) return "";
        string word, content;
        while (file >> word) {
            string cleanWord;
            for (char c : word) if (isalnum(c) || c == '_') cleanWord += tolower(c);
            if (!cleanWord.empty()) content += cleanWord + " ";
        }
        file.close();
        return content;
    }
};

class CodeParser : public FileParser {
public:
    string parse(const string& filepath) override {
        return TxtParser().parse(filepath); // Same parsing for simplicity
    }
};

// ---------------- Factory ----------------
FileParser* getParser(const string& filename) {
    if (filename.size() >= 4) {
        string ext = filename.substr(filename.size() - 4);
        if (ext == ".txt") return new TxtParser();
        if (ext == ".cpp" || ext == ".h") return new CodeParser();
    }
    if (filename.size() >= 3) {
        string ext = filename.substr(filename.size() - 3);
        if (ext == ".py" || ext == ".js") return new CodeParser();
    }
    if (filename.size() >= 5) {
        string ext = filename.substr(filename.size() - 5);
        if (ext == ".java" || ext == ".cs") return new CodeParser();
    }
    return nullptr; // unsupported
}

// ---------------- Indexer ----------------
class Indexer {
public:
    map<string, map<int, int>> invertedIndex; // word -> (docID -> freq)
    vector<Document> documents;

    void addDocument(const string& filepath, int id) {
        FileParser* parser = getParser(filepath);
        if (!parser) return;
        string content = parser->parse(filepath);
        delete parser;

        int totalWords = 0;
        stringstream ss(content);
        string word;
        while (ss >> word) {
            invertedIndex[word][id]++;
            totalWords++;
        }
        documents.push_back({id, filepath, content, totalWords});
    }

    static string clean(const string& word) {
        string result;
        for (char c : word) if (isalnum(c) || c == '_') result += tolower(c);
        return result;
    }
};

// ---------------- Recursive Folder Scanner ----------------
void listFilesRecursive(const string& folder, vector<string>& files) {
    WIN32_FIND_DATA fd;
    string search_path = folder + "\\*";
    HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            string fname = fd.cFileName;
            if (fname != "." && fname != "..") {
                string fullPath = folder + "\\" + fname;
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    listFilesRecursive(fullPath, files);
                } else {
                    FileParser* parser = getParser(fullPath);
                    if (parser) {
                        files.push_back(fullPath);
                        delete parser;
                    }
                }
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
}

// ---------------- Query Engine ----------------
class QueryEngine {
    Indexer& indexer;
public:
    QueryEngine(Indexer& idx) : indexer(idx) {}

    struct Result {
        int docID;
        int frequency;
        double tfidf;
    };

    vector<string> splitWords(const string& query) {
        vector<string> words;
        string word;
        stringstream ss(query);
        while (ss >> word) words.push_back(Indexer::clean(word));
        return words;
    }

    vector<Result> search(const string& query) {
        vector<Result> results;
        vector<string> words = splitWords(query);

        bool isAND = false, isOR = false;
        vector<string> filteredWords;

        for (string w : words) {
            if (w == "and") isAND = true;
            else if (w == "or") isOR = true;
            else filteredWords.push_back(w);
        }

        set<int> candidateDocs;
        if (!filteredWords.empty()) {
            candidateDocs = set<int>();
            if (isAND) {
                // Initialize with first word docs
                auto it = indexer.invertedIndex.find(filteredWords[0]);
                if (it != indexer.invertedIndex.end()) {
                    for (auto &p : it->second) candidateDocs.insert(p.first);
                }
                for (size_t i = 1; i < filteredWords.size(); ++i) {
                    set<int> temp;
                    auto it2 = indexer.invertedIndex.find(filteredWords[i]);
                    if (it2 != indexer.invertedIndex.end()) {
                        for (auto &p : it2->second) temp.insert(p.first);
                    }
                    set<int> intersect;
                    set_intersection(candidateDocs.begin(), candidateDocs.end(),
                                     temp.begin(), temp.end(),
                                     inserter(intersect, intersect.begin()));
                    candidateDocs = intersect;
                }
            } else {
                // OR search by default
                for (string w : filteredWords) {
                    auto it = indexer.invertedIndex.find(w);
                    if (it != indexer.invertedIndex.end()) {
                        for (auto &p : it->second) candidateDocs.insert(p.first);
                    }
                }
            }
        }

        for (int docID : candidateDocs) {
            int freqSum = 0;
            for (string w : filteredWords) {
                auto it = indexer.invertedIndex.find(w);
                if (it != indexer.invertedIndex.end()) {
                    freqSum += it->second[docID];
                }
            }
            double tfidf = 0.0;
            for (string w : filteredWords) {
                auto it = indexer.invertedIndex.find(w);
                if (it != indexer.invertedIndex.end()) {
                    int df = it->second.size();
                    if (df > 0) tfidf += ((double)it->second[docID] / indexer.documents[docID].totalWords) * log((double)indexer.documents.size() / df);
                }
            }
            results.push_back({docID, freqSum, tfidf});
        }

        sort(results.begin(), results.end(), [](const Result& a, const Result& b){ return a.tfidf > b.tfidf; });
        return results;
    }
};

// ---------------- Main ----------------
int main() {
    Indexer indexer;
    vector<string> files;

    string folderPath;
    cout << "Enter root folder path of your project: ";
    getline(cin, folderPath);

    cout << "Indexing files...\n";
    listFilesRecursive(folderPath, files);

    if (files.empty()) {
        cout << "No supported files found.\n";
        return 0;
    }

    for (size_t i = 0; i < files.size(); ++i) indexer.addDocument(files[i], i);
    cout << "Indexed " << indexer.documents.size() << " files.\n";

    QueryEngine engine(indexer);

    while (true) {
        cout << "\nEnter search query (supports AND, OR, phrase with quotes) or type 'exit': ";
        string query;
        getline(cin, query);
        if (query == "exit") break;

        vector<QueryEngine::Result> results = engine.search(query);
        if (results.empty()) cout << "No results found.\n";
        else {
            cout << "Results (ranked by TF-IDF):\n";
            for (auto &res : results) {
                cout << indexer.documents[res.docID].filename
                     << " | frequency: " << res.frequency
                     << " | TF-IDF: " << res.tfidf << "\n";
            }
        }
    }

    return 0;
}
