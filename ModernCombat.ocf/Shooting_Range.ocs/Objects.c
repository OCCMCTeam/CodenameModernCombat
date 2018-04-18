/* Automatically created objects file */

func InitializeObjects()
{
	var Branch001 = CreateObject(Branch, 80, 110);
	Branch001->SetCon(96);
	Branch001->SetR(42);
	Branch001->SetPosition(80, 110);
	var Branch002 = CreateObject(Branch, 56, 109);
	Branch002->SetCon(96);
	Branch002->SetR(-33);
	Branch002->SetPosition(56, 109);
	var Branch003 = CreateObject(Branch, 48, 124);
	Branch003->SetCon(110);
	Branch003->SetR(-85);
	Branch003->SetPosition(48, 124);

	var Fern001 = CreateObjectAbove(Fern, 425, 185);
	Fern001->SetCon(101);
	var CMC_GUI_Controller001 = CreateObject(CMC_GUI_Controller, 0, 0, 0);
	CMC_GUI_Controller001->SetPosition(0, 0);
	return true;
}
