#include <logger/c++/Logger.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

bool global_log_logger_           = false;
LogLevel global_log_level_        = LogLevelInfo;
bool use_global_log_             = true;
const char *global_log_file_path_ = "output.log";

/*
 * Builders
 */

Logger *Logger::forItem(string item_name, LogLevel level) {
    Logger *l = new Logger(true, item_name, level);
    return l;
}

LogLevel Logger::getLogLevelFromString(string level_str) {
    if (level_str == "fatal") {
        return LogLevelFatal;
    } else if (level_str == "error") {
        return LogLevelError;
    } else if (level_str == "warn" || level_str == "warning") {
        return LogLevelWarning;
    } else if (level_str == "debug") {
        return LogLevelDebug;
    }
    return LogLevelInfo;
}

/*
 * Global Settings
 */

bool Logger::isGlobalLoggingLogger() {
    return global_log_logger_;
}

void Logger::setGlobalLoggingLogger(bool global_log_logger) {
    global_log_logger_ = global_log_logger;
}

LogLevel Logger::getGlobalLogLevel() {
    return global_log_level_;
}

void Logger::setGlobalLogLevel(LogLevel global_log_level) {
    global_log_level_ = global_log_level;
}

bool Logger::isUseGlobalLog() {
    return use_global_log_;
}

void Logger::setUseGlobalLog(bool use_global_log) {
    use_global_log_ = use_global_log;
}

const char *Logger::getGlobalLogFilePath() {
    return global_log_file_path_;
}

void Logger::setGlobalLogFilePath(const char *path) {
    global_log_file_path_ = path;
}


/*
 * Local Settings
 */

bool Logger::isPrintItemName() {
    return print_item_name_;
}

void Logger::setPrintItemName(bool print_item_name) {
    print_item_name_ = print_item_name;
}

string Logger::getItemName() {
    return item_name_;
}

void Logger::setItemName(string item_name) {
    item_name_ = std::move(item_name);
}

LogLevel Logger::getLogLevel() {
    return level_;
}

void Logger::setLogLevel(LogLevel level) {
    level_ = level;
}

bool Logger::isLoggingLogger() {
    return log_logger_;
}

void Logger::setLogLogger(bool log_logger) {
    log_logger_ = log_logger;
}



/*
 * Log Methods
 */
int Logger::fatal(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelFatal, prefixes, prefixes_len, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::fatal(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelFatal, nullptr, 0, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::error(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelError, prefixes, prefixes_len, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::error(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelError, nullptr, 0, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::warn(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelWarning, prefixes, prefixes_len, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::warn(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelWarning, nullptr, 0, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::info(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelInfo, prefixes, prefixes_len, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::info(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelInfo, nullptr, 0, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::debug(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelDebug, prefixes, prefixes_len, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::debug(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelDebug, nullptr, 0, std::move(message), args);
    va_end(args);
    return log_res;
}

const char *Logger::level_msg(LogLevel l) {
    static const char *msgs[5] = {
            "\033[38;2;220;100;220mdebug\033[m",
            " \033[38;2;100;210;210minfo\033[m",
            " \033[38;2;220;220;100mwarn\033[m",
            "\033[38;2;255;0;0merror\033[m",
            "\033[5;38;2;255;0;0m>\033[m\033[1;38;2;255;0;0mfatal\033[0m\033[5;38;2;255;0;0m<\033[m"
    };

    if (l < 0 || l > 4) {
        return "";
    }

    return (msgs[l]);
}

/*
 * Heavy Lifting
 */

int Logger::log_(bool write_to_log, LogLevel level, string *prefixes, int prefixes_len, string message, ...) {
    int log_res;
    va_list args;
    va_start(args, message);
    log_res = log_(write_to_log, level, prefixes, prefixes_len, std::move(message), args);
    va_end(args);
    return log_res;
}

int Logger::log_(bool write_to_log, LogLevel level, string *prefixes, int prefixes_len, string message, va_list args) {
    if ((level < global_log_level_ || level < level_)) {
        return -1;
    }

    int log_res;
    ofstream logFile;
    string full_prefixes;

    logFile.open(log_file_path_, ios::app);

    if (!logFile.is_open()) {
        cerr << "[ fatal ] [selflog] [ " << item_name_ << " ] Failed to open '" << log_file_path_ << "' (error: " <<
            strerror(errno) << ")";

    }

    full_prefixes.append("[ ").append(level_msg(level)).append(" ] ");

    if (print_item_name_) {
        full_prefixes.append("[ ").append(item_name_).append(" ] ");
    }

    if (isLoggingLogger()) {
        cout << "[selflog] [ " << ((!item_name_.empty()) ? item_name_ : "-") << " ] Prefixes: " << full_prefixes << endl;
    }
    for (int i = 0; i < prefixes_len; i++) {
        if (isLoggingLogger()) {
            cout << "[selflog] [ " << ((!item_name_.empty()) ? item_name_ : "-") << " ] Found prefix: " << prefixes[i] << endl;
        }
        full_prefixes.append("[ ").append(prefixes[i]).append(" ] ");
        if (isLoggingLogger()) {
            cout << "[selflog] [ " << ((!item_name_.empty()) ? item_name_ : "-") << " ] Prefixes: " << full_prefixes << endl;
        }
    }

    char fmt[8192];

    vsprintf(fmt, message.c_str(), args);

    ((level == LogLevelError || level == LogLevelFatal) ? cerr : cout) << full_prefixes << fmt << endl;
    logFile << full_prefixes << fmt << endl;

    if (!logFile.good()) {
        cerr << "[ error ] [selflog] [ " << ((!item_name_.empty()) ? item_name_ : "-") << " ] Failed to write to log '" <<
            log_file_path_ << "' (state: " << logFile.rdstate() << ")" << endl;
    }

    log_res = logFile.rdstate();

    logFile.close();

    return log_res;
}
