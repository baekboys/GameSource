#include "CMapEditor.h"

int main()
{
	static CMapEditor editor;
	if( !editor.Init() )
		return 0;

	while(editor.Run())
	{
		editor.Loop();
	}
}