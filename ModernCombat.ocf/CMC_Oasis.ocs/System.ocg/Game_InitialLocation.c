/**
	Initial spawning location before game configuration is finished.
 */

#appendto CMC_Game_Session_Configurator

func ContainCrewAt()
{
	var location = new Arena_RelaunchLocation{};
	return location->SetX(3415)->SetY(360);
}
