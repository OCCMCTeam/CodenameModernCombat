/**
	Initial spawning location before game configuration is finished.
 */

#appendto CMC_Game_Session_Configurator

func ContainCrewAt()
{
	var location = new Arena_RelaunchLocation{};
	return location->SetX(985)->SetY(550);
}
