#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize (600, 400);
	connect(9000);

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

    //g.setFont (Font (16.0f));
    //g.setColour (Colours::white);
    //g.drawText ("Atomic OSC Logger", getLocalBounds(), Justification::centred, true);
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
			activationTime = Time::getMillisecondCounterHiRes();
			addListener(this);
			saveLogButton.setEnabled(false);
			loggingButton.setToggleState(true, dontSendNotification);
			loggingButton.setButtonText("Deactivate");
		}
		else
		{
			removeListener(this);
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
	if (message.size() == 3 && message.getAddressPattern() == "/rendering/htrpy")
	{
		float roll = message[0].getFloat32();
		float pitch = message[1].getFloat32();
		float yaw = message[2].getFloat32();

		double time = Time::getMillisecondCounterHiRes() - activationTime;

		String messageText = String(time) + "," + String(roll) + "," + String(pitch) + "," + String(yaw) + "\n";
		oscMessageList.add(messageText);
	}
}

void MainComponent::saveLog()
{
	File logFile;

#if JUCE_MODAL_LOOPS_PERMITTED
	FileChooser fc("Select or create results export file...",
		File::getCurrentWorkingDirectory(),
		"*.csv",
		true);

	if (fc.browseForFileToSave(true))
	{
		logFile = fc.getResult();
	}
#endif

	if (logFile.exists())
	{
		logFile.replaceWithText("");
	}
	else
	{
		logFile.create();
	}

	FileOutputStream fos(logFile);
	fos << "time,roll,pitch,yaw\n";
	for (int i = 0; i < oscMessageList.size(); ++i)
	{
		fos << oscMessageList[i];
	}
}
