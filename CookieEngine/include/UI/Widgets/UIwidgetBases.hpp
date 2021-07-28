#ifndef	__COOKIEUI_WIDGET_BASES_HPP__
#define __COOKIEUI_WIDGET_BASES_HPP__

#include <memory>


namespace Cookie::UIwidget
{
	struct WindowBase
	{
	protected:
		const char* windowName;
		bool		opened;
		bool		contentVisible = true;

	protected:
		virtual bool BeginWindow(int windowFlags = 0);

	public:
		WindowBase(const char* _windowName, bool _opened = true);
		
		virtual ~WindowBase() = default;

		
		virtual void WindowDisplay() = 0;
	};
	
	
	struct GameWindowBase : WindowBase
	{
	private:
		// Invalid game windows are not ran at the beginning of a game and cannot be saved. Their tittle bar is red.
		bool invalid = false;

	public:
		float	xPos	= 0, 
				yPos	= 0, 
				width	= 100,
				height	= 100;

	protected:
		// Note: The flags to prevent docking and collapsing are always given.
		virtual bool BeginWindow(int windowFlags = 0) override;

		virtual void WindowPreview() {};

	public:
		inline GameWindowBase(const char* _nameTag, bool _opened)
			: WindowBase	(_nameTag, _opened)
		{}

		bool WindowEdit();

		inline const char* GetName() { return windowName; }

		virtual inline int GetID() = 0;
	};


	struct ItemBase
	{
	protected:
		const char* itemName;
		const char* shortcutSeq;
		bool		visible;


	public:
		ItemBase(const char* _itemName,							  bool _visible = true);
		ItemBase(const char* _itemName, const char* _shortcutSeq, bool _visible = true);

		virtual ~ItemBase() = default;


		virtual void ItemDisplay();
	};


	struct WItemBase : WindowBase, ItemBase
	{
	protected:
		virtual bool BeginWindow(int windowFlags = 0) override;

		inline void Flip()
		{opened = !opened; visible = !visible;}

	public:

		/*
			Most basic contructor for the WItem, providing a name used by both Window and Item form and indicating what form it starts in.

			@param _name		Name of the WItem
			@param _isItemFirst (True if not indicated otherwise) Indicates if the WItem should begin as an item
		*/
		inline WItemBase(const char* _name, bool _isItemFirst = true)
			:	WindowBase	(_name, !_isItemFirst),
				ItemBase	(_name,  _isItemFirst)
		{}


		/*
			Contructor for the WItem, providing a name for the Window and the Item forms and indicating what form it starts in.

			@param _windowName	Name of the Window form
			@param _itemName	Name of the Item form
			@param _isItemFirst (True if not indicated otherwise) Indicates if the WItem should begin as an item
		*/
		inline WItemBase(const char* _windowName, const char* _itemName, bool _isItemFirst = true)
			:	WindowBase	(_windowName, !_isItemFirst),
				ItemBase	(_itemName,    _isItemFirst)
		{}


		/*
			Advanced contructor for the WItem, providing a name for the Window and the Item forms and indicating what form it starts in, with the additional mention of a usable shortcut.

			@param _windowName	Name of the Window form
			@param _itemName	Name of the Item form
			@param _shortcutSeq	Shortcut sequence to trigger the item remotely
			@param _isItemFirst (True if not indicated otherwise) Indicates if the WItem should begin as an item
		*/
		inline WItemBase(const char* _windowName, const char* _itemName, const char* _shortcutSeq, bool _isItemFirst = true)
			:	WindowBase	(_windowName,			   !_isItemFirst),
				ItemBase	(_itemName,	  _shortcutSeq, _isItemFirst)
		{}


		virtual ~WItemBase() override = default;


		virtual void ItemDisplay() override;
	};


	/* [Engine shortcut]
		-> Usable inside WItems and Windows.

		It will attempt to open the imgui window using the flags.

		Will stop the function it is in if the window is not opened.
		Use brackets: everything inside will be executed if the window's content is visible.
	*/
	#define TryBeginWindow(flags) if (!BeginWindow(flags)) return; if (contentVisible)
}


#endif