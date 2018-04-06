#include Library_CMC_Scenario


private func InitializeAmbience()
{
	SetSkyParallax(1, 50, 15);
	_inherited(...);
}


private func CreateInterior()
{
	_inherited(...);

  // Ramps
  DrawMaterialQuad("Wall-wall_concrete2", 560, 550, 590, 540, 590, 550, 575, 550, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 651, 550, 621, 540, 621, 550, 636, 550, DMQ_Bridge);

  DrawMaterialQuad("Wall-wall_concrete2", 661, 670, 631, 660, 631, 670, 646, 670, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 691, 680, 661, 670, 661, 680, 676, 680, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 631, 660, 601, 650, 601, 660, 616, 660, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 601, 650, 571, 640, 571, 650, 586, 650, DMQ_Bridge);

  DrawMaterialQuad("Wall-wall_concrete2", 690, 630, 720, 620, 720, 630, 705, 630, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 760, 550, 790, 540, 790, 550, 775, 550, DMQ_Bridge);

  DrawMaterialQuad("Wall-wall_concrete2", 830, 660, 860, 650, 860, 660, 845, 660, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 860, 650, 890, 640, 890, 650, 875, 650, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 800, 670, 830, 660, 830, 670, 815, 670, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 770, 680, 800, 670, 800, 680, 785, 680, DMQ_Bridge);

  DrawMaterialQuad("Wall-wall_concrete2", 1151, 590, 1121, 580, 1121, 590, 1136, 590, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1181, 600, 1151, 590, 1151, 600, 1166, 600, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1121, 580, 1091, 570, 1091, 580, 1106, 580, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1091, 570, 1061, 560, 1061, 570, 1076, 570, DMQ_Bridge);

  DrawMaterialQuad("Wall-wall_concrete2", 1191, 470, 1161, 460, 1161, 470, 1176, 470, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1261, 551, 1231, 541, 1231, 551, 1246, 551, DMQ_Bridge);

  DrawMaterialQuad("Wall-wall_concrete2", 1290, 590, 1320, 580, 1320, 590, 1305, 590, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1260, 600, 1290, 590, 1290, 600, 1275, 600, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1320, 580, 1350, 570, 1350, 580, 1335, 580, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1350, 570, 1380, 560, 1380, 570, 1365, 570, DMQ_Bridge);

  DrawMaterialQuad("Wall-wall_concrete2", 1300, 470, 1330, 460, 1330, 470, 1315, 470, DMQ_Bridge);
  DrawMaterialQuad("Wall-wall_concrete2", 1391, 470, 1361, 460, 1361, 470, 1376, 470, DMQ_Bridge);
}
