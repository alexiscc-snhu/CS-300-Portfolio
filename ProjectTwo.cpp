//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Alexis Cordero
// Course      : CS 300
// Professor   : Nijim
// Date        : August 23, 2026
// Description : Project Two - ABCU Advising Assistance Program
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;
};

class BinarySearchTree {
private:
    struct Node {
        Course course;
        Node* left;
        Node* right;

        Node(const Course& aCourse) {
            course = aCourse;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root;

    void addNode(Node*& node, const Course& course) {
        if (node == nullptr) {
            node = new Node(course);
        } else if (course.courseNumber < node->course.courseNumber) {
            addNode(node->left, course);
        } else {
            addNode(node->right, course);
        }
    }

    void inOrder(Node* node) const {
        if (node == nullptr) {
            return;
        }

        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.title << endl;
        inOrder(node->right);
    }

    void destroyTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        destroyTree(root);
        root = nullptr;
    }

    void clear() {
        destroyTree(root);
        root = nullptr;
    }

    void insert(const Course& course) {
        addNode(root, course);
    }

    Course* search(const string& courseNumber) {
        Node* current = root;

        while (current != nullptr) {
            if (courseNumber == current->course.courseNumber) {
                return &(current->course);
            }

            if (courseNumber < current->course.courseNumber) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        return nullptr;
    }

    const Course* search(const string& courseNumber) const {
        Node* current = root;

        while (current != nullptr) {
            if (courseNumber == current->course.courseNumber) {
                return &(current->course);
            }

            if (courseNumber < current->course.courseNumber) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        return nullptr;
    }

    void printCourseList() const {
        inOrder(root);
    }
};

string trim(const string& value) {
    size_t start = value.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

string toUpper(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
    return value;
}

bool parseCourseLine(const string& line, Course& course) {
    vector<string> tokens;
    string token;
    stringstream stream(line);

    while (getline(stream, token, ',')) {
        tokens.push_back(trim(token));
    }

    if (tokens.size() < 2 || tokens[0].empty() || tokens[1].empty()) {
        return false;
    }

    course.courseNumber = toUpper(tokens[0]);
    course.title = tokens[1];
    course.prerequisites.clear();

    for (size_t i = 2; i < tokens.size(); ++i) {
        if (!tokens[i].empty()) {
            course.prerequisites.push_back(toUpper(tokens[i]));
        }
    }

    return true;
}

bool loadCourses(const string& fileName, BinarySearchTree& courseTree) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Unable to open file " << fileName << "." << endl;
        return false;
    }

    vector<Course> courses;
    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        ++lineNumber;

        if (trim(line).empty()) {
            continue;
        }

        Course course;
        if (!parseCourseLine(line, course)) {
            cout << "Error: Invalid file format on line " << lineNumber << "." << endl;
            return false;
        }

        courses.push_back(course);
    }

    inputFile.close();

    if (courses.empty()) {
        cout << "Error: The file contains no course data." << endl;
        return false;
    }

    // Validate that every prerequisite appears as a course in the file.
    for (const Course& course : courses) {
        for (const string& prerequisite : course.prerequisites) {
            bool prerequisiteFound = false;

            for (const Course& candidate : courses) {
                if (candidate.courseNumber == prerequisite) {
                    prerequisiteFound = true;
                    break;
                }
            }

            if (!prerequisiteFound) {
                cout << "Error: Prerequisite " << prerequisite
                     << " for " << course.courseNumber
                     << " does not exist in the course file." << endl;
                return false;
            }
        }
    }

    // Only replace the current tree after the entire file has been validated.
    courseTree.clear();
    for (const Course& course : courses) {
        courseTree.insert(course);
    }

    cout << "Course data loaded successfully." << endl;
    return true;
}

void printCourseInformation(const Course& course, const BinarySearchTree& courseTree) {
    cout << course.courseNumber << ", " << course.title << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;
    for (const string& prerequisiteNumber : course.prerequisites) {
        const Course* prerequisite = courseTree.search(prerequisiteNumber);

        if (prerequisite != nullptr) {
            cout << "  " << prerequisite->courseNumber
                 << ", " << prerequisite->title << endl;
        }
    }
}

void displayMenu() {
    cout << endl;
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << endl;
}

int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        displayMenu();
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter 1, 2, 3, or 9." << endl;
            continue;
        }

        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: {
                string fileName;
                cout << "What file would you like to load? ";
                getline(cin, fileName);
                dataLoaded = loadCourses(trim(fileName), courseTree);
                break;
            }

            case 2:
                if (!dataLoaded) {
                    cout << "Please load the course data first." << endl;
                } else {
                    cout << "Here is a sample schedule:" << endl << endl;
                    courseTree.printCourseList();
                }
                break;

            case 3: {
                if (!dataLoaded) {
                    cout << "Please load the course data first." << endl;
                    break;
                }

                string courseNumber;
                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);
                courseNumber = toUpper(trim(courseNumber));

                const Course* course = courseTree.search(courseNumber);
                if (course == nullptr) {
                    cout << "Course " << courseNumber << " not found." << endl;
                } else {
                    printCourseInformation(*course, courseTree);
                }
                break;
            }

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    return 0;
}
