// $Id$

#include "CassettePort.hh"
#include "CassetteDevice.hh"
#include "CassettePlayer.hh"
#include "components.hh"
#ifdef COMPONENT_JACK
#include "CassetteJack.hh"
#endif
#include "DummyCassetteDevice.hh"
#include "MSXMotherBoard.hh"
#include "PluggingController.hh"
#include "checked_cast.hh"
#include "serialize.hh"
#include <memory>

using std::auto_ptr;
using std::string;

namespace openmsx {

// CassettePortInterface //

CassettePortInterface::CassettePortInterface()
    : Connector("cassetteport", auto_ptr<Pluggable>(new DummyCassetteDevice()))
{
}

void CassettePortInterface::unplug(const EmuTime& time)
{
	Connector::unplug(time);
}

const string& CassettePortInterface::getDescription() const
{
	static const string desc("MSX Cassette port.");
	return desc;
}

const string& CassettePortInterface::getClass() const
{
	static const string className("Cassette Port");
	return className;
}

CassetteDevice& CassettePortInterface::getPluggedCasDev() const
{
	return *checked_cast<CassetteDevice*>(&getPlugged());
}


// DummyCassettePort //

void DummyCassettePort::setMotor(bool /*status*/, const EmuTime& /*time*/)
{
	// do nothing
}
void DummyCassettePort::cassetteOut(bool /*output*/, const EmuTime& /*time*/)
{
	// do nothing
}
bool DummyCassettePort::cassetteIn(const EmuTime& /*time*/)
{
	return false;
}
bool DummyCassettePort::lastOut() const
{
	return false; // not relevant
}


// CassettePort //

CassettePort::CassettePort(MSXMotherBoard& motherBoard_)
	: CassettePortInterface()
	, motherBoard(motherBoard_)
	, nextSample(0)
{
	cassettePlayer.reset(new CassettePlayer(
		motherBoard.getCommandController(),
		motherBoard.getMSXMixer(),
		motherBoard.getScheduler(),
		motherBoard.getMSXEventDistributor(),
		motherBoard.getEventDistributor(),
		motherBoard.getMSXCliComm()));
	PluggingController& pluggingController = motherBoard.getPluggingController();
	pluggingController.registerConnector(*this);
	pluggingController.registerPluggable(cassettePlayer.get());
#ifdef COMPONENT_JACK
	cassetteJack.reset(new CassetteJack(motherBoard.getScheduler()));
	pluggingController.registerPluggable(cassetteJack.get());
#endif
}

CassettePort::~CassettePort()
{
	unplug(motherBoard.getCurrentTime());
	PluggingController& pluggingController = motherBoard.getPluggingController();
	pluggingController.unregisterPluggable(cassettePlayer.get());
#ifdef COMPONENT_JACK
	pluggingController.unregisterPluggable(cassetteJack.get());
#endif
	pluggingController.unregisterConnector(*this);
}


void CassettePort::setMotor(bool status, const EmuTime& time)
{
	//TODO make 'click' sound
	//PRT_DEBUG("CassettePort: motor " << status);
	getPluggedCasDev().setMotor(status, time);
}

void CassettePort::cassetteOut(bool output, const EmuTime& time)
{
	lastOutput = output;
	// leave everything to the pluggable
	getPluggedCasDev().setSignal(output, time);
}

bool CassettePort::lastOut() const
{
	return lastOutput;
}

bool CassettePort::cassetteIn(const EmuTime& time)
{
	// All analog filtering is ignored for now
	//   only important component is DC-removal
	//   we just assume sample has no DC component
	short sample = getPluggedCasDev().readSample(time); // read 1 sample
	bool result = (sample >= 0); // comparator
	//PRT_DEBUG("CassettePort:: read " << result);
	return result;
}

} // namespace openmsx
