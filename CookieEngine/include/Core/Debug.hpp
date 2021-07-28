#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include <vector>
#include <string>
#include <time.h>

namespace Cookie
{
	namespace Core
	{
		struct DebugMessage
		{
			const std::string	text;
			char				timestamp[10] {};

			enum Type
			{ 
				// Basic information. Will slowly flash blue at arrival.
				Log, 
					
				// Indicate a warning. Will flash yellow once.
				Warning,

				// Signal an error. Will quickly flash red thrice.
				Error
				
			} messageType;
				
			unsigned short	colorBounces	= 0;
			uint8_t			colorVariant	= 255;
			bool			bouncing		= false;


			inline DebugMessage(const std::string& _text, Type&& _type, unsigned short&& _colorBounces = 0)
				: text			(_text),
				  messageType	(_type),
				  colorBounces	(_colorBounces)
			{}
		};

		// [Singleton class closely linked with the console widget. Can be used anywhere after being initialized.]
		class DebugMessageHandler
		{
		private:
			time_t	currentTime;
			tm		currentTmcov;

		public:
			std::vector<DebugMessage> storedMessages;


		private:
			DebugMessageHandler()								= default;
			DebugMessageHandler(DebugMessageHandler&&)			= delete;
			DebugMessageHandler(const DebugMessageHandler& )	= delete;


			inline void AddMessage(DebugMessage&& newMessage)
			{
				sprintf(newMessage.timestamp, "[%d:%d:%d]", currentTmcov.tm_hour, currentTmcov.tm_min, currentTmcov.tm_sec);
				storedMessages.push_back(std::move(newMessage));
			}
		
		public:
			// Summons the Debug manager.
			inline static DebugMessageHandler& Summon()
			{ static DebugMessageHandler debugSingleton; debugSingleton.UpdateTime(); return debugSingleton; }


			inline void UpdateTime()
			{ time(&currentTime); localtime_s(&currentTmcov, &currentTime); }


			inline void Log(const char*&& text)
			{ AddMessage({text, DebugMessage::Log}); }

			inline void Log(const std::string& strext)
			{ AddMessage({strext.c_str(), DebugMessage::Log}); }
			
			
			inline void Warning(const char*&& text)
			{ AddMessage({text, DebugMessage::Warning, 1}); }

			inline void Warning(const std::string& strext)
			{ AddMessage({strext.c_str(), DebugMessage::Warning, 1}); }


			inline void Error(const char*&& text)
			{ AddMessage({text, DebugMessage::Error, 2}); }

			inline void Error(const std::string& strext)
			{ AddMessage({strext.c_str(), DebugMessage::Error, 2}); }
		};
	}
}


#define CDebug Cookie::Core::DebugMessageHandler::Summon()

#endif // !__DEBUG__
