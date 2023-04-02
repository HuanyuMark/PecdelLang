#pragma once
#include <functional>
#include <iostream>
using namespace std;

class Logger {
	public:
		enum class LogLevel {
			ERROR = 0,
			WARN,
			INFO,
			DEBUG,
			TRACE
		};
		class Prefix {
			public:
				constexpr static const char* ERROR = "[ERROR] ";
				constexpr static const char* WARN = "[WARN] ";
				constexpr static const char* INFO = "[INFO] ";
				constexpr static const char* DEBUG = "[DEBUG] ";
				constexpr static const char* TRACE = "[TRACE] ";
		};

		LogLevel level() {
			return Logger::_logLevel;
		}
	
		static void level(LogLevel level) {
			_logLevel = level;
		}

		const char* logLevelPrefix() {
			switch (_logLevel) {
			case LogLevel::ERROR:
				return Prefix::ERROR;
				break;
			case LogLevel::WARN:
				return Prefix::WARN;
				break;
			case LogLevel::INFO:
				return Prefix::INFO;
				break;
			case LogLevel::DEBUG:
				return Prefix::DEBUG;
				break;
			case LogLevel::TRACE:
				return Prefix::TRACE;
				break;
			}
		}

		static Logger createLogger() {
			static Logger logger;
			return logger;
		}

		static Logger createLogger(function<void(function<void()>)> voidLogFunction) {
			Logger logger;
			logger.innerLogFunction = logger.innerLogFunctionFactory(voidLogFunction);
			return logger;
		}

		static Logger createLogger(function<char* (function<void()>)> charsLogFunction, function<void(char*)> charsHandler) {
			Logger logger;
			logger.innerLogFunction = logger.innerLogFunctionFactory(nullptr, charsLogFunction, charsHandler);
			return logger;
		}

		Logger& error(function<void()> operation) {
			cout << Prefix::ERROR;
			innerLogFunction(operation);
			return *this;
		}

		Logger& warn(function<void()> operation) {
			if (_logLevel >= LogLevel::WARN) {
				cout << Prefix::WARN;
				innerLogFunction(operation);
			}
			return *this;
		}

		Logger& info(function<void()> operation) {
			if (_logLevel >= LogLevel::INFO) {
				cout << Prefix::INFO;
				innerLogFunction(operation);
			}
			return *this;
		}

		Logger& debug(function<void()> operation) {
			if (_logLevel >= LogLevel::DEBUG) {
				cout << Prefix::DEBUG;
				innerLogFunction(operation);
			}
			return *this;
		}

		Logger& trace(function<void()> operation) {
			if (_logLevel >= LogLevel::TRACE) {
				cout << Prefix::TRACE;
				innerLogFunction(operation);
			}
			return *this;
		}

		Logger& operator()(function<void()> operation) {
			cout << logLevelPrefix();
			innerLogFunction(operation);
			return *this;
		}

		Logger& operator()(LogLevel level) {
			Logger::_logLevel = level;
			return *this;
		}
		
		
	
	protected:
		/**
		 * 根据日志等级来打印
		 * trace > debug > info > warn > error
		 * 等级高的可以看到等级低,反之不行
		 */
		static LogLevel _logLevel;
		/**
		 * @brief  生成所有打印方法使用的innerLogFunction的工厂函数
		 * @param charsHandler 处理,由charsLogFunction传出的char*
		 * @param charsLogFunction 返回chars的包装函数, 一般就将这些char* 存入硬盘生成日志文件
		 * @param voidLogFunction 无返回值的包装函数,
		 * 						 在这个函数里一般要做的就是打印一些这个Logger实例的titel之类的
		 * @return innerLogFunction
		 */
		static function<void(function<void()>)> innerLogFunctionFactory(function<void(function<void()>)> voidLogFunction,
		        function<char* (function<void()>)> charsLogFunction = nullptr, function<void(char*)> charsHandler = nullptr) {
			if (voidLogFunction)  {

				return voidLogFunction;
			}
			if (charsLogFunction && charsHandler) {
				return  [&](function<void()> target) {
					charsHandler(charsLogFunction(target));
				};
			}
			return [](function<void()> target){
				target();
			};
		}

		function<void(function<void()>)> innerLogFunction;

		Logger() {}
};

Logger::LogLevel Logger::_logLevel = Logger::LogLevel::INFO;


