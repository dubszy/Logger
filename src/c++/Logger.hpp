#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <cstddef>
#include <cstdarg>
#include <stdint.h>

typedef enum {
    LogLevelDebug   = 0,
    LogLevelInfo    = 1,
    LogLevelWarning = 2,
    LogLevelError   = 3,
    LogLevelFatal   = 4,
    LogLevelAll     = 9
} LogLevel;

typedef struct {
    uint8_t debug[3];
    uint8_t info[3];
    uint8_t warning[3];
    uint8_t error[3];
    uint8_t fatal[3];
    uint8_t all[3];
} LevelColor;

/**
 * Whether to log what all Logger instances are doing;
 * takes precedence over individual Logger instances
 */
extern bool global_log_logger_;
/**
 * The log level specified by global_log_level_ takes
 * precedence over all individual Logger instances
 */
extern LogLevel global_log_level_;
/**
 * Whether output from all Logger instances is logged
 * to the path specified by global_log_file_path_
 */
extern bool use_global_log_;
/**
 * Output from all Logger instances is logged to
 * global_log_file_path_ if use_global_log_ is true
 */
extern const char *global_log_file_path_;

class Logger {

public:
    Logger(const char *log_file_path = global_log_file_path_, bool log_logger = false)
      : print_item_name_(false), item_name_(NULL), level_(global_log_level_), log_file_path_(log_file_path), log_logger_(log_logger) {
    };

    Logger(bool print_item_name, const char *item_name, LogLevel level, const char *log_file_path = global_log_file_path_, bool log_logger = false)
      : print_item_name_(print_item_name), item_name_(item_name), level_(level), log_file_path_(log_file_path), log_logger_(log_logger) {
    }

    /**
     * Builder for Logger using a class name for its item name.
     *
     * Example: Logger *log = Logger::forClass<ClassName>(LogLevelInfo);
     *
     * @return A new instance of {@link Logger}
     */
    template <class C>
    static Logger *forClass(LogLevel level);

    /**
     * Builder for Logger using a char array for its item name.
     *
     * @return A new instance of {@link Logger}
     */
    static Logger *forItem(const char *item_name, LogLevel level);

    /**
     * Get whether all Logger instances are logging their actions
     *
     * @return {@link global_log_logger_}
     */
    static bool isGlobalLoggingLogger();

    /**
     * Set whether all Logger instances should log their actions
     */
    static void setGlobalLoggingLogger(bool global_log_logger);

    /**
     * Get the global log level
     *
     * @return {@link global_log_level_}
     */
    static LogLevel getGlobalLogLevel();

    /**
     * Set the global log level
     */
    static void setGlobalLogLevel(LogLevel global_log_level);

    /**
     * Get whether the global log is in use
     *
     * @return {@link use_global_log_}
     */
    static bool isUseGlobalLog();

    /**
     * Set whether the global log file should be used
     */
    static void setUseGlobalLog(bool use_global_log);

    /**
     * Get the global log file path
     *
     * @return {@link global_log_file_path_}
     */
    static const char *getGlobalLogFilePath();

    /**
     * Set the global log file path to {@code path}
     */
    static void setGlobalLogFilePath(const char *path);


    /**
     * Get whether the item name should be printed as a prefix
     *
     * @return {@link print_item_name_}
     */
    bool isPrintItemName();

    /**
     * Set whether the item name should be printed as a prefix
     */
    void setPrintItemName(bool print_item_name);

    /**
     * Get the item name
     *
     * @return {@link item_name_}
     */
    const char *getItemName();

    /**
     * Set the item name to {@code item_name}
     */
    void setItemName(const char *item_name);

    /**
     * Get the log level of this instance
     *
     * @return {@link level_}
     */
    LogLevel getLogLevel();

    /**
     * Set the log level of this instance to {@code level}
     */
    void setLogLevel(LogLevel level);

    /**
     * Get whether this instance is logging its actions
     *
     * @return {@link log_logger_}
     */
    bool isLoggingLogger();

