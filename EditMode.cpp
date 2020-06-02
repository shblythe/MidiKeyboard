#include "EditMode.h"
#include "Display.h"

EditMode EditMode::instance;

void EditMode::assignController(AssignableController* a)
{
	assignableController=a;
}

void EditMode::loop()
{
	if (!active)
		return;
	switch (state)
	{
		case STATE_EDITMENU:
			Display::it()->displayMessage(Display::MSG_BLANK);
			if (keyPressed==KEY_ASSIGN)
			{
				keyPressed=KEY_NONE;
				state=STATE_ASSIGN;
				assignableController=0;
				Display::it()->displayMessage(Display::MSG_CHOOSE);

			}
			break;
		case STATE_ASSIGN:
			if (assignableController!=0)
			{
				state=STATE_ASSIGN_NUM_ENTRY;
				Display::it()->displayLEDsValue(assignableController->getControlNumber());
				numDigitsEntered=0;
			}
			if (keyPressed==KEY_ENTER)
			{
				keyPressed=KEY_NONE;
				state=STATE_EDITMENU;
			}
			break;
		case STATE_ASSIGN_NUM_ENTRY:
			if (keyPressed>=KEY_0 && keyPressed<=KEY_9)
			{
				if (numDigitsEntered<Display::LED_NUMDIGITS)
					numEntry[numDigitsEntered++]=keyPressed-KEY_0;
				Display::it()->displayNumString(numEntry,numDigitsEntered);
				keyPressed=KEY_NONE;
			}
			if (keyPressed==KEY_ENTER)
			{
				int num=0;
				for (int i=0; i<numDigitsEntered; i++)
				{
					num*=10;
					num+=numEntry[i];
				}
				if (numDigitsEntered==0 || num<256 && assignableController->setControl(num))
					state=STATE_EDITMENU;
				else
					state=STATE_ERROR;
				assignableController=0;
				keyPressed=KEY_NONE;
			}
			break;
		case STATE_ERROR:
			Display::it()->displayMessage(Display::MSG_ERROR);
			errorStartMs=millis();
			state=STATE_ERROR_DELAY;
			break;
		case STATE_ERROR_DELAY:
			if (millis()-errorStartMs>errorLengthMs)
				state=STATE_EDITMENU;
			break;

	}
}