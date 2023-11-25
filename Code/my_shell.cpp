#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <iterator>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <fstream>

class Shell {
public:
    Shell() {
        setenv("MY_SHELL_VAR", "Hello, I'm a shell variable!", 1);
        updatePrompt();
    }

    void run() {
        std::string input;
        while (true) {
            displayPrompt();
            std::getline(std::cin, input);

            if (input.empty()) {
                continue;
            }

            executeCommand(input);
            updatePrompt();
        }
    }

private:
    std::string prompt;
    std::map<std::string, std::string> variables;

    void displayPrompt() {
        std::cout << prompt;
    }

    void updatePrompt() {
        char buffer[PATH_MAX];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            prompt = "(" + std::string(buffer) + ") $ ";
        } else {
            prompt = "(Directorio Desconocido) $ ";
        }
    }

    void executeCommand(const std::string& input) {
        std::istringstream iss(input);
        std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>());

        if (tokens.empty()) {
            return;
        }

        std::string command = tokens[0];
        if (command == "rm") {
            executeRemove(tokens);
        } else if (command == "more") {
            executeMore(tokens);
        } else if (command == "mv") {
            executeMove(tokens);
        } else if (command == "cp") {
            executeCopy(tokens);
        } else if (command == "cd") {
            executeChangeDirectory(tokens);
        } else if (command == "mkdir") {
            executeMakeDirectory(tokens);
        } else if (command == "pwd") {
            executePrintWorkingDirectory();
        } else if (command == "run-script") {
            tokens.erase(tokens.begin());
            executeScript(tokens);
        } else if (command == "print-env") {
            printEnvironmentVariables();
        } else if (command == "system-cmd") {
            tokens.erase(tokens.begin());
            executeSystemCommand(tokens);
        } else if (command == "set-prompt") {
            setPrompt(tokens);
        } else if (command == "if") {
            executeIfStatement(tokens);
        } else if (command == "for") {
            executeForLoop(tokens);
        } else if (command == "exit") {
            std::cout << "Saliendo de la shell. ¡Hasta luego!" << std::endl;
            std::exit(0);
        } else {
            std::cout << "Comando no reconocido: " << command << std::endl;
        }
    }

    void executeRemove(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso incorrecto: rm <archivo>" << std::endl;
            return;
        }

        std::string filename = tokens[1];
        if (std::remove(filename.c_str()) != 0) {
            std::cout << "Error al eliminar el archivo." << std::endl;
        }
    }

    void executeMore(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso incorrecto: more <archivo>" << std::endl;
            return;
        }

        std::string filename = tokens[1];
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            file.close();
        } else {
            std::cout << "No se puede abrir el archivo." << std::endl;
        }
    }

    void executeMove(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Uso incorrecto: mv <origen> <destino>" << std::endl;
            return;
        }

        std::string source = tokens[1];
        std::string destination = tokens[2];
        if (std::rename(source.c_str(), destination.c_str()) != 0) {
            std::cout << "Error al mover el archivo/directorio." << std::endl;
        }
    }

    void executeCopy(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Uso incorrecto: cp <origen> <destino>" << std::endl;
            return;
        }

        std::string source = tokens[1];
        std::string destination = tokens[2];
        std::ifstream srcFile(source, std::ios::binary);
        std::ofstream dstFile(destination, std::ios::binary);

        if (srcFile && dstFile) {
            dstFile << srcFile.rdbuf();
            srcFile.close();
            dstFile.close();
        } else {
            std::cout << "Error al copiar el archivo." << std::endl;
        }
    }

    void executeChangeDirectory(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso incorrecto: cd <directorio>" << std::endl;
            return;
        }

        std::string directory = tokens[1];
        if (chdir(directory.c_str()) != 0) {
            std::cout << "Error al cambiar de directorio." << std::endl;
        }
    }

    void executeMakeDirectory(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso incorrecto: mkdir <directorio>" << std::endl;
            return;
        }

        std::string directory = tokens[1];
        if (mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            std::cout << "Error al crear el directorio." << std::endl;
        }
    }

    void executePrintWorkingDirectory() {
        char buffer[PATH_MAX];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            std::cout << buffer << std::endl;
        } else {
            std::cout << "Error al obtener el directorio actual." << std::endl;
        }
    }

    void executeScript(const std::vector<std::string>& script) {
        std::ostringstream commandString;
        for (const auto& command : script) {
            commandString << command << ' ';
        }

        int result = std::system(commandString.str().c_str());
        if (result == -1) {
            std::cout << "Error al ejecutar el script." << std::endl;
        }
    }

    void printEnvironmentVariables() {
        std::cout << "Imprimiendo variables de entorno:" << std::endl;
        char** env = environ;
        while (*env != nullptr) {
            std::cout << *env << std::endl;
            ++env;
        }
    }

    void executeSystemCommand(const std::vector<std::string>& command) {
        std::ostringstream commandString;
        for (const auto& arg : command) {
            commandString << arg << ' ';
        }

        int result = std::system(commandString.str().c_str());
        if (result == -1) {
            std::cout << "Error al ejecutar el comando del sistema." << std::endl;
        }
    }

    void setPrompt(const std::vector<std::string>& tokens) {
        if (tokens.size() == 2) {
            prompt = tokens[1] + " ";
        } else {
            std::cout << "Uso incorrecto: set-prompt <nuevo_prompt>" << std::endl;
        }
    }

    bool isNumber(const std::string& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
    }

    void executeIfStatement(const std::vector<std::string>& tokens) {
        if (tokens.size() < 7 || tokens[2] != "==" || tokens[4] != "then" || tokens.back() != "fi") {
            std::cout << "Sintaxis incorrecta para la sentencia if." << std::endl;
            return;
        }

        std::string leftStr = tokens[1];
        std::string rightStr = tokens[3];
        int left, right;

        if (isNumber(leftStr) && isNumber(rightStr)) {
            std::istringstream(leftStr) >> left;
            std::istringstream(rightStr) >> right;

            if (left == right) {
                std::vector<std::string> ifBlock(tokens.begin() + 5, tokens.end() - 1);
                executeCommand(ifBlock);
            }
        } else {
            std::cout << "Sintaxis incorrecta para la comparación numérica en la sentencia if." << std::endl;
        }
    }

    void executeForLoop(const std::vector<std::string>& tokens) {
        if (tokens.size() < 9 || tokens[6] != "do" || tokens.back() != "done") {
            std::cout << "Sintaxis incorrecta para la sentencia for." << std::endl;
            return;
        }

        std::string variable = tokens[1];
        std::string rangeStart = tokens[4];
        std::string rangeEnd = tokens[6];
        std::vector<std::string> command(tokens.begin() + 8, std::prev(tokens.end()));

        for (int i = std::stoi(rangeStart); i <= std::stoi(rangeEnd); ++i) {
            variables[variable] = std::to_string(i);
            executeCommand(command);
        }
    }

    void executeCommand(const std::vector<std::string>& commands) {
        std::ostringstream commandString;
        for (const auto& command : commands) {
            commandString << command << ' ';
        }

        std::string fullCommand = commandString.str();

        if (fullCommand.find("for") != std::string::npos) {
            std::istringstream iss(fullCommand);
            std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                            std::istream_iterator<std::string>());
            executeForLoop(tokens);
        } else {
            int result = std::system(fullCommand.c_str());
            if (result == -1) {
                std::cout << "Error al ejecutar el comando." << std::endl;
            }
        }
    }
};

int main() {
    Shell myShell;
    myShell.run();

    return 0;
}
