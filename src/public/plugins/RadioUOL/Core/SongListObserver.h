//////////////////////////////////////////////////////////////////////////
// CSongListObserver
//////////////////////////////////////////////////////////////////////////

class CSongListObserver 
{
public:
	virtual void OnLoadedChannels(BOOL ) = 0;
	virtual void OnLoadedWMPURL(BOOL ) = 0;
	virtual void OnLoadedSongTitle(BOOL ) = 0;
	virtual void OnLoadedFerrugem(BOOL ) = 0;
};