    /**
     * Set whether this instance should log its actions
     */
    void setLogLogger(bool log_logger);

    /*
     * For the following methods with signature:
     * int loglevel(const char **prefixes, int prefixes_len, const char *message, ...)
     *
     * @param prefixes What to prefix the log statement with; each prefix takes the
     *                 form: "[ prefix ]"; example: given 2 prefixes: "foo" and "bar",
     *                 the output will be "[ loglevel ] [ foo ] [ bar ] Message";
     *                 prefixes are printed in order from lowest to highest index
     *
     * @param prefixes_len The amount of prefixes; if {@code prefixes_len} is larger
     *                     than the amount of {@code prefixes} specified, the result
     *                     is undefined; if {@code prefixes_len} is smaller than the
     *                     amount of {@code prefixes} specified, only
     *                     {@code prefixes_len} will be printed
     *
     * @param message The message format to print
     */

    /**
     * Log a fatal error with prefixes and a message format
     */
    int fatal(const char **prefixes, int prefixes_len, const char *message, ...);

    /**
     * Log a fatal error
     */
    int fatal(const char *message, ...);

    /**
     * Log an error with prefixes and a message format
     */
    int error(const char **prefixes, int prefixes_len, const char *message, ...);

    /**
     * Log an error
     */
    int error(const char *message, ...);

    /**
     * Log a warning with prefixes and a message format
     */
    int warn(const char **prefixes, int prefixes_len, const char *message, ...);

    /**
     * Log a warning
     */
    int warn(const char *message, ...);

    /**
     * Log at info level with prefixes and a message format
     */
    int info(const char **prefixes, int prefixes_len, const char *message, ...);

    /**
     * Log at info level
     */
    int info(const char *message, ...);

    /**
     * Log at debug level with prefixes and a message format
     */
    int debug(const char **prefixes, int prefixes_len, const char *message, ...);

    /**
     * Log at debug level
     */
    int debug(const char *message, ...);

private:
    bool print_item_name_;     //< Whether the logger should print item_name_ as a prefix
    const char *item_name_;     //< Object that the logger is logging for (for semantic purposes)
    LogLevel level_;            //< Log level for this instance
    const char *log_file_path_; //< Log file path for this instance
    bool log_logger_;           //< Log what the logger is doing (uses printf)

    /**
     * Get the default level label for a specified level
     */
    const char *level_msg(LogLevel level);

    /**
     * Perform a log
     *
     * @param write_to_log Whether the log file(s) should be written to
     * @param level The level of the statement to log
     * @param prefixes What to prefix the log statement with
     * @param prefixes_len The amount of {@code prefixes}
     * @param message The message format to log
     *
     * @return -1 if {@code level} is less than the global or local log level
     * @return -2 if the log file could not be opened
     * @return If {@code write_to_log} is {@code false}, the return value of writing to stderr or stdout
     * @return If {@code write_to_log} is {@code true}, the return value of writing to the file at {@link log_file_path_}
     */
    int log_(bool write_to_log, LogLevel level, const char **prefixes, int prefixes_len, const char *message, ...);

    /**
     * Perform a log
     *
     * @param write_to_log Whether the log file(s) should be written to
     * @param level The level of the statement to log
     * @param prefixes What to prefix the log statement with
     * @param prefixes_len The amount of {@code prefixes}
     * @param message The message format to log
     * @param args A list of string format varargs to supply to {@code message}
     *
     * @return -1 if {@code level} is less than the global or local log level
     * @return -2 if the log file could not be opened
     * @return If {@code write_to_log} is {@code false}, the return value of writing to stderr or stdout
     * @return If {@code write_to_log} is {@code true}, the return value of writing to the file at {@link log_file_path_}
     */
    int log_(bool write_to_log, LogLevel level, const char **prefixes, int prefixes_len, const char *message, va_list args);
};

#endif /* _LOGGER_HPP_ */
