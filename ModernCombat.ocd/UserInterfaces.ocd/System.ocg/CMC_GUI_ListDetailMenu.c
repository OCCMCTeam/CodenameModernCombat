/**
	Vertical list menu with menu caption, icon, title caption, details.

	@author Marky
 */

static const CMC_GUI_ListDetailMenu = new GUI_Element
{
	BackgroundColor = GUI_CMC_Background_Color_Default,
	Style = GUI_FitChildren,

	Assemble = func ()
	{
		var description_height = new GUI_Dimension{}->SetEm(20);

		var header_height = GuiDimensionCmc(nil, GUI_CMC_Element_Default_Height);
		var header = new GUI_Element {};
		header->SetHeight(header_height);
		header->AddTo(this, nil, "header", true);

		var header_icon = new GUI_Element {};
		header_icon->SetWidth(header_height)
		           ->SetHeight(header_height)
		           ->AddTo(header, nil, "icon", true);

		var header_caption = new GUI_Element {Style = GUI_TextHCenter | GUI_TextVCenter,};
		header_caption->SetLeft(header_icon->GetRight())
		              ->SetHeight(header_height)
		              ->AddTo(header, nil, "caption", true);

		var body = new GUI_Element { Style = GUI_VerticalLayout | GUI_FitChildren | GUI_TextHCenter | GUI_TextVCenter, };
		body->SetTop(header->GetBottom())
		    ->AddTo(this, nil, "body", true);

		var list = new CMC_GUI_List { Priority = 1 };
		list->AddTo(body, nil, "list", true);

		var description = new GUI_Element { ID = 9999, Priority = 2, Style = GUI_FitChildren, };
		description->SetHeight(description_height)
		           ->AddTo(body, nil, "description", true);
		return this;		
	},
	
	SetDescription = func (string text)
	{
		this.body.description.Text = text;
		this.body.description->Update();
		return this;
	},
	
	SetHeaderIcon = func (symbol)
	{
		this.header.icon.Symbol = symbol;
		this.header.icon->Update();
		return this;
	},
	
	SetHeaderCaption = func (string text)
	{
		this.header.caption.Text = text;
		this.header.caption->Update();
		return this;
	},
	
	SetBackgroundCaption = func (string text)
	{
		this.body.Text = text;
		this.body->Update();
		return this;
	},
	
	GetList = func ()
	{
		return this.body.list;
	},
};
