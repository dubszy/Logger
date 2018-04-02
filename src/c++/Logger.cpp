#include "Logger.hpp"

#include <cxxabi.h>
#include <errno.h>
#include <memory>
#include <stdio.h>
#include <string.h>

bool global_log_logger_           = false;
LogLevel global_log_level_        = LogLevelInfo;
bool use_global_log_             = true;
const char *global_log_file_path_ = "output.log";

/*
 * Builders
 */

template <class C>
Logger *Logger::forClass(LogLevel level) {
    char *demangled = abi::__cxa_demangle(typeid(C).name(), 0, 0, NULL);
    Logger *l = new Logger(demangled, level);
    printf("Logger::forClass : classname: %s\n", demangled);
    l->setPrintItemName(true);
    free(demangled);
    return l;
}

Logger *Logger::forItem(const char *item_name, LogLevel level) {
    Logger *l = new Logger(item_name, level);
    l->setPrintItemName(true);
    return l;
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

const char *Logger::getItemName() {
    return item_name_;
}

void Logger::setItemName(const char *item_name) {
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
int Logger::fatal(const char **prefixes, int prefixes_len, const char *message, ...) {
    va_list args;
    va_start(args, message);
    return log_(true, LogLevelFatal, prefixes, prefixes_len, message, args);
}

int Logger::fatal(const char *message, ...) {
    va_list args;
    va_start(args, message);
    return fatal(NULL, 0, message, args);
}

int Logger::error(const char **prefixes, int prefixes_len, const char *message, ...) {
    va_list args;
    va_start(args, message);
    return log_(true, LogLevelError, prefixes, prefixes_len, message, args);
}

int Logger::error(const char *message, ...) {
    va_list args;
    va_start(args, message);
    return error(NULL, 0, message, args);
}

int Logger::warn(const char **prefixes, int prefixes_len, const char *message, ...) {
    va_list args;
    va_start(args, message);
    return log_(true, LogLevelWarning, prefixes, prefixes_len, message, args);
}

int Logger::warn(const char *message, ...) {
    va_list args;
    va_start(args, message);
    return warn(NULL, 0, message, args);
}

int Logger::info(const char **prefixes, int prefixes_len, const char *message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelInfo, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::info(const char *message, ...) {
    va_list args;
    va_start(args, message);
    int log_res = log_(true, LogLevelInfo, NULL, 0, message, args);
    va_end(args);
    return log_res;
}

int Logger::debug(const char **prefixes, int prefixes_len, const char *message, ...) {
    va_list args;
    va_start(args, message);
    return log_(true, LogLevelInfo, prefixes, prefixes_len, message, args);
}

int Logger::debug(const char *message, ...) {
    va_list args;
    va_start(args, message);
    return debug(NULL, 0, message, args);
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

int Logger::log_(bool write_to_log, LogLevel level, const char **prefixes, int prefixes_len, const char *message, ...) {
    int log_res;
    va_list args;
    va_start(args, message);
    log_res = log_(write_to_log, level, prefixes, prefixes_len, message, args);
    va_end(args);
    return log_res;
}

int Logger::log_(bool write_to_log, LogLevel level, const char **prefixes, int prefixes_len, const char *message, va_list args) {
    if ((level < global_log_level_ || level < level_)) {
        return -1;
    }

    int log_res;
    FILE *logfile;
    char msg[512];

    strcpy(msg, "[ ");
    strcat(msg, level_msg(level));
    strcat(msg, " ] ");
    if (isLoggingLogger()) {
        printf("[selflog] [ %s ] Message: %s\n", ((item_name_) ? item_name_ : "-"), msg);
    }
    for (int i = 0; i < prefixes_len; i++) {
        if (isLoggingLogger()) {
            printf("[selflog] [ %s ] Found prefix: %s\n", ((item_name_) ? item_name_ : "-"), prefixes[i]);
        }
        strcat(msg, "[ ");
        strcat(msg, prefixes[i]);
        strcat(msg, " ] ");
        if (isLoggingLogger()) {
            printf("[selflog] [ %s ] Message: %s\n", ((item_name_) ? item_name_ : "-"), msg);
        }
    }

    char fmt[512];

    if (args) {
        vsprintf(fmt, message, args);
    } else {
        strcpy(fmt, message);
    }

    strcat(msg, fmt);

    log_res = fprintf((level == LogLevelError || level == LogLevelFatal) ? stderr : stdout, "%s", msg);

    if (write_to_log) {
        if ((logfile = fopen(log_file_path_, "a")) == NULL) {
            printf("[ fatal ] [selflog] [ %s ] Failed to open '%s' (error: %s)\n",
                    ((item_name_) ? item_name_ : "-"), log_file_path_, strerror(errno));
            return -2;
        }
        if ((log_res = fprintf(logfile, message, args)) < 1) {
            printf("[ fatal ] [selflog] [ %s ] Failed to write to '%s' (error: %s)\n",
                    ((item_name_) ? item_name_ : "-"), log_file_path_, strerror(errno));
        }
        fclose(logfile);
    }

    return log_res;
}
