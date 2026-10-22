// ProjectTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

struct Course {
	string courseNumber;
	string courseTitle;
	vector<string> prerequisites;
};

struct TreeNode {
	Course course;
	TreeNode* left;
	TreeNode* right;

	TreeNode(Course newCourse) {
		course = newCourse;
		left = nullptr;
		right = nullptr;
	}
};

TreeNode* insertCourse(TreeNode* currentNode, Course course) {
	// If the current position is empty, create a new node
	if (currentNode == nullptr) {
		return new TreeNode(course);
	}

	// Insert courses with lower numbers into the left subtree
	if (course.courseNumber < currentNode->course.courseNumber) {
		currentNode->left = insertCourse(currentNode->left, course);
	}

	// Insert courses with higher numbers into the right subtree
	else if (course.courseNumber > currentNode->course.courseNumber) {
		currentNode->right = insertCourse(currentNode->right, course);
	}

	// Return the current node to maintain the tree structure
	return currentNode;
}

TreeNode* searchCourseNode(TreeNode* currentNode, string courseNumber) {
	// Return nullptr if the course does not exist
	if (currentNode == nullptr) {
		return nullptr;
	}

	// Return the node if the course number matches
	if (courseNumber == currentNode->course.courseNumber) {
		return currentNode;
	}

	// Search the left subtree if the course number is lower
	else if (courseNumber < currentNode->course.courseNumber) {
		return searchCourseNode(currentNode->left, courseNumber);
	}

	// Otherwise search the right subtree
	else {
		return searchCourseNode(currentNode->right, courseNumber);
	}
}

string trim(const string& value) {
	// Find the first character that is not whitespace
	size_t start = value.find_first_not_of(" \t\r\n");

	if (start == string::npos) {
		return "";
	}

	// Find the last character that is not whitespace
	size_t end = value.find_last_not_of(" \t\r\n");
	return value.substr(start, end - start + 1);
}

bool validateCourseFile(string filename) {
	ifstream file(filename);

	// Verify that the file can be opened
	if (!file.is_open()) {
		cout << "Error: Unable to open file." << endl;
		return false;
	}

	vector<Course> temporaryCourses;
	vector<string> courseNumbers;
	string line;
	int lineNumber = 0;

	// Read and validate each line in the file
	while (getline(file, line)) {
		lineNumber++;

		if (trim(line).empty()) {
			cout << "Error: Empty line found at line " << lineNumber << "." << endl;
			return false;
		}

		stringstream ss(line);
		vector<string> tokens;
		string token;

		// Split the line at each comma
		while (getline(ss, token, ',')) {
			tokens.push_back(trim(token));
		}

		// Ensure the course number and title exist
		if (tokens.size() < 2 || tokens[0].empty() || tokens[1].empty()) {
			cout << "Error: Missing course number or title at line " << lineNumber << "." << endl;
			return false;
		}

		Course currentCourse;
		currentCourse.courseNumber = tokens[0];
		currentCourse.courseTitle = tokens[1];

		// Prevent duplicate course numbers
		if (find(courseNumbers.begin(), courseNumbers.end(), currentCourse.courseNumber) != courseNumbers.end()) {
			cout << "Error: Duplicate course number " << currentCourse.courseNumber << "." << endl;
			return false;
		}

		courseNumbers.push_back(currentCourse.courseNumber);

		// Store all nonempty prerequisites
		for (size_t i = 2; i < tokens.size(); i++) {
			if (!tokens[i].empty()) {
				currentCourse.prerequisites.push_back(tokens[i]);
			}
		}
		temporaryCourses.push_back(currentCourse);
	}
	file.close();

	// Prevent loading an empty course file
	if (temporaryCourses.empty()) {
		cout << "Error: Course file is empty." << endl;
		return false;
	}

	// Verify that every prerequisite exists in the file
	for (const Course& course : temporaryCourses) {
		for (const string& prerequisite : course.prerequisites) {
			if (find(courseNumbers.begin(), courseNumbers.end(), prerequisite) == courseNumbers.end()) {
				cout << "Error: Prerequisite " << prerequisite << " for course " << course.courseNumber << " does not exist in the course file." << endl;
				return false;
			}
		}
	}
	return true;
}

bool loadCourses(string filename, TreeNode*& root) {
	// Validate the file before loading courses
	if (!validateCourseFile(filename)) {
		return false;
	}

	// Open the validated course file
	ifstream file(filename);

	if (!file.is_open()) {
		cout << "Error: Unable to open file." << endl;
		return false;
	}
	string line;

	// Read each line and create a Course object
	while (getline(file, line)) {
		stringstream ss(line);
		string token;
		vector<string> tokens;

		// Separate the CSV values
		while (getline(ss, token, ',')) {
			tokens.push_back(trim(token));
		}
		Course currentCourse;
		
		// Assign the course number and title
		currentCourse.courseNumber = tokens[0];
		currentCourse.courseTitle = tokens[1];

		// Store any prerequisites
		for (size_t i = 2; i < tokens.size(); i++) {
			if (!tokens[i].empty()) {
				currentCourse.prerequisites.push_back(tokens[i]);
			}
		}

		// Insert the course into the binary search tree
		root = insertCourse(root, currentCourse);
	}
	file.close();
	return true;
}

