#include "MainComponent.h"

MainComponent::MainComponent()	: port(9000)
								, activationTime(0)
{
    setSize (600, 400);
	startTimerHz(60);

	loggingButton.setButtonText("Activate");
	loggingButton.setToggleState(false, dontSendNotification);
	loggingButton.addListener(this);
	addAndMakeVisible(loggingButton);

	saveLogButton.setButtonText("Save Log");
	saveLogButton.addListener(this);
	saveLogButton.setEnabled(false);
	addAndMakeVisible(saveLogButton);

	addAndMakeVisible(messageCounter);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
	loggingButton.setBounds(50, 50, 150, 50);
	saveLogButton.setBounds(50, 150, 150, 50);
	messageCounter.setBounds(50, 250, 150, 50);
}

void MainComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == &loggingButton)
	{
		if (!loggingButton.getToggleState())
		{
			connect(port);
			addListener(this);
			activationTime = Time::getMillisecondCounterHiRes();
			saveLogButton.setEnabled(false);
			loggingButton.setToggleState(true, dontSendNotification);
			loggingButton.setButtonText("Deactivate");
		}
		else
		{
			removeListener(this);
			disconnect();
			saveLogButton.setEnabled(true);
			loggingButton.setToggleState(false, dontSendNotification);
			loggingButton.setButtonText("Activate");

		}
	}
	else if (buttonThatWasClicked == &saveLogButton)
	{
		if (oscMessageList.size() > 0) saveLog();
	}
}

void MainComponent::timerCallback()
{
	messageCounter.setText(String(oscMessageList.size()), dontSendNotification);
}

void MainComponent::oscMessageReceived(const OSCMessage& message)
{
	processOscMessage(message);
}

void MainComponent::oscBundleReceived(const OSCBundle& bundle)
{
	OSCBundle::Element elem = bundle[0];
	processOscMessage(elem.getMessage());
}

void MainComponent::processOscMessage(const OSCMessage& message)
{
	String arguments;
	for (int i = 0; i < message.size(); ++i)
	{
		arguments += "," + String(message[i].getFloat32());
	}

	double time = Time::getMillisecondCounterHiRes() - activationTime;
	String messageText =	String(time) + "," + message.getAddressPattern().toString() + arguments + "\n";
	oscMessageList.add(messageText);
}

void MainComponent::saveLog()
{
	File logFile;

	FileChooser fc("Select or create results export file...",
		File::getCurrentWorkingDirectory(),
		"*.csv",
		true);

	if (fc.browseForFileToSave(true))
	{
		logFile = fc.getResult();


		if (!logFile.exists())
		{
			logFile.create();
			logFile.replaceWithText("time,address,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12\n");
		}

		FileOutputStream fos(logFile);
		for (int i = 0; i < oscMessageList.size(); ++i)
		{
			fos << oscMessageList[i];
		}
	}
}
