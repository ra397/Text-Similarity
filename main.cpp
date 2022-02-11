/* Program that computes a similarity score between 2 text documents
 * by taking the cross product of their term frequency vectors. */

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>

using namespace std;

class Word {
public:
    Word() {
        word = "";
        num_occurrence = 0;
        tf = 0;
    }

    string word;
    int num_occurrence; // number of occurrence of the word in its document
    double tf; // term frequency score (num_occurrences / number of words in document)
};

// function that returns true if word is in array, false otherwise
bool inArray(string const word, string array[], int array_size);

// function that returns the index of word if in vector of words, -1 otherwise
int inVector(string const word, vector<Word> vector);

class Document {
public:
    Document() {
        num_words = 0;
    }

    Document(const Document &oldDocument) {
        // copy the word from oldDocument to current document object, NOT THE NUM_OCCURRENCES
        // since num_occurrences is relative to each document
        for (int i = 0; i < oldDocument.document_info.size(); i++) {
            Word word;
            word.word = oldDocument.document_info.at(i).word;
            document_info.push_back(word);
        }
        num_words = 0;
    }

    void print() { // for testing purposes
        for (int i = 0; i < this->document_info.size(); i++) {
            cout << "Index: " << i << ", " << this->document_info.at(i).word << ", " << this->document_info.at(i).tf
                 << ", " << this->document_info.at(i).num_occurrence << endl;
        }
    }

    vector<Word> document_info; // a set of unique words that occur in all documents
    int num_words; // the number of words in the document
};

int main() {
    // load in stop words into stop_words array
    const int NUM_STOP_WORDS = 851;
    string stop_words[NUM_STOP_WORDS];
    ifstream file("stop_words_english.txt");
    if (!file.is_open()) {
        cout << "stop_words_english.txt could not open" << endl;
        exit(1);
    }
    int index = 0;
    while (!file.eof()) {
        string s;
        getline(file, s, '\n');
        stop_words[index] = s;
        index++;
    }

    cout << "Document 1: " << endl;
    string input1;
    getline(cin, input1, '\n');
    cout << "Document 2: " << endl;
    string input2;
    getline(cin, input2, '\n');

    // open txt files to be read
    ifstream file1(input1);
    if (!file1.is_open()) {
        cout << input1 << " could not open" << endl;
        exit(0);
    }
    ifstream file2(input2);
    if (!file2.is_open()) {
        cout << input2 << " could not open" << endl;
        exit(1);
    }

    Document document1; // create document object
    string line;
    while (getline(file1, line)) {
        // parse line by each word
        stringstream linestream(line);
        string s;
        while (linestream >> s) {
            // standardize string s
            if (!s.empty()) {
                while (s[s.size() - 1] == ',' || s[s.size() - 1] == '.' || s[s.size() - 1] == '!' ||
                       s[s.size() - 1] == '?' ||
                       s[s.size() - 1] == ';' || s[s.size() - 1] == ':' || s[s.size() - 1] == '"' ||
                       s[s.size() - 1] == ')' || s[s.size() - 1] == '\'') {
                    s.pop_back();
                }
                while (s[0] == '"' || s[0] == '\'' || s[0] == '(') {
                    s.erase(0, 1);
                }
                for (int i = 0; i < s.size(); i++) { // make the word all lowercase
                    s[i] = tolower(s[i]);
                }
                document1.num_words++;

                if (!inArray(s, stop_words, NUM_STOP_WORDS)) {
                    // populate input1 words vector with unique words
                    int i = inVector(s, document1.document_info);
                    if (i == -1) { // if word has not been added
                        Word word;
                        word.word = s;
                        word.num_occurrence++;
                        document1.document_info.push_back(word);
                    } else { // if word has been added
                        document1.document_info.at(i).num_occurrence++;
                    }
                }
            }
        }
    }

    Document document2(document1); // copy over words in doc1 to doc2, with fresh num_occurances
    string line2;

    while (getline(file2, line2)) {
        stringstream linestream(line2);
        string s;
        while (linestream >> s) {
            if (!s.empty()) {
                while (s[s.size() - 1] == ',' || s[s.size() - 1] == '.' || s[s.size() - 1] == '!' ||
                       s[s.size() - 1] == '?' ||
                       s[s.size() - 1] == ';' || s[s.size() - 1] == ':' || s[s.size() - 1] == '"' ||
                       s[s.size() - 1] == '\'' || s[s.size() - 1] == '\n' || s[s.size() - 1] == '\t' ||
                       s[s.size() - 1] == ')') {
                    s.pop_back();
                }
                while (s[0] == '"' || s[0] == '\'' || s[0] == '(') {
                    s.erase(0, 1);
                }
                for (int i = 0; i < s.size(); i++) { // make the word all lowercase
                    s[i] = tolower(s[i]);
                }
                document2.num_words++;

                if (!inArray(s, stop_words, NUM_STOP_WORDS)) {
                    int i = inVector(s, document2.document_info);
                    if (i == -1) { // if word is unique (not in doc1 or 2)
                        Word word;
                        word.word = s;
                        document1.document_info.push_back(word);
                        word.num_occurrence++;
                        document2.document_info.push_back(word);
                    } else { // seen in 1 and 2
                        document2.document_info.at(i).num_occurrence++;
                    }
                }
            }
        }
    }

    // calculate the tf score for all words in each document
    for (int i = 0; i < document1.document_info.size(); i++) {
        document1.document_info.at(i).tf =
                (document1.document_info.at(i).num_occurrence / (double) document1.num_words);
    }
    for (int i = 0; i < document2.document_info.size(); i++) {
        document2.document_info.at(i).tf =
                (document2.document_info.at(i).num_occurrence / (double) document2.num_words);
    }

    // find the cosine similarity of each document's vector of words
    double dot_product_sum = 0;
    double doc1_sum = 0;
    double doc2_sum = 0;
    for (int i = 0; i < document1.document_info.size(); i++) {
        dot_product_sum += document1.document_info.at(i).tf * document2.document_info.at(i).tf;
        doc1_sum += pow(document1.document_info.at(i).tf, 2);
        doc2_sum += pow(document2.document_info.at(i).tf, 2);
    }
    double doc1_magnitude = sqrt(doc1_sum);
    double doc2_magnitude = sqrt(doc2_sum);

    double score = dot_product_sum / (doc1_magnitude * doc2_magnitude);
    cout << "Similarity score: " << score << endl;
    
    return 0;
}

int inVector(string const word, vector <Word> vector) {
    for (int i = 0; i < vector.size(); i++) {
        if (vector.at(i).word == word) {
            return i;
        }
    }
    return -1;
}

bool inArray(string const word, string array[], int array_size) {
    for (int i = 0; i < array_size; i++) {
        if (word == array[i]) {
            return true;
        }
    }
    return false;
}
