#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainComponent   : public Component,
						public Button::Listener,
						public Timer,
						public OSCReceiver,
						public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    MainComponent();
    ~MainComponent();

    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button* buttonThatWasClicked) override;
	void timerCallback() override;
	void oscMessageReceived(const OSCMessage& message) override;
	void oscBundleReceived(const OSCBundle& bundle) override;

private:

	void processOscMessage(const OSCMessage& message);
	void saveLog();

	int port;
	double activationTime;

	TextButton loggingButton, saveLogButton;
	Label messageCounter;

	StringArray oscMessageList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
