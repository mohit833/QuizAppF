#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

class User {
public:
    std::string username;
    std::string password;

    User(std::string uname, std::string pwd) : username(std::move(uname)), password(std::move(pwd)) {}
};

class UserManager {
public:
    void registerUser(const std::string& username, const std::string& password) {
        std::ofstream file("users.txt", std::ios::app);
        file << username << " " << password << "\n";
        std::cout << "Registration successful.\n";
    }

    bool authenticateUser(const std::string& username, const std::string& password) {
        std::ifstream file("users.txt");
        std::string uname, pwd;
        while (file >> uname >> pwd) {
            if (uname == username && pwd == password) {
                return true;
            }
        }
        return false;
    }
};

class Question {
public:
    std::string text;
    std::map<int, std::string> options;
    int correctAnswer;

    Question(std::string qText, std::map<int, std::string> opts, int correct)
        : text(std::move(qText)), options(std::move(opts)), correctAnswer(correct) {}
};

class Quiz {
public:
    std::vector<Question> questions;
    std::string title;

    Quiz(std::string t) : title(std::move(t)) {}

    void addQuestion(const Question& question) {
        questions.push_back(question);
    }

    int startQuiz() {
        int score = 0;
        std::cout << "\nStarting Quiz: " << title << "\n";
        for (const auto& q : questions) {
            std::cout << q.text << "\n";
            for (const auto& opt : q.options) {
                std::cout << opt.first << ": " << opt.second << "\n";
            }
            int answer;
            std::cout << "Your answer: ";
            std::cin >> answer;
            if (q.correctAnswer == answer) {
                std::cout << "Correct!\n";
                score++;
            }
            else {
                std::cout << "Wrong. The correct answer was: " << q.correctAnswer << "\n";
            }
        }
        std::cout << "Your score: " << score << "/" << questions.size() << "\n";
        return score;
    }
};

class QuizManager {
public:
    std::vector<Quiz> loadQuizzes(const std::string& filename) {
        std::vector<Quiz> quizzes;
        std::ifstream file(filename);
        std::string line, title;
        Quiz* currentQuiz = nullptr;

        while (getline(file, line)) {
            // Replace starts_with with a substring comparison for pre-C++20 compatibility
            if (line.substr(0, 11) == "Quiz Title:") {
                if (currentQuiz != nullptr) {
                    quizzes.push_back(*currentQuiz);
                    delete currentQuiz;
                }
                title = line.substr(11);
                currentQuiz = new Quiz(title);
            } else if (currentQuiz && !line.empty()) {
                std::istringstream iss(line);
                std::string questionText;
                getline(iss, questionText, '|');
                std::vector<std::string> options;
                std::string option;
                int correctAnswer;
                while (getline(iss, option, ',')) {
                    options.push_back(option);
                }
                iss >> correctAnswer;
                currentQuiz->addQuestion(Question(questionText, options, correctAnswer));
            }
        }
        if (currentQuiz != nullptr) {
            quizzes.push_back(*currentQuiz);
            delete currentQuiz;
        }
        return quizzes;
    }
};

class UserProgressTracker {
public:
    void updateScore(const std::string& username, int score, const std::string& quizTitle) {
        std::ofstream file("scores.txt", std::ios::app);
        file << username << " scored " << score << " on " << quizTitle << "\n";
    }

    void viewScores(const std::string& username) {
        std::ifstream file("scores.txt");
        std::string line;
        std::cout << "Scores for " << username << ":\n";
        while (getline(file, line)) {
            if (line.find(username) != std::string::npos) {
                std::cout << line << "\n";
            }
        }
    }
};

int main() {
    UserManager userManager;
    QuizManager quizManager;
    UserProgressTracker progressTracker;
    std::string username, password;

    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    std::cout << "Do you want to log in or register? (login/register): ";
    std::string action;
    std::cin >> action;

    if (action == "register") {
        userManager.registerUser(username, password);
    }
    else if (action == "login") {
        if (!userManager.authenticateUser(username, password)) {
            std::cout << "Login failed. Please check your credentials.\n";
            return 1;
        }
    }
    else {
        std::cout << "Invalid action.\n";
        return 1;
    }

    auto quizzes = quizManager.loadQuizzes("quizzes.txt");
    std::cout << "Quizzes loaded: " << quizzes.size() << "\n";

    int quizChoice = 1;
    while (quizChoice != 0) {
        std::cout << "\nAvailable quizzes:\n";
        for (size_t i = 0; i < quizzes.size(); ++i) {
            std::cout << i + 1 << ": " << quizzes[i].title << "\n";
        }
        std::cout << "Select a quiz to take (or enter 0 to exit): ";
        std::cin >> quizChoice;
        if (quizChoice < 0 || quizChoice > quizzes.size()) {
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }
        if (quizChoice == 0) {
            break;
        }

        // Adjust for zero-based indexing
        int score = quizzes[quizChoice - 1].startQuiz();
        progressTracker.updateScore(username, score, quizzes[quizChoice - 1].title);

        std::cout << "Would you like to view your scores? (yes/no): ";
        std::string viewScores;
        std::cin >> viewScores;
        if (viewScores == "yes") {
            progressTracker.viewScores(username);
        }
    }

    std::cout << "Thank you for using the Quiz Application. Goodbye!\n";
    return 0;