void deleteTree(TreeNode* currentNode) {
	// Stop when an empty node is reached
	if (currentNode == nullptr) {
		return;
	}

	// Delete the left and right subtrees first
	deleteTree(currentNode->left);
	deleteTree(currentNode->right);

	// Delete the current node
	delete currentNode;
}

void printInOrder(TreeNode* currentNode) {
	// Stop when an empty node is reached
	if (currentNode == nullptr) {
		return;
	}

	// Visit the left subtree first
	printInOrder(currentNode->left);

	// Display the current course
	cout << currentNode->course.courseNumber << ", " << currentNode->course.courseTitle << endl;

	// Visit the right subtree
	printInOrder(currentNode->right);
}

void printCourseList(TreeNode* root) {
	// Check whether the tree contains course data
	if (root == nullptr) {
		cout << "No course data available." << endl;
		return;
	}
	cout << "Here is a sample schedule:" << endl;
	cout << endl;

	// Print all courses in alphanumeric order
	printInOrder(root);
}

void searchCourse(TreeNode* root, string courseNumber) {
	// Search the binary search tree for the course
	TreeNode* result = searchCourseNode(root, courseNumber);

	// Display and error if the course was not found
	if (result == nullptr) {
		cout << "Course not found." << endl;
		return;
	}
	
	// Display the requested course
	cout << result->course.courseNumber << ", " << result->course.courseTitle << endl;

	// Display if the course has prerequisites
	if (result->course.prerequisites.empty()) {
		cout << "Prerequisites: None" << endl;
	}
	else {
		cout << "Prerequisites:" << endl;

		// Search for each prerequisite in the BST
		for (const string& prerequisite : result->course.prerequisites) {
			TreeNode* prerequisiteCourse = searchCourseNode(root, prerequisite);

			if (prerequisiteCourse != nullptr) {
				cout << prerequisiteCourse->course.courseNumber << ", " << prerequisiteCourse->course.courseTitle << endl;
			}
		}
	}
}

string toUpperCase(string value) {
	// Convert each character to uppercase
	for (char& character : value) {
		character = static_cast<char>(toupper(static_cast<unsigned char>(character)));
	}
	return value;
}



int main() {
	//Create an empty binary search tree
	TreeNode* root = nullptr;

	string filename;
	string courseNumber;
	string menuInput;
	bool dataLoaded = false;
	int menuChoice = 0;

	cout << "Welcome to the course planner." << endl;

	// Continue displaying the menu until the user exits
	do {
		cout << endl;
		cout << "1. Load Data Structure" << endl;
		cout << "2. Print Course List" << endl;
		cout << "3. Print Course" << endl;
		cout << "9. Exit" << endl;
		cout << endl;

		cout << "What would you like to do? ";
		getline(cin, menuInput);

		// Validate that the user entered a valid number
		stringstream inputStream(menuInput);
		char extraCharacter;

		if (!(inputStream >> menuChoice) || (inputStream >> extraCharacter)) {
			cout << "Invalid menu selection." << endl;
			continue;
		}

		// Option 1: Load the course data
		if (menuChoice == 1) {
			cout << "Enter the course data file name: ";
			getline(cin, filename);

			// Create a temporary tree for the new course data
			TreeNode* newRoot = nullptr;

			if (loadCourses(filename, newRoot)) {
				// Remove previously loaded course data
				deleteTree(root);
				// Replace the old tree with the new tree
				root = newRoot;
				dataLoaded = true;

				cout << "Course data loaded successfully." << endl;
			}
			else {
				// Clean up the unsuccessful load
				deleteTree(newRoot);

				cout << "Course data could not be loaded." << endl;
			}
		}

		// Option 2: Print all courses
		else if (menuChoice == 2) {
			if (dataLoaded) {
				printCourseList(root);
			}
			else {
				cout << "Please load the course data first." << endl;
			}
		}

		// Option 3: Print an individual course
		else if (menuChoice == 3) {
			if (dataLoaded) {
				cout << "What course do you want ot know about? ";
				getline(cin, courseNumber);

				// Remove spaces and convert to uppercase
				courseNumber = toUpperCase(trim(courseNumber));
				searchCourse(root, courseNumber);
			}
			else {
				cout << "Please load the course data first." << endl;
			}
		}

		// Option 9: Exit the program
		else if (menuChoice == 9) {
			cout << "Thank you for using the course planner." << endl;
		}

		// Handle invalid menu selections
		else {
			cout << menuChoice << " is not a valid option." << endl;
		}
	} while (menuChoice != 9);
	// Free allocated memory before exiting
	deleteTree(root);
	
	return 0;
}

