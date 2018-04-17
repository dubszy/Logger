#include "Logger.hpp"

#include <errno.h>
#include <stdio.h>
#include <string.h>

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
    if (level_str.compare("fatal")) {
        return LogLevelFatal;
    } else if (level_str.compare("error")) {
        return LogLevelError;
    } else if (level_str.compare("warn") || level_str.compare("warning")) {
        return LogLevelWarning;
    } else if (level_str.compare("debug")) {
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
    item_name_ = item_name;
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
    int log_res = log_(true, LogLevelFatal, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::fatal(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelFatal, NULL, 0, message, args);
    va_end(args);
    return log_res;
}

int Logger::error(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelError, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::error(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelError, NULL, 0, message, args);
    va_end(args);
    return log_res;
}

int Logger::warn(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelWarning, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::warn(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelWarning, NULL, 0, message, args);
    va_end(args);
    return log_res;
}

int Logger::info(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelInfo, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::info(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelInfo, NULL, 0, message, args);
    va_end(args);
    return log_res;
}

int Logger::debug(string *prefixes, int prefixes_len, string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelDebug, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::debug(string message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelDebug, NULL, 0, message, args);
    va_end(args);
    return log_res;
}

const char *Logger::level_msg(LogLevel l) {
    static const char *msgs[5] = {
            "debug",
            " info",
            " warn",
            "error",
            "fatal"
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
    log_res = log_(write_to_log, level, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::log_(bool write_to_log, LogLevel level, string *prefixes, int prefixes_len, string message, va_list args) {
    if ((level < global_log_level_ || level < level_)) {
        return -1;
    }

    int log_res;
    FILE *logfile;
    string full_prefixes;
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

    char fmt[4096];

    if (args) {
        vsprintf(fmt, message.c_str(), args);
    } else {
        strcpy(fmt, message.c_str());
    }

    ((level == LogLevelError || level == LogLevelFatal) ? cerr : cout) << full_prefixes << fmt << endl;

    //log_res = fprintf(((level == LogLevelError || level == LogLevelFatal) ? stderr : stdout), "%s", msg);

//    if (write_to_log) {
//        if ((logfile = fopen(log_file_path_, "a")) == NULL) {
//            printf("[ fatal ] [selflog] [ %s ] Failed to open '%s' (error: %s)\n",
//                    ((item_name_) ? item_name_ : "-"), log_file_path_, strerror(errno));
//            return -2;
//        }
//        if ((log_res = fprintf(logfile, message, args)) < 1) {
//            printf("[ fatal ] [selflog] [ %s ] Failed to write to '%s' (error: %s)\n",
//                    ((item_name_) ? item_name_ : "-"), log_file_path_, strerror(errno));
//        }
//        fclose(logfile);
//    }

    return log_res;
}
