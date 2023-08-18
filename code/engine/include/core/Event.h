#pragma once


namespace rush
{
	/**
	 * EventType
	 */
	enum class EventType
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowMoved,

		AppTick,
		AppUpdate,
		AppRender,

		KeyTyped,
		KeyPressed,
		KeyReleased,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,

		AssetModified,
	};

	/**
	 * EventCategory
	 */
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication		= RUSH_BIT(0),
		EventCategoryInput				= RUSH_BIT(1),
		EventCategoryKeyboard			= RUSH_BIT(2),
		EventCategoryMouse				= RUSH_BIT(3),
		EventCategoryMouseButton		= RUSH_BIT(4),
		EventEditor 					= RUSH_BIT(5),
	};

	/**
	 * MouseCode
	 */
	using MouseCode = uint16_t;
	namespace Mouse
	{
		enum MouseCode
		{
			ButtonLeft = 0,
			ButtonRight = 1,
			ButtonMiddle = 2,
		};
	}

	/**
	 * KeyCode
	 */
	using KeyCode = uint16_t;
	namespace Key
	{
		enum KeyCode
		{
			// From glfw3.h
			Space = 32,
			Apostrophe = 39, /* ' */
			Comma = 44, /* , */
			Minus = 45, /* - */
			Period = 46, /* . */
			Slash = 47, /* / */

			D0 = 48, /* 0 */
			D1 = 49, /* 1 */
			D2 = 50, /* 2 */
			D3 = 51, /* 3 */
			D4 = 52, /* 4 */
			D5 = 53, /* 5 */
			D6 = 54, /* 6 */
			D7 = 55, /* 7 */
			D8 = 56, /* 8 */
			D9 = 57, /* 9 */

			Semicolon = 59, /* ; */
			Equal = 61, /* = */

			A = 65,
			B = 66,
			C = 67,
			D = 68,
			E = 69,
			F = 70,
			G = 71,
			H = 72,
			I = 73,
			J = 74,
			K = 75,
			L = 76,
			M = 77,
			N = 78,
			O = 79,
			P = 80,
			Q = 81,
			R = 82,
			S = 83,
			T = 84,
			U = 85,
			V = 86,
			W = 87,
			X = 88,
			Y = 89,
			Z = 90,

			LeftBracket = 91,  /* [ */
			Backslash = 92,  /* \ */
			RightBracket = 93,  /* ] */
			GraveAccent = 96,  /* ` */

			World1 = 161, /* non-US #1 */
			World2 = 162, /* non-US #2 */

			/* Function keys */
			Escape = 256,
			Enter = 257,
			Tab = 258,
			Backspace = 259,
			Insert = 260,
			Delete = 261,
			Right = 262,
			Left = 263,
			Down = 264,
			Up = 265,
			PageUp = 266,
			PageDown = 267,
			Home = 268,
			End = 269,
			CapsLock = 280,
			ScrollLock = 281,
			NumLock = 282,
			PrintScreen = 283,
			Pause = 284,
			F1 = 290,
			F2 = 291,
			F3 = 292,
			F4 = 293,
			F5 = 294,
			F6 = 295,
			F7 = 296,
			F8 = 297,
			F9 = 298,
			F10 = 299,
			F11 = 300,
			F12 = 301,
			F13 = 302,
			F14 = 303,
			F15 = 304,
			F16 = 305,
			F17 = 306,
			F18 = 307,
			F19 = 308,
			F20 = 309,
			F21 = 310,
			F22 = 311,
			F23 = 312,
			F24 = 313,
			F25 = 314,

			/* Keypad */
			KP0 = 320,
			KP1 = 321,
			KP2 = 322,
			KP3 = 323,
			KP4 = 324,
			KP5 = 325,
			KP6 = 326,
			KP7 = 327,
			KP8 = 328,
			KP9 = 329,
			KPDecimal = 330,
			KPDivide = 331,
			KPMultiply = 332,
			KPSubtract = 333,
			KPAdd = 334,
			KPEnter = 335,
			KPEqual = 336,

			LeftShift = 340,
			LeftControl = 341,
			LeftAlt = 342,
			LeftSuper = 343,
			RightShift = 344,
			RightControl = 345,
			RightAlt = 346,
			RightSuper = 347,
			Menu = 348
		};
	}

	/**
	 * Event
	 */
	class Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string DebugString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool Handled = false;
	};

	using EventCallbackFn = std::function<void(Event&)>;
	#define RUSH_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	/**
	 * EventDispatcher
	 */
	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		// F will be deduced by the compiler
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.DebugString();
	}

#define EVENT_CLASS_TYPE(TYPE) static EventType GetStaticType() { return EventType::TYPE; }\
								virtual EventType GetType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #TYPE; }

