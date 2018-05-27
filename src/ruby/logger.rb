module CDAT

  LOG_LEVEL = {
      fatal: 'fatal',
      error: 'error',
      warn: 'warn',
      info: 'info',
      debug: 'debug',
      trace: 'trace'
  }.freeze

  class Logger

    @global_log_logger = false
    @global_log_level = LOG_LEVEL[:info]
    @use_global_log = false
    @global_log_file_path = ''

    class << self; attr_accessor :global_log_logger,
                                 :global_log_level,
                                 :use_global_log,
                                 :global_log_file_path end

    attr_reader :item

    def initialize(item, level)
      @item = item
      @level = level
    end

    def self.for_module(mod, level)
      raise ArgumentError, 'mod must be a Module' unless mod.is_a?(Module)
      Logger.new mod, level
    end

    def self.for_class(clazz, level)
      raise ArgumentError, 'clazz must be a Class' unless clazz.is_a?(Class)
      Logger.new clazz, level
    end

    def self.for_item(item, level)
      raise ArgumentError, 'item must not be nil' if item.nil?
      Logger.new item, level
    end

    private def log(level, message, *prefixes)
      case level
      when :trace
        level_msg = 'trace'
      when :debug
        level_msg = "\033[38;2;220;100;220mdebug\033[m"
      when :info
        level_msg = " \033[38;2;100;210;210minfo\033[m"
      when :warn
        level_msg = " \033[38;2;220;220;100mwarn\033[m"
      when :error
        level_msg = "\033[38;2;255;0;0merror\033[m"
      when :fatal
        level_msg = "\033[5;38;2;255;0;0m>\033[m\033[1;38;2;255;0;0mfatal\033[0m\033[5;38;2;255;0;0m<\033[m"
      else
        level_msg = '  -  '
      end

      prefixes_msg = ''

      if prefixes.length > 0
        prefixes.each { |prefix| prefixes_msg += "[ #{prefix} ]" }
        prefixes_msg += ' '
      end

      puts "[ #{level_msg} ] [ #{item} ] #{prefixes_msg}#{message}"
    end

    def fatal(message, *prefixes)
      log :fatal, message, *prefixes
    end

    def error(message, *prefixes)
      log :error, message, *prefixes
    end

    def warn(message, *prefixes)
      log :warn, message, *prefixes
    end

    def info(message, *prefixes)
      log :info, message, *prefixes
    end

    def debug(message, *prefixes)
      log :debug, message, *prefixes
    end

    def trace(message, *prefixes)
      log :trace, message, *prefixes
    end
  end
end