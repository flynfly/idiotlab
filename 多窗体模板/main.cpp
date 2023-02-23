#include "main.h"

CBForm form1(ID_form1);

int main()
{
	form1_EventsMap();

	form1.IconSet(IDI_ICON1);
	form1.Show();
	return 0;
}