#define EVENT_CLASS_CATEGORY(CATEGORY) virtual int GetCategoryFlags() const override { return CATEGORY; }


	// Application Event
	class WindowResizeEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		WindowResizeEvent(const std::string& windowID, unsigned int width, unsigned int height)
			: m_WindowID(windowID), m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }
		const std::string& GetWindowID() const { return m_WindowID; }

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

	private:
		unsigned int m_Width, m_Height;
		std::string m_WindowID;
	};

	class WindowCloseEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		WindowCloseEvent(const std::string& windowID)
			: m_WindowID(windowID) {}

		WindowCloseEvent() = default;

		const std::string& GetWindowID() const { return m_WindowID; }

	private:
		std::string m_WindowID;
	};

    class WindowFocusEvent : public Event
    {
    public:
        EVENT_CLASS_TYPE(WindowFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

		WindowFocusEvent(const std::string& windowID, bool focus)
            : m_WindowID(windowID) , m_Focus(focus) {}

		WindowFocusEvent() = default;

        const std::string& GetWindowID() const { return m_WindowID; }

		bool GetFocus() const { return m_Focus; }

    private:
        std::string m_WindowID;
		bool m_Focus;
    };

	class AppQuitEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		AppQuitEvent() = default;
	};

	class AppUpdateEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		AppUpdateEvent() = default;
	};

	class AppRenderEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		AppRenderEvent() = default;
	};

	// Mouse Events
	class MouseMovedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

		MouseMovedEvent(const std::string& windowID, int x, int y)
			: m_WindowID(windowID)
			, m_MouseX(x)
			, m_MouseY(y)
		{
		}

		const std::string& GetWindowID() const { return m_WindowID; }

		int GetX() const { return m_MouseX; }
		int GetY() const { return m_MouseY; }

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

	private:
        std::string m_WindowID;
        int m_MouseX, m_MouseY;
	};


	class MouseScrolledEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

		MouseScrolledEvent(const std::string& windowID, float xOffset, float yOffset)
            : m_WindowID(windowID)
            , m_XOffset(xOffset)
			, m_YOffset(yOffset)
		{
		}

        const std::string& GetWindowID() const { return m_WindowID; }

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}
	private:
        std::string m_WindowID;
        float m_XOffset, m_YOffset;
	};


	class MouseButtonEvent : public Event
	{
	public:
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

		MouseCode GetMouseButton() const { return m_Button; }

        const std::string& GetWindowID() const { return m_WindowID; }

	protected:
		MouseButtonEvent(const std::string& windowID, MouseCode button)
            : m_WindowID(windowID)
            , m_Button(button)
		{
		}


	protected:
        std::string m_WindowID;
        MouseCode m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		EVENT_CLASS_TYPE(MouseButtonPressed)

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		MouseButtonPressedEvent(const std::string& windowID, MouseCode button)
			:MouseButtonEvent(windowID, button)
		{
		}
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		EVENT_CLASS_TYPE(MouseButtonReleased)

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		MouseButtonReleasedEvent(const std::string& windowID, MouseCode button)
			:MouseButtonEvent(windowID, button)
		{
		}
	};

	// keyboard event

	class KeyEvent : public Event
	{
	public:
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

        const std::string& GetWindowID() const { return m_WindowID; }

		KeyCode GetKeyCode() const { return m_KeyCode; }

	protected:
		KeyEvent(const std::string& windowID, const KeyCode keycode)
			: m_WindowID(windowID)
			, m_KeyCode(keycode) {}

        std::string m_WindowID;
        KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		EVENT_CLASS_TYPE(KeyPressed)

		KeyPressedEvent(const std::string& windowID, const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(windowID, keycode), m_Repeat(isRepeat) {}

		bool IsRepeat() const { return m_Repeat; }

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_Repeat << ")";
			return ss.str();
		}

	private:
		bool m_Repeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		EVENT_CLASS_TYPE(KeyReleased)

		KeyReleasedEvent(const std::string& windowID, const KeyCode keycode)
			: KeyEvent(windowID, keycode) {}

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

	};

	class KeyRepeatedEvent : public KeyEvent
	{
	public:
		EVENT_CLASS_TYPE(KeyTyped)

		KeyRepeatedEvent(const std::string& windowID, const KeyCode keycode)
			: KeyEvent(windowID, keycode) {}

		std::string DebugString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

	};

	// Event Manager
	class EventManager
	{
	public:
		EventManager();
		virtual ~EventManager();

		static EventManager& Inst()
		{
			if (s_Inst == nullptr)
				s_Inst = new EventManager();
			return *s_Inst;
		}

		void RegisterCallbacks(std::initializer_list<EventType> eventTypes, EventCallbackFn callback);

		void RegisterCallback(EventType eventType, EventCallbackFn callback);

		void UnRegisterCallbacks(std::initializer_list<EventType> eventTypes, EventCallbackFn callback);

		void UnRegisterCallback(EventType eventType, EventCallbackFn callback);

		void PublishEvent(Event& event);

	protected:
		static EventManager* s_Inst;

		std::map<EventType, std::list<EventCallbackFn>> m_EventCallbacks;
	};